//===-- TL45InstrInfo.h - TL45 Instruction Information --------*- C++
//-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the TL45 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TL45_TL45INSTRINFO_H
#define LLVM_LIB_TARGET_TL45_TL45INSTRINFO_H

#include "TL45RegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/ISDOpcodes.h"

#define GET_INSTRINFO_HEADER

#include "TL45GenInstrInfo.inc"

namespace llvm {

class TL45InstrInfo : public TL45GenInstrInfo {
  const TL45RegisterInfo RI;

public:
  TL45InstrInfo();

  // Adjust SP by FrameSize bytes. Save RA, S0, S1
  void makeFrame(unsigned SP, int64_t FrameSize, MachineBasicBlock &MBB,
                 MachineBasicBlock::iterator I) const;

  // Adjust SP by FrameSize bytes. Restore RA, S0, S1
  void restoreFrame(unsigned SP, int64_t FrameSize, MachineBasicBlock &MBB,
                    MachineBasicBlock::iterator I) const;

  /// Emit a series of instructions to load an immediate.
  // This is to adjust some FrameReg. We return the new register to be used
  // in place of FrameReg and the adjusted immediate field (&NewImm)
  unsigned loadImmediate(unsigned FrameReg, int64_t Imm, MachineBasicBlock &MBB,
                         MachineBasicBlock::iterator II, const DebugLoc &DL,
                         unsigned &NewImm) const;

  static bool validImmediate(unsigned Opcode, unsigned Reg, int64_t Amount);

  // Generates instructions to conditionally execute the next instruction only
  // if the comparison (a CC b) was true. Returns number of instructions added
  unsigned resolveComparison(MachineBasicBlock &MBB, MachineBasicBlock::iterator I, const DebugLoc &DL,
                             ISD::CondCode ConditionCode, MachineOperand &a,
                             MachineOperand &b, unsigned int &JmpOpcode) const;

  // Overrides
  void storeRegToStackSlot(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MBBI, unsigned SrcReg,
                           bool IsKill, int FrameIndex,
                           const TargetRegisterClass *RC,
                           const TargetRegisterInfo *TRI) const override;

  void loadRegFromStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MBBI, unsigned DstReg,
                            int FrameIndex, const TargetRegisterClass *RC,
                            const TargetRegisterInfo *TRI) const override;

  bool isAsCheapAsAMove(const MachineInstr &MI) const override;

  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
                   const DebugLoc &DL, unsigned DestReg, unsigned SrcReg,
                   bool KillSrc) const override;

  bool expandPostRAPseudo(MachineInstr &MI) const override;

  unsigned insertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
                        MachineBasicBlock *FBB, ArrayRef<MachineOperand> Cond,
                        const DebugLoc &DL,
                        int *BytesAdded = nullptr) const override;

  unsigned getInstSizeInBytes(const MachineInstr &MI) const override {
    // DUDE RISC LMAO
    return 4U;
  }

  bool analyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB,
                            MachineBasicBlock *&FBB,
                            SmallVectorImpl<MachineOperand> &Cond,
                            bool AllowModify = false) const override;

  unsigned removeBranch(MachineBasicBlock &MBB,
                        int *BytesRemoved = nullptr) const override;

};
} // namespace llvm
#endif
