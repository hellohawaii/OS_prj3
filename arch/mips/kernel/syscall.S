#include "asm.h"

LEAF(invoke_syscall) //the kernel function is executed during running this function
    // TODO syscall entry
    //will accept sveral paremeters
    //needed parameters: syscall number:v0 parameter:a0,a1,a2
    //prepared parameters: syscall number: a0, arg1:a1, arg2:a2, arg3:a3
    or v0,a0,zero
    or a0,a1,zero
    or a1,a2,zero
    or a2,a3,zero
    syscall
    //at this time, will use the function of the kernel, may get some value returned.
    jr ra
    //syscall end, go back to executer the instruction after calling syscall
END(invoke_syscall)
