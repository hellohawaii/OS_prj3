#include "sem.h"
#include "stdio.h"

void do_semaphore_init(semaphore_t *s, int val)
{
    if(val>=0){
        s->sem=val;
        queue_init(&(s->waiting_queue));
    }else{
        printk("wrong init value of semaphore\n");
    }
}

void do_semaphore_up(semaphore_t *s)
{
    (s->sem)=(s->sem)+1;
    if((s->sem)<=0){
        do_unblock_one(&(s->waiting_queue));//this function will also change the status
    }
    do_scheduler();//the operation have finished before here, so the do_scheduler will not destory the atomicity of this operation
}

void do_semaphore_down(semaphore_t *s)
{
    (s->sem)=(s->sem)-1;
    if((s->sem)<0){
        do_block(&(s->waiting_queue));//this function will also change the status
    }
    do_scheduler();//the operation have finished before here, so the do_scheduler will not destory the atomicity of this operation
}
