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

//===----------------------------------------------------------------------===//
// Selection DAG types
//===----------------------------------------------------------------------===//
def SDT_CallSeqStart      : SDCallSeqStart<[SDTCisVT<0, i32>,
                                            SDTCisVT<1, i32>]>;
def SDT_CallSeqEnd        : SDCallSeqEnd<[SDTCisVT<0, i32>,
                                          SDTCisVT<1, i32>]>;

def SDT_LC2200Call        : SDTypeProfile<0, -1, [SDTCisVT<0, i32>]>;

def SDT_LC2200Jmp         : SDTypeProfile<0, 1, [SDTCisVT<0, OtherVT>]>;

def SDT_LC2200CmpSkip     : SDTypeProfile<0, 3, [SDTCisVT<0, i32>, SDTCisVT<1, i32>, SDTCisVT<2, i32>]>;

def SDT_LC2200SelectMove  : SDTypeProfile<1, 2, [SDTCisVT<0, i32>, SDTCisVT<1, i32>, SDTCisVT<2, i32>]>;

def SDT_LC2200Nand        : SDTypeProfile<1, 2, [SDTCisVT<0, i32>, SDTCisVT<1, i32>, SDTCisVT<2, i32>]>;


//===----------------------------------------------------------------------===//
// Selection DAG Nodes
//===----------------------------------------------------------------------===//
def callseq_start         : SDNode<"ISD::CALLSEQ_START", SDT_CallSeqStart,
                                   [SDNPHasChain, SDNPOutGlue]>;
def callseq_end           : SDNode<"ISD::CALLSEQ_END", SDT_CallSeqEnd,
                                   [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;


def lc2200_call           : SDNode<"LC2200ISD::CALL", SDT_LC2200Call,
                                   [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue,
                                   SDNPVariadic]>;

def lc2200_ret            : SDNode<"LC2200ISD::RET", SDTNone,
                                   [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

// Implements a conditional comparison: generates skips/goto to
// conditionally execute the immediately following instruction
// Paired with a following JMP, it forms a BR_CC node.
// Operands: [CC, A, B]
def lc2200_cmp_skip       : SDNode<"LC2200ISD::CMP_SKIP", SDT_LC2200CmpSkip, [SDNPOutGlue]>;

// Implements a selectable move: generates two sequential
// moves to the destination, first from the False source, then
// from the True source. Paired with a preceding CMP_SKIP, it forms
// a SELECT_CC.
// Operands: [T, F]
def lc2200_select_move    : SDNode<"LC2200ISD::SELECT_MOVE", SDT_LC2200SelectMove, [SDNPInGlue]>;

// Implements a direct unconditional branch
// Operands: [Dst]
def lc2200_jmp            : SDNode<"LC2200ISD::JMP", SDT_LC2200Jmp, [SDNPHasChain, SDNPOptInGlue]>;

// Implements NAND
// Operands: [Dst, Src1, Src2]
def lc2200_nand           : SDNode<"LC2200ISD::NAND", SDT_LC2200Nand, []>;

//==========================
// R Type Instructions
//==========================

def ADD : RTypeInstr<0b0000, "add",
    [(set i32:$dr, (add i32:$sr1, i32:$sr2))]> {
    let isAdd = 1;
}
def NAND: RTypeInstr<0b0001, "nand",
    [(set i32:$dr, (lc2200_nand (i32 GRRegs:$sr1), (i32 GRRegs:$sr2)))]>;


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

def LW: ITypeInstr<0b0011, (outs GRRegs:$dst), (ins GRRegs:$base, LC2200imm20:$off20),
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

def SW: ITypeInstr<0b0100, (outs), (ins GRRegs:$src1, GRRegs:$base, LC2200imm20:$offset),
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
//    [(br bb:$offset)]> {
    []> {
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

def SKPE  : SKPTypeInstr<0b1000, 0b0000, "skpe",  []>;
def SKPLT : SKPTypeInstr<0b1000, 0b0001, "skplt", []>;

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
// PseudoCALL is a pseudo instruction which will eventually expand to auipc
// and jalr while encoding. This is desirable, as an auipc+jalr pair with
// R_RISCV_CALL and R_RISCV_RELAX relocations can be be relaxed by the linker
// if the offset fits in a signed 21-bit immediate.
// Define AsmString to print "call" when compile with -S flag.
// Define isCodeGenOnly = 0 to support parsing assembly "call" instruction.
let isCall = 1, Defs = [ra, at], isCodeGenOnly = 1 in
def PseudoCALL : Pseudo<(outs), (ins i32imm:$func), []> {
    let AsmString = "call\t$func";
}

// PseudoRET
def : Pat<(lc2200_call tglobaladdr:$func), (PseudoCALL tglobaladdr:$func)>;
def : Pat<(lc2200_call texternalsym:$func), (PseudoCALL texternalsym:$func)>;

let isReturn = 1, isBarrier = 1, isTerminator = 1 in {
  def PseudoRET : Pseudo<(outs), (ins), [(lc2200_ret)]>,
                         PseudoInstExpansion<(JALR zero, ra)>;
}

// ADJCALLSTACKDOWN/UP
let Defs = [sp, ra], Uses = [sp] in {
  def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i32imm:$amt, i32imm:$amt2),
                                [(callseq_start timm:$amt, timm:$amt2)]>;
  def ADJCALLSTACKUP   : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                                [(callseq_end timm:$amt1, timm:$amt2)]>;
}

// CMP_SKIP
def CMP_SKIP : Pseudo<(outs), (ins cc_val:$cc, GRRegs:$a, GRRegs:$b),
                     [(lc2200_cmp_skip (i32 cc_val:$cc), (i32 GRRegs:$a), (i32 GRRegs:$b))]> {
  let hasSideEffects = 1;
}

// JMP
def JMP : Pseudo<(outs), (ins b_target:$dst),
                 [(lc2200_jmp bb:$dst)]> {
  // let Defs = [at];
  let isBarrier = 1;
  let isTerminator = 1;
  let isBranch = 1;
}

// SELECT_MOVE
def SELECT_MOVE : Pseudo<(outs GRRegs:$dst), (ins GRRegs:$src1, GRRegs:$src2),
                         [(set (i32 GRRegs:$dst), (lc2200_select_move (i32 GRRegs:$src1), (i32 GRRegs:$src2)))]>;
