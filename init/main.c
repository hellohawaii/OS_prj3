/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *            Copyright (C) 2018 Institute of Computing Technology, CAS
 *               Author : Han Shukai (email : hanshukai@ict.ac.cn)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *         The kernel's entry, where most of the initialization work is done.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this 
 * software and associated documentation files (the "Software"), to deal in the Software 
 * without restriction, including without limitation the rights to use, copy, modify, 
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit 
 * persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * */

#include "irq.h"
#include "test.h"
#include "stdio.h"
#include "sched.h"
#include "screen.h"
#include "common.h"
#include "syscall.h"

#include "regs.h"

#include "sem.h"
#include "cond.h"
#include "barrier.h"
//#include "screen.h"
/* screen buffer */
extern char new_screen[SCREEN_HEIGHT * SCREEN_WIDTH];
extern char old_screen[SCREEN_HEIGHT * SCREEN_WIDTH];

void process_show(void);
void do_spawn(task_info_t * exec_info);
void do_wait(pid_t pid);
void do_kill(pid_t pid);
void do_exit(void);
int do_getpid(void);

int int_time;
int int_handle_time;

int next_pid;
int historical_num;  //the number of processes(including the processes no longer exist)
                     //help to advocate the memory

static void init_pcb()
{
    int i;
    int j;
    //initial current_running here
    current_running=pcb;
    //initial queues;
    for(i=0;i<3;i++){
        queue_init(ready_queue_array+i);
    }
    queue_init(&sleep_queue);
    //initial PCB table
    for(i=2;i<NUM_MAX_TASK;i++){
        pcb[i].status=5;
    }
    //initial the shell process into pcb[1]
    //universal reg
    pcb[1].kernel_context.regs[29]=0xa0f00000+0x10000;//sp
    pcb[1].user_context.regs[29]=0xa0f00000+0x10000-0x01000;//sp
    pcb[1].kernel_context.regs[31]=(uint32_t)(handle_int+0x14);//ra
    pcb[1].user_context.regs[31]=(uint32_t)&test_shell;//ra
    //printk("univer reg\n");
    //process attribute
    pcb[1].type=3;//USER_PROCESS
    pcb[1].pid=1;
    pcb[1].status=TASK_READY;
    pcb[1].priority=3;//priority=1/2/3
    queue_init(&(pcb[1].waiting_queue));
    pcb[1].lock_waiting=NULL;
    pcb[1].waiting_pcb = -1;//PCB=-1 mean not exist
    for(j=0;j<MAX_MUTEX_OWN;j++)
        (pcb[1].lock_owned)[j]=NULL;
    //printk("attr\n");
    //special reg
    pcb[1].user_context.cp0_status=0x10008000;
    pcb[1].user_context.cp0_badvaddr=0;
    pcb[1].user_context.cp0_cause=0;
    pcb[1].user_context.cp0_epc=(uint32_t)&test_shell;
    pcb[1].kernel_context.cp0_status=0x10008000;
    pcb[1].kernel_context.cp0_badvaddr=0;
    pcb[1].kernel_context.cp0_cause=0;
    pcb[1].kernel_context.cp0_epc=(uint32_t)&test_shell;
    //push to ready_queue
    queue_push(ready_queue_array+2,pcb+1);
    //update next pid
    next_pid=2;
    historical_num=1;
}

static void init_exception_handler()
{
}

static void init_exception()
{
    // 1. Get CP0_STATUS
    //only change part of CP_STATUS, perhaps this is the reason why we gets CP0_STATUS
    uint32_t initial_STATUS;
    initial_STATUS=get_CP0_STATUS();
    printk("%d\n",initial_STATUS);
    // 2. Disable all interrupt
    close_int();
    // 3. Copy the level 2 exception handling code to 0x80000180
    memcpy(BEV0_EBASE+BEV0_OFFSET,exception_handler_begin,exception_handler_end-exception_handler_begin);//irq.h define the macro
    //printk("hhhh\n");
    // 4. reset CP0_COMPARE & CP0_COUNT register
    reset_timer();
    //restore the CP0_STATUS and open interrupt
    set_CP0_STATUS(initial_STATUS & 0xffff80ff | 0x8000 | STATUS_CU0);
    initial_STATUS=get_CP0_STATUS();
    printk("%x\n",initial_STATUS);
    reset_timer();
    //open_int();
    //printk("STATUS:%x,COMPARE:%x,COUNT:%x\n",get_CP0_STATUS(),get_CP0_COMPARE(),get_CP0_COUNT());
}

