#include "irq.h"
#include "time.h"
#include "sched.h"
#include "string.h"

static void irq_timer()
{
    // TODO clock interrupt handler.
    // scheduler, time counter in here to do, emmmmmm maybe.
    //show_handle_int();
    do_scheduler();
    return;
}

void interrupt_helper(uint32_t status, uint32_t cause)
{
    // TODO interrupt handler.
    // Leve3 exception Handler.
    // read CP0 register to analyze the type of interrupt.
    uint32_t int_cause=get_CP0_CAUSE();
    uint32_t event=cause & 0x0000ff00;
    if(event==0x00008000)
        irq_timer();
    else
        printk("unknown interrupt!,event is %x,cause is %x\n",event, cause);
    return;
    //in fact always jump to irq_timer, because now all interrupts are clock interrupts
}

void other_exception_handler()
{
    // TODO other exception handler
}
