//===- TL45CallingConv.td - Calling Conventions for TL45 -*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This describes the calling conventions for TL45 architecture.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// TL45 Return Value Calling Convention
//===----------------------------------------------------------------------===//
def RetCC_TL45 : CallingConv<[
  // i32 are returned in registers v0
  CCIfType<[i32], CCAssignToReg<[r1]>>,

  // Integer values get stored in stack slots that are 4 bytes in
  // size and 4-byte aligned.
  CCIfType<[i32], CCAssignToStack<4, 4>>
]>;

//===----------------------------------------------------------------------===//
// TL45 Argument Calling Conventions
//===----------------------------------------------------------------------===//
def CC_TL45 : CallingConv<[
  // Promote i8 arguments to i32.
  CCIfType<[i8], CCPromoteToType<i32>>,

  // Promote i16 arguments to i32.
  CCIfType<[i16], CCPromoteToType<i32>>,

  // The first 3 integer arguments are passed in integer registers.
  CCIfType<[i32], CCAssignToReg<[r2, r3, r4, r5, r6]>>,

  // Integer values get stored in stack slots that are 4 bytes in
  // size and 4-byte aligned.
  CCIfType<[i32], CCAssignToStack<4, 4>>
]>;

def CC_Save : CalleeSavedRegs<(add r7, r8, r9, r10, r11, r12, r13, bp)>;