static void init_syscall(void)
{
    // init system call table.
    // defined in syscall.h
    // the syscall number is the order in the array, the function handling it is stored in the corresponding place
    //TODO : I don't know how to use the pointer of funcion
    //need to write or find proper kernel method
    //before task4, the test program using kernel method directly, but in task4, using syscall to use these method
    //all of the methods except do_sleep have been used before in task1-3
    
    //defined in sched.c
    syscall[SYSCALL_SLEEP]=(int (*)())(&do_sleep);
    syscall[SYSCALL_BLOCK]=(int (*)())(&do_block);
    syscall[SYSCALL_UNBLOCK_ONE]=(int (*)())(&do_unblock_one);
    syscall[SYSCALL_UNBLOCK_ALL]=(int (*)())(&do_unblock_all);

    //defined in screen.c, used by printf
    syscall[SYSCALL_WRITE]=(int (*)())(&screen_write);
    syscall[SYSCALL_REFLUSH]=(int (*)())(&screen_reflush);
    syscall[SYSCALL_WRITE_CH]=(int (*)())(&screen_write_ch);
    syscall[SYSCALL_CLEAR]=(int (*)())(&screen_clear);

    //defined in main.c
    syscall[SYSCALL_PROCESS_SHOW]=(int (*)())(&process_show);

    //defined in main.c
    syscall[SYSCALL_SPAWN]=(int (*)())(&do_spawn);
    syscall[SYSCALL_WAITPID]=(int (*)())(&do_wait);
    syscall[SYSCALL_KILL]=(int (*)())(&do_kill);
    syscall[SYSCALL_EXIT]=(int (*)())(&do_exit);
    syscall[SYSCALL_GETPID]=(int (*)())(&do_getpid);

    //defined in screen.c, used by sys_move_cursor
    syscall[SYSCALL_CURSOR]=(int (*)())(&screen_move_cursor);


    //defined in lock.c
    syscall[SYSCALL_MUTEX_LOCK_INIT]=(int (*)())(&do_mutex_lock_init);
    syscall[SYSCALL_MUTEX_LOCK_ACQUIRE]=(int (*)())(&do_mutex_lock_acquire);
    syscall[SYSCALL_MUTEX_LOCK_RELEASE]=(int (*)())(&do_mutex_lock_release);

    //defined in sem.c
    syscall[SYSCALL_SEM_INIT]=(int (*)())(&do_semaphore_init);
    syscall[SYSCALL_SEM_UP]=(int (*)())(&do_semaphore_up);
    syscall[SYSCALL_SEM_DOWN]=(int (*)())(&do_semaphore_down);

    //defined in barrier.c
    syscall[SYSCALL_BARRIER_INIT]=(int (*)())(&do_barrier_init);
    syscall[SYSCALL_BARRIER_WAIT]=(int (*)())(&do_barrier_wait);

    //defined in cond.c
    syscall[SYSCALL_COND_INIT]=(int (*)())(&do_condition_init);
    syscall[SYSCALL_COND_WAIT]=(int (*)())(&do_condition_wait);
    syscall[SYSCALL_COND_SIGNAL]=(int (*)())(&do_condition_signal);
    syscall[SYSCALL_COND_BROADCAST]=(int (*)())(&do_condition_broadcast);
} 

// jump from bootloader.
// The beginning of everything >_< ~~~~~~~~~~~~~~
void __attribute__((section(".entry_function"))) _start(void)
{
	// Close the cache, no longer refresh the cache 
	// when making the exception vector entry copy
	asm_start();
        printk("hello OS!\n");
        schedule_time=0;
        //old_screen[SCREEN_HEIGHT * SCREEN_WIDTH]={0};
        //new_screen[SCREEN_HEIGHT * SCREEN_WIDTH]={0};
        int i;
        for(i=0;i<SCREEN_HEIGHT * SCREEN_WIDTH;i++)
            old_screen[i]='0';
        for(i=0;i<SCREEN_HEIGHT * SCREEN_WIDTH;i++)
            new_screen[i]='0';
        int_time=0;
        int_handle_time=0;
	// init Process Control Block (-_-!)
	// init interrupt (^_^)
	init_exception();
	printk("> [INIT] Interrupt processing initialization succeeded temp.\n");
        //open_int();
	init_pcb();
	printk("> [INIT] PCB initialization succeeded.\n");

	// init interrupt (^_^)
	init_exception();
	printk("> [INIT] Interrupt processing initialization succeeded.\n");

	// init system call table (0_0)
	init_syscall();
	printk("> [INIT] System call initialized successfully.\n");

	// init screen (QAQ)
	init_screen();
	//printk("> [INIT] SCREEN initialization succeeded.\n");

	// TODO Enable interrupt
	open_int();
        //printk(":-)\n");
        reset_timer();
	while (1)
	{
		// (QAQQQQQQQQQQQ)
		// If you do non-preemptive scheduling, you need to use it to surrender control
                //printk("looping...\n");
		//do_scheduler();
                
	};
        //printk(":-(\n");
	return;
}

