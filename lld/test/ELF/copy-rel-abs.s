# REQUIRES: x86

## Test SHN_ABS symbols are not copy relocated.

# RUN: llvm-mc -filetype=obj -triple=x86_64 %p/Inputs/copy-rel-abs.s -o %t1.o
# RUN: ld.lld -shared %t1.o -o %t1.so
# RUN: llvm-readelf --dyn-syms %t1.so | FileCheck --check-prefix=SYMS %s

# SYMS: [[ADDR:[0-9a-f]+]] {{.*}}   5 bar
# SYMS: [[ADDR]]           {{.*}}   5 foo
# SYMS: [[ADDR]]           {{.*}} ABS zed

# RUN: llvm-mc -filetype=obj -triple=x86_64 %s -o %t2.o
# RUN: ld.lld %t2.o %t1.so -o %t2
# RUN: llvm-objdump -t %t2 | FileCheck %s --implicit-check-not=zed

# CHECK: SYMBOL TABLE:
# CHECK: .bss.rel.ro {{.*}} foo
# CHECK: .bss.rel.ro {{.*}} bar

.global _start
_start:
.quad foo
