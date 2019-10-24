//===-- TL45InstrFormats.td - TL45 Instruction Formats ---*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

include "TL45Operators.td"


class InstTL45<dag outs, dag ins, bits<5> binOpcode, string opcodestr, string argstr, list<dag> pattern> : Instruction
{
  field bits<32> Inst;

  let Namespace = "TL45";
  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString = opcodestr # "\t" # argstr;
  let Pattern = pattern;
  let Size = 4; //???
  let Inst{31-27} = binOpcode;

  // SoftFail is a field the disassembler can use to provide a way for
  // instructions to not match without killing the whole decode process. It is
  // mainly used for ARM, but Tablegen expects this field to exist or it fails
  // to build the decode table.
  field bits<32> SoftFail = 0;
}


class TL45PseudoInstr<dag outs, dag ins, string opcodestr, string argstr, list<dag> pattern> : InstTL45<outs, ins, 0, opcodestr, argstr, pattern> {
  let isPseudo = 1;
}

// Pseudo instructions
class Pseudo<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : InstTL45<outs, ins, 0, opcodestr, argstr, pattern> {
  let isPseudo = 1;
  let isCodeGenOnly = 1;
}

// 5 - op code  [31-27]
// 3 - mode     [26-24] {imm?, hilo, signed}
// 4 - dr       [23-20]
// 4 - sr1      [19-16]
// 4 - sr2      [15-12]  OR  16 - immediate [15 - 0]
// 12 - padding [11-0]       ...

class RRRTypeInstr<bits<5> opcode, string opcodestr, list<dag> pattern> : InstTL45<(outs GRRegs:$dr), (ins GRRegs:$sr1, GRRegs:$sr2), opcode, opcodestr, "$dr, $sr1, $sr2", pattern> {
  bits<4> dr;
  bits<4> sr1;
  bits<4> sr2;
  bits<3> mode = 0b000;

  let Inst{26-24} = mode;
  let Inst{23-20} = dr;
  let Inst{19-16} = sr1;
  let Inst{15-12} = sr2;
  let Inst{11-0} = 0; // padding
  // let isCommutable = 1;
}

class RRITypeInstr<bits<5> opcode, string opcodestr, list<dag> pattern, string argstr="$dr, $sr1, $imm"> : InstTL45<(outs GRRegs:$dr), (ins GRRegs:$sr1, TL45imm16:$imm), opcode, opcodestr, argstr, pattern> {
  bits<4> dr;
  bits<4> sr1;
  bits<16> imm;
  bits<3> mode = 0b101;

  let Inst{26-24} = mode;
  let Inst{23-20} = dr;
  let Inst{19-16} = sr1;
  let Inst{15-0} = imm;
  // let isCommutable = 1;
}

class RRIStoreTypeInstr<bits<5> opcode, string opcodestr, list<dag> pattern, string argstr="$dr, $sr1, $imm"> : InstTL45<(outs), (ins GRRegs:$dr, GRRegs:$sr1, TL45imm16:$imm), opcode, opcodestr, argstr, pattern> {
  bits<4> dr;
  bits<4> sr1;
  bits<16> imm;
  bits<3> mode = 0b101;

  let Inst{26-24} = mode;
  let Inst{23-20} = dr;
  let Inst{19-16} = sr1;
  let Inst{15-0} = imm;
  // let isCommutable = 1;
}

multiclass ALUTypeInstr<bits<5> opcode, string opcodestr, SDNode OpNode> {
    def "" : RRRTypeInstr<opcode, opcodestr, [(set i32:$dr, (OpNode i32:$sr1, i32:$sr2))]>;
    def "I" : RRITypeInstr<opcode, !strconcat(opcodestr, "i"), [(set i32:$dr, (OpNode i32:$sr1, TL45imm16:$imm))]>;
}

class RRTypeInstr<bits<5> opcode, string opcodestr, list<dag> pattern> : InstTL45<(outs GRRegs:$dr), (ins GRRegs:$sr1), opcode, opcodestr, "$dr, $sr1", pattern> {
  bits<4> dr;
  bits<4> sr1;
  bits<3> mode = 0b000;

