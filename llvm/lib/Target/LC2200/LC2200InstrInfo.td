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

// Intrinsics

//def CallSymbol : AsmOperandClass {
//  let Name = "CallSymbol";
//  let RenderMethod = "addImmOperands";
//  let DiagnosticType = "InvalidCallSymbol";
//  let ParserMethod = "parseCallSymbol";
// }

// A bare symbol used in call/tail only.
//def call_symbol : Operand<i32> {
//  let ParserMatchClass = CallSymbol;
//}

// Target-independent type requirements, but with target-specific formats.
def SDT_CallSeqStart : SDCallSeqStart<[SDTCisVT<0, i32>,
                                       SDTCisVT<1, i32>]>;
def SDT_CallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i32>,
                                     SDTCisVT<1, i32>]>;

def SDT_LC2200Call     : SDTypeProfile<0, -1, [SDTCisVT<0, i32>]>;

// Target-independent nodes, but with target-specific formats.
def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_CallSeqStart,
                           [SDNPHasChain, SDNPOutGlue]>;
def callseq_end   : SDNode<"ISD::CALLSEQ_END", SDT_CallSeqEnd,
                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

//==========================
// R Type Instructions
//==========================

def ADD : RTypeInstr<0b0000, "add",
    [(set i32:$dr, (add i32:$sr1, i32:$sr2))]> {
    let isAdd = 1;
}
def NAND: RTypeInstr<0b0001, "nand",
    [(set (i32 GRRegs:$dr), (xor (xor (i32 GRRegs:$sr1), (xor (i32 GRRegs:$sr1), (i32 GRRegs:$sr2))), (xor (i32 GRRegs:$sr2), (xor (i32 GRRegs:$sr1), (i32 GRRegs:$sr2)))))]>;


//=========================
// I Type Instruction
//==========================

def ADDI : ITypeInstr<0b0010, (outs GRRegs:$dst), (ins GRRegs:$src1, LC2200imm20:$imm),
    "addi", "$dst, $src1, $imm",
    [(set (i32 GRRegs:$dst), (add (i32 GRRegs:$src1), LC2200imm20:$imm))]> {
        bits<4> dst;
        bits<4> src1;
        bits<20> imm;
        let Inst{27-24} = src1;
        let Inst{23-20} = dst;
        let Inst{19-0} = imm;
    }

def LW: ITypeInstr<0b0011, (outs GRRegs:$dst), (ins LC2200imm20:$off20, GRRegs:$base),
    "lw", "$dst, ${off20}(${base})",
    [(set (i32 GRRegs:$dst), (load (add (i32 GRRegs:$base), LC2200imm20:$off20)))]> {
        bits<4> dst;
        bits<4> base;
        bits<20> off20;
        let Inst{23-20} = dst;
        let Inst{27-24} = base;
        let Inst{19-0} = off20;
        let mayLoad = 1;
    }

def SW: ITypeInstr<0b0100, (outs), (ins GRRegs:$src1, LC2200imm20:$offset, GRRegs:$base),
    "sw", "$src1, ${offset}(${base})",
    [(store i32:$src1, (add (i32 GRRegs:$base), LC2200imm20:$offset)), (store i32:$src1, (i32 GRRegs:$base))]> {
        bits<4> src1;
        bits<4> base;
        bits<20> offset;
        let Inst{27-24} = base;
        let Inst{23-20} = src1;
        let Inst{19-0} = offset;
        let mayStore = 1;
    }

def GOTO: ITypeInstr<0b0101, (outs), (ins b_target:$offset),
    "goto", "$offset",
    [(br bb:$offset)]> {
        bits<20> offset;
        let Inst{27-24} = 0;
        let Inst{23-20} = 0;
        let Inst{19-0} = offset;
        let isBranch = 1;
        let isBarrier = 1;
        let isTerminator = 1;
    }

def JALR: ITypeInstr<0b0101, (outs GRRegs:$dr), (ins GRRegs:$sr1),
    "jalr", "$dr, $sr1",
    []> {
        bits<4> dr;
        bits<4> sr1;
        let Inst{27-24} = dr;
        let Inst{23-20} = sr1;
        let Inst{19-0} = 0;
        let isBranch = 1;
        let isTerminator = 1;
        let isReturn = 1;
    }

def HALT : InstLC2200<(outs), (ins), 0b0111, "halt", "", []> {
    let Inst{27-0} = 0;
    let isTerminator = 1;
    let hasSideEffects = 1;
}

def SKPE : SKPTypeInstr<0b1000, 0b0000, "skpe", []>;
def SKPLT : SKPTypeInstr<0b1000, 0b0001, "skplt", []>;


def LEA: ITypeInstr<0b1001, (outs GRRegs:$dr), (ins LC2200imm20:$offset),
    "lea", "$dr, $offset",
    [(set i32:$dr, (add pc, LC2200imm20:$offset))]> {
        bits<4> dr;
        bits<20> offset;
        let Inst{27-24} = dr;
        let Inst{23-20} = 0;
        let Inst{19-0} = offset;
        let isAdd = 1;
    }


// def BEQ: ITypeInstr<0b0100, (outs SRCRegs:$src1), (ins SRCRegs:$src2, i32imm:$pcoffset),
//     "BEQ $src1, $src2, $pcoffset",
//     [(BR_CC, i32:$src1, i32:$src2, LC2200imm20:$pcoffset))]> { // pcoffset?? (add $pc, offset, 1???)
//         bits<4> src1;
//         bits<4> src2;
//         bits<20> pcoffset;
//         let Inst{27-24} = src1;
//         let Inst{23-20} = src2;
//         let Inst{19-0} = pcoffset;
//     }

