#include "barrier.h"
#include "sched.h"

void do_barrier_init(barrier_t *barrier, int goal)
{
    barrier->num_expected=goal;
    barrier->num_now=0;
    queue_init(&(barrier->waiting_queue));
}

void do_barrier_wait(barrier_t *barrier)
{
    (barrier->num_now)=(barrier->num_now)+1;
    if((barrier->num_now) < (barrier->num_expected)){
        do_block(&(barrier->waiting_queue));
        do_scheduler();//must have this
    }else if((barrier->num_now) == (barrier->num_expected)){
        do_unblock_all(&(barrier->waiting_queue));
        barrier->num_now=0;
        do_scheduler();
    }else{
        printk("wrong while waiting!expected:%d,now:%d\n",barrier->num_expected,barrier->num_now);
    }
}
