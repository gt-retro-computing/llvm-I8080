//
// Created by codetector on 9/7/19.
//

#ifndef LLVM_CLANG_LIB_BASIC_TARGETS_TL45_H
#define LLVM_CLANG_LIB_BASIC_TARGETS_TL45_H

#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"

namespace clang {
namespace targets {

class LLVM_LIBRARY_VISIBILITY TL45TargetInfo : public TargetInfo {
  static const TargetInfo::GCCRegAlias GCCRegAliases[];
  static const char *const GCCRegNames[];
  static const Builtin::Info BuiltinInfo[];
public:
  TL45TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts) : TargetInfo(Triple) {
    resetDataLayout("e" // Little Endian
                    "-m:e"
                    "-b32" // Byte Size ??? Like mim accessibility
                    "-S32" // Stack Align 32bits
                    "-p:32:32:32" // Pointer Type 32bits, 32bits aligned, (Not sure what that last is for)
                    "-i32:32:32"
                    "-a:32:32" // Address 32bits 32bits aligned
                    "-n32" // Native Int 32
    );
    

    PointerWidth = PointerAlign = 32;
    BoolWidth = BoolAlign = 32;
    CharWidth = CharAlign = 32;
    ShortWidth = ShortAlign = 32;
    IntWidth = IntAlign = 32;
  }

  unsigned getCharWidth() const { return 8; } // FIXME
  unsigned getCharAlign() const { return 8; } // FIXME

  /// Return the size of 'signed short' and 'unsigned short' for this
  /// target, in bits.
  unsigned getShortWidth() const { return 16; } // FIXME

  /// Return the alignment of 'signed short' and 'unsigned short' for
  /// this target.
  unsigned getShortAlign() const { return 16; } // FIXME

  ArrayRef<const char *> getGCCRegNames() const override;

  ArrayRef<GCCRegAlias> getGCCRegAliases() const override;

  ArrayRef<Builtin::Info> getTargetBuiltins() const override;

  void getTargetDefines(const LangOptions &Opts,
                        MacroBuilder &Builder) const override;

  BuiltinVaListKind getBuiltinVaListKind() const override {
    return TargetInfo::VoidPtrBuiltinVaList;
  }

  bool validateAsmConstraint(const char *&Name,
                             TargetInfo::ConstraintInfo &info) const override {
    return false;
  }

  const char *getClobbers() const override { return ""; }

};
}
}

#endif //LLVM_CLANG_LIB_BASIC_TARGETS_TL45_H