//===----------------------------------------------------------------------===//
// ALU Instructions
//===----------------------------------------------------------------------===//
// multiclass BinOp<bits<4> opcode, string opstr, SDNode opnode> {
//   def rr : ALUInst<opcode, (outs GRRegs:$dst), (ins GRRegs:$src1, GRRegs:$src2),
//                     !strconcat(opstr, " $dst, $src1, $src2"),
//                     [(set i16:$dst, (opnode i16:$src1, i16:$src2))]> {
//     bits<4> dst;
//     bits<4> src1;
//     bits<4> src2;
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


/// Predicates

def IsOrAdd: PatFrag<(ops node:$A, node:$B), (or node:$A, node:$B), [{
  return isOrEquivalentToAdd(N);
}]>;

/// Immediates

def : Pat<(LC2200imm20:$imm), (ADDI zero, LC2200imm20:$imm)>;

// Addressing modes.
// Necessary because a frameindex can't be matched directly in a pattern.
def AddrFI : ComplexPattern<iPTR, 1, "SelectAddrFI", [frameindex], []>;

/// FrameIndex calculations

def : Pat<(add (i32 AddrFI:$Rs), LC2200imm20:$imm12),
          (ADDI (i32 AddrFI:$Rs), LC2200imm20:$imm12)>;
def : Pat<(IsOrAdd (i32 AddrFI:$Rs), LC2200imm20:$imm12),
        (ADDI (i32 AddrFI:$Rs), LC2200imm20:$imm12)>;

// FrameIndex Loads

multiclass LdPat<PatFrag LoadOp, InstLC2200 Inst> {
  def : Pat<(LoadOp GRRegs:$rs1), (Inst GRRegs:$rs1, 0)>;
  def : Pat<(LoadOp AddrFI:$rs1), (Inst AddrFI:$rs1, 0)>;
  def : Pat<(LoadOp (add GRRegs:$rs1, LC2200imm20:$imm12)),
            (Inst GRRegs:$rs1, LC2200imm20:$imm12)>;
  def : Pat<(LoadOp (add AddrFI:$rs1, LC2200imm20:$imm12)),
            (Inst AddrFI:$rs1, LC2200imm20:$imm12)>;
  def : Pat<(LoadOp (IsOrAdd AddrFI:$rs1, LC2200imm20:$imm12)),
            (Inst AddrFI:$rs1, LC2200imm20:$imm12)>;
}

defm : LdPat<load, LW>;

/// Stores

multiclass StPat<PatFrag StoreOp, InstLC2200 Inst, RegisterClass StTy> {
  def : Pat<(StoreOp StTy:$rs2, GRRegs:$rs1), (Inst StTy:$rs2, GRRegs:$rs1, 0)>;

  def : Pat<(StoreOp StTy:$rs2, AddrFI:$rs1), (Inst StTy:$rs2, AddrFI:$rs1, 0)>;
  def : Pat<(StoreOp StTy:$rs2, (add GRRegs:$rs1, LC2200imm20:$imm12)),
            (Inst StTy:$rs2, GRRegs:$rs1, LC2200imm20:$imm12)>;
  def : Pat<(StoreOp StTy:$rs2, (add AddrFI:$rs1, LC2200imm20:$imm12)),
            (Inst StTy:$rs2, AddrFI:$rs1, LC2200imm20:$imm12)>;
  def : Pat<(StoreOp StTy:$rs2, (IsOrAdd AddrFI:$rs1, LC2200imm20:$imm12)),
            (Inst StTy:$rs2, AddrFI:$rs1, LC2200imm20:$imm12)>;


}

def : Pat<(store GRRegs:$rs2, GRRegs:$rs1), (SW GRRegs:$rs2, 0, GRRegs:$rs1)>;


//===----------------------------------------------------------------------===//
// Pseudo Instructions
//===----------------------------------------------------------------------===//

def lc2200_call      : SDNode<"LC2200ISD::CALL", SDT_LC2200Call,
                             [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue,
                              SDNPVariadic]>;

def lc2200_ret_flag  : SDNode<"LC2200ISD::RET_FLAG", SDTNone,
                             [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

// PseudoCALL is a pseudo instruction which will eventually expand to auipc
// and jalr while encoding. This is desirable, as an auipc+jalr pair with
// R_RISCV_CALL and R_RISCV_RELAX relocations can be be relaxed by the linker
// if the offset fits in a signed 21-bit immediate.
// Define AsmString to print "call" when compile with -S flag.
// Define isCodeGenOnly = 0 to support parsing assembly "call" instruction.
let isCall = 1, Defs = [fp, at], isCodeGenOnly = 0 in
def PseudoCALL : Pseudo<(outs), (ins i32imm:$func), []> {
    let AsmString = "call\t$func";
}


def : Pat<(lc2200_call tglobaladdr:$func), (PseudoCALL tglobaladdr:$func)>;
def : Pat<(lc2200_call texternalsym:$func), (PseudoCALL texternalsym:$func)>;

let isBarrier = 1, isReturn = 1, isTerminator = 1 in
 def PseudoRET : Pseudo<(outs), (ins), [(lc2200_ret_flag)]>,
                 PseudoInstExpansion<(JALR zero, ra)>;


let Defs = [sp, ra], Uses = [sp] in {
def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i32imm:$amt, i32imm:$amt2),
                                    [(callseq_start timm:$amt, timm:$amt2)]>;
def ADJCALLSTACKUP   : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                                    [(callseq_end timm:$amt1, timm:$amt2)]>;
}