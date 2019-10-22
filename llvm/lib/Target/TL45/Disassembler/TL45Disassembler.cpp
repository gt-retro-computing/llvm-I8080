//===-- TL45Disassembler.cpp - Disassembler for TL45 --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the TL45Disassembler class.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/TL45MCTargetDesc.h"
// #include "TargetInfo/TL45TargetInfo.h"
#include "MCTargetDesc/TL45BaseInfo.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCFixedLenDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "tl45-disassembler"

typedef MCDisassembler::DecodeStatus DecodeStatus;

namespace {
class TL45Disassembler : public MCDisassembler {

public:
  TL45Disassembler(const MCSubtargetInfo &STI, MCContext &Ctx)
      : MCDisassembler(STI, Ctx) {}

  DecodeStatus getInstruction(MCInst &Instr, uint64_t &Size,
                              ArrayRef<uint8_t> Bytes, uint64_t Address,
                              raw_ostream &VStream,
                              raw_ostream &CStream) const override;
};
} // end anonymous namespace

static MCDisassembler *createTL45Disassembler(const Target &T,
                                               const MCSubtargetInfo &STI,
                                               MCContext &Ctx) {
  return new TL45Disassembler(STI, Ctx);
}

extern "C" void LLVMInitializeTL45Disassembler() {
  // Register the disassembler for each target.
  TargetRegistry::RegisterMCDisassembler(TheTL45Target, createTL45Disassembler);

}

static const Register GPRDecoderTable[] = {
    TL45::r0, TL45::r1, TL45::r2, TL45::r3,
    TL45::r4, TL45::r5, TL45::r6, TL45::r7,
    TL45::r8, TL45::r9, TL45::r10, TL45::r11,
    TL45::r12, TL45::r13, TL45::bp, TL45::sp,
};

static DecodeStatus DecodeGRRegsRegisterClass(MCInst &Inst, uint64_t RegNo,
                                           uint64_t Address,
                                           const void *Decoder) {

  if (RegNo > array_lengthof(GPRDecoderTable))
    return MCDisassembler::Fail;

  // We must define our own mapping from RegNo to register identifier.
  // Accessing index RegNo in the register class will work in the case that
  // registers were added in ascending order, but not in general.
  Register Reg = GPRDecoderTable[RegNo];
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

template <unsigned N>
static DecodeStatus decodeUImmOperand(MCInst &Inst, uint64_t Imm,
                                      int64_t Address, const void *Decoder) {
  assert(isUInt<N>(Imm) && "Invalid immediate");

  Inst.addOperand(MCOperand::createImm(Imm));
  return MCDisassembler::Success;
}

template <unsigned N>
static DecodeStatus decodeSImmOperand(MCInst &Inst, uint64_t Imm,
                                      int64_t Address, const void *Decoder) {
  assert(isUInt<N>(Imm) && "Invalid immediate");

  // Sign-extend the number in the bottom N bits of Imm
  Inst.addOperand(MCOperand::createImm(SignExtend64<N>(Imm)));
  return MCDisassembler::Success;
}

#include "TL45GenDisassemblerTables.inc"

DecodeStatus TL45Disassembler::getInstruction(MCInst &MI, uint64_t &Size,
                                               ArrayRef<uint8_t> Bytes,
                                               uint64_t Address,
                                               raw_ostream &OS,
                                               raw_ostream &CS) const {

  uint32_t Insn;
  DecodeStatus Result;

  if (Bytes.size() < 4) {
    Size = 0;
    return MCDisassembler::Fail;
  }

  Insn = support::endian::read32be(Bytes.data());
  Result = decodeInstruction(DecoderTable32, MI, Insn, Address, this, STI);
  Size = 4;

  return Result;
}
