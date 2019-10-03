#include "ECE2031MCTargetDesc.h"
#include "ECE2031MCAsmInfo.h"
#include "InstPrinter/ECE2031InstPrinter.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "ECE2031GenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "ECE2031GenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "ECE2031GenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createECE2031MCInstrInfo() {
	MCInstrInfo *X = new MCInstrInfo();
	InitECE2031MCInstrInfo(X);
	return X;
}

static MCRegisterInfo *createECE2031MCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitECE2031MCRegisterInfo(X, 0);
  return X;
}

//static MCSubtargetInfo *createECE2031MCSubtargetInfo(const Triple &TT, StringRef CPU,
//                                                    StringRef FS) {
//  return createECE2031MCSubtargetInfoImpl(TT, CPU, FS);
//}

static MCAsmInfo *createECE2031MCAsmInfo(const MCRegisterInfo &MRI, const Triple &TT) {
  MCAsmInfo *MAI = new ECE2031MCAsmInfo(TT);
  return MAI;
}

//static MCCodeGenInfo *createECE2031MCCodeGenInfo(const Triple &TT, Reloc::Model RM,
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
createECE2031MCInstPrinter(const Triple &T, unsigned SyntaxVariant,
                          const MCAsmInfo &MAI, const MCInstrInfo &MII,
                          const MCRegisterInfo &MRI) {
  return new ECE2031InstPrinter(MAI, MII, MRI);
}

static MCStreamer* createECE2031ELFStreamer(const Triple &T, MCContext &Ctx,
        std::unique_ptr<MCAsmBackend> &&MAB,
        std::unique_ptr<MCObjectWriter> &&OW,
        std::unique_ptr<MCCodeEmitter> &&Emitter, bool RelaxAll) {
    return createELFStreamer(Ctx, std::move(MAB), std::move(OW), std::move(Emitter), RelaxAll);
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
extern "C" void LLVMInitializeECE2031TargetMC() {

// Register the MC asm info.
  RegisterMCAsmInfoFn X(TheECE2031Target, createECE2031MCAsmInfo);

// Register the MC codegen info.
//  TargetRegistry::RegisterMCCodeGenInfo(TheECE2031Target,
//                                        createECE2031MCCodeGenInfo);

// Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(TheECE2031Target,
createECE2031MCInstrInfo);
// Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(TheECE2031Target,
                                    createECE2031MCRegisterInfo);
// Register the MC subtarget info.
TargetRegistry::RegisterMCSubtargetInfo(TheECE2031Target,
createECE2031MCSubtargetInfoImpl);

// Register the MCInstPrinter
TargetRegistry::RegisterMCInstPrinter(TheECE2031Target,
                                        createECE2031MCInstPrinter);
// Register the ASM Backend.
TargetRegistry::RegisterMCAsmBackend(TheECE2031Target,
createECE2031AsmBackend);

// Register the assembly streamer.
//TargetRegistry::RegisterAsmStreamer(TheECE2031Target,
//createMCAsmStreamer);

// Register the object streamer.
TargetRegistry::RegisterELFStreamer(TheECE2031Target, createECE2031ELFStreamer);
// Register the MCCodeEmitter
TargetRegistry::RegisterMCCodeEmitter(TheECE2031Target, createECE2031MCCodeEmitter);
}