//===--- Preamble.cpp - Reusing expensive parts of the AST ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "Preamble.h"
#include "Logger.h"
#include "Trace.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/PreprocessorOptions.h"

namespace clang {
namespace clangd {
namespace {

bool compileCommandsAreEqual(const tooling::CompileCommand &LHS,
                             const tooling::CompileCommand &RHS) {
  // We don't check for Output, it should not matter to clangd.
  return LHS.Directory == RHS.Directory && LHS.Filename == RHS.Filename &&
         llvm::makeArrayRef(LHS.CommandLine).equals(RHS.CommandLine);
}

// This collects macro definitions in the *preamble region* of the main file.
// (Contrast with CollectMainFileMacroExpansions in ParsedAST.cpp, which
// collects macro *expansions* in the rest of the main file.
class CollectMainFileMacros : public PPCallbacks {
public:
  explicit CollectMainFileMacros(const SourceManager &SM,
                                 std::vector<std::string> *Out)
      : SM(SM), Out(Out) {}

  void FileChanged(SourceLocation Loc, FileChangeReason,
                   SrcMgr::CharacteristicKind, FileID Prev) {
    InMainFile = SM.isWrittenInMainFile(Loc);
  }

  void MacroDefined(const Token &MacroName, const MacroDirective *MD) {
    if (InMainFile)
      MainFileMacros.insert(MacroName.getIdentifierInfo()->getName());
  }

  void EndOfMainFile() {
    for (const auto &Entry : MainFileMacros)
      Out->push_back(Entry.getKey());
    llvm::sort(*Out);
  }

private:
  const SourceManager &SM;
  bool InMainFile = true;
  llvm::StringSet<> MainFileMacros;
  std::vector<std::string> *Out;
};

class CppFilePreambleCallbacks : public PreambleCallbacks {
public:
  CppFilePreambleCallbacks(PathRef File, PreambleParsedCallback ParsedCallback)
      : File(File), ParsedCallback(ParsedCallback) {
  }

  IncludeStructure takeIncludes() { return std::move(Includes); }

  std::vector<std::string> takeMainFileMacros() {
    return std::move(MainFileMacros);
  }

  CanonicalIncludes takeCanonicalIncludes() { return std::move(CanonIncludes); }

  void AfterExecute(CompilerInstance &CI) override {
    if (!ParsedCallback)
      return;
    trace::Span Tracer("Running PreambleCallback");
    ParsedCallback(CI.getASTContext(), CI.getPreprocessorPtr(), CanonIncludes);
  }

  void BeforeExecute(CompilerInstance &CI) override {
    addSystemHeadersMapping(&CanonIncludes, CI.getLangOpts());
    SourceMgr = &CI.getSourceManager();
  }

  std::unique_ptr<PPCallbacks> createPPCallbacks() override {
    assert(SourceMgr && "SourceMgr must be set at this point");
    return std::make_unique<PPChainedCallbacks>(
        collectIncludeStructureCallback(*SourceMgr, &Includes),
        std::make_unique<CollectMainFileMacros>(*SourceMgr, &MainFileMacros));
  }

