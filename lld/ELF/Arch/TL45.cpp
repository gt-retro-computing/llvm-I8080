//===- TL45.cpp ---------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// The TL45 is a 16-bit microcontroller RISC architecture. The instruction set
// has only 27 core instructions orthogonally augmented with a variety
// of addressing modes for source and destination operands. Entire address space
// of TL45 is 64KB (the extended TL45X architecture is not considered here).
// A typical TL45 MCU has several kilobytes of RAM and ROM, plenty
// of peripherals and is generally optimized for a low power consumption.
//
//===----------------------------------------------------------------------===//

#include "InputFiles.h"
#include "Symbols.h"
#include "Target.h"
#include "lld/Common/ErrorHandler.h"
#include "llvm/Object/ELF.h"
#include "llvm/Support/Endian.h"

using namespace llvm;
using namespace llvm::object;
using namespace llvm::support::endian;
using namespace llvm::ELF;
using namespace lld;
using namespace lld::elf;

namespace {
class TL45 final : public TargetInfo {
public:
  TL45();
  RelExpr getRelExpr(RelType type, const Symbol &s,
                     const uint8_t *loc) const override;
  void relocateOne(uint8_t *loc, RelType type, uint64_t val) const override;


};
} // namespace

TL45::TL45() {

  // FIXME add trap instruction
  trapInstr = {0x00, 0x00, 0x00, 0x00};

  execFirst = true;
  config->imageBase = Optional<uint64_t >(0);
}

RelExpr TL45::getRelExpr(RelType type, const Symbol &s,
                           const uint8_t *loc) const {
  return R_ABS;
}

inline void checkIntValid(uint8_t *loc, uint64_t v, RelType type) {
  checkAlignment(loc, v, 4, type);
}

void TL45::relocateOne(uint8_t *loc, RelType type, uint64_t val) const {
  switch (type) {
//  case R_TL45_32:
//    checkIntUInt(loc, val, 8, type);
//    *loc = val;
//    break;
  case R_TL45_LO16_I:
    checkIntValid(loc, val, type);
    val >>= 2;

    checkIntUInt(loc, val, 32, type);
    write16be(loc+2, val & 0xFFFF);
    break;
  case R_TL45_HI16_I:
    checkIntValid(loc, val, type);
    val >>= 2;

    checkIntUInt(loc, val, 32, type);
    val >>= 16;
    write16be(loc+2, val & 0xFFFF);
    break;
  case R_TL45_32:
    checkIntValid(loc, val, type);
    val >>= 2;

    checkIntUInt(loc, val, 32, type);
    write32be(loc, val);
    break;
//  case R_TL45_16:
//  case R_TL45_16_PCREL:
//  case R_TL45_16_BYTE:
//  case R_TL45_16_PCREL_BYTE:
//    checkIntUInt(loc, val, 16, type);
//    write16le(loc, val);
//    break;
//  case R_TL45_32:
//    checkIntUInt(loc, val, 32, type);
//    write32le(loc, val);
//    break;
//  case R_TL45_10_PCREL: {
//    int16_t offset = ((int16_t)val >> 1) - 1;
//    checkInt(loc, offset, 10, type);
//    write16le(loc, (read16le(loc) & 0xFC00) | (offset & 0x3FF));
//    break;
//  }
  default:
    error(getErrorLocation(loc) + "unrecognized relocation " + toString(type));
  }
}

TargetInfo *elf::getTL45TargetInfo() {
  static TL45 target;
  return &target;
}
