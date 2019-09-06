#ifndef LC2200SUBTARGET_H
#define LC2200SUBTARGET_H

#include "LC2200.h"

#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/Target/TargetMachine.h"
#include "LC2200FrameLowering.h"
#include "LC2200ISelLowering.h"
#include "LC2200InstrInfo.h"

#define GET_SUBTARGETINFO_HEADER
#include "LC2200GenSubtargetInfo.inc"

namespace llvm {

class LC2200Subtarget : public LC2200GenSubtargetInfo {
//virtual void anchor();
// private:
//	const DataLayout DL;	// Calculates type size & alignment.
  LC2200RegisterInfo RI;
	LC2200InstrInfo InstrInfo;
  LC2200TargetLowering TLInfo;
//	LC2200SelectionDAGInfo TSInfo;
//	LC2200FrameLowering FrameLowering;
//	InstrItineraryData InstrItins;
  LC2200FrameLowering FrameLowering;
  LC2200ABI::ABI TargetABI = LC2200ABI::ABI_Unknown;

public:
	LC2200Subtarget(const Triple &TT, const StringRef CPU, const StringRef FS, const LC2200TargetMachine &TM);

// 	const InstrItineraryData *getInstrItineraryData() const override {
//		return &InstrItins;
//	}
//
	const LC2200InstrInfo *getInstrInfo() const override { return &InstrInfo; }

	const LC2200RegisterInfo *getRegisterInfo() const override {
		return &RI;
	}

	const LC2200TargetLowering *getTargetLowering() const override {
		return &TLInfo;
	}

	const LC2200FrameLowering *getFrameLowering() const override {
		return &FrameLowering;
	}
//
//	const LC2200SelectionDAGInfo *getSelectionDAGInfo() const override {
//		return &TSInfo;
//	}
//
//	const DataLayout *getDataLayout() const override { return &DL; }
//
	void ParseSubtargetFeatures(StringRef CPU, StringRef FS);

  LC2200ABI::ABI getTargetABI() const { return TargetABI; }
};

}
//
#endif