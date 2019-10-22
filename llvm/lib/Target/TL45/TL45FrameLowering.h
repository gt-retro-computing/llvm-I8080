//===-- TL45FrameLowering.h - Define frame lowering for TL45 ----*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_TL45_TL45FRAMELOWERING_H
#define LLVM_LIB_TARGET_TL45_TL45FRAMELOWERING_H

#include "TL45.h"
#include "llvm/CodeGen/TargetFrameLowering.h"

namespace llvm {
class TL45Subtarget;

class TL45FrameLowering : public TargetFrameLowering {
protected:
  const TL45Subtarget &STI;

public:
  explicit TL45FrameLowering(const TL45Subtarget &sti, unsigned Alignment)
          : TargetFrameLowering(StackGrowsDown, Alignment, 0, Alignment), STI(sti) {}

  void determineFrameLayout(MachineFunction &MF) const;

  void adjustReg(MachineBasicBlock &MBB,
                 MachineBasicBlock::iterator MBBI,
                 const DebugLoc &DL, Register DestReg,
                 Register SrcReg, int64_t Val,
                 MachineInstr::MIFlag Flag) const;

  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  virtual int getFrameIndexReference(const MachineFunction &MF, int FI,
                                     unsigned &FrameReg) const override;

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

  MachineBasicBlock::iterator
  eliminateCallFramePseudoInstr(MachineFunction &MF,
                                MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator I) const override;

protected:
  uint64_t estimateStackSize(const MachineFunction &MF) const;
};

/// Create TL45FrameLowering objects.
const TL45FrameLowering *createTL45FrameLowering(const TL45Subtarget &ST);

} // End llvm namespace

#endif