  let Inst{26-24} = mode;
  let Inst{23-20} = dr;
  let Inst{19-16} = sr1;
  let Inst{15-12} = 0b0000;
  let Inst{11-0} = 0; // padding
  // let isCommutable = 1;
}

class RITypeInstr<bits<5> opcode, string opcodestr, list<dag> pattern> : InstTL45<(outs GRRegs:$dr), (ins TL45imm16:$imm), opcode, opcodestr, "$dr, $imm", pattern> {
  bits<4> dr;
  bits<16> imm;
  bits<3> mode = 0b100;

  let Inst{26-24} = mode;
  let Inst{23-20} = dr;
  let Inst{19-16} = 0b0000;
  let Inst{15-0} = imm;
  // let isCommutable = 1;
}

class RRCmpTypeInstr<bits<5> opcode, string opcodestr, list<dag> pattern> : InstTL45<(outs), (ins GRRegs:$sr1, GRRegs:$sr2), opcode, opcodestr, "$sr1, $sr2", pattern> {
  bits<4> sr1;
  bits<4> sr2;
  bits<3> mode = 0b000;

  let Inst{26-24} = mode;
  let Inst{23-20} = 0b0000;
  let Inst{19-16} = sr1;
  let Inst{15-12} = sr2;
  let Inst{11-0} = 0; // padding
  // let isCommutable = 1;
}

class RICmpTypeInstr<bits<5> opcode, string opcodestr, list<dag> pattern> : InstTL45<(outs), (ins GRRegs:$sr1, TL45imm16:$imm), opcode, opcodestr, "$sr1, $imm", pattern> {
  bits<4> sr1;
  bits<16> imm;
  bits<3> mode = 0b100;

  let Inst{26-24} = mode;
  let Inst{23-20} = 0b0000;
  let Inst{19-16} = sr1;
  let Inst{15-0} = imm;
  // let isCommutable = 1;
}

class RIOutTypeInstr<bits<5> opcode, string opcodestr, list<dag> pattern> : InstTL45<(outs), (ins GRRegs:$dr, TL45imm16:$imm), opcode, opcodestr, "$dr, $imm", pattern> {
  bits<4> dr;
  bits<16> imm;
  bits<3> mode = 0b100;

  let Inst{26-24} = mode;
  let Inst{23-20} = dr;
  let Inst{19-16} = 0b0000;
  let Inst{15-0} = imm;
  // let isCommutable = 1;
}


class ITypeInstr<bits<5> opcode, string opcodestr, list<dag> pattern> : InstTL45<(outs), (ins TL45imm16:$imm), opcode, opcodestr, "$imm", pattern> {
  bits<4> dr;
  bits<16> imm;
  bits<3> mode = 0b000;

  let Inst{26-24} = mode;
  let Inst{23-20} = 0b0000;
  let Inst{19-16} = 0b0000;
  let Inst{15-0} = imm;
  // let isCommutable = 1;
}

class CallTypeInstr<bits<5> opcode, string opcodestr, list<dag> pattern> : InstTL45<(outs), (ins GRRegs:$base, TL45imm16:$imm), opcode, opcodestr, "$base, $imm", pattern> {
  bits<4> base;
  bits<16> imm;
  bits<3> mode = 0b000;

  // the encoding of register sp
  bits<4> dr = 0b1111;

  let isCall = 1;
  let Inst{26-24} = mode;
  let Inst{23-20} = dr;
  let Inst{19-16} = base;
  let Inst{15-0} = imm;
  // let isCommutable = 1;
}

class CallRTypeInstr<bits<5> opcode, string opcodestr, list<dag> pattern> : InstTL45<(outs), (ins GRRegs:$base), opcode, opcodestr, "$base", pattern> {
  bits<4> base;
  bits<16> imm = 0;
  bits<3> mode = 0b000;

