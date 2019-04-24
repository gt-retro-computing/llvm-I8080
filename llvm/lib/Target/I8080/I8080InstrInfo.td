
// Instruction Name please reference the Intel 8080 Datasheet. 

def ALU_Instr<> : InstI8080<outs, ins, opcodestr, argstr, []>

def ADDr : InstI8080<(outs ALUR: $dst),
    (ins ALUR: $dst, OR8: $src),
    "ADD $src",
    [(set i8:$dst, (add i8:$dst, i8:$src))];
>;
