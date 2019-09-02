#include "LC2200MCTargetDesc.h"
#include "InstPrinter/LC2200InstPrinter.h"
#include "LC2200MCAsmInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
//#include "LC2200GenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
//#include "LC2200GenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
//#include "LC2200GenRegisterInfo.inc"

using namespace llvm;

//static MCInstrInfo *createLC2200MCInstrInfo() {
//	MCInstrInfo *X = new MCInstrInfo();
//	InitLC2200MCInstrInfo(X);
//	return X;
//}

//static MCRegisterInfo *createLC2200MCRegisterInfo(StringRef TT) {
//	MCRegisterInfo *X = new MCRegisterInfo();
//	InitLC2200MCRegisterInfo(X, LC2200::LR);
//	return X;
//}

//static MCSubtargetInfo *createLC2200MCSubtargetInfo(StringRef TT, StringRef CPU,
//	StringRef FS) {
//	MCSubtargetInfo *X = new MCSubtargetInfo();
//	InitLC2200MCSubtargetInfo(X, TT, CPU, FS);
//	return X;
//}

static MCAsmInfo *createLC2200MCAsmInfo(const MCRegisterInfo &MRI, StringRef TT) {
	MCAsmInfo *MAI = new LC2200MCAsmInfo(TT);
	return MAI;
}

//static MCCodeGenInfo *createLC2200MCCodeGenInfo(StringRef TT, Reloc::Model RM,
//	CodeModel::Model CM, CodeGenOpt::Level OL) {
//	MCCodeGenInfo *X = new MCCodeGenInfo();
//	if (RM == Reloc::Default) {
//		RM = Reloc::Static;
//	}
//	if (CM == CodeModel::Default) {
//		CM = CodeModel::Small;
//	}
//	if (CM != CodeModel::Small && CM != CodeModel::Large) {
//		report_fatal_error("Target only supports CodeModel Small or Large");
//	}
//	X->InitMCCodeGenInfo(RM, CM, OL);
//	return X;
//}

static MCInstPrinter *
createLC2200MCInstPrinter(const Target &T, unsigned SyntaxVariant, 
	const MCAsmInfo &MAI, const MCInstrInfo &MII,
	const MCRegisterInfo &MRI, const MCSubtargetInfo &STI) {
	return new LC2200InstPrinter(MAI, MII, MRI);
}

//static MCStreamer *
//createMCAsmStreamer(MCContext &Ctx, formatted_raw_ostream &OS,
//	bool isVerboseAsm, bool useDwarfDirectory, MCInstPrinter *InstPrint,
//	MCCodeEmitter *CE,MCAsmBackend *TAB, bool ShowInst) {
//	return createAsmStreamer(Ctx, OS, isVerboseAsm,
//		useDwarfDirectory, InstPrint, CE, TAB, ShowInst);
//}
//
//static MCStreamer *createMCStreamer(const Target &T,
//	StringRef TT, MCContext &Ctx, MCAsmBackend &MAB,
//	raw_ostream &OS, MCCodeEmitter *Emitter, const MCSubtargetInfo &STI,
//	bool RelaxAll, bool NoExecStack) {
//	return createELFStreamer(Ctx, MAB, OS, Emitter, false, NoExecStack);
//}

// Force static initialization.
//extern "C" void LLVMInitializeLC2200TargetMC() {

// Register the MC asm info.
//RegisterMCAsmInfoFn X(TheLC2200Target, createLC2200MCAsmInfo);
//
//// Register the MC codegen info.
//TargetRegistry::RegisterMCCodeGenInfo(TheLC2200Target,
//createLC2200MCCodeGenInfo);
//
//// Register the MC instruction info.
//TargetRegistry::RegisterMCInstrInfo(TheLC2200Target,
//createLC2200MCInstrInfo);
//// Register the MC register info.
//TargetRegistry::RegisterMCRegInfo(TheLC2200Target,
//createLC2200MCRegisterInfo);
//// Register the MC subtarget info.
//TargetRegistry::RegisterMCSubtargetInfo(TheLC2200Target,
//createLC2200MCSubtargetInfo);
//// Register the MCInstPrinter
//TargetRegistry::RegisterMCInstPrinter(TheLC2200Target,
//createLC2200MCInstPrinter);
//// Register the ASM Backend.
//TargetRegistry::RegisterMCAsmBackend(TheLC2200Target,
//createLC2200AsmBackend);
//// Register the assembly streamer.
//TargetRegistry::RegisterAsmStreamer(TheLC2200Target,
//createMCAsmStreamer);
//// Register the object streamer.
//TargetRegistry::RegisterMCObjectStreamer(TheLC2200Target,
//createMCStreamer);
//// Register the MCCodeEmitter
//TargetRegistry::RegisterMCCodeEmitter(TheLC2200Target,
//createLC2200MCCodeEmitter);
//}