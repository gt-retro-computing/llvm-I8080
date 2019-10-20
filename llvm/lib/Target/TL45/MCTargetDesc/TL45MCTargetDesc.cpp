#include "TL45MCTargetDesc.h"
#include "TL45MCAsmInfo.h"
#include "InstPrinter/TL45InstPrinter.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "TL45GenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "TL45GenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "TL45GenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createTL45MCInstrInfo() {
	MCInstrInfo *X = new MCInstrInfo();
	InitTL45MCInstrInfo(X);
	return X;
}

static MCRegisterInfo *createTL45MCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitTL45MCRegisterInfo(X, 0);
  return X;
}

//static MCSubtargetInfo *createTL45MCSubtargetInfo(const Triple &TT, StringRef CPU,
//                                                    StringRef FS) {
//  return createTL45MCSubtargetInfoImpl(TT, CPU, FS);
//}

static MCAsmInfo *createTL45MCAsmInfo(const MCRegisterInfo &MRI, const Triple &TT) {
  MCAsmInfo *MAI = new TL45MCAsmInfo(TT);
  return MAI;
}

//static MCCodeGenInfo *createTL45MCCodeGenInfo(const Triple &TT, Reloc::Model RM,
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
createTL45MCInstPrinter(const Triple &T, unsigned SyntaxVariant,
                          const MCAsmInfo &MAI, const MCInstrInfo &MII,
                          const MCRegisterInfo &MRI) {
  return new TL45InstPrinter(MAI, MII, MRI);
}

static MCStreamer* createTL45ELFStreamer(const Triple &T, MCContext &Ctx,
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
extern "C" void LLVMInitializeTL45TargetMC() {

// Register the MC asm info.
  RegisterMCAsmInfoFn X(TheTL45Target, createTL45MCAsmInfo);

// Register the MC codegen info.
//  TargetRegistry::RegisterMCCodeGenInfo(TheTL45Target,
//                                        createTL45MCCodeGenInfo);

// Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(TheTL45Target,
createTL45MCInstrInfo);
// Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(TheTL45Target,
                                    createTL45MCRegisterInfo);
// Register the MC subtarget info.
TargetRegistry::RegisterMCSubtargetInfo(TheTL45Target,
createTL45MCSubtargetInfoImpl);

// Register the MCInstPrinter
TargetRegistry::RegisterMCInstPrinter(TheTL45Target,
                                        createTL45MCInstPrinter);
// Register the ASM Backend.
TargetRegistry::RegisterMCAsmBackend(TheTL45Target,
createTL45AsmBackend);

// Register the assembly streamer.
//TargetRegistry::RegisterAsmStreamer(TheTL45Target,
//createMCAsmStreamer);

// Register the object streamer.
TargetRegistry::RegisterELFStreamer(TheTL45Target, createTL45ELFStreamer);
// Register the MCCodeEmitter
TargetRegistry::RegisterMCCodeEmitter(TheTL45Target, createTL45MCCodeEmitter);
}