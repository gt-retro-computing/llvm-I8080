#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetOptions.h"

#include "ECE2031FrameLowering.h"
#include "ECE2031Subtarget.h"

using namespace llvm;
//
//static unsigned materializeOffset(MachineFunction &MF, MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI, unsigned Offset) {
//	const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
//	DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() :
//	DebugLoc();
//	const uint64_t MaxSubImm = 0xfff;
//	if (Offset <= MaxSubImm) {
//		return 0;
//	}
//	else {
//		unsigned OffsetReg = TOY::R2;
//		unsigned OffsetLo = (unsigned)(Offset & 0xffff);
//		unsigned OffsetHi = (unsigned)((Offset & 0xffff0000) >> 16);
//		BuildMI(MBB, MBBI, dl, TII.get(TOY::MOVLOi16), OffsetReg)
//		.addImm(OffsetLo)
//		.setMIFlag(MachineInstr::FrameSetup);
//		if (OffsetHi) {
//			BuildMI(MBB, MBBI, dl, TII.get(TOY::MOVHIi16), OffsetReg)
//			.addReg(OffsetReg)
//			.addImm(OffsetHi)
//			.setMIFlag(MachineInstr::FrameSetup);
//		}
//	return OffsetReg;
//	}
//}
//
//
//uint64_t ECE2031FrameLowering::computeStackSize(MachineFunction &MF) const {
//	MachineFrameInfo *MFI = MF.getFrameInfo();
//	uint64_t StackSize = MFI->getStackSize();
//	unsigned StackAlign = getStackAlignment();
//	if (StackAlign > 0) {
//		StackSize = RoundUpToAlignment(StackSize, StackAlign);
//	}
//	return StackSize;
//}

void ECE2031FrameLowering::determineFrameLayout(MachineFunction &MF) const {
  MachineFrameInfo &MFI = MF.getFrameInfo();
  const ECE2031RegisterInfo *RI = STI.getRegisterInfo();

  // Get the number of bytes to allocate from the FrameInfo.
  uint64_t FrameSize = MFI.getStackSize();

  // Get the alignment.
  unsigned StackAlign = getStackAlignment();
  if (RI->needsStackRealignment(MF)) {
    unsigned MaxStackAlign = std::max(StackAlign, MFI.getMaxAlignment());
    FrameSize += (MaxStackAlign - StackAlign);
    StackAlign = MaxStackAlign;
  }

  // Set Max Call Frame Size
  uint64_t MaxCallSize = alignTo(MFI.getMaxCallFrameSize(), StackAlign);
  MFI.setMaxCallFrameSize(MaxCallSize);

  // Make sure the frame is aligned.
  FrameSize = alignTo(FrameSize, StackAlign);

  // Update frame info.
  MFI.setStackSize(FrameSize);
}

void ECE2031FrameLowering::adjustReg(MachineBasicBlock &MBB,
                                    MachineBasicBlock::iterator MBBI,
                                    const DebugLoc &DL, Register DestReg,
                                    Register SrcReg, int64_t Val,
                                    MachineInstr::MIFlag Flag) const {
  MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
  const ECE2031InstrInfo *TII = STI.getInstrInfo();

  if (DestReg == SrcReg && Val == 0)
    return;

  if (isInt<20>(Val)) {
    BuildMI(MBB, MBBI, DL, TII->get(ECE2031::ADDI), DestReg)
            .addReg(SrcReg)
            .addImm(Val)
            .setMIFlag(Flag);
  } else if (isInt<32>(Val)) {
    llvm_unreachable("lol can't do yet");
  } else {
    report_fatal_error("adjustReg cannot yet handle adjustments >32 bits");
  }
}

