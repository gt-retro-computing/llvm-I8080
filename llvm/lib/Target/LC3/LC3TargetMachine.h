//
// Created by codetector on 5/5/19.
//

#ifndef LLVM_LC3TARGETMACHINE_H
#define LLVM_LC3TARGETMACHINE_H

#include "llvm/Target/TargetMachine.h"

namespace llvm {
    class LC3TargetMachine : public TargetMachine {

    public:
        LC3TargetMachine(const llvm::Target &t, llvm::StringRef dataLayoutString,
                         const llvm::Triple &targetTriple, llvm::StringRef cpu, llvm::StringRef fs,
                         const llvm::TargetOptions &options, const llvm::Target &T,
                         const llvm::Triple &TT, llvm::StringRef CPU, llvm::StringRef FS,
                         const llvm::TargetOptions &Options, llvm::Reloc::Model RM,
                         llvm::CodeModel::Model CM, llvm::CodeGenOpt::Level OL);
    };
}

#endif //LLVM_LC3TARGETMACHINE_H
