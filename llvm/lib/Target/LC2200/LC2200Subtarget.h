#ifndef LC2200SUBTARGET_H
#define LC2200SUBTARGET_H

#include "LC2200.h"
//#include "LC2200FrameLowering.h"
//#include "LC2200ISelLowering.h"
//#include "LC2200InstrInfo.h"
//#include "LC2200SelectionDAGInfo.h"
#include "LC2200Subtarget.h"
#include "llvm/Target/TargetMachine.h"

#define GET_SUBTARGETINFO_HEADER
#include "LC2200GenSubtargetInfo.inc"

class LC2200Subtarget : public LC2200GenSubtargetInfo {
//virtual void anchor();
// private:
//	const DataLayout DL;	// Calculates type size & alignment.
//	LC2200InstrInfo InstrInfo;
//	LC2200TargetLowering TLInfo;
//	LC2200SelectionDAGInfo TSInfo;
//	LC2200FrameLowering FrameLowering;
//	InstrItineraryData InstrItins;
 public:
// 	LC2200Subtarget(const std::string &TT, const std::string &CPU, const std::string &FS);
//
// 	const InstrItineraryData *getInstrItineraryData() const override {
//		return &InstrItins;
//	}
//
//	const LC2200InstrInfo *getInstrInfo() const override { return &InstrInfo; }
//
//	const LC2200RegisterInfo *getRegisterInfo() const override {
//		return &InstrInfo.getRegisterInfo();
//	}
//
//	const LC2200TargetLowering *getTargetLowering() const override {
//		return &TLInfo;
//	}
//
//	const LC2200FrameLowering *getFrameLowering() const override {
//		return &FrameLowering;
//	}
//
//	const LC2200SelectionDAGInfo *getSelectionDAGInfo() const override {
//		return &TSInfo;
//	}
//
//	const DataLayout *getDataLayout() const override { return &DL; }
//
//	void ParseSubtargetFeatures(StringRef CPU, StringRef FS);
};
//
#endif