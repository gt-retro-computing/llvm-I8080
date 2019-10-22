//===-- TL45FixupKinds.h - TL45 Specific Fixup Entries --------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TL45_MCTARGETDESC_TL45FIXUPKINDS_H
#define LLVM_LIB_TARGET_TL45_MCTARGETDESC_TL45FIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

#undef TL45

namespace llvm {
namespace TL45 {
enum Fixups {
  // fixup_tl45_hi20 - 20-bit fixup corresponding to hi(foo) for
  // instructions like lui
  fixup_tl45_hi16_i = FirstTargetFixupKind,
  // fixup_tl45_lo12_i - 12-bit fixup corresponding to lo(foo) for
  // instructions like addi
  fixup_tl45_lo16_i,

  fixup_tl45_32,

  // fixup_tl45_invalid - used as a sentinel and a marker, must be last fixup
  fixup_tl45_invalid,
  NumTargetFixupKinds = fixup_tl45_invalid - FirstTargetFixupKind
};
} // end namespace TL45
} // end namespace llvm

#endif
