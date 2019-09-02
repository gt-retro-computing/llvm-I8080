//===-- LC2200InstrFormats.td - LC2200 Instruction Formats ---*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

include "LC2200Operators.td"


class InstLC2200<dag outs, dag ins, bits<4> binOpcode, string opcodestr, string argstr, list<dag> pattern> : Instruction
{
  field bits<32> Inst;

  let Namespace = "LC2200";
  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString = opcodestr # "\t" # argstr;
  let Pattern = pattern;
  let Size = 4; //???
  let Inst{31-28} = binOpcode;
}


class LC2200PseudoInstr<dag outs, dag ins, string opcodestr, string argstr, list<dag> pattern> : InstLC2200<outs, ins, 0, opcodestr, argstr, pattern> {
  let isPseudo = 1;
}

class RTypeInstr<bits<4> opcode, string opcodestr, list<dag> pattern> : InstLC2200<(outs GRRegs:$dr), (ins SRCRegs:$sr1, SRCRegs:$sr2), opcode, opcodestr, "$dr, $sr1, $sr2", pattern> {
  bits<4> dr;
  bits<4> sr1;
  bits<4> sr2;
  let Inst{27-24} = sr1;
  let Inst{23-20} = sr2;
  let Inst{3-0} = dr;
  let Inst{19-4} = 0; // padding
}

// class ITypeInstr<bits<4> opcode, dag outs, dag ins, string asmstr, string argstr, list<dag> pattern> : InstLC2200<outs, ins, asmstr, pattern> {
//   let Inst{31-28} = opcode;
// }

// class JTypeInstr<bits<4> opcode, dag outs, dag ins, string asmstr, list<dag> pattern> : InstLC2200<outs, ins, asmstr, pattern> {
//   let Inst{31-28} = opcode;
// }

// class OTypeInstr<bits<4> opcode, dag outs, dag ins, string asmstr, list<dag> pattern> : InstLC2200<outs, ins, asmstr, pattern> {
//   let Inst{31-28} = opcode;
// }





// def immSExt5 : PatLeaf<(imm), [{ return isInt<5>(N->getSExtValue()); }]>;

// // Format A instruction class in LC2200 : <|opcode|dr|sr1|0|00|sr2|>
// class FA<bits<4> op, dag outs, dag ins, string asmstr, list<dag> pattern, InstrItinClass itin> : LC2200Inst<outs, ins, asmstr, pattern, itin, FrmA>
// {
//   bits<3> ra;
//   bits<3> rb;
//   bits<3> rc;

//   let Opcode = op;

//   let Inst{11-9} = ra; //DR
//   let Inst{8-6} = rb; //SR1
//   let Insr{2-0} = rc; //SR2

// }

// def Addii : ArithLogicI<0x1, "ADD", add, 

// // Format L instruction class in LC2200 : <|opcode|dr|sr1|1|imm5|>
// class FL<bits<4> op, dag outs, dag ins, string asmstr, list<dag> pattern, InstrItinClass itin> : LC2200Inst<outs, ins, asmstr, pattern, itin, FrmL>
// {
//   bits<3> dr;
//   bits<3> sr1;
//   bits<5> imm5;

//   let Opcode = op;

//   let Inst{11-9} = ra; //DR
//   let Inst{8-6} = rb; //SR1
//   let Insr{4-0} = imm5;
// }

// def ADDiu : ArithLogicI<0x1, "ADD", add, simm5, immSExt5, CPURegs>;

// def ANDiu : ArithLogicI<0x5, "AND", and, simm5, immSExt5, CPURegs>;

// // Arithmetic and logical instructions with 2 register operands
// class ArithLogicI<bits<4> op, string instr_asm, SDNode OpNode, Operand Od, PatLeaf imm_type, RegisterClass RC> :
//   FL<op, (outs GPROut:$ra), (ins RC:$rb, 0d:$imm5), !strconcat(instr_asm, "\t $ra, $rb, $imm5"),
//   [(set GPROut:$ra, (OpNode RC:$rb, imm_type:$imm5))], IIAlu> {
//     let isReMaterializable = 1;
//   }

// // Arithmetic and logical instructions with 3 register operands
// class ArithLogicI<bits<4> op, string instr_asm, SDNode OpNode, Operand Od, PatLeaf imm_type, RegisterClass RC> :
//   FA<op, (outs GPROut:$ra), (ins RC:$rb, 0d:$imm5), !strconcat(instr_asm, "\t $ra, $rb, $imm5"),
//   [(set GPROut:$ra, (OpNode RC:$rb, imm_type:$imm5))], IIAlu> {
//     let isReMaterializable = 1;
//   }





// class InstLC2200<...> : Instruction {
//   field bits<32> Inst;
//   ...
// } 