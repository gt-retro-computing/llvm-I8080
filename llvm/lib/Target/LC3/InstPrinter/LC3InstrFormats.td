class InstLC3<dag outs, dag ins, string asmstr, list<dag> pattern> : Instruction {
  field bits<16> Inst;
  let Namespace = "LC3";
  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString = asmstr;
  let Pattern = pattern;
  let Size = 1;
}