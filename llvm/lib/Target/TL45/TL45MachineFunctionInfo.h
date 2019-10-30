//=- TL45MachineFunctionInfo.h - TL45 machine function info -----*- C++ -*-=//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares TL45-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TL45_TL45MACHINEFUNCTIONINFO_H
#define LLVM_LIB_TARGET_TL45_TL45MACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

/// TL45MachineFunctionInfo - This class is derived from MachineFunctionInfo
/// and contains private TL45-specific information for each MachineFunction.
class TL45MachineFunctionInfo : public MachineFunctionInfo {
private:
  MachineFunction &MF;
  /// FrameIndex for start of varargs area
  int VarArgsFrameIndex = 0;
  /// Size of the save area used for varargs
  int VarArgsSaveSize = 0;

public:
  TL45MachineFunctionInfo(MachineFunction &MF) : MF(MF) {}

  int getVarArgsFrameIndex() const { return VarArgsFrameIndex; }
  void setVarArgsFrameIndex(int Index) { VarArgsFrameIndex = Index; }

  unsigned getVarArgsSaveSize() const { return VarArgsSaveSize; }
  void setVarArgsSaveSize(int Size) { VarArgsSaveSize = Size; }
  
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_TL45_TL45MACHINEFUNCTIONINFO_H
