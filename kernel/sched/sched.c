#include "lock.h"
#include "time.h"
#include "stdio.h"
#include "sched.h"
#include "queue.h"
#include "screen.h"

pcb_t pcb[NUM_MAX_TASK];

/* current running task PCB */
pcb_t *current_running;

/* global process id */
pid_t process_id = 1;

//define the queue needed
queue_t ready_queue;
queue_t block_queue;

static void check_sleeping()
{
}

void scheduler(void)
{
    // TODO schedule
    // Modify the current_running pointer.

    //printk("running:scheduler\n");
    //store old current_running
    if(current_running!=pcb && current_running!=block_queue.tail)//pcb[0] actually is not a process
        queue_push(&ready_queue,current_running);//always think current is ready before do_scheduler

    //get new current_running
    current_running=queue_dequeue(&ready_queue);
}

void do_sleep(uint32_t sleep_time)
{
    // TODO sleep(seconds)
}

void do_block(queue_t *queue)
{
    // block the current_running task into the queue
    queue_push(&block_queue,current_running);
}

void do_unblock_one(queue_t *queue)
{
    // unblock the head task from the queue
    queue_push(&ready_queue,queue_dequeue(&block_queue));
    //don't forget to push into the ready queue
}

void do_unblock_all(queue_t *queue)
{
    // unblock all task in the queue
    while(!queue_is_empty(&block_queue))
        queue_push(&ready_queue,queue_dequeue(&block_queue));
}

