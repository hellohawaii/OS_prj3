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
    lock->owner=NULL;
    queue_init(&(lock->block_queue));
}

void do_mutex_lock_acquire(mutex_lock_t *lock)
//I think this function is used by processes
{
    while(lock->status==LOCKED){
        do_block(&(lock->block_queue));
        do_scheduler();
    }
    lock->status=LOCKED;
    lock->owner=current_running;
    //add the lock to the 'lock_owned' field
    int i;
    for(i=0;i<MAX_MUTEX_OWN;i++){
        if((current_running->lock_owned)[i] != NULL){
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
    lock->status=UNLOCKED;
    do_unblock_all(&(lock->block_queue));
    lock->owner=NULL;
    //remove the lock from the 'lock_owned' field
    //the field is not guaranteed to be 'continued'
    for(i=0;i<MAX_MUTEX_OWN;i++){
        if((current_running->lock_owned)[i] != lock){
            (current_running->lock_owned)[i] = NULL;
            return;
        }
    }
    //used for debugging
    printk("sth wrong when removing lock from 'lock_owned' field");
    while(1);
}
