

#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/CodeGen/TargetFrameLowering.h"

#include "LC2200RegisterInfo.h"
#include "LC2200FrameLowering.h"
#include "LC2200Subtarget.h"
#include "MCTargetDesc/LC2200MCTargetDesc.h"

#define DEBUG_TYPE "lc2200-reg-info"

#define GET_REGINFO_TARGET_DESC
#include "LC2200GenRegisterInfo.inc"

using namespace llvm;

LC2200RegisterInfo::LC2200RegisterInfo() : LC2200GenRegisterInfo(LC2200::ra) {}

const uint32_t *LC2200RegisterInfo::getCallPreservedMask(const MachineFunction &MF, CallingConv::ID) const {
  return CC_Save_RegMask;
}

const MCPhysReg *LC2200RegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CC_Save_SaveList;
}

BitVector LC2200RegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  static const MCPhysReg ReservedGPR32[] = {
          LC2200::zero, LC2200::sp, LC2200::fp, LC2200::pc
  };

  BitVector Reserved(getNumRegs());

  for (unsigned short I : ReservedGPR32) {
    Reserved.set(I);
  }

  return Reserved;
}

void LC2200RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj, unsigned FIOperandNum,
                                             RegScavenger *RS) const {
  MachineInstr &MI = *II;
  MachineFunction &MF = *MI.getParent()->getParent();

  LLVM_DEBUG(errs() << "\nFunction : " << MF.getName() << "\n";
                     errs() << "<--------->\n"
                            << MI);

  int FrameIndex = MI.getOperand(FIOperandNum).getIndex();
  uint64_t StackSize = MF.getFrameInfo().getStackSize();
  int64_t SPOffset = MF.getFrameInfo().getObjectOffset(FrameIndex);

  LLVM_DEBUG(errs() << "FrameIndex : " << FrameIndex << "\n"
                    << "spOffset   : " << SPOffset << "\n"
                    << "stackSize  : " << StackSize << "\n"
                    << "alignment  : "
                    << MF.getFrameInfo().getObjectAlignment(FrameIndex)
                    << "\n");

  unsigned OpNo = FIOperandNum;

  MachineFrameInfo &MFI = MF.getFrameInfo();

  const std::vector<CalleeSavedInfo> &CSI = MFI.getCalleeSavedInfo();
  int MinCSFI = 0;
  int MaxCSFI = -1;

  if (CSI.size()) {
    MinCSFI = CSI[0].getFrameIdx();
    MaxCSFI = CSI[CSI.size() - 1].getFrameIdx();
  }

  // The following stack frame objects are always
  // referenced relative to $sp:
  //  1. Outgoing arguments.
  //  2. Pointer to dynamically allocated stack space.
  //  3. Locations for callee-saved registers.
  // Everything else is referenced relative to whatever register
  // getFrameRegister() returns.
  unsigned FrameReg;

  if (FrameIndex >= MinCSFI && FrameIndex <= MaxCSFI)
    FrameReg = LC2200::sp;
  else {
    const TargetFrameLowering *TFI = MF.getSubtarget().getFrameLowering();
    if (TFI->hasFP(MF)) {
      FrameReg = LC2200::fp;
    }
    else {
      if ((MI.getNumOperands()> OpNo+2) && MI.getOperand(OpNo+2).isReg())
        FrameReg = MI.getOperand(OpNo+2).getReg();
      else
        FrameReg = LC2200::sp;
    }
  }
  // Calculate final offset.
  // - There is no need to change the offset if the frame object
  //   is one of the
  //   following: an outgoing argument, pointer to a dynamically allocated
  //   stack space or a $gp restore location,
  // - If the frame object is any of the following,
  //   its offset must be adjusted
  //   by adding the size of the stack:
  //   incoming argument, callee-saved register location or local variable.
  int64_t Offset;
  bool IsKill = false;
  Offset = SPOffset + (int64_t)StackSize;
  Offset += MI.getOperand(OpNo + 1).getImm();

  LLVM_DEBUG(errs() << "Offset     : " << Offset << "\n"
                    << "<--------->\n");

  if (!MI.isDebugValue() &&
      !LC2200InstrInfo::validImmediate(MI.getOpcode(), FrameReg, Offset)) {
    MachineBasicBlock &MBB = *MI.getParent();
    DebugLoc DL = II->getDebugLoc();
    unsigned NewImm;
    const LC2200InstrInfo &TII = *static_cast<const LC2200InstrInfo *>(MF.getSubtarget().getInstrInfo());
    FrameReg = TII.loadImmediate(FrameReg, Offset, MBB, II, DL, NewImm);
    Offset = SignExtend64<16>(NewImm);
    IsKill = true;
  }
  MI.getOperand(OpNo).ChangeToRegister(FrameReg, false, false, IsKill);
  MI.getOperand(OpNo + 1).ChangeToImmediate(Offset);


}

Register LC2200RegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  const LC2200Subtarget &Subtarget = MF.getSubtarget<LC2200Subtarget>();
  const TargetFrameLowering *TFI = Subtarget.getFrameLowering();
  return TFI->hasFP(MF) ? LC2200::fp : LC2200::sp;
}
