//===- LC2200CallingConv.td - Calling Conventions for LC2200 -*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This describes the calling conventions for LC2200 architecture.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// LC2200 Return Value Calling Convention
//===----------------------------------------------------------------------===//
def RetCC_LC2200 : CallingConv<[
  // i32 are returned in registers v0
  CCIfType<[i32], CCAssignToReg<[v0]>>,

  // Integer values get stored in stack slots that are 4 bytes in
  // size and 4-byte aligned.
  CCIfType<[i32], CCAssignToStack<4, 4>>
]>;

//===----------------------------------------------------------------------===//
// LC2200 Argument Calling Conventions
//===----------------------------------------------------------------------===//
def CC_LC2200 : CallingConv<[
  // Promote i8 arguments to i32.
  CCIfType<[i8], CCPromoteToType<i32>>,

  // Promote i16 arguments to i32.
  CCIfType<[i16], CCPromoteToType<i32>>,

  // The first 3 integer arguments are passed in integer registers.
  CCIfType<[i32], CCAssignToReg<[a0, a1, a2]>>,

  // Integer values get stored in stack slots that are 4 bytes in
  // size and 4-byte aligned.
  CCIfType<[i32], CCAssignToStack<4, 4>>
]>;

def CC_Save : CalleeSavedRegs<(add s0, s1, s2, fp)>;
