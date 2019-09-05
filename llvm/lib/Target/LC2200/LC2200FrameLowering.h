//===-- LC2200FrameLowering.h - Define frame lowering for LC2200 ----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_LC2200_LC2200FRAMELOWERING_H
#define LLVM_LIB_TARGET_LC2200_LC2200FRAMELOWERING_H

#include "LC2200.h"
#include "llvm/CodeGen/TargetFrameLowering.h"

namespace llvm {
class LC2200Subtarget;

class LC2200FrameLowering : public TargetFrameLowering {
protected:
  const LC2200Subtarget &STI;

public:
  explicit LC2200FrameLowering(const LC2200Subtarget &sti, unsigned Alignment)
          : TargetFrameLowering(StackGrowsDown, Alignment, 0, Alignment), STI(sti) {}

  void determineFrameLayout(MachineFunction &MF) const;

  void adjustReg(MachineBasicBlock &MBB,
                 MachineBasicBlock::iterator MBBI,
                 const DebugLoc &DL, Register DestReg,
                 Register SrcReg, int64_t Val,
                 MachineInstr::MIFlag Flag) const;

  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

//  bool spillCalleeSavedRegisters(MachineBasicBlock &MBB,
//                                 MachineBasicBlock::iterator MI,
//                                 const std::vector<CalleeSavedInfo> &CSI,
//                                 const TargetRegisterInfo *TRI) const override;
//
//  bool restoreCalleeSavedRegisters(MachineBasicBlock &MBB,
//                                   MachineBasicBlock::iterator MI,
//                                   std::vector<CalleeSavedInfo> &CSI,
//                                   const TargetRegisterInfo *TRI) const override;

//  bool hasReservedCallFrame(const MachineFunction &MF) const override;
//
//  void determineCalleeSaves(MachineFunction &MF, BitVector &SavedRegs,
//                            RegScavenger *RS) const override;

  bool hasFP(const MachineFunction &MF) const override;

//  bool hasBP(const MachineFunction &MF) const;

  bool isFPCloseToIncomingSP() const override { return false; }

  bool enableShrinkWrapping(const MachineFunction &MF) const override {
    return true;
  }

//  MachineBasicBlock::iterator
//  eliminateCallFramePseudoInstr(MachineFunction &MF,
//                                MachineBasicBlock &MBB,
//                                MachineBasicBlock::iterator I) const override;

protected:
  uint64_t estimateStackSize(const MachineFunction &MF) const;
};

/// Create LC2200FrameLowering objects.
const LC2200FrameLowering *createLC2200FrameLowering(const LC2200Subtarget &ST);

} // End llvm namespace

#endif
