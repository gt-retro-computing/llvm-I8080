//===-- LC3Operators.td - LC3-specific operators ------*- tblgen-*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Type profiles
//===----------------------------------------------------------------------===//

// def MoveImm16Ty : SDTypeProfile<1, 1, [
//   SDTCisSameAs<0, 1>, SDTCisInt<0>
// ]>;

// def SDT_LC3CallSeqStart : SDCallSeqStart<[ SDTCisVT<0, i16> ]>;

// def SDT_LC3CallSeqEnd   : SDCallSeqEnd<[
//   SDTCisVT<0, i16>, SDTCisVT<1, i16>
// ]>;

// def LC3RetFlag    : SDNode<"LC3ISD::RET_FLAG", SDTNone,
//                            [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;
// def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_LC3CallSeqStart,
//                            [SDNPHasChain, SDNPOutGlue]>;
// def callseq_end   : SDNode<"ISD::CALLSEQ_END",   SDT_LC3CallSeqEnd,
//                            [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

// def SDT_LC3Call    : SDTypeProfile<0, -1, [SDTCisPtrTy<0>]>;


//===----------------------------------------------------------------------===//
// Custom SDNodes.
//===----------------------------------------------------------------------===//

//def load_sym : SDNode<"LC3ISD::LOAD_SYM", SDTIntUnaryOp>;

//def movei32 : SDNode<"LC3ISD::MOVEi32", MoveImm32Ty>;

// def lc3_call
//     : SDNode<"LC3ISD::CALL", SDT_LC3Call,
//              [ SDNPHasChain, SDNPOptInGlue, SDNPOutGlue, SDNPVariadic ]>;



//===----------------------------------------------------------------------===//
// Operand Definitions.
//===----------------------------------------------------------------------===//

// def bl_target : Operand<i16>;

// def b_target : Operand<OtherVT>;

// def cc_val : Operand<i32> {
//   let PrintMethod = "printCondCode";
// }

// def memsrc : Operand<i16> {
//   let MIOperandInfo = (ops GRRegs, i16imm);
//   let PrintMethod = "printAddrModeMemSrc";
//   let EncoderMethod = "getMemSrcValue";
// }

def LC2200imm20 : Operand<i32>, ImmLeaf<i32, [{return isInt<20>(Imm);}]> {
  let MCOperandPredicate = [{
    int64_t Imm;
    if (MCOp.evaluateAsConstantImm(Imm))
      return isInt<20>(Imm);
    return MCOp.isBareSymbolRef();
  }];
}

//===----------------------------------------------------------------------===//
// Complex Pattern Definitions.
//===----------------------------------------------------------------------===//

def addr : ComplexPattern<iPTR, 4, "SelectAddr", [], []>; 