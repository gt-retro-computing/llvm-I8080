#include "TL45TargetMachine.h"
#include "TL45.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include <iostream>
#include "llvm/CodeGen/GlobalISel/InstructionSelect.h"



//class TL45PassConfig : public TargetPassConfig {
// public:
//	virtual bool addInstSelector();
//};
//
//bool TL45PassConfig::addInstSelector() {
//	addPass(createTL45ISelDag(getTL45TargetMachine()));
//	return false;
//}
//
using namespace llvm;
//

TL45TargetMachine::TL45TargetMachine(const llvm::Target &T, const llvm::Triple &TT, llvm::StringRef CPU,
                                   llvm::StringRef FS, const llvm::TargetOptions &Options,
                                   Optional<llvm::Reloc::Model> RM,
                                   Optional<llvm::CodeModel::Model> CM, llvm::CodeGenOpt::Level OL, bool JIT) :
        LLVMTargetMachine(T,
                "e-m:e-b32-S32-p:32:32:32-i32:32:32-a:32:32-n32",
                TT, CPU, FS, Options, RM.getValueOr(Reloc::Model::Static),
                CM.getValueOr(CodeModel::Model::Tiny), OL
        ), TLOF(std::make_unique<TargetLoweringObjectFileELF>()), DefaultSubtarget(TT, CPU, FS, *this) {
    initAsmInfo();
}



namespace {
	class TL45PassConfig : public TargetPassConfig {
	 public:
		TL45PassConfig(TL45TargetMachine &TM, PassManagerBase &PM)
		: TargetPassConfig(TM, PM) {}
		TL45TargetMachine &getTL45TargetMachine() const {
			return getTM<TL45TargetMachine>();
		}
		bool addPreISel() override;
		bool addInstSelector() override;
		void addPreEmitPass() override;
    bool addGlobalInstructionSelect() override;
		};
} // namespace

TargetPassConfig
*TL45TargetMachine::createPassConfig(PassManagerBase &PM) {
	return new TL45PassConfig(*this, PM);
}



bool TL45PassConfig::addPreISel() { return false; }

bool TL45PassConfig::addInstSelector() {
	addPass(createTL45ISelDag(getTL45TargetMachine(), getOptLevel()));
	return false;
}

bool TL45PassConfig::addGlobalInstructionSelect() {
  addPass(new InstructionSelect());
  return false;
}

void TL45PassConfig::addPreEmitPass() { }

// Force static initialization.
extern "C" void LLVMInitializeTL45Target() {
	RegisterTargetMachine<TL45TargetMachine> X(TheTL45Target);
}
//
//void TL45TargetMachine::addAnalysisPasses(PassManagerBase &PM) {}
