//
// Created by codetector on 9/1/19.
//

#ifndef LLVM_LIB_TARGET_LC2200_LC2200TARGETMACHINE_H
#define LLVM_LIB_TARGET_LC2200_LC2200TARGETMACHINE_H

#include "llvm/Target/TargetMachine.h"
#include "LC2200Subtarget.h"

namespace llvm {
    class LC2200TargetMachine : public LLVMTargetMachine {
      std::unique_ptr<TargetLoweringObjectFile> TLOF;
      LC2200Subtarget DefaultSubtarget;

    public:
        LC2200TargetMachine(const llvm::Target &T, const llvm::Triple &TT, llvm::StringRef CPU,
                            llvm::StringRef FS, const llvm::TargetOptions &Options,
                            Optional<llvm::Reloc::Model> RM,
                            Optional<llvm::CodeModel::Model> CM, llvm::CodeGenOpt::Level OL, bool JIT);

      // Pass Pipeline Configuration
      TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

      const LC2200Subtarget *getSubtargetImpl(const Function &) const override {
        return &DefaultSubtarget;
      }

      TargetLoweringObjectFile *getObjFileLowering() const override {
        return TLOF.get();
      }

    };
}

#endif //LLVM_LC2200TARGETMACHINE_H