void process_show(void){
    //screen_cursor_y++;//TODO:do not know why
    printk("\n\n[PROCESS TABLE]\n");
    //int closed, can not use syscall like printf?
    //perhaps need to go through the whole PCB table, because some processes may quit, producing 
    //blanks in the PCB table
    int i;
    int process_num=0;
    for(i=1;i<NUM_MAX_TASK;i++){
        if(pcb[i].status!=5){
            screen_cursor_y++;
            printk("[%d] PID:%d STATUS: ", process_num++,pcb[i].pid);//interrupt closed, so using printk. this is kernel func
            switch(pcb[i].status){
                case 0: printk("BLOCKED\n");    break;
                case 1: printk("RUNNING\n");    break;
                case 2: printk("READY\n");    break;
                case 3: printk("EXITED\n");    break;
                case 4: printk("SLEEPING\n");    break;
                default: printk("WRONG! UNKNOWN\n");
            }
        }
    }
    screen_cursor_y++;
}

void do_spawn(task_info_t * exec_info){//TODO : not sure about the parameter order
    uint32_t entry=exec_info->entry_point;
    task_type_t type=exec_info->type;
    char *name=exec_info->name;
    int i;
    int j;
    //fine the spare space in PCB table
    for(i=2;i<NUM_MAX_TASK && pcb[i].status!=TASK_NOT_EXIST;i++){
        ;
    }
    if(i==NUM_MAX_TASK){
        printk("TOO MANY TASKS");
        while(1);
    }
    //universal reg
    pcb[i].kernel_context.regs[29]=0xa0f00000+0x10000*(historical_num+1);//sp
    pcb[i].user_context.regs[29]=0xa0f00000+0x10000*(historical_num+1)-0x01000*(historical_num+1);//sp
    pcb[i].kernel_context.regs[31]=(uint32_t)(handle_int+0x14);//ra
    pcb[i].user_context.regs[31]=entry;//ra
    //process attribute
    pcb[i].type=type;//USER_PROCESS
    pcb[i].pid=next_pid++;
    pcb[i].status=TASK_READY;
    pcb[i].priority=1;//priority=1/2/3
    queue_init(&(pcb[i].waiting_queue));
    pcb[i].lock_waiting=NULL;
    pcb[i].waiting_pcb = -1;//PCB=-1 mean not exist
    for(j=0;j<MAX_MUTEX_OWN;j++)
        (pcb[i].lock_owned)[j]=NULL;
    //printk("attr\n");
    //special reg
    pcb[i].user_context.cp0_status=0x10008000;
    pcb[i].user_context.cp0_badvaddr=0;
    pcb[i].user_context.cp0_cause=0;
    pcb[i].user_context.cp0_epc=entry;
    pcb[i].kernel_context.cp0_status=0x10008000;
    pcb[i].kernel_context.cp0_badvaddr=0;
    pcb[i].kernel_context.cp0_cause=0;
    pcb[i].kernel_context.cp0_epc=entry;
    //push to ready_queue
    queue_push(ready_queue_array,pcb+i);
    //update histotical_num
    historical_num++;
    //TODO,not sure
    do_scheduler();
}

void do_wait(pid_t pid){
    current_running->status=TASK_BLOCKED;
    //find the location of pid in the pcb table, same as do_kill
    int i;
    for(i=2;i<NUM_MAX_TASK;i++){
        if(pcb[i].pid == pid)
            break;
    }
    if(pcb[i].pid != pid){
        printk("No process with pid=%d\n",pid);
    }
    //push to the wait_queue
    //in the kill & exit func, do not forget to wait up waiting processes
    queue_push(&(pcb[i].waiting_queue),current_running);
    //change the waiting pcb, to record which pcb this process is waiting
    current_running->waiting_pcb = pid;
    do_scheduler();
}

