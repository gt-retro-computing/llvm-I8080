//===- ECE2031ISelLowering.h - ECE2031 DAG Lowering Interface ---------*- C++
//-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that ECE2031 uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ECE2031_ECE2031ISELLOWERING_H
#define LLVM_LIB_TARGET_ECE2031_ECE2031ISELLOWERING_H

#include "ECE2031.h"
#include "MCTargetDesc/ECE2031MCTargetDesc.h"
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
class ECE2031CCState;
class ECE2031FunctionInfo;
class ECE2031Subtarget;
class ECE2031TargetMachine;
class TargetLibraryInfo;
class TargetRegisterClass;

namespace ECE2031ISD {
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
} // namespace ECE2031ISD

//===--------------------------------------------------------------------===//
// TargetLowering Implementation
//===--------------------------------------------------------------------===//

class ECE2031TargetLowering : public TargetLowering {
  const ECE2031Subtarget &Subtarget;

public:
  explicit ECE2031TargetLowering(const ECE2031TargetMachine &TM,
                                const ECE2031Subtarget &STI);

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

#endif // LLVM_LIB_TARGET_ECE2031_ECE2031ISELLOWERING_H
