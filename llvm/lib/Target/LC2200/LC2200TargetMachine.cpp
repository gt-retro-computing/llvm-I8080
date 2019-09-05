#include "LC2200TargetMachine.h"
#include "LC2200.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include <iostream>
#include "llvm/CodeGen/GlobalISel/InstructionSelect.h"



//class LC2200PassConfig : public TargetPassConfig {
// public:
//	virtual bool addInstSelector();
//};
//
//bool LC2200PassConfig::addInstSelector() {
//	addPass(createLC2200ISelDag(getLC2200TargetMachine()));
//	return false;
//}
//
using namespace llvm;
//

LC2200TargetMachine::LC2200TargetMachine(const llvm::Target &T, const llvm::Triple &TT, llvm::StringRef CPU,
                                   llvm::StringRef FS, const llvm::TargetOptions &Options,
                                   Optional<llvm::Reloc::Model> RM,
                                   Optional<llvm::CodeModel::Model> CM, llvm::CodeGenOpt::Level OL, bool JIT) :
        LLVMTargetMachine(T,
                "e-m:e-p:32:32:32-i32:32:32-a:0:16-n32",
                TT, CPU, FS, Options, RM.getValueOr(Reloc::Model::Static),
                CM.getValueOr(CodeModel::Model::Tiny), OL
        ), TLOF(std::make_unique<TargetLoweringObjectFileELF>()), DefaultSubtarget(TT, CPU, FS, *this) {
    std::cout << "Create Triple: "  << TT.str() << "\n";
    initAsmInfo();
}



namespace {
	class LC2200PassConfig : public TargetPassConfig {
	 public:
		LC2200PassConfig(LC2200TargetMachine &TM, PassManagerBase &PM)
		: TargetPassConfig(TM, PM) {}
		LC2200TargetMachine &getLC2200TargetMachine() const {
			return getTM<LC2200TargetMachine>();
		}
		bool addPreISel() override;
		bool addInstSelector() override;
		void addPreEmitPass() override;
    bool addGlobalInstructionSelect() override;
		};
} // namespace

TargetPassConfig
*LC2200TargetMachine::createPassConfig(PassManagerBase &PM) {
	return new LC2200PassConfig(*this, PM);
}



bool LC2200PassConfig::addPreISel() { return false; }

bool LC2200PassConfig::addInstSelector() {
	addPass(createLC2200ISelDag(getLC2200TargetMachine(), getOptLevel()));
	return false;
}

bool LC2200PassConfig::addGlobalInstructionSelect() {
  addPass(new InstructionSelect());
  return false;
}

void LC2200PassConfig::addPreEmitPass() { }

// Force static initialization.
extern "C" void LLVMInitializeLC2200Target() {
	RegisterTargetMachine<LC2200TargetMachine> X(TheLC2200Target);
}
//
//void LC2200TargetMachine::addAnalysisPasses(PassManagerBase &PM) {}
