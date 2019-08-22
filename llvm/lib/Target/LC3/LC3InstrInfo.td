include "LC3InstrFormats.td"
include "LC3Operators.td"

//===----------------------------------------------------------------------===//
// ALU Instructions
//===----------------------------------------------------------------------===//
multiclass BinOp<bits<4> opcode, string opstr, SDNode opnode> {
  def rr : ALUInst<opcode, (outs GRRegs:$dst), (ins GRRegs:$src1, GRRegs:$src2),
                    !strconcat(opstr, " $dst, $src1, $src2"),
                    [(set i16:$dst, (opnode i16:$src1, i16:$src2))]> {
    bits<3> dst;
    bits<3> src1;
    bits<3> src2;
    let Inst{5} = 0;
    let Inst{7-6} = 0b00;
    let Inst{11-9} = dst;
    let Inst{8-6} = src1;
    let Inst{2-0} = src2;
    }
  def ri : ALUInst<opcode, (outs GRRegs:$dst), (ins GRRegs:$src1, i16imm:$imm),
                    !strconcat(opstr, " $dst, $src1, $imm"),
                    [(set i16:$dst, (opnode i16:$src1, LC3imm5:$imm))]> {
    bits<3> dst;
    bits<3> src1;
    bits<5> imm;
    let Inst{5} = 1;
    let Inst{11-9} = dst;
    let Inst{8-6} = src1;
    let Inst{4-0} = imm;
    }
}

defm ADD : BinOp<0b0001, "add", add>;
defm AND : BinOp<0b0101, "and", and>;


