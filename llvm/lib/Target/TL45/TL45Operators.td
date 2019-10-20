//===-- TL45Operators.td - TL45-specific operators ------*- tblgen-*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//


def TL45imm16 : Operand<i32>, ImmLeaf<i32, [{return isInt<16>(Imm);}]> {
  let MCOperandPredicate = [{
    int64_t Imm;
    if (MCOp.evaluateAsConstantImm(Imm))
      return isInt<16>(Imm);
    return MCOp.isBareSymbolRef();
  }];
}

//===----------------------------------------------------------------------===//
// Operand Definitions.
//===----------------------------------------------------------------------===//

def b_target : Operand<OtherVT>;

def cc_val : Operand<i32>, ImmLeaf<i32, [{}]> {
//   let PrintMethod = "printCondCode";
}

//===----------------------------------------------------------------------===//
// Complex Pattern Definitions.
//===----------------------------------------------------------------------===//

// def addr : ComplexPattern<iPTR, 4, "SelectAddr", [], []>;