
// Instruction Name please reference the Intel 8080 Datasheet.

class InstI8080<bits<8> opcode, bits<2> immSize, dag outs, dag ins,
      string asmstr, list<dag> pattern = [], string constraints = "">
    : Instruction {
        bits<8> Opcode = opcode;
        let Namespace = "I8080";
        dag OutOperandList = outs;
        dag InOperandList = ins;
        let AsmString = asmstr;
        let Pattern = pattern;

        let isCodeGenOnly = 1;
    }

// [ALU r]egister operation
class ALUr_Instr<bits<3> opcode, string opcodestr, string argstr, SDNode dagOp, 
      string constraints = "">
      : InstI8080<{0b10 ,opcode, 0b000}, 0x0, (outs ALUR:$dst), (ins ALUR: $dst, OR8: $src), 
      !strconcat(opcodestr,argstr), [(set i8:$dst, (dagOp i8:$dst, i8:$src))], constraints>;

def ADDr : ALUr_Instr<0b000, "ADD", " $src", add>;
def ADCr : ALUr_Instr<0b000, "ADD", " $src", adde>;
def SUBr : ALUr_Instr<0b000, "ADD", " $src", sub>;
def SBBr : ALUr_Instr<0b000, "ADD", " $src", sube>;
def ANAr : ALUr_Instr<0b000, "ADD", " $src", and>;
def XRAr : ALUr_Instr<0b000, "ADD", " $src", xor>;
def ORAr : ALUr_Instr<0b000, "ADD", " $src", or>;
//def CMPr : ALUr_Instr<0b000, "ADD", " $src", cp>;

