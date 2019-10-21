//===- IntrinsicsTl45.td - Defines Tl45 intrinsics ---------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines all of the TL45-specific intrinsics.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// TL45 DSP data types
def tl45_v2q15_ty: LLVMType<v2i16>;
def tl45_v4q7_ty: LLVMType<v4i8>;
def tl45_q31_ty: LLVMType<i32>;

let TargetPrefix = "tl45" in {  // All intrinsics start with "llvm.tl45.".

def int_tl45_out : GCCBuiltin<"__builtin_tl45_out">,
  Intrinsic<[], [llvm_i32_ty, llvm_i32_ty], [ImmArg<1>], "llvm.tl45.out">;

def int_tl45_in : GCCBuiltin<"__builtin_tl45_in">,
  Intrinsic<[llvm_i32_ty], [llvm_i32_ty], [ImmArg<0>], "llvm.tl45.in">;

}
