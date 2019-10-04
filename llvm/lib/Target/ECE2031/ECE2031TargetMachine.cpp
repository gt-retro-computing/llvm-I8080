#include "ECE2031TargetMachine.h"
#include "ECE2031.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include <iostream>
#include "llvm/CodeGen/GlobalISel/InstructionSelect.h"



//class ECE2031PassConfig : public TargetPassConfig {
// public:
//	virtual bool addInstSelector();
//};
//
//bool ECE2031PassConfig::addInstSelector() {
//	addPass(createECE2031ISelDag(getECE2031TargetMachine()));
//	return false;
//}
//
using namespace llvm;
//

ECE2031TargetMachine::ECE2031TargetMachine(const llvm::Target &T, const llvm::Triple &TT, llvm::StringRef CPU,
                                   llvm::StringRef FS, const llvm::TargetOptions &Options,
                                   Optional<llvm::Reloc::Model> RM,
                                   Optional<llvm::CodeModel::Model> CM, llvm::CodeGenOpt::Level OL, bool JIT) :
        LLVMTargetMachine(T,
                "e-m:e-b16-S16-p:10:16:16-i16:16:16-a:16:16-n16",
                TT, CPU, FS, Options, RM.getValueOr(Reloc::Model::Static),
                CM.getValueOr(CodeModel::Model::Tiny), OL
        ), TLOF(std::make_unique<TargetLoweringObjectFileELF>()), DefaultSubtarget(TT, CPU, FS, *this) {
    initAsmInfo();
}



namespace {
	class ECE2031PassConfig : public TargetPassConfig {
	 public:
		ECE2031PassConfig(ECE2031TargetMachine &TM, PassManagerBase &PM)
		: TargetPassConfig(TM, PM) {}
		ECE2031TargetMachine &getECE2031TargetMachine() const {
			return getTM<ECE2031TargetMachine>();
		}
		bool addPreISel() override;
		bool addInstSelector() override;
		void addPreEmitPass() override;
    bool addGlobalInstructionSelect() override;
		};
} // namespace

TargetPassConfig
*ECE2031TargetMachine::createPassConfig(PassManagerBase &PM) {
	return new ECE2031PassConfig(*this, PM);
}



bool ECE2031PassConfig::addPreISel() { return false; }

bool ECE2031PassConfig::addInstSelector() {
	addPass(createECE2031ISelDag(getECE2031TargetMachine(), getOptLevel()));
	return false;
}

bool ECE2031PassConfig::addGlobalInstructionSelect() {
  addPass(new InstructionSelect());
  return false;
}

void ECE2031PassConfig::addPreEmitPass() { }

// Force static initialization.
extern "C" void LLVMInitializeECE2031Target() {
	RegisterTargetMachine<ECE2031TargetMachine> X(TheECE2031Target);
}
//
//void ECE2031TargetMachine::addAnalysisPasses(PassManagerBase &PM) {}
