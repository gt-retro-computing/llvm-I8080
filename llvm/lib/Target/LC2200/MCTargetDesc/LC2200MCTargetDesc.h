#ifndef LC2200MCTARGETDESC_H
#define LC2200MCTARGETDESC_H

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
	extern Target TheLC2200Target;
	MCCodeEmitter *createLC2200MCCodeEmitter(const MCInstrInfo &MCII,
		const MCRegisterInfo &MRI, MCContext &Ctx);
	MCAsmBackend *createLC2200AsmBackend(const Target &T, const MCSubtargetInfo &STI, const MCRegisterInfo &MRI, const MCTargetOptions &Options);
	std::unique_ptr<MCObjectTargetWriter> createLC2200ELFObjectWriter(uint8_t OSABI);
} // End llvm namespace

#define GET_REGINFO_ENUM
#include "LC2200GenRegisterInfo.inc"

#define GET_INSTRINFO_ENUM
#include "LC2200GenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "LC2200GenSubtargetInfo.inc"

#endif 