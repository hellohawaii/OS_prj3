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
//#include "screen.h"
/* screen buffer */
extern char new_screen[SCREEN_HEIGHT * SCREEN_WIDTH];
extern char old_screen[SCREEN_HEIGHT * SCREEN_WIDTH];

void process_show(void);

int int_time;
int int_handle_time;

int next_pid;

static void init_pcb()
{
    int i;
    //initial current_running here
    current_running=pcb;
    //initial queues;
    for(i=0;i<3;i++){
        queue_init(ready_queue_array+i);
    }
    queue_init(&sleep_queue);
    queue_init(&block_queue);
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

    //TODO:find the definition
    syscall[SYSCALL_PROCESS_SHOW]=(int (*)())(&process_show);

    //defined in screen.c, used by sys_move_cursor
    syscall[SYSCALL_CURSOR]=(int (*)())(&screen_move_cursor);


    //defined in lock.c
    syscall[SYSCALL_MUTEX_LOCK_INIT]=(int (*)())(&do_mutex_lock_init);
    syscall[SYSCALL_MUTEX_LOCK_ACQUIRE]=(int (*)())(&do_mutex_lock_acquire);
    syscall[SYSCALL_MUTEX_LOCK_RELEASE]=(int (*)())(&do_mutex_lock_release);
} 

// jump from bootloader.
// The beginning of everything >_< ~~~~~~~~~~~~~~
void __attribute__((section(".entry_function"))) _start(void)
{
	// Close the cache, no longer refresh the cache 
	// when making the exception vector entry copy
	asm_start();
        printk("hello OS!\n");
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
	printk("> [INIT] SCREEN initialization succeeded.\n");

	// TODO Enable interrupt
	open_int();
        printk(":-)\n");
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
    printk("[PROCESS TABLE]\n");
    //int closed, can not use syscall like printf?
    //perhaps need to go through the whole PCB table, because some processes may quit, producing 
    //blanks in the PCB table
    int i;
    int process_num=0;
    for(i=1;i<NUM_MAX_TASK;i++){
        if(pcb[i].status!=5){
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
    printk("error:beq not true,CP0_CAUSE is %d\n",get_CP0_CAUSE());
    printk("EPC:%x\n",get_EPC());
    stop_here();
}

void show_me(void){
    printk("I am here\n");
}

void show_me_again(void){
    printk("I am here now\n");
}
