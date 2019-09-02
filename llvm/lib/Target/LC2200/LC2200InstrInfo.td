//===-- LC2200InstrInfo.td - Target Description for LC2200 -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the LC2200 instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

include "LC2200InstrFormats.td"
include "LC2200Operators.td"

//==========================
// R Type Instructions
//==========================

def ADD : RTypeInst<0b0000, (outs GRRegs:$dst), (ins SRCRegs:$src1, SRCRegs:$src2),
    "ADD $dst, $src1, $src2"),
    [(set i32:$dst, (add i32:$src1, i32:$src2))]> {
        bits<3> dst;
        bits<3> src1;
        bits<3> src2;
        let Inst{27-24} = $src1;
        let Inst{23-20} = $src2;
        let Inst{3-0} = $dst;
        let Inst{19-4} = 0; // padding
    }
def NAND: RTypeInst<0b0001, (outs GRRegs:$dst), (ins SRCRegs:$src1, SRCRegs:$src2),
    "NADD $dst, $src1, $src2"),
    [(set i32:$dst, (nand i32:$src1, i32:$src2))]> {
        bits<3> dst;
        bits<3> src1;
        bits<3> src2;
        let Inst{27-24} = $src1;
        let Inst{23-20} = $src2;
        let Inst{3-0} = $dst;
        let Inst{19-4} = 0; // padding
    }


//=========================
// I Type Instruction
//==========================

def ADDI : ITypeInstr<0b0010, (outs GRRegs:$dst), (ins SRCRegs:$src1, i32imm:$imm),
    "ADDI $dst, $src1, $imm",
    [(set i32:$dst, (add i32:$src1, LC2200imm20:$imm))]> {
        bits<3> dst;
        bits<3> src1;
        bits<20> imm;
        let Inst{27-24} = src1;
        let Inst{23-20} = dst;
        let Inst{19-0} = imm;
    }

def LW: ITypeInstr<0b0011, (outs GRRegs:$dst), (ins SRCRegs:$base, i32imm:$offset),
    "LW $dst, $offset($base)",
    [(set i32:$dst, (load (add addr:$base, addr:$offset)))]> {
        bits<3> dst;
        bits<3> base;
        bits<20> offset;
        let Inst{27-24} = base;
        let Inst{23-20} = dst;
        let Inst{19-0} = offset;
    }

def SW: ITypeInstr<0b0100, (outs SRCRegs:$src1), (ins SRCRegs:$base, i32imm:$offset),
    "SW $src1, $offset($base)",
    [(store i32:$src1, (add addr:$base, addr:$offset))]> {
        bits<3> src1;
        bits<3> base;
        bits<20> offset;
        let Inst{27-24} = base;
        let Inst{23-20} = src1;
        let Inst{19-0} = offset;
    }

def BEQ: ITypeInstr<0b0100, (outs SRCRegs:$src1), (ins SRCRegs:$src2, i32imm:$pcoffset),
    "BEQ $src1, $src2, $pcoffset",
    [(br_cc "chain", seteq, i32:$src1, i32:$src2, LC2200imm20:$pcoffset))]> { // pcoffset?? (add $pc, offset, 1???)
        bits<3> src1;
        bits<3> src2;
        bits<20> pcoffset;
        let Inst{27-24} = src1;
        let Inst{23-20} = src2;
        let Inst{19-0} = pcoffset;
    }

//===----------------------------------------------------------------------===//
// ALU Instructions
//===----------------------------------------------------------------------===//
// multiclass BinOp<bits<4> opcode, string opstr, SDNode opnode> {
//   def rr : ALUInst<opcode, (outs GRRegs:$dst), (ins GRRegs:$src1, GRRegs:$src2),
//                     !strconcat(opstr, " $dst, $src1, $src2"),
//                     [(set i16:$dst, (opnode i16:$src1, i16:$src2))]> {
//     bits<3> dst;
//     bits<3> src1;
//     bits<3> src2;
//     let Inst{5} = 0;
//     let Inst{7-6} = 0b00;
//     let Inst{11-9} = dst;
//     let Inst{8-6} = src1;
//     let Inst{2-0} = src2;
//     }
//   def ri : ALUInst<opcode, (outs GRRegs:$dst), (ins GRRegs:$src1, i16imm:$imm),
//                     !strconcat(opstr, " $dst, $src1, $imm"),
//                     [(set i16:$dst, (opnode i16:$src1, LC2200imm5:$imm))]> {
//     bits<3> dst;
//     bits<3> src1;
//     bits<5> imm;
//     let Inst{5} = 1;
//     let Inst{11-9} = dst;
//     let Inst{8-6} = src1;
//     let Inst{4-0} = imm;
//     }
// }

// defm ADD : BinOp<0b0001, "add", add>;
// defm AND : BinOp<0b0101, "and", and>;


