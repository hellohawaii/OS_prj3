#include "lock.h"
#include "sched.h"
#include "syscall.h"

void spin_lock_init(spin_lock_t *lock)
{
    lock->status = UNLOCKED;
}

void spin_lock_acquire(spin_lock_t *lock)
{
    while (LOCKED == lock->status)
    {
    };
    lock->status = LOCKED;
}

void spin_lock_release(spin_lock_t *lock)
{
    lock->status = UNLOCKED;
}

void do_mutex_lock_init(mutex_lock_t *lock)
{
    lock->status=UNLOCKED;
    lock->owner=-1;
    queue_init(&(lock->block_queue));
}

void do_mutex_lock_acquire(mutex_lock_t *lock)
//I think this function is used by processes
{
    while(lock->status==LOCKED){
        do_block(&(lock->block_queue));
        current_running->lock_waiting=lock;
        do_scheduler();
    }
    lock->status=LOCKED;
    lock->owner=current_running->pid;
    current_running->lock_waiting=NULL;
    //add the lock to the 'lock_owned' field
    int i;
    for(i=0;i<MAX_MUTEX_OWN;i++){
        if((current_running->lock_owned)[i] == NULL){
            (current_running->lock_owned)[i] = lock;
            return;
        }
    }
    //used for debugging
    printk("acquired too much lock");
    while(1);
}

void do_mutex_lock_release(mutex_lock_t *lock)
//I think this function is used by processes
{
    int i,j;
    //find the pcb of owner according to lock->owner
    //note the owner may not be current_running
    for(j=2;j<NUM_MAX_TASK;j++){
        if(pcb[j].pid == lock->owner)
            break;
    }
    if(pcb[j].pid != lock->owner){
        printk("Error when release the lock, can not find the owner process: pid=%d\n",lock->owner);
    }
    //remove the lock from the 'lock_owned' field
    //do this before resetting lock->owner
    //the field is not guaranteed to be 'continued'
    for(i=0;i<MAX_MUTEX_OWN;i++){
        if((pcb[j].lock_owned)[i] == lock){
            (pcb[j].lock_owned)[i] = NULL;
            //return;
        }
    }
    //reset the lock
    lock->status=UNLOCKED;
    do_unblock_all(&(lock->block_queue));
    lock->owner=-1;

    //used for debugging
    //printk("sth wrong when removing lock from 'lock_owned' field");
    //while(1);
}