void do_kill(pid_t pid){
    //TODO:what if there are semaphore/conditional varible/barrier and so on?
    //find the location of pid in the pcb table, same as do_wait
    int i;
    void *temp_pcb;
    for(i=2;i<NUM_MAX_TASK;i++){
        if(pcb[i].pid == pid)
            break;
    }
    if(pcb[i].pid != pid){
        printk("No process with pid=%d\n",pid);
    }
    //remove from queues, TODO
    if(pcb[i].status==TASK_READY){
        //we can using the priority attribute
        for(temp_pcb=(ready_queue_array+pcb[i].priority-1)->head;temp_pcb!=NULL;temp_pcb=(((pcb_t *)temp_pcb)->next)){
            if((((pcb_t *)temp_pcb)->pid) == pid){
                queue_remove(ready_queue_array+pcb[i].priority-1, temp_pcb);
            }		
        }
    }else if(pcb[i].status==TASK_BLOCKED){
        //There are two possible situation resulting into being blocked
        //the process is killed, so no longer waits for the lock/process
        //waiting for a mutex
        if(pcb[i].lock_waiting != NULL){
            //remove the killed process from block_queue
            //need to search the queue.
            for(temp_pcb=((pcb[i].lock_waiting)->block_queue).head;temp_pcb!=NULL;temp_pcb=(((pcb_t *)temp_pcb)->next)){
                if((((pcb_t *)temp_pcb)->pid) == pid){
                    queue_remove(&((pcb[i].lock_waiting)->block_queue), temp_pcb);
                }		
            }
        }else if(pcb[i].waiting_pcb!=-1){//waiting for a process
            pid_t target_pcb=pcb[i].waiting_pcb;
            //TODO:perhaps, if store the addr in pcb instead of pid, it will be better, do not need to search in the pcb table
            //TODO:perhaps, package a function, search & remove the process with specific pid from the given queue.
            //search the table to find the process
            int j;
            for(j=2;j<NUM_MAX_TASK;j++){
                if(pcb[j].pid == target_pcb)
                    break;
            }
            if(pcb[j].pid != target_pcb){
                 printk("No process with pid=%d\n",target_pcb);
            }
            //remove the killed process from waiting_queue
            //need to search the queue.
            for(temp_pcb=(pcb[j].waiting_queue).head;temp_pcb!=NULL;temp_pcb=(((pcb_t *)temp_pcb)->next)){
                if((((pcb_t *)temp_pcb)->pid) == pid){
                    queue_remove(&(pcb[j].waiting_queue), temp_pcb);
                }		
            }
        }else{
            printk("Unknown block reason");
            while(1);
        }
    }else if(pcb[i].status==TASK_SLEEPING){
        for(temp_pcb=sleep_queue.head;temp_pcb!=NULL;temp_pcb=(((pcb_t *)temp_pcb)->next)){
            if((((pcb_t *)temp_pcb)->pid) == pid){
                queue_remove(&sleep_queue, temp_pcb);
            }		
        }
    }
    //change status. do this after removing from queues
    pcb[i].status=TASK_NOT_EXIST;
    //wake up processes waiting for him
    //change the waiting pcb of process in the waiting queue;
    //using pcb[i] instead of current_running
    for(temp_pcb=(pcb[i].waiting_queue).head;temp_pcb!=NULL;temp_pcb=(((pcb_t *)temp_pcb)->next)){
        ((pcb_t *)temp_pcb)->waiting_pcb=-1;//-1 means waiting for nothing
    }
    do_unblock_all(&(pcb[i].waiting_queue));
    //release lock
    int j;
    for(j=0;j<MAX_MUTEX_OWN;j++){
        if((pcb[i].lock_owned)[j] != NULL){
            do_mutex_lock_release((pcb[i].lock_owned)[j]);
        }
    }
    do_scheduler();
}

void do_exit(void){
    //TODO:what if there are semaphore/conditional varible/barrier and so on?
    //change status
    current_running->status=TASK_NOT_EXIST;
    //remove from queues, fortunatly, current process doesn't belong to any queue
    //wake up processes waiting for him
    //change the waiting pcb of process int he waiting queue;
    void *temp_pcb;
    for(temp_pcb=(current_running->waiting_queue).head;temp_pcb!=NULL;temp_pcb=(((pcb_t *)temp_pcb)->next)){
        ((pcb_t *)temp_pcb)->waiting_pcb=-1;//-1 means waiting for nothing
    }
    do_unblock_all(&(current_running->waiting_queue));
    //release lock
    int i;
    for(i=0;i<MAX_MUTEX_OWN;i++){
        if((current_running->lock_owned)[i] != NULL){
            do_mutex_lock_release((current_running->lock_owned)[i]);
        }
    }
    do_scheduler();
}

int do_getpid(void){
    return current_running->pid;
}

void stop_here(void){
    while(1);
}

void show_int(void){
    printk("\n\nnow is handling interrupt(%d)\n",int_time++);
    int_time++;
}


void show_handle_int(void){
    printk("\n\n\nnow is show_handle_int(%d)\n",int_handle_time++);
}

void show_error(void){
    printk("error:beq not true,CP0_CAUSE is %d,badvaddr is %d\n",get_CP0_CAUSE(),get_BadVaddr());
    printk("EPC:%x\n",get_EPC());
    stop_here();
}

void show_me(void){
    printk("I am here\n");
}

void show_me_again(void){
    printk("I am here now\n");
}
