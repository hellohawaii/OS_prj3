#include "lock.h"
#include "sched.h"
#include "common.h"
#include "screen.h"
#include "syscall.h"

void system_call_helper(int fn, int arg1, int arg2, int arg3)
{
    //printk("syscall num:%d\n",fn);
    // syscall[fn](arg1, arg2, arg3)
    //just need to call the function needed, just use fn (syscall number) to find the function needed in array syscall
    if(fn >= 0 && fn <NUM_SYSCALLS)
        syscall[fn](arg1, arg2, arg3);
    else{
        printk("invalid syscall number!\n");		
        while(1);
    }
}

void sys_sleep(uint32_t time)
{
    invoke_syscall(SYSCALL_SLEEP, time, IGNORE, IGNORE);
}

void sys_block(queue_t *queue)
{
    invoke_syscall(SYSCALL_BLOCK, (int)queue, IGNORE, IGNORE);
}

void sys_unblock_one(queue_t *queue)
{
    invoke_syscall(SYSCALL_UNBLOCK_ONE, (int)queue, IGNORE, IGNORE);
}

void sys_unblock_all(queue_t *queue)
{
    invoke_syscall(SYSCALL_UNBLOCK_ALL, (int)queue, IGNORE, IGNORE);
}

void sys_write(char *buff)
{
    invoke_syscall(SYSCALL_WRITE, (int)buff, IGNORE, IGNORE);
}

void sys_reflush()
{
    invoke_syscall(SYSCALL_REFLUSH, IGNORE, IGNORE, IGNORE);
}

void sys_move_cursor(int x, int y)
{
    invoke_syscall(SYSCALL_CURSOR, x, y, IGNORE);
}

void sys_process_show(){
    invoke_syscall(SYSCALL_PROCESS_SHOW, IGNORE, IGNORE, IGNORE);
}

void mutex_lock_init(mutex_lock_t *lock)
{
    invoke_syscall(SYSCALL_MUTEX_LOCK_INIT, (int)lock, IGNORE, IGNORE);
}

void mutex_lock_acquire(mutex_lock_t *lock)
{
    invoke_syscall(SYSCALL_MUTEX_LOCK_ACQUIRE, (int)lock, IGNORE, IGNORE);
}

void mutex_lock_release(mutex_lock_t *lock)
{
    invoke_syscall(SYSCALL_MUTEX_LOCK_RELEASE, (int)lock, IGNORE, IGNORE);
}

void sys_spawn(/*TODO*/)
{

}
void sys_kill(/*TODO*/)
{

}

void sys_exit(/*TODO*/)
{

}

void sys_waitpid(/*TODO*/)
{

}

void sys_getpid(/*TODO*/)
{
//TODO do not know where it is used
}

void semaphore_init(/*TODO*/){

}

void semaphore_up(/*TODO*/){

}

void semaphore_down(/*TODO*/){

}

void barrier_init(/*TODO*/){

}

void barrier_wait(/*TODO*/){

}

void condition_wait(/*TODO*/){

}

void condition_broadcast(/*TODO*/){

}
