// REQUIRES: aarch64
// RUN: llvm-mc -filetype=obj -triple=aarch64-none-linux %s -o %t.o
// RUN: ld.lld %t.o -o %t
// RUN: llvm-objdump -d --no-show-raw-insn %t | FileCheck %s

// Check that the ARM 64-bit ABI rules for undefined weak symbols are applied.
// Branch instructions are resolved to the next instruction. Undefined
// Symbols in relative are resolved to the place so S - P + A = A.

 .weak target

 .text
 .global _start
_start:
// R_AARCH64_JUMP26
 b target
// R_AARCH64_CALL26
 bl target
// R_AARCH64_CONDBR19
 b.eq target
// R_AARCH64_TSTBR14
 cbz x1, target
// R_AARCH64_ADR_PREL_LO21
 adr x0, target
// R_AARCH64_ADR_PREL_PG_HI21
 adrp x0, target
// R_AARCH64_PREL32
 .word target - .
// R_AARCH64_PREL64
 .xword target - .
// R_AARCH64_PREL16
 .hword target - .
// R_AARCH64_LD_PREL_LO19
 ldr x8, target

// CHECK: Disassembly of section .text:
// CHECK-EMPTY:
// 2162688 = 0x210000
// CHECK:         210120:       b       #4
// CHECK-NEXT:    210124:       bl      #4
// CHECK-NEXT:    210128:       b.eq    #4
// CHECK-NEXT:    21012c:       cbz     x1, #4
// CHECK-NEXT:    210130:       adr     x0, #0
// CHECK-NEXT:    210134:       adrp    x0, #0
// CHECK:         210138:       00 00 00 00 .word   0x00000000
// CHECK-NEXT:    21013c:       00 00 00 00 .word   0x00000000
// CHECK-NEXT:    210140:       00 00 00 00 .word   0x00000000
// CHECK-NEXT:    210144:       00 00 .short  0x0000
// CHECK:         $x.2:
// CHECK-NEXT:    210146:       ldr     x8, #0
