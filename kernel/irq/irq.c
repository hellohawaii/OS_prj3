#include "irq.h"
#include "time.h"
#include "sched.h"
#include "string.h"

static void irq_timer()
{
    // TODO clock interrupt handler.
    // scheduler, time counter in here to do, emmmmmm maybe.
    scheduler();
}

void interrupt_helper(uint32_t status, uint32_t cause)
{
    // TODO interrupt handler.
    // Leve3 exception Handler.
    // read CP0 register to analyze the type of interrupt.
    
    //void *p=irq_timer;
    //can not use the reg name directly,$26 for k0,$27 for k1,$13 for CP0_CAUSE
    //TODO, why can not? perhaps because these are defined by regs.h
    uint32_t int_cause=get_CP0_CAUSE();
    uint32_t event=cause & 0x0000ff00;
    if(event==0x00008000)
        irq_timer();
    return;
    //in fact always jump to irq_timer, because now all interrupts are clock interrupts
}

void other_exception_handler()
{
    // TODO other exception handler
}
