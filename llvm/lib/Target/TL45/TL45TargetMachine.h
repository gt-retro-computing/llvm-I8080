//
// Created by codetector on 9/1/19.
//

#ifndef LLVM_LIB_TARGET_TL45_TL45TARGETMACHINE_H
#define LLVM_LIB_TARGET_TL45_TL45TARGETMACHINE_H

#include "llvm/Target/TargetMachine.h"
#include "TL45Subtarget.h"

namespace llvm {
    class TL45TargetMachine : public LLVMTargetMachine {
      std::unique_ptr<TargetLoweringObjectFile> TLOF;
      TL45Subtarget DefaultSubtarget;

    public:
        TL45TargetMachine(const llvm::Target &T, const llvm::Triple &TT, llvm::StringRef CPU,
                            llvm::StringRef FS, const llvm::TargetOptions &Options,
                            Optional<llvm::Reloc::Model> RM,
                            Optional<llvm::CodeModel::Model> CM, llvm::CodeGenOpt::Level OL, bool JIT);

      // Pass Pipeline Configuration
      TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

      const TL45Subtarget *getSubtargetImpl(const Function &) const override {
        return &DefaultSubtarget;
      }

      TargetLoweringObjectFile *getObjFileLowering() const override {
        return TLOF.get();
      }

    };
}

#endif //LLVM_TL45TARGETMACHINE_H
