//
// Created by codetector on 9/1/19.
//

#ifndef LLVM_LIB_TARGET_LC2200_LC2200TARGETMACHINE_H
#define LLVM_LIB_TARGET_LC2200_LC2200TARGETMACHINE_H

#include "llvm/Target/TargetMachine.h"

namespace llvm {
    class LC2200TargetMachine : public TargetMachine {
    public:
        LC2200TargetMachine(const llvm::Target &T, const llvm::Triple &TT, llvm::StringRef CPU,
                            llvm::StringRef FS, const llvm::TargetOptions &Options,
                            Optional<llvm::Reloc::Model> RM,
                            Optional<llvm::CodeModel::Model> CM, llvm::CodeGenOpt::Level OL, bool JIT);
    };
}

#endif //LLVM_LC2200TARGETMACHINE_H
