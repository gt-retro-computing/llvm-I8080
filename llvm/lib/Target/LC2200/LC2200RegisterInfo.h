//===-- LC2200RegisterInfo.h - LC2200 Register Information Impl ---*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the LC2200 implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_LC2200_LC2200REGISTERINFO_H
#define LLVM_LIB_TARGET_LC2200_LC2200REGISTERINFO_H

#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "LC2200GenRegisterInfo.inc"

namespace llvm {

struct LC2200RegisterInfo : public LC2200GenRegisterInfo {

  LC2200RegisterInfo();

  const uint32_t *getCallPreservedMask(const MachineFunction &MF,
                                       CallingConv::ID) const override;

  const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const override;

  BitVector getReservedRegs(const MachineFunction &MF) const override;

//  bool isConstantPhysReg(unsigned PhysReg) const override;
//
//  const uint32_t *getNoPreservedMask() const override;

  void eliminateFrameIndex(MachineBasicBlock::iterator MI, int SPAdj,
                           unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override;

  Register getFrameRegister(const MachineFunction &MF) const override;

  bool requiresRegisterScavenging(const MachineFunction &MF) const override {
    return true;
  }

  bool requiresFrameIndexScavenging(const MachineFunction &MF) const override {
    return true;
  }

  bool trackLivenessAfterRegAlloc(const MachineFunction &) const override {
    return true;
  }

  const TargetRegisterClass *getPointerRegClass(const MachineFunction &MF,
          unsigned Kind = 0) const override {
    return &LC2200::GRRegsRegClass;
  }
};
}

#endif
