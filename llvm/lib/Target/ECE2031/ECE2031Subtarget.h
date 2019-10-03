#ifndef ECE2031SUBTARGET_H
#define ECE2031SUBTARGET_H

#include "ECE2031.h"

#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/Target/TargetMachine.h"
#include "ECE2031FrameLowering.h"
#include "ECE2031ISelLowering.h"
#include "ECE2031InstrInfo.h"

#define GET_SUBTARGETINFO_HEADER
#include "ECE2031GenSubtargetInfo.inc"

namespace llvm {

class ECE2031Subtarget : public ECE2031GenSubtargetInfo {
//virtual void anchor();
// private:
//	const DataLayout DL;	// Calculates type size & alignment.
  ECE2031RegisterInfo RI;
	ECE2031InstrInfo InstrInfo;
  ECE2031TargetLowering TLInfo;
//	ECE2031SelectionDAGInfo TSInfo;
//	ECE2031FrameLowering FrameLowering;
//	InstrItineraryData InstrItins;
  ECE2031FrameLowering FrameLowering;
  ECE2031ABI::ABI TargetABI = ECE2031ABI::ABI_Unknown;

public:
	ECE2031Subtarget(const Triple &TT, const StringRef CPU, const StringRef FS, const ECE2031TargetMachine &TM);

// 	const InstrItineraryData *getInstrItineraryData() const override {
//		return &InstrItins;
//	}
//
	const ECE2031InstrInfo *getInstrInfo() const override { return &InstrInfo; }

	const ECE2031RegisterInfo *getRegisterInfo() const override {
		return &RI;
	}

	const ECE2031TargetLowering *getTargetLowering() const override {
		return &TLInfo;
	}

	const ECE2031FrameLowering *getFrameLowering() const override {
		return &FrameLowering;
	}
//
//	const ECE2031SelectionDAGInfo *getSelectionDAGInfo() const override {
//		return &TSInfo;
//	}
//
//	const DataLayout *getDataLayout() const override { return &DL; }
//
	void ParseSubtargetFeatures(StringRef CPU, StringRef FS);

  ECE2031ABI::ABI getTargetABI() const { return TargetABI; }
};

}
//
#endif