#ifndef LC2200MCTARGETDESC_H
#define LC2200MCTARGETDESC_H
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
	class MCInstPrinter;
	class MCObjectWriter;
	class MCAsmBackend;
	class StringRef;
	class raw_ostream;
	extern Target TheLC2200Target;
	MCCodeEmitter *createLC2200MCCodeEmitter(const MCInstrInfo &MCII,
		const MCRegisterInfo &MRI, const MCSubtargetInfo &STI, MCContext &Ctx);
	MCAsmBackend *createLC2200AsmBackend(const Target &T, const MCRegisterInfo &MRI,
		StringRef TT, StringRef CPU);
	MCObjectWriter *createLC2200ELFObjectWriter(raw_ostream &OS, uint8_t OSABI);
} // End llvm namespace

#define GET_REGINFO_TARGET_DESC
#include "LC2200GenRegisterInfo.inc"

#define GET_INSTRINFO_ENUM
#include "LC2200GenInstInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "LC2200GenSubtargetInfo.inc"

#endif 