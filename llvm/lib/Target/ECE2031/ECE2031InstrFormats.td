//===-- ECE2031InstrFormats.td - ECE2031 Instruction Formats ---*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

include "ECE2031Operators.td"

class ECE2031Instr<bits<6> binOpcode, dag outs, dag ins, string opcodestr, string argstr, list<dag> pattern> : Instruction
{
  field bits<16> Inst;
  field bits<10> Immediate;

  let Namespace = "ECE2031";
  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString = opcodestr # "\t" # argstr;
  let Pattern = pattern;
  let Size = 2; // in bytes(?)
  let Inst{15-10} = binOpcode;
  let Inst{9-0} = Immediate;
}

class ECE2031PseudoInstr<dag outs, dag ins, string opcodestr, string argstr, list<dag> pattern> : ECE2031Instr<outs, ins, 0, opcodestr, argstr, pattern> {
  let isPseudo = 1;
}

// Pseudo instructions
class Pseudo<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : ECE2031Instr<outs, ins, 0, opcodestr, argstr, pattern> {
  let isPseudo = 1;
  let isCodeGenOnly = 1;
}
