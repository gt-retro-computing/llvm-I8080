//===- LC2200ISelLowering.h - LC2200 DAG Lowering Interface ---------*- C++
//-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that LC2200 uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_LC2200_LC2200ISELLOWERING_H
#define LLVM_LIB_TARGET_LC2200_LC2200ISELLOWERING_H

#include "LC2200.h"
#include "MCTargetDesc/LC2200MCTargetDesc.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/ISDOpcodes.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/MachineValueType.h"
#include "llvm/Target/TargetMachine.h"
#include <algorithm>
#include <cassert>
#include <deque>
#include <string>
#include <utility>
#include <vector>

namespace llvm {

class Argument;
class CCState;
class CCValAssign;
class FastISel;
class FunctionLoweringInfo;
class MachineBasicBlock;
class MachineFrameInfo;
class MachineInstr;
class LC2200CCState;
class LC2200FunctionInfo;
class LC2200Subtarget;
class LC2200TargetMachine;
class TargetLibraryInfo;
class TargetRegisterClass;

namespace LC2200ISD {
enum NodeType : unsigned {
  FIRST_NUMBER = ISD::BUILTIN_OP_END,
  RET,
  CALL,
  JMP,
  CMP_SKIP,
  CMP_JMP,
  SELECT_MOVE,
  CMP_SELECT_MOVE,
  NAND
};
} // namespace LC2200ISD

//===--------------------------------------------------------------------===//
// TargetLowering Implementation
//===--------------------------------------------------------------------===//

class LC2200TargetLowering : public TargetLowering {
  const LC2200Subtarget &Subtarget;

public:
  explicit LC2200TargetLowering(const LC2200TargetMachine &TM,
                                const LC2200Subtarget &STI);

  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                               bool IsVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins,
                               const SDLoc &DL, SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
                      SelectionDAG &DAG) const override;

  SDValue LowerCall(CallLoweringInfo & /*CLI*/,
                    SmallVectorImpl<SDValue> & /*InVals*/) const override;

  // Provide custom lowering hooks for some operations.
  SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

  const char *getTargetNodeName(unsigned Opcode) const override;

private:
  void analyzeInputArgs(MachineFunction &MF, CCState &CCInfo,
                        const SmallVectorImpl<ISD::InputArg> &Ins,
                        bool IsRet) const;
  void analyzeOutputArgs(MachineFunction &MF, CCState &CCInfo,
                         const SmallVectorImpl<ISD::OutputArg> &Outs,
                         bool IsRet, CallLoweringInfo *CLI) const;

  // copied from LegalizeDAG. We emit our own libcalls not directly supported by LLVM.
  SDValue ExpandLibCall(const char *LibcallName, SDValue Op, bool isSigned, SelectionDAG &DAG) const;

  SDValue lowerShiftLeft(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerBrCc(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerBr(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerSelectCc(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerAnd(SDValue Op, SelectionDAG &DAG) const; // can't believe this god damn method exist -.-
  SDValue lowerOr(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerXor(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_LC2200_LC2200ISELLOWERING_H