void ECE2031FrameLowering::emitPrologue(MachineFunction &MF,
                                       MachineBasicBlock &MBB) const {
  assert(&MF.front() == &MBB && "Shrink-wrapping not yet supported");

  bool hasFramePointer = hasFP(MF);

  MachineFrameInfo &MFI = MF.getFrameInfo();
  // auto *RVFI = MF.getInfo<ECE2031MachineFunctionInfo>();
  const ECE2031RegisterInfo *RI = STI.getRegisterInfo();
  const ECE2031InstrInfo *TII = STI.getInstrInfo();
  MachineBasicBlock::iterator MBBI = MBB.begin();

  if (RI->needsStackRealignment(MF) && MFI.hasVarSizedObjects()) {
    report_fatal_error(
            "RISC-V backend can't currently handle functions that need stack "
            "realignment and have variable sized objects");
  }

  Register FPReg = ECE2031::fp;
  Register SPReg = ECE2031::sp;

  // Debug location must be unknown since the first debug location is used
  // to determine the end of the prologue.
  DebugLoc DL;

  // Determine the correct frame layout
  determineFrameLayout(MF);

  // FIXME (note copied from Lanai): This appears to be overallocating.  Needs
  // investigation. Get the number of bytes to allocate from the FrameInfo.
  uint64_t StackSize = MFI.getStackSize();

  if (hasFramePointer) StackSize++;

  // Early exit if there is no need to allocate on the stack
  if (StackSize == 0 && !MFI.adjustsStack())
    return;

  // Allocate space on the stack if necessary.
  adjustReg(MBB, MBBI, DL, SPReg, SPReg, -StackSize, MachineInstr::FrameSetup);

  // Emit ".cfi_def_cfa_offset StackSize"
  unsigned CFIIndex = MF.addFrameInst(
          MCCFIInstruction::createDefCfaOffset(nullptr, -StackSize));
  BuildMI(MBB, MBBI, DL, TII->get(TargetOpcode::CFI_INSTRUCTION))
          .addCFIIndex(CFIIndex);

  // The frame pointer is callee-saved, and code has been generated for us to
  // save it to the stack. We need to skip over the storing of callee-saved
  // registers as the frame pointer must be modified after it has been saved
  // to the stack, not before.

  if (hasFramePointer) {
    BuildMI(MBB, MBBI, DL, TII->get(ECE2031::SW)).addReg(ECE2031::fp).addReg(ECE2031::sp).addImm(StackSize - 1);
  }

  // FIXME: assumes exactly one instruction is used to save each callee-saved
  // register.
  const std::vector<CalleeSavedInfo> &CSI = MFI.getCalleeSavedInfo();
  std::advance(MBBI, CSI.size());

  // Iterate over list of callee-saved registers and emit .cfi_offset
  // directives.
  for (const auto &Entry : CSI) {
    int64_t Offset = MFI.getObjectOffset(Entry.getFrameIdx());
    Register Reg = Entry.getReg();
    unsigned CFIIndex = MF.addFrameInst(MCCFIInstruction::createOffset(
            nullptr, RI->getDwarfRegNum(Reg, true), Offset));
    BuildMI(MBB, MBBI, DL, TII->get(TargetOpcode::CFI_INSTRUCTION))
            .addCFIIndex(CFIIndex);
  }

  // Generate new FP.
  if (hasFramePointer) {
    BuildMI(MBB, MBBI, DL, TII->get(ECE2031::ADD)).addReg(ECE2031::fp).addReg(ECE2031::sp).addReg(ECE2031::zero);
    // Realign Stack
    const ECE2031RegisterInfo *RI = STI.getRegisterInfo();
    if (RI->needsStackRealignment(MF)) {
      llvm_unreachable("cannot realign");
//      unsigned MaxAlignment = MFI.getMaxAlignment();
//
//      const ECE2031InstrInfo *TII = STI.getInstrInfo();
//      if (isInt<12>(-(int)MaxAlignment)) {
//        BuildMI(MBB, MBBI, DL, TII->get(RISCV::ANDI), SPReg)
//                .addReg(SPReg)
//                .addImm(-(int)MaxAlignment);
//      } else {
//        unsigned ShiftAmount = countTrailingZeros(MaxAlignment);
//        Register VR =
//                MF.getRegInfo().createVirtualRegister(&RISCV::GPRRegClass);
//        BuildMI(MBB, MBBI, DL, TII->get(RISCV::SRLI), VR)
//                .addReg(SPReg)
//                .addImm(ShiftAmount);
//        BuildMI(MBB, MBBI, DL, TII->get(RISCV::SLLI), SPReg)
//                .addReg(VR)
//                .addImm(ShiftAmount);
//      }
    }
  }
}