  CommentHandler *getCommentHandler() override {
    IWYUHandler = collectIWYUHeaderMaps(&CanonIncludes);
    return IWYUHandler.get();
  }

private:
  PathRef File;
  PreambleParsedCallback ParsedCallback;
  IncludeStructure Includes;
  CanonicalIncludes CanonIncludes;
  std::vector<std::string> MainFileMacros;
  std::unique_ptr<CommentHandler> IWYUHandler = nullptr;
  SourceManager *SourceMgr = nullptr;
};

} // namespace

PreambleData::PreambleData(PrecompiledPreamble Preamble,
                           std::vector<Diag> Diags, IncludeStructure Includes,
                           std::vector<std::string> MainFileMacros,
                           std::unique_ptr<PreambleFileStatusCache> StatCache,
                           CanonicalIncludes CanonIncludes)
    : Preamble(std::move(Preamble)), Diags(std::move(Diags)),
      Includes(std::move(Includes)), MainFileMacros(std::move(MainFileMacros)),
      StatCache(std::move(StatCache)), CanonIncludes(std::move(CanonIncludes)) {
}

std::shared_ptr<const PreambleData>
buildPreamble(PathRef FileName, CompilerInvocation &CI,
              std::shared_ptr<const PreambleData> OldPreamble,
              const tooling::CompileCommand &OldCompileCommand,
              const ParseInputs &Inputs, bool StoreInMemory,
              PreambleParsedCallback PreambleCallback) {
  // Note that we don't need to copy the input contents, preamble can live
  // without those.
  auto ContentsBuffer =
      llvm::MemoryBuffer::getMemBuffer(Inputs.Contents, FileName);
  auto Bounds =
      ComputePreambleBounds(*CI.getLangOpts(), ContentsBuffer.get(), 0);

  if (OldPreamble &&
      compileCommandsAreEqual(Inputs.CompileCommand, OldCompileCommand) &&
      OldPreamble->Preamble.CanReuse(CI, ContentsBuffer.get(), Bounds,
                                     Inputs.FS.get())) {
    vlog("Reusing preamble for file {0}", llvm::Twine(FileName));
    return OldPreamble;
  }
  vlog("Preamble for file {0} cannot be reused. Attempting to rebuild it.",
       FileName);

  trace::Span Tracer("BuildPreamble");
  SPAN_ATTACH(Tracer, "File", FileName);
  StoreDiags PreambleDiagnostics;
  llvm::IntrusiveRefCntPtr<DiagnosticsEngine> PreambleDiagsEngine =
      CompilerInstance::createDiagnostics(&CI.getDiagnosticOpts(),
                                          &PreambleDiagnostics, false);

  // Skip function bodies when building the preamble to speed up building
  // the preamble and make it smaller.
  assert(!CI.getFrontendOpts().SkipFunctionBodies);
  CI.getFrontendOpts().SkipFunctionBodies = true;
  // We don't want to write comment locations into PCH. They are racy and slow
  // to read back. We rely on dynamic index for the comments instead.
  CI.getPreprocessorOpts().WriteCommentListToPCH = false;

  CppFilePreambleCallbacks SerializedDeclsCollector(FileName, PreambleCallback);
  if (Inputs.FS->setCurrentWorkingDirectory(Inputs.CompileCommand.Directory)) {
    log("Couldn't set working directory when building the preamble.");
    // We proceed anyway, our lit-tests rely on results for non-existing working
    // dirs.
  }

  llvm::SmallString<32> AbsFileName(FileName);
  Inputs.FS->makeAbsolute(AbsFileName);
  auto StatCache = std::make_unique<PreambleFileStatusCache>(AbsFileName);
  auto BuiltPreamble = PrecompiledPreamble::Build(
      CI, ContentsBuffer.get(), Bounds, *PreambleDiagsEngine,
      StatCache->getProducingFS(Inputs.FS),
      std::make_shared<PCHContainerOperations>(), StoreInMemory,
      SerializedDeclsCollector);

  // When building the AST for the main file, we do want the function
  // bodies.
  CI.getFrontendOpts().SkipFunctionBodies = false;

  if (BuiltPreamble) {
    vlog("Built preamble of size {0} for file {1}", BuiltPreamble->getSize(),
         FileName);
    std::vector<Diag> Diags = PreambleDiagnostics.take();
    return std::make_shared<PreambleData>(
        std::move(*BuiltPreamble), std::move(Diags),
        SerializedDeclsCollector.takeIncludes(),
        SerializedDeclsCollector.takeMainFileMacros(), std::move(StatCache),
        SerializedDeclsCollector.takeCanonicalIncludes());
  } else {
    elog("Could not build a preamble for file {0}", FileName);
    return nullptr;
  }
}

} // namespace clangd
} // namespace clang
