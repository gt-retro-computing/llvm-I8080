//===-- LC2200InstrInfo.h - LC2200 Instruction Information --------*- C++
//-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the LC2200 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_LC2200_LC2200INSTRINFO_H
#define LLVM_LIB_TARGET_LC2200_LC2200INSTRINFO_H

#include "LC2200RegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "LC2200GenInstrInfo.inc"

namespace llvm {

class LC2200InstrInfo : public LC2200GenInstrInfo {
  const LC2200RegisterInfo RI;

public:
  LC2200InstrInfo();

  // Adjust SP by FrameSize bytes. Save RA, S0, S1
  void makeFrame(unsigned SP, int64_t FrameSize, MachineBasicBlock &MBB,
                 MachineBasicBlock::iterator I) const;

  // Adjust SP by FrameSize bytes. Restore RA, S0, S1
  void restoreFrame(unsigned SP, int64_t FrameSize, MachineBasicBlock &MBB,
                    MachineBasicBlock::iterator I) const;

  void storeRegToStackSlot(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MBBI, unsigned SrcReg,
                           bool IsKill, int FrameIndex,
                           const TargetRegisterClass *RC,
                           const TargetRegisterInfo *TRI) const override;

  void loadRegFromStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MBBI, unsigned DstReg,
                            int FrameIndex, const TargetRegisterClass *RC,
                            const TargetRegisterInfo *TRI) const override;

  /// Emit a series of instructions to load an immediate.
  // This is to adjust some FrameReg. We return the new register to be used
  // in place of FrameReg and the adjusted immediate field (&NewImm)
  unsigned loadImmediate(unsigned FrameReg, int64_t Imm, MachineBasicBlock &MBB,
                         MachineBasicBlock::iterator II, const DebugLoc &DL,
                         unsigned &NewImm) const;

  static bool validImmediate(unsigned Opcode, unsigned Reg, int64_t Amount);

  bool isAsCheapAsAMove(const MachineInstr &MI) const override;

  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
                   const DebugLoc &DL, unsigned DestReg, unsigned SrcReg,
                   bool KillSrc) const override;

  bool expandPostRAPseudo(MachineInstr &MI) const override;

  void resolveComparison(MachineInstr &MI, ISD::CondCode ConditionCode,
                         MachineOperand &a, MachineOperand &b) const;
};
} // namespace llvm
#endif
