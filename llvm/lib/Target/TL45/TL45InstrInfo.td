//===-- TL45InstrInfo.td - Target Description for TL45 -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the TL45 instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

include "TL45InstrFormats.td"

//===----------------------------------------------------------------------===//
// Selection DAG types
//===----------------------------------------------------------------------===//
def SDT_CallSeqStart      : SDCallSeqStart<[SDTCisVT<0, i32>,
                                            SDTCisVT<1, i32>]>;
def SDT_CallSeqEnd        : SDCallSeqEnd<[SDTCisVT<0, i32>,
                                          SDTCisVT<1, i32>]>;

def SDT_Call        : SDTypeProfile<0, -1, [SDTCisVT<0, i32>]>;

// [cc, lhs, rhs, dest]
def SDT_TL45CmpJmp     : SDTypeProfile<0, 4, [SDTCisVT<0, i32>, SDTCisVT<1, i32>, SDTCisVT<2, i32>]>;

def SDT_TL45SelectCC : SDTypeProfile<1, 5, [SDTCisSameAs<1, 2>,
                                             SDTCisSameAs<0, 4>,
                                             SDTCisSameAs<4, 5>]>;

//===----------------------------------------------------------------------===//
// Selection DAG Nodes
//===----------------------------------------------------------------------===//
def callseq_start         : SDNode<"ISD::CALLSEQ_START", SDT_CallSeqStart,
                                   [SDNPHasChain, SDNPOutGlue]>;
def callseq_end           : SDNode<"ISD::CALLSEQ_END", SDT_CallSeqEnd,
                                   [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;


def tl45_call           : SDNode<"TL45ISD::CALL", SDT_Call,
                                   [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue,
                                   SDNPVariadic]>;

def tl45_ret            : SDNode<"TL45ISD::RET", SDTNone,
                                   [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def tl45_selectcc  : SDNode<"TL45ISD::SELECT_CC", SDT_TL45SelectCC,
                             [SDNPInGlue]>;


// Implements a conditional jump: generates skips/goto to
// conditionally execute the immediately following instruction
// Paired with a following JMP, it forms a BR_CC node.
// Operands: [CC, A, B]
def tl45_cmp_jmp       : SDNode<"TL45ISD::CMP_JMP", SDT_TL45CmpJmp, [SDNPHasChain, SDNPOptInGlue]>;



//===----------------------------------------------------------------------===//
// Instructions
//===----------------------------------------------------------------------===//

//  -debug-only=asm-matcher

let Defs = [eflags], isAsCheapAsAMove = 1 in {
  let isAdd = 1, isCommutable = 1 in
    defm ADD : ALUTypeInstr<0x1, "add", add>;
  defm SUB : ALUTypeInstr<0x2, "sub", sub>;
  // defm MUL : ALUTypeInstr<0x3, "mul", mul>;

  let isCommutable = 1 in {
    defm OR : ALUTypeInstr<0x6, "or", or>;
    defm XOR : ALUTypeInstr<0x7, "xor", xor>;
    defm AND : ALUTypeInstr<0x8, "and", and>;
  }

  let mode = 0b001, isCompare = 1, isTerminator = 1 in
    def CMP : RRCmpTypeInstr<0x5, "cmp", []>;
  let mode = 0b101, isCompare = 1, isTerminator = 1 in
    def CMPI : RICmpTypeInstr<0x5, "cmpi", []>;
}

let mode = 0b001, isAsCheapAsAMove = 1 in {
  def XCHG : RRTypeInstr<0x4, "xchg", []>;

  let Defs = [eflags] in
    def NOT : RRTypeInstr<0x9, "not", []>;
}

def IN : RITypeInstr<0x10, "in", []>;
def OUT : RIOutTypeInstr<0x11, "out", []>;

let mode = 0b001 in {
  let mayLoad = 1 in
    def LW : RRITypeInstr<0x16, "lw", [
      (set (i32 GRRegs:$dr), (load (add (i32 GRRegs:$sr1), TL45imm16:$imm)))
    ]>;

  // yes it's backward for SW. MEM[$sr1+imm] = $dr
  let mayStore = 1 in
    def SW : RRIStoreTypeInstr<0x17, "sw", [
      (store i32:$dr, (add (i32 GRRegs:$sr1), TL45imm16:$imm)),
      (store i32:$dr, (i32 GRRegs:$sr1))
    ]>;
}

def PUSH : RRITypeInstr<0x14, "push", [], "$dr">;
def POP : RRITypeInstr<0x15, "pop", [], "$dr">;

// multiclass JTypeInstr<bits<5> opcode, string opcodestr>

let Uses = [eflags], isBranch = 1, isTerminator = 1 in {
  defm JO : JTypeInstr<0xC, 0x0, "jo">;
  defm JNO : JTypeInstr<0xC, 0x1, "jno">;
  defm JS : JTypeInstr<0xC, 0x2, "js">;
  defm JNS : JTypeInstr<0xC, 0x3, "jns">;

  defm JE : JTypeInstr<0xC, 0x4, "je">;
  defm JNE : JTypeInstr<0xC, 0x5, "jne">;
  defm JB : JTypeInstr<0xC, 0x6, "jb">;
  defm JNB : JTypeInstr<0xC, 0x7, "jnb">;

  defm JBE : JTypeInstr<0xC, 0x8, "jbe">;
  defm JA : JTypeInstr<0xC, 0x9, "ja">;
  defm JL : JTypeInstr<0xC, 0xA, "jl">;
  defm JGE : JTypeInstr<0xC, 0xB, "jge">;

  defm JLE : JTypeInstr<0xC, 0xC, "jle">;
  defm JG : JTypeInstr<0xC, 0xD, "jg">;
}

let isBranch = 1, isTerminator = 1, isBarrier = 1 in
  defm JMP : JTypeInstr<0xC, 0xF, "jmp", [(br bb:$imm)]>;

let isTerminator = 1, isBarrier = 1, isReturn = 1, dr = 0b1111 in
  def RET : NoneTypeInstr<0xE, "ret", [(tl45_ret)]>;

let isBarrier = 1, hasSideEffects = 1 in
  def HALT : NoneTypeInstr<0x0, "halt", []>;

let isCall = 1 in
  def CALL : CallTypeInstr<0xD, "call", [(tl45_call GRRegs:$base, TL45imm16:$imm)]>;


// let isCall = 1 in
//   def CALLI : CallITypeInstr<0xD, "calli", [(tl45_call TL45imm16:$imm)]>;


/*  Various Patterns and Pseudo */

def : Pat<(store GRRegs:$rs2, GRRegs:$rs1), (SW GRRegs:$rs2, GRRegs:$rs1, 0)>;
def : Pat<(truncstorei8 GRRegs:$rs2, GRRegs:$rs1), (SW GRRegs:$rs2, GRRegs:$rs1, 0)>;

/// Predicates

def IsOrAdd: PatFrag<(ops node:$A, node:$B), (or node:$A, node:$B), [{
  return isOrEquivalentToAdd(N);
}]>;

/// Immediates

def : Pat<(TL45imm16:$imm), (ADDI r0, TL45imm16:$imm)>;

// Addressing modes.
// Necessary because a frameindex can't be matched directly in a pattern.
def AddrFI : ComplexPattern<iPTR, 1, "SelectAddrFI", [frameindex], []>;

/// FrameIndex calculations

def : Pat<(add (i32 AddrFI:$Rs), TL45imm16:$imm12),
          (ADDI (i32 AddrFI:$Rs), TL45imm16:$imm12)>;
def : Pat<(IsOrAdd (i32 AddrFI:$Rs), TL45imm16:$imm12),
        (ADDI (i32 AddrFI:$Rs), TL45imm16:$imm12)>;

// FrameIndex Loads

multiclass LdPat<PatFrag LoadOp, InstTL45 Inst> {
  def : Pat<(LoadOp GRRegs:$rs1), (Inst GRRegs:$rs1, 0)>;
  def : Pat<(LoadOp AddrFI:$rs1), (Inst AddrFI:$rs1, 0)>;

  def : Pat<(LoadOp (add GRRegs:$rs1, TL45imm16:$imm12)),
            (Inst GRRegs:$rs1, TL45imm16:$imm12)>;

  def : Pat<(LoadOp (add AddrFI:$rs1, TL45imm16:$imm12)),
            (Inst AddrFI:$rs1, TL45imm16:$imm12)>;
  def : Pat<(LoadOp (IsOrAdd AddrFI:$rs1, TL45imm16:$imm12)),
            (Inst AddrFI:$rs1, TL45imm16:$imm12)>;
}

defm : LdPat<load, LW>;


/*
let isReturn = 1, isBarrier = 1, isTerminator = 1 in {
  def PseudoRET : Pseudo<(outs), (ins), [(lc2200_ret)]>, PseudoInstExpansion<(RET)>;
}
*/

def : Pat<(tl45_call tglobaladdr:$func), (CALL r0, tglobaladdr:$func)>;
def : Pat<(tl45_call texternalsym:$func), (CALL r0, texternalsym:$func)>;

def : Pat<(tl45_call texternalsym:$func), (CALL r0, texternalsym:$func)>;

def : Pat<(tl45_call GRRegs:$func), (CALL GRRegs:$func, 0)>;

// ADJCALLSTACKDOWN/UP
let Defs = [sp], Uses = [sp] in {
  def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i32imm:$amt, i32imm:$amt2),
                                [(callseq_start timm:$amt, timm:$amt2)]>;
  def ADJCALLSTACKUP   : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                                [(callseq_end timm:$amt1, timm:$amt2)]>;
}
// 01101

// CMP_JMP
def CMP_JMP : Pseudo<(outs), (ins cc_val:$cc, GRRegs:$a, GRRegs:$b, b_target:$dst),
                     [(tl45_cmp_jmp (i32 cc_val:$cc), (i32 GRRegs:$a), (i32 GRRegs:$b), bb:$dst)]> {
  let hasSideEffects = 1;
  let isBranch = 1;
  let isTerminator = 1;
  let isCompare    = 1;
}



// 9 == the CC for not equal
// def : Pat<(brcond GRRegs:$a, bb:$dst), (JNEI 9, $a, zero, bb:$dst)>;

/*
class BccPat<PatFrag CondOp, InstTL45 Inst>
    : Pat<(brcond (CondOp GRRegs:$rs1, GRRegs:$rs2), bb:$imm12),
          (Inst GRRegs:$rs1, GRRegs:$rs2, TL45imm16:$imm12)>;

def : BccPat<seteq, JEI>;
def : BccPat<setne, JNEI>;
def : BccPat<setlt, JLI>;
def : BccPat<setge, JGEI>;
def : BccPat<setult, JBI>;
def : BccPat<setuge, JNBI>;
*/


def : Pat<(int_tl45_out i32:$a, TL45imm16:$b), (OUT i32:$a, TL45imm16:$b)>;
def : Pat<(int_tl45_in TL45imm16:$b), (IN TL45imm16:$b)>;


/*


    ins_class_call("CALL", 0xD),
    ins_class_ret("RET", 0xE),

    # HALT
    ins_class_none("HALT", 0x0)


*/


let usesCustomInserter = 1 in
  class SelectCC_rrirr<RegisterClass valty, RegisterClass cmpty>
    : Pseudo<(outs valty:$dst),
             (ins cmpty:$lhs, cmpty:$rhs, TL45imm16:$imm,
              valty:$truev, valty:$falsev),
             [(set valty:$dst, (tl45_selectcc cmpty:$lhs, cmpty:$rhs,
              (i32 imm:$imm), valty:$truev, valty:$falsev))]>;

def Select_GRRegs_Using_CC_GRRegs : SelectCC_rrirr<GRRegs, GRRegs>;