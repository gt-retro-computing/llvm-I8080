#include "LC2200TargetMachine.h"
#include "LC2200.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"


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
//using namespace llvm;
//
//LC2200TargetMachine::LC2200TargetMachine(const Target &T, StringRef TT,
//	StringRef CPU, StringRef FS, const TargetOptions &Options,
//	Reloc::Model RM, CodeModel::Model CM,
//	CodeGenOpt::Level OL)
//	: LLVMTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL),
//	Subtarget(TT, CPU, FS, *this) {
//		initAsmInfo();
//	}
//
//namespace {
//	class LC2200PassConfig : public TargetPassConfig {
//	 public:
//		LC2200PassConfig(LC2200TargetMachine *TM, PassManagerBase &PM)
//		: TargetPassConfig(TM, PM) {}
//		LC2200TargetMachine &getLC2200TargetMachine() const {
//			return getTM<LC2200TargetMachine>();
//		}
//		virtual bool addPreISel();
//		virtual bool addInstSelector();
//		virtual bool addPreEmitPass();
//		};
//} // namespace
//
//TargetPassConfig
//*LC2200TargetMachine::createPassConfig(PassManagerBase &PM) {
//	return new LC2200PassConfig(this, PM);
//}
//
//bool LC2200PassConfig::addPreISel() { return false; }
//
//bool LC2200PassConfig::addInstSelector() {
//	addPass(createLC2200ISelDag(getLC2200TargetMachine(),
//	getOptLevel()));
//	return false;
//}
//
//bool LC2200PassConfig::addPreEmitPass() { return false; }
//
//// Force static initialization.
//extern "C" void LLVMInitializeLC2200Target() {
//	RegisterTargetMachine<LC2200TargetMachine> X(TheLC2200Target);
//}
//
//void LC2200TargetMachine::addAnalysisPasses(PassManagerBase &PM) {}
