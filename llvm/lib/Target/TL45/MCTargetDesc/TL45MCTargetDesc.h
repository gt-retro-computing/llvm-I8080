#ifndef TL45MCTARGETDESC_H
#define TL45MCTARGETDESC_H

#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/DataTypes.h"

namespace llvm {
	class Target;
	class MCInstrInfo;
	class MCRegisterInfo;
	class MCSubtargetInfo;
	class MCContext;
	class MCCodeEmitter;
	class MCAsmInfo;
	class MCCodeGenInfo;
	class MCRegisterClass;
	class MCInstPrinter;
	class MCObjectWriter;
	class MCAsmBackend;
	class StringRef;
	class raw_ostream;
	extern Target TheTL45Target;
	MCCodeEmitter *createTL45MCCodeEmitter(const MCInstrInfo &MCII,
		const MCRegisterInfo &MRI, MCContext &Ctx);
	MCAsmBackend *createTL45AsmBackend(const Target &T, const MCSubtargetInfo &STI, const MCRegisterInfo &MRI, const MCTargetOptions &Options);
	std::unique_ptr<MCObjectTargetWriter> createTL45ELFObjectWriter(uint8_t OSABI);
} // End llvm namespace

#define GET_REGINFO_ENUM
#include "TL45GenRegisterInfo.inc"

#define GET_INSTRINFO_ENUM
#include "TL45GenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "TL45GenSubtargetInfo.inc"

#endif 