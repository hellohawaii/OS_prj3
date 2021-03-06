#include "cond.h"
#include "lock.h"
#include "sched.h"

void do_condition_init(condition_t *condition)
{
    condition->num_waiting=0;
    queue_init(&(condition->waiting_queue));
}

void do_condition_wait(mutex_lock_t *lock, condition_t *condition)
//before do this, need to acquire the lock. User process need to ganrantee this.
{
    //recheck whether have acquired the lock.
    if(lock->owner != current_running->pid){
        printk("Error, please acquire the lock before calling for condition_wait\n");
    }
    //body of this function
    (condition->num_waiting)++;
    do_block(&(condition->waiting_queue));
    do_mutex_lock_release(lock);
    do_scheduler();
    do_mutex_lock_acquire(lock);//however, after acquiring the lock(wake up from waiting), the condition may disappear!
}

void do_condition_signal(condition_t *condition)
{
    if((condition->num_waiting)>0){
        do_unblock_one(&(condition->waiting_queue));
        //do not ganrantee the newly unblocked process to own the lock of the condition
        //this is ganranteed by wait, because after being unblocked, will try to acquire the lock again
        (condition->num_waiting)--;
    }
}

void do_condition_broadcast(condition_t *condition)
{
    if((condition->num_waiting)>0){
        do_unblock_all(&(condition->waiting_queue));
        //do not ganrantee the newly unblocked process to own the lock of the condition
        //this is ganranteed by wait, because after being unblocked, will try to acquire the lock again
        (condition->num_waiting)=0;
    }
}