//===----------------------------------------------------------------------===//
// Load/Store Instructions
//===----------------------------------------------------------------------===//
// def LD : InstLC2200<(outs GRRegs:$val), (ins memsrc:$addr),
//                   "ld $val, $addr",
//                   [(set i16:$val, (load addr:$addr))]> {
//     bits<3> val;
//     bits<9> addr;
//     let Inst{15-12} = 0b0010;
//     let Inst{11-9} = val;
//     let Inst{8-0} = addr;
//   }

//def ST : InstLC2200<(outs), (ins GRRegs:$val, memsrc:$addr),
 //                 "st $val, $addr",
  //                [(store i16:$val, addr:$addr)] {
 //   bits<3> val;
 //   bits<9> addr;
 //   let Inst{15-12} = 0b0011;
 //   let Inst{11-9} = val;
 //   let Inst{8-0} = addr;
 // }

// def LDR : InstLC2200<(outs GRRegs:$val), (ins GRRegs:$src1, i16imm:$imm),
//                     "ldr $val, $src1, $imm",
//                     [(set i16:$val, (load (add addr:$src1, addr:$imm)))]> {
//     bits<3> val;
//     bits<3> src1; //Base Register
//     bits<6> addr;
//     let Inst{15-12} = 0b0110;
//     let Inst{11-9} = val;
//     let Inst{8-6} = src1;
//     let Inst{5-0} = addr;
//   }

// def STR : InstLC2200<(outs GRRegs:$val), (ins GRRegs:$src1, i16imm:$imm),
//                     "str $val, $src1, $imm",
//                     [(store i16:$val, (add addr:$src1, addr:$imm))]> {
//     bits<3> val;
//     bits<3> src1; //Base Register
//     bits<6> addr;
//     let Inst{15-12} = 0b0110;
//     let Inst{11-9} = val;
//     let Inst{8-6} = src1;
//     let Inst{5-0} = addr;
//   }

// def LDI : InstLC2200<(outs GRRegs:$val), (ins memsrc:$addr),
//                   "ldi $val, $addr",
//                   [(set i16:$val, (load (load addr:$addr)))]> {
//     bits<3> val;
//     bits<9> addr;
//     let Inst{15-12} = 0b0010;
//     let Inst{11-9} = val;
//     let Inst{8-0} = addr;
//   }


//===----------------------------------------------------------------------===//
// Return Instructions
//===----------------------------------------------------------------------===//

// let isTerminator = 1, isReturn = 1, isBarrier = 1, Uses = [LR] in {
//   def RET : InstLC2200<(outs), (ins variable_ops),
//                     "bx lr",  [(LC2200RetFlag)]> {
//     let Inst{27-0}  = 0b0001001011111111111100011110;
//   }
// }


//===----------------------------------------------------------------------===//
// Call Instructions
//===----------------------------------------------------------------------===//
// JSR
// let isCall = 1, Defs = [LR], Uses = [SP] in {
//   def BL : BranchInst<0b1011, (outs), (ins GRRegs:$addr),
//                       "bl $addr",
//                       [(lc3_call i16:$addr)]> {
//     bits<4> addr;
//     let Inst{31-28} = 0b1110;
//     let Inst{3-0} = addr;
//   }
// }

// def : Pattern<(i32 (load_sym tglobaladdr:$addr)),  [(MOVi32 $addr)]>;



//===----------------------------------------------------------------------===//
//Branch Instructions
//===----------------------------------------------------------------------===//
// let isTerminator = 1, isBranch = 1, isBarrier = 1 in {
//   def B : InstLC2200<(outs), (ins b_target:$dst),
//                   "brnzp $dst", [(br bb:$dst)]> {
//     bits<9> dst;
//     bits<3> cc;
//     let Inst{15-12} = 0b0000;
//     let Inst{11-9} = 0b111;
//     let Inst{8-0} = dst;
//   }
// }

//let isTerminator = 1, isBranch = 1, Uses = [CPSR] in {
//  def Bcc : InstLC2200<(outs), (ins cc_val:$cc, b_target:$dst),
//                      "br$cc, $dst", []> {
//  }
//}


//===----------------------------------------------------------------------===//
// Pseudo Instructions
//===----------------------------------------------------------------------===//

// let Defs = [R7], Uses = [R7] in {
// def ADJCALLSTACKDOWN : LC2200PseudoInst<(outs), (ins i16imm:$amt),
//                                     "# ADJCALLSTACKDOWN $amt",
//                                     [(callseq_start timm:$amt)]>;
// def ADJCALLSTACKUP   : LC2200PseudoInst<(outs), (ins i16imm:$amt1, i16imm:$amt2),
//                                     "# ADJCALLSTACKUP $amt1",
//                                     [(callseq_end timm:$amt1, timm:$amt2)]>;
// } 