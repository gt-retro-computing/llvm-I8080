//
// Created by codetector on 9/1/19.
//

#ifndef LLVM_LIB_TARGET_ECE2031_ECE2031TARGETMACHINE_H
#define LLVM_LIB_TARGET_ECE2031_ECE2031TARGETMACHINE_H

#include "llvm/Target/TargetMachine.h"
#include "ECE2031Subtarget.h"

namespace llvm {
    class ECE2031TargetMachine : public LLVMTargetMachine {
      std::unique_ptr<TargetLoweringObjectFile> TLOF;
      ECE2031Subtarget DefaultSubtarget;

    public:
        ECE2031TargetMachine(const llvm::Target &T, const llvm::Triple &TT, llvm::StringRef CPU,
                            llvm::StringRef FS, const llvm::TargetOptions &Options,
                            Optional<llvm::Reloc::Model> RM,
                            Optional<llvm::CodeModel::Model> CM, llvm::CodeGenOpt::Level OL, bool JIT);

      // Pass Pipeline Configuration
      TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

      const ECE2031Subtarget *getSubtargetImpl(const Function &) const override {
        return &DefaultSubtarget;
      }

      TargetLoweringObjectFile *getObjFileLowering() const override {
        return TLOF.get();
      }

    };
}

#endif //LLVM_ECE2031TARGETMACHINE_H