  // the encoding of register sp
  bits<4> dr = 0b1111;

  let isCall = 1;
  let Inst{26-24} = mode;
  let Inst{23-20} = dr;
  let Inst{19-16} = base;
  let Inst{15-0} = imm;
  // let isCommutable = 1;
}

class CallITypeInstr<bits<5> opcode, string opcodestr, list<dag> pattern> : InstTL45<(outs), (ins TL45imm16:$imm), opcode, opcodestr, "$imm", pattern> {
  bits<4> base = 0; // r0
  bits<16> imm;
  bits<3> mode = 0b000;

  // the encoding of register sp
  bits<4> dr = 0b1111;

  let isCall = 1;
  let Inst{26-24} = mode;
  let Inst{23-20} = dr;
  let Inst{19-16} = base;
  let Inst{15-0} = imm;
  // let isCommutable = 1;
}

class JRTypeInstr<bits<5> opcode, bits<4> flag, string opcodestr, list<dag> pattern> : InstTL45<(outs), (ins GRRegs:$dst), opcode, opcodestr, "$dst", pattern> {
  bits<4> dst;
  bits<3> mode = 0b100;

  let Inst{26-24} = mode;
  let Inst{23-20} = flag;
  let Inst{19-16} = 0b0000;
  let Inst{15-12} = dst;
  let Inst{11-0} = 0; // padding
  // let isCommutable = 1;
  let isIndirectBranch = 1;
}

class JITypeInstr<bits<5> opcode, bits<4> flag, string opcodestr, list<dag> pattern> : InstTL45<(outs), (ins jmptarget:$imm), opcode, opcodestr, "$imm", pattern> {
  bits<16> imm;
  bits<3> mode = 0b100;

  let Inst{26-24} = mode;
  let Inst{23-20} = flag;
  let Inst{19-16} = 0b0000;
  let Inst{15-0} = imm;
  // let isCommutable = 1;
}

multiclass JTypeInstr<bits<5> opcode, bits<4> flag, string opcodestr, list<dag> pattern=[]> {
    def "" : JRTypeInstr<opcode, flag, opcodestr, []>;
    def "I" : JITypeInstr<opcode, flag, !strconcat(opcodestr, "i"), pattern>;
}

class NoneTypeInstr<bits<5> opcode, string opcodestr, list<dag> pattern> : InstTL45<(outs), (ins), opcode, opcodestr, "", pattern> {
  bits<3> mode = 0b000;
  bits<4> dr = 0;

  let Inst{26-24} = mode;
  let Inst{23-20} = dr;
  let Inst{19-0} = 0;
  // let isCommutable = 1;
}


/*
class RTypeInstr<bits<4> opcode, string opcodestr, list<dag> pattern> : InstTL45<(outs GRRegs:$dr), (ins GRRegs:$sr1, GRRegs:$sr2), opcode, opcodestr, "$dr, $sr1, $sr2", pattern> {
  bits<4> dr;
  bits<4> sr1;
  bits<4> sr2;
  let Inst{27-24} = dr;
  let Inst{23-20} = sr1;
  let Inst{3-0} = sr2;
  let Inst{19-4} = 0; // padding
  let isCommutable = 1;
}

class SKPTypeInstr<bits<4> opcode, bits<4> mode, string opcodestr, list<dag> pattern> : InstTL45<(outs), (ins GRRegs:$sr1, GRRegs:$sr2), opcode, opcodestr, "$sr1, $sr2", []> {
    bits<4> sr1;
    bits<4> sr2;
    let Inst{27-24} = mode;
    let Inst{23-20} = sr1;
    let Inst{3-0} = sr2;
    let Inst{19-4} = 0; // padding
    let isBranch = 1;
    let isCompare = 1;
}

class ITypeInstr<bits<4> opcode, dag outs, dag ins, string asmstr, string argstr, list<dag> pattern> : InstTL45<outs, ins, opcode, asmstr, argstr, pattern> {
}
*/

