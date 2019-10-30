    .globl _start
_start:
    addi sp, r0, 0x1000
    call r0, main
    halt
    
    .globl halt
halt:
    jmpi halt

    .globl nothing
nothing:
    ret

    .globl _Z7nothingv
_Z7nothingv:
    ret

    .globl _Z7nothingi
_Z7nothingi:
    ret

