//
// Created by codetector on 9/7/19.
//

#include <clang/Basic/TargetBuiltins.h>
#include "TL45.h"

using namespace clang;
using namespace clang::targets;

const Builtin::Info TL45TargetInfo::BuiltinInfo[] = {
#define BUILTIN(ID, TYPE, ATTRS)                                               \
  {#ID, TYPE, ATTRS, nullptr, ALL_LANGUAGES, nullptr},
#define LIBBUILTIN(ID, TYPE, ATTRS, HEADER)                                    \
  {#ID, TYPE, ATTRS, HEADER, ALL_LANGUAGES, nullptr},
#include "clang/Basic/BuiltinsTL45.def"
};

const char *const TL45TargetInfo::GCCRegNames[] = {
        "r0", "r1", "r2", "r3",
        "r4", "r5", "r6", "r7",
        "r8", "r9", "r10", "r11",
        "r12", "r13", "r14", "r15",
        NULL
};

ArrayRef<const char *> TL45TargetInfo::getGCCRegNames() const {
  return llvm::makeArrayRef(GCCRegNames);
}

const TargetInfo::GCCRegAlias TL45TargetInfo::GCCRegAliases[] = {
        {{}, "r0"},
        {{}, "r1"},
        {{}, "r2"},
        {{}, "r3"},
        {{}, "r4"},
        {{}, "r5"},
        {{}, "r6"},
        {{}, "r7"},
        {{}, "r8"},
        {{}, "r9"},
        {{}, "r10"},
        {{}, "r11"},
        {{}, "r12"},
        {{}, "r13"},
        {{}, "r14"},
        {{}, "r15"},
};

ArrayRef<TargetInfo::GCCRegAlias> TL45TargetInfo::getGCCRegAliases() const {
  return llvm::makeArrayRef(GCCRegAliases);
}

ArrayRef<Builtin::Info> TL45TargetInfo::getTargetBuiltins() const {
  return llvm::makeArrayRef(BuiltinInfo, clang::TL45::LastTSBuiltin -
                                         Builtin::FirstTSBuiltin);
}



void TL45TargetInfo::getTargetDefines(const clang::LangOptions &Opts, clang::MacroBuilder &Builder) const {
}


