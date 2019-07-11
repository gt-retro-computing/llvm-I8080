//
// Created by codetector on 5/5/19.
//

#ifndef LLVM_LC3TARGETMACHINE_H
#define LLVM_LC3TARGETMACHINE_H

#include "llvm/Target/TargetMachine.h"

namespace llvm {
    class LC3TargetMachine : public TargetMachine {

    public:
        LC3TargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                StringRef FS, const TargetOptions &Options, Optional<Reloc::Model> RM,
                Optional<CodeModel::Model> CM, CodeGenOpt::Level OL, bool JIT);
    };
} // end namespace llvm

#endif //LLVM_LC3TARGETMACHINE_H
