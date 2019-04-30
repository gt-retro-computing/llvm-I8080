
include "LC3Operators.td"

class LC3InstR<bits<4> opcode,
    dag outs, dag ins,
    string opcodestr, string argstr, list<dag> pattern> : Instruction {
    bits<16> Inst;
    let Namespace = "LC3";
    let Size = 2;
    let Inst{15-12} = opcode;
    let AsmString = "$opcodestr $argsstr";
    dag OutOperandList = outs;
    dag InOperandList = ins;
    let Pattern = pattern;
}

class LC3_ALU3r_Inst<bits<4> opcode, string opcodestr, SDNode opnode>
    : LC3InstR<opcode, (outs GRRegs:$dr), (ins GRRegs:$sr1, GRRegs:$sr2),
    opcodestr, "$dr, $sr1, $sr2", [(set i16:$dr, (opnode i16:$sr1, i16:$sr2))]> {
    bits<3> dr;
    bits<3> sr1;
    bits<3> sr2;

    let Inst{11-9} = dr;
    let Inst{8-6} = sr1;
    let Inst{5-3} = 0b000;
    let Inst{2-0} = sr2;
}
class LC3_ALU2ri_Inst<bits<4> opcode, string opcodestr, SDNode opnode>
    : LC3InstR<opcode, (outs GRRegs:$dr), (ins GRRegs:$sr1, i16imm:$imm),
    opcodestr, "$dr, $sr1, $imm", [(set i16:$dr, (opnode i16:$sr1, LC3imm5:$imm))]> {
    bits<3> dr;
    bits<3> sr1;
    bits<5> imm;

    let Inst{11-9} = dr;
    let Inst{8-6} = sr1;
    let Inst{5} = 0b1;
    let Inst{4-0} = imm;
}

def LC3_ADD3r : LC3_ALU3r_Inst<0b0001, "ADD", add>;
def LC3_AND3r : LC3_ALU3r_Inst<0b0101, "ADD", and>;
def LC3_ADD2ri : LC3_ALU2ri_Inst<0b0001, "ADD", add>;
def LC3_AND2ri : LC3_ALU2ri_Inst<0b0101, "ADD", and>;
