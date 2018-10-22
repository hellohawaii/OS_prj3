#include "asm.h"

.equ    KERNEL, 0
.equ    USER,   156

.macro STI
.endm

.macro CLI
.endm

LEAF(asm_start)
    mtc0    zero, CP0_STATUS
    mtc0    zero, CP0_WATCHLO
    mtc0    zero, CP0_WATCHHI

    mfc0    t0, CP0_CONFIG
    and     t0, ~0x7
    ori     t0, 0x2
    mtc0    t0, CP0_CONFIG
    
    jr  ra
END(asm_start)

.macro SAVE_CONTEXT offset
    // TODO save context
    lw k0,current_running
    //TODO:not sure, if current_running is the addr of variable current_running
    //then should use lw. Or we should use la
    // now, t0 is current_running, i.e. the addr of current pcb
    //sw $1,4(k0)#seems that can not use at as register name
    //may not save and restore at
    sw v0,8(k0)
    sw v1,12(k0)
    sw a0,16(k0)
    sw a1,20(k0)
    sw a2,24(k0)
    sw a3,28(k0)
    sw t0,32(k0)
    sw t1,36(k0)
    sw t2,40(k0)
    sw t3,44(k0)
    sw t4,48(k0)
    sw t5,52(k0)
    sw t6,56(k0)
    sw t7,60(k0)
    sw s0,64(k0)
    sw s1,68(k0)
    sw s2,72(k0)
    sw s3,76(k0)
    sw s4,80(k0)
    sw s5,84(k0)
    sw s6,88(k0)
    sw s7,92(k0)
    sw t8,96(k0)
    #sw t9,100(k0)
    #sw k0,104(k0)
    sw k1,108(k0)
    sw gp,112(k0)
    sw sp,116(k0)
    sw s8,120(k0)
    sw ra,124(k0)

    //can not store special registers into memery directly
    mfc0 k1, CP0_STATUS
    sw k1,128(k0)
    mfc0 k1, CP0_TAGHI
    sw k1,132(k0)
    mfc0 k1, CP0_TAGLO
    sw k1,136(k0)
    mfc0 k1, CP0_BADVADDR
    sw k1,140(k0)
    mfc0 k1, CP0_CAUSE
    sw k1,144(k0)
    mfc0 k1, CP0_EPC
    sw k1,148(k0)
    //not sure, is it necessary to store all of these special regs?
    //TODO, store PC, it is important when using clock inter, need to know stopat where
.endm

.macro RESTORE_CONTEXT offset
    // TODO restore context
    lw k0,current_running
    //not sure, if current_running is the addr of variable current_running
    //then should use lw. Or we should use la
    // now, t0 is current_running, i.e. the addr of current pcb
    //lw $1,4(k0)
    lw v0,8(k0)
    lw v1,12(k0)
    lw a0,16(k0)
    lw a1,20(k0)
    lw a2,24(k0)
    lw a3,28(k0)
    lw t0,32(k0)
    lw t1,36(k0)
    lw t2,40(k0)
    lw t3,44(k0)
    lw t4,48(k0)
    lw t5,52(k0)
    lw t6,56(k0)
    lw t7,60(k0)
    lw s0,64(k0)
    lw s1,68(k0)
    lw s2,72(k0)
    lw s3,76(k0)
    lw s4,80(k0)
    lw s5,84(k0)
    lw s6,88(k0)
    lw s7,92(k0)
    lw t8,96(k0)
    #lw t9,100(k0)
    #lw k0,104(k0)
    lw k1,108(k0)
    lw gp,112(k0)
    lw sp,116(k0)
    lw s8,120(k0)
    lw ra,124(k0)

    lw k1,128(k0)
    mtc0 k1,CP0_STATUS
    lw k1,132(k0)
    mtc0 k1,CP0_TAGHI
    lw k1,136(k0)
    mtc0 k1,CP0_TAGLO
    lw k1,140(k0)
    mtc0 k1,CP0_BADVADDR
    lw k1,144(k0)
    mtc0 k1,CP0_CAUSE
    lw k1,148(k0)
    mtc0 k1,CP0_EPC
nop
.endm

NESTED(do_scheduler, 0, ra)
    SAVE_CONTEXT(KERNEL)
    jal     scheduler
    RESTORE_CONTEXT(KERNEL)
    jr      ra
END(do_scheduler)

.global exception_handler_begin
.global exception_handler_end

NESTED(exception_handler_entry, 0, sp)   
exception_handler_begin:
    // TODO close interrupt
    // jmp exception_handler[i] which decided by CP0_CAUSE
    // Leve2 exception Handler.
exception_handler_end:
END(exception_handler_entry)

NESTED(handle_int, 0, sp)
    // interrupt handler
    // Leve3 exception Handler.
END(handle_int)

NESTED(handle_syscall, 0, sp)
    // system call handler
END(handle_syscall)

NESTED(handle_other, 0, sp)
    // other exception handler
END(handle_other)


