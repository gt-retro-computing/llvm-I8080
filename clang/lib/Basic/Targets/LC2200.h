//
// Created by codetector on 9/7/19.
//

#ifndef LLVM_CLANG_LIB_BASIC_TARGETS_LC2200_H
#define LLVM_CLANG_LIB_BASIC_TARGETS_LC2200_H

#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"

namespace clang {
namespace targets {

class LLVM_LIBRARY_VISIBILITY LC2200TargetInfo : public TargetInfo {
  static const TargetInfo::GCCRegAlias GCCRegAliases[];
  static const char *const GCCRegNames[];
public:
  LC2200TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts) : TargetInfo(Triple) {
    resetDataLayout("e" // Little Endian
                    "-m:e"
                    "-b32" // Byte Size ??? Like mim accessibility
                    "-S32" // Stack Align 32bits
                    "-p:32:32:32" // Pointer Type 32bits, 32bits aligned, (Not sure what that last is for)
                    "-i32:32:32"
                    "-a:32:32" // Address 32bits 32bits aligned
                    "-n32" // Native Int 32
    );
  }
  ArrayRef<const char *> getGCCRegNames() const override;

  ArrayRef<GCCRegAlias> getGCCRegAliases() const override;

  void getTargetDefines(const LangOptions &Opts,
          MacroBuilder &Builder) const override;

  BuiltinVaListKind getBuiltinVaListKind() const override {
    return TargetInfo::VoidPtrBuiltinVaList;
  }

  ArrayRef<Builtin::Info> getTargetBuiltins() const override { return None; }

  bool validateAsmConstraint(const char *&Name,
          TargetInfo::ConstraintInfo &info) const override {
    return false;
  }

  const char *getClobbers() const override { return ""; }

};
}
}

#endif //LLVM_CLANG_LIB_BASIC_TARGETS_LC2200_H