void ECE2031FrameLowering::emitEpilogue(MachineFunction &MF,
                                       MachineBasicBlock &MBB) const {
  bool hasFramePointer = hasFP(MF);

  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  const ECE2031RegisterInfo *RI = STI.getRegisterInfo();
  MachineFrameInfo &MFI = MF.getFrameInfo();
//  auto *RVFI = MF.getInfo<RISCVMachineFunctionInfo>();
  DebugLoc DL = MBBI->getDebugLoc();
  const ECE2031InstrInfo *TII = STI.getInstrInfo();
  Register FPReg = ECE2031::fp;
  Register SPReg = ECE2031::sp;

  // Skip to before the restores of callee-saved registers
  // FIXME: assumes exactly one instruction is used to restore each
  // callee-saved register.
  auto LastFrameDestroy = std::prev(MBBI, MFI.getCalleeSavedInfo().size());

  uint64_t StackSize = MFI.getStackSize();
  if (hasFramePointer) StackSize++;

  // Restore the stack pointer using the value of the frame pointer. Only
  // necessary if the stack pointer was modified, meaning the stack size is
  // unknown.
  if (RI->needsStackRealignment(MF) || MFI.hasVarSizedObjects()) {
    assert(hasFP(MF) && "frame pointer should not have been eliminated");
    BuildMI(MBB, LastFrameDestroy, DL, TII->get(ECE2031::ADD)).addReg(ECE2031::sp).addReg(ECE2031::fp)
            .addReg(ECE2031::zero);
  }

  // Add CFI directives for callee-saved registers.
  const std::vector<CalleeSavedInfo> &CSI = MFI.getCalleeSavedInfo();
  // Iterate over list of callee-saved registers and emit .cfi_restore
  // directives.
  for (const auto &Entry : CSI) {
    Register Reg = Entry.getReg();
    unsigned CFIIndex = MF.addFrameInst(MCCFIInstruction::createRestore(
            nullptr, RI->getDwarfRegNum(Reg, true)));
    BuildMI(MBB, MBBI, DL, TII->get(TargetOpcode::CFI_INSTRUCTION))
            .addCFIIndex(CFIIndex);
  }

  // Restore FramePointer if it was setup.
  if (hasFramePointer) {
    // To find the instruction restoring FP from stack.
    BuildMI(MBB, MBBI, DL, TII->get(ECE2031::LW)).addReg(ECE2031::fp).addReg(ECE2031::fp)
            .addImm(StackSize - 1);
  }

  // Deallocate stack (aka restore SP)
  adjustReg(MBB, MBBI, DL, SPReg, SPReg, StackSize, MachineInstr::FrameDestroy);

  // After restoring $sp, we need to adjust CFA to $(sp + 0)
  // Emit ".cfi_def_cfa_offset 0"
  unsigned CFIIndex =
          MF.addFrameInst(MCCFIInstruction::createDefCfaOffset(nullptr, 0));
  BuildMI(MBB, MBBI, DL, TII->get(TargetOpcode::CFI_INSTRUCTION))
          .addCFIIndex(CFIIndex);
}

bool ECE2031FrameLowering::hasFP(const MachineFunction &MF) const {
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  const TargetRegisterInfo *TRI = STI.getRegisterInfo();

  return MF.getTarget().Options.DisableFramePointerElim(MF) ||
         MFI.hasVarSizedObjects() || MFI.isFrameAddressTaken() ||
         TRI->needsStackRealignment(MF);
}

int ECE2031FrameLowering::getFrameIndexReference(const MachineFunction &MF, int FI, unsigned &FrameReg) const {
  int offset = TargetFrameLowering::getFrameIndexReference(MF, FI, FrameReg);
  assert(offset % 4 == 0 && "4-byte addressibility");
  return offset / 4;
}
