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
}

void do_mutex_lock_acquire(mutex_lock_t *lock)
//I think this function is used by processes
{
    while(lock->status==LOCKED){
        do_block(&block_queue);
        do_scheduler();
    }
    lock->status=LOCKED;
}

void do_mutex_lock_release(mutex_lock_t *lock)
//I think this function is used by processes
{
    lock->status=UNLOCKED;
    do_unblock_all(&block_queue);
}
