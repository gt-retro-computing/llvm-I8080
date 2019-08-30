#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetLowering.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

static unsigned materializeOffset(MachineFunction &MF, MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI, unsigned Offset) {
	const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
	DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() :
	DebugLoc();
	const uint64_t MaxSubImm = 0xfff;
	if (Offset <= MaxSubImm) {
		return 0;
	}
	else {
		unsigned OffsetReg = TOY::R2;
		unsigned OffsetLo = (unsigned)(Offset & 0xffff);
		unsigned OffsetHi = (unsigned)((Offset & 0xffff0000) >> 16);
		BuildMI(MBB, MBBI, dl, TII.get(TOY::MOVLOi16), OffsetReg)
		.addImm(OffsetLo)
		.setMIFlag(MachineInstr::FrameSetup);
		if (OffsetHi) {
			BuildMI(MBB, MBBI, dl, TII.get(TOY::MOVHIi16), OffsetReg)
			.addReg(OffsetReg)
			.addImm(OffsetHi)
			.setMIFlag(MachineInstr::FrameSetup);
		}
	return OffsetReg;
	}
}


uint64_t LC2200FrameLowering::computeStackSize(MachineFunction &MF) const {
	MachineFrameInfo *MFI = MF.getFrameInfo();
	uint64_t StackSize = MFI->getStackSize();
	unsigned StackAlign = getStackAlignment();
	if (StackAlign > 0) {
		StackSize = RoundUpToAlignment(StackSize, StackAlign);
	}
	return StackSize;
}


void LC2200FrameLowering::emitPrologue(MachineFunction &MF) const {
	const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
	MachineBasicBlock &MBB = MF.front();
	MachineBasicBlock::iterator MBBI = MBB.begin();
	DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() :
	DebugLoc();
	uint64_t StackSize = computeStackSize(MF);
	if (!StackSize) {
		return;
	}
	unsigned StackReg = TOY::SP;
	unsigned OffsetReg = materializeOffset(MF, MBB, MBBI, (unsigned)StackSize);
	if (OffsetReg) {
		BuildMI(MBB, MBBI, dl, TII.get(TOY::SUBrr), StackReg)
		.addReg(StackReg)
		.addReg(OffsetReg)
		.setMIFlag(MachineInstr::FrameSetup);
	} 
	else {
		BuildMI(MBB, MBBI, dl, TII.get(TOY::SUBri), StackReg)
		.addReg(StackReg)
		.addImm(StackSize)
		.setMIFlag(MachineInstr::FrameSetup);
	}
}


void LC2200FrameLowering::emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const {
	const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
	MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
	DebugLoc dl = MBBI->getDebugLoc();
	uint64_t StackSize = computeStackSize(MF);
	if (!StackSize) {
		return;
	}
	unsigned StackReg = TOY::SP;
	unsigned OffsetReg = materializeOffset(MF, MBB, MBBI, (unsigned)StackSize);
	if (OffsetReg) {
		BuildMI(MBB, MBBI, dl, TII.get(TOY::ADDrr), StackReg)
		.addReg(StackReg)
		.addReg(OffsetReg)
		.setMIFlag(MachineInstr::FrameSetup);
	}
	else {
		BuildMI(MBB, MBBI, dl, TII.get(TOY::ADDri), StackReg)
		.addReg(StackReg)
		.addImm(StackSize)
		.setMIFlag(MachineInstr::FrameSetup);
	}
}

