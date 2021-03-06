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
    int return_val;
    if(fn >= 0 && fn <NUM_SYSCALLS){
        return_val=(int)(syscall[fn](arg1, arg2, arg3));
        current_running->user_context.regs[2] = return_val;//by referring to Xue Feng's code, write this line
    }
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

void sys_write_ch(char c)
{
    invoke_syscall(SYSCALL_WRITE_CH, (int)c, IGNORE, IGNORE);
}

void sys_reflush()
{
    invoke_syscall(SYSCALL_REFLUSH, IGNORE, IGNORE, IGNORE);
}

void sys_clear(int line1,int line2)
{
    invoke_syscall(SYSCALL_CLEAR, line1, line2, IGNORE);
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

void sys_spawn(task_info_t * exec_info)
{
    invoke_syscall(SYSCALL_SPAWN, (int)exec_info, IGNORE, IGNORE);//need convert to int
}

void sys_kill(pid_t pid)
{
    invoke_syscall(SYSCALL_KILL, pid, IGNORE, IGNORE);
}

void sys_exit(void)
{
    invoke_syscall(SYSCALL_EXIT, IGNORE, IGNORE, IGNORE);
}

void sys_waitpid(pid_t pid)
{
    invoke_syscall(SYSCALL_WAITPID, pid, IGNORE, IGNORE);
}

int sys_getpid(void)
{
    int c;
    c = invoke_syscall(SYSCALL_GETPID, IGNORE, IGNORE, IGNORE);//at this time, v0 is return num, this is what we want;
    //printk("\n\n\n\n\n\n\n\n\n%d\n\n",c);
    return c;
}

void semaphore_init(semaphore_t *s, int val){
    invoke_syscall(SYSCALL_SEM_INIT, (int)s, val, IGNORE);
}

void semaphore_up(semaphore_t *s){
    invoke_syscall(SYSCALL_SEM_UP, (int)s, IGNORE, IGNORE);
}

void semaphore_down(semaphore_t *s){
    invoke_syscall(SYSCALL_SEM_DOWN, (int)s, IGNORE, IGNORE);
}

void barrier_init(barrier_t *barrier, int goal){
    invoke_syscall(SYSCALL_BARRIER_INIT, (int)barrier, goal, IGNORE);
}

void barrier_wait(barrier_t *barrier){
    invoke_syscall(SYSCALL_BARRIER_WAIT, (int)barrier, IGNORE, IGNORE);
}

void condition_init(condition_t *condition)
{
    invoke_syscall(SYSCALL_COND_INIT, (int)condition, IGNORE, IGNORE);
}

void condition_wait(mutex_lock_t *lock, condition_t *condition){
    invoke_syscall(SYSCALL_COND_WAIT, (int)lock, (int)condition, IGNORE);
}

void condition_signal(condition_t *condition)
{
    invoke_syscall(SYSCALL_COND_SIGNAL, (int)condition, IGNORE, IGNORE);
}

void condition_broadcast(condition_t *condition){
    invoke_syscall(SYSCALL_COND_BROADCAST, (int)condition, IGNORE, IGNORE);
}
