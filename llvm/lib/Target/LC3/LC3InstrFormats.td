
class InstLC3<dag outs, dag ins, string asmstr, list<dag> pattern> : Instruction {
  field bits<16> Inst;

  let Namespace = "LC3";
  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString = asmstr;
  let Pattern = pattern;
  let Size = 2; //???

  let Inst{15-12} = 0b0000;
}

class ALUInst<bits<4> opcode, dag outs, dag ins, string asmstr, list<dag> pattern> : InstLC3<outs, ins, asmstr, pattern> {
  let Inst{15-12} = opcode;
}