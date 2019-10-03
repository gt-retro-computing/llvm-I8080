#ifndef ECE2031MCTARGETDESC_H
#define ECE2031MCTARGETDESC_H

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
	extern Target TheECE2031Target;
	MCCodeEmitter *createECE2031MCCodeEmitter(const MCInstrInfo &MCII,
		const MCRegisterInfo &MRI, MCContext &Ctx);
	MCAsmBackend *createECE2031AsmBackend(const Target &T, const MCSubtargetInfo &STI, const MCRegisterInfo &MRI, const MCTargetOptions &Options);
	std::unique_ptr<MCObjectTargetWriter> createECE2031ELFObjectWriter(uint8_t OSABI);
} // End llvm namespace

#define GET_REGINFO_ENUM
#include "ECE2031GenRegisterInfo.inc"

#define GET_INSTRINFO_ENUM
#include "ECE2031GenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "ECE2031GenSubtargetInfo.inc"

#endif 