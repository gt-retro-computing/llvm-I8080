//===-- ECE2031InstrInfo.td - Target Description for ECE2031 -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the ECE2031 instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

include "ECE2031InstrFormats.td"

//===----------------------------------------------------------------------===//
// Selection DAG types
//===----------------------------------------------------------------------===//
def SDT_CallSeqStart      : SDCallSeqStart<[SDTCisVT<0, i32>,
                                            SDTCisVT<1, i32>]>;
def SDT_CallSeqEnd        : SDCallSeqEnd<[SDTCisVT<0, i32>,
                                          SDTCisVT<1, i32>]>;

def SDT_ECE2031Call        : SDTypeProfile<0, -1, [SDTCisVT<0, i32>]>;

def SDT_ECE2031Jmp         : SDTypeProfile<0, 1, [SDTCisVT<0, OtherVT>]>;

//===----------------------------------------------------------------------===//
// Selection DAG Nodes
//===----------------------------------------------------------------------===//
def callseq_start         : SDNode<"ISD::CALLSEQ_START", SDT_CallSeqStart,
                                   [SDNPHasChain, SDNPOutGlue]>;
def callseq_end           : SDNode<"ISD::CALLSEQ_END", SDT_CallSeqEnd,
                                   [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;


def lc2200_call           : SDNode<"ECE2031ISD::CALL", SDT_ECE2031Call,
                                   [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue,
                                   SDNPVariadic]>;

def lc2200_ret            : SDNode<"ECE2031ISD::RET", SDTNone,
                                   [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

//==========================
// Instructions
//==========================
def NOP : InstECE2031<0b000000, (outs), (ins),
    "NOP", "", []
>;

def LOAD : InstECE2031<0b000001, (outs AccReg:$dst), (ins ECE2031imm10:$address),
    "LOAD", "${address}",
    [(set i16:$dst) (load ECE2031imm10:$address))]> {
    let Immediate = address;
    let mayLoad = 1;
}

def STORE : InstECE2031<0b000010, (outs), (ins AccReg:$src ECE2031imm10:$address),
    "STORE", "${address}",
    [(store i16:$dst ECE2031imm10:$address)]> {
    let Immediate = address;
    let mayStore = 1;
}

def ADD : InstECE2031<0b000011, (outs AccReg:$dst), (ins AccReg:$src ECE2031imm10:$address),
    "ADD", "${address}",
    [(set i16:$dst (add i32:$src (load ECE2031imm10:$address)))]> {
    let Immediate = address;
    let isAdd = 1;
}

def SUB : InstECE2031<0b000011, (outs AccReg:$dst), (ins AccReg:$src ECE2031imm10:$address),
    "SUB", "${address}",
    [(set i16:$dst (sub i32:$src (load ECE2031imm10:$address)))]> {
    let Immediate = address;
}

def JUMP : InstECE2031<0b000011, (outs), (ins b_target:$address),
    "JUMP", "${address}",
    [(br bb:$address]> {
    let Immediate = address;
    let isBarrier = 1;
    let isTerminator = 1;
}


//===----------------------------------------------------------------------===//
// Pseudo Instructions
//===----------------------------------------------------------------------===//
