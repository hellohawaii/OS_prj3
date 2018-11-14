#ifndef INCLUDE_BARRIER_H_
#define INCLUDE_BARRIER_H_

#include "queue.h"

typedef struct barrier
{
    int num_expected;//expect how many tasks to reach barrier
    int num_now;//how many task have reached the barrier
    queue_t waiting_queue;
} barrier_t;

void do_barrier_init(barrier_t *, int);
void do_barrier_wait(barrier_t *);

#endif
