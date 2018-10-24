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

static void init_pcb()
{
//the following codes are for task1
    //intialize the queue
    queue_init(&ready_queue);
    //store PCBs to the array
    pcb[1].kernel_context.regs[29]=0xa0f00000;//sp
    pcb[1].kernel_context.regs[31]=sched1_tasks[0]->entry_point;//ra
    pcb[1].type=sched1_tasks[0]->type;
    pcb[1].pid=1;
    pcb[1].status=TASK_READY;
    pcb[2].kernel_context.regs[29]=0xa0f10000;//sp
    pcb[2].kernel_context.regs[31]=sched1_tasks[1]->entry_point;//ra
    pcb[2].type=sched1_tasks[1]->type;
    pcb[2].pid=2;
    pcb[2].status=TASK_READY;
    pcb[3].kernel_context.regs[29]=0xa0f20000;//sp
    pcb[3].kernel_context.regs[31]=sched1_tasks[2]->entry_point;//ra
    pcb[3].type=sched1_tasks[2]->type;
    pcb[3].pid=3;
    pcb[3].status=TASK_READY;
    //move them to the ready queue
    //modify the pointer in PCB, may not be necessary
    //ready_queue.head=pcb+1;
    //pcb[1].prev=NULL;
    //pcb[1].next=pcb+2;
    //pcb[2].prev=pcb+1;
    //pcb[2].next=pcb+3;
    //pcb[3].prev=pcb+2;
    //pcb[3].next=NULL;
    queue_push(&ready_queue,pcb+1);
    queue_push(&ready_queue,pcb+2);
    queue_push(&ready_queue,pcb+3);
    //initial current_running here
    current_running=pcb;


/*the following codes are for task2
    //intialize the queue
    queue_init(&ready_queue);
    queue_init(&block_queue);
    //store PCBs to the array
    int i;
    for(i=1;i<=3;i++){
        pcb[i].kernel_context.regs[29]=0xa0f00000+0x10000*i;//sp
        pcb[i].kernel_context.regs[31]=sched1_tasks[i-1]->entry_point;//ra
        pcb[i].type=sched1_tasks[i-1]->type;
        pcb[i].pid=i;
        pcb[i].status=TASK_READY;
        pcb[i].kernel_context.cp0_status=0;
        pcb[i].kernel_context.hi=0;
        pcb[i].kernel_context.lo=0;
        pcb[i].kernel_context.cp0_badvaddr=0;
    }
    for(i=4;i<=5;i++){
        pcb[i].kernel_context.regs[29]=0xa0f00000+0x10000*i;//sp
        pcb[i].kernel_context.regs[31]=lock_tasks[i-4]->entry_point;//ra
        pcb[i].type=lock_tasks[i-4]->type;
        pcb[i].pid=i;
        pcb[i].status=TASK_READY;
        pcb[i].status=TASK_READY;
        pcb[i].kernel_context.cp0_status=0;
        pcb[i].kernel_context.hi=0;
        pcb[i].kernel_context.lo=0;
        pcb[i].kernel_context.cp0_badvaddr=0;
    }
    //move them to the ready queue
    queue_push(&ready_queue,pcb+1);
    queue_push(&ready_queue,pcb+2);
    queue_push(&ready_queue,pcb+3);
    queue_push(&ready_queue,pcb+4);
    queue_push(&ready_queue,pcb+5);
    //initial current_running here
    current_running=pcb;
*/
}

static void init_exception_handler()
{
}

static void init_exception()
{
    // 1. Get CP0_STATUS
    //only change part of CP_STATUS, perhaps this is the reason why we gets CP0_STATUS
    int initial_STATUS;
    //can not use the reg name directly,$26 for k0,$27 for k1,$13 for CP0_CAUSE
    //$11 for CP0_COMPARE, $9 for CP0_COUNT, $12 for CP0_STATUS
    asm volatile(
        "mfc0 $27, $12\n\t"
        "sw $27,%0\n\t"
        :"=m"(initial_STATUS)
        :
        :"k0"
    );
    printk("%d\n",initial_STATUS);
/*only use regs
    asm volatile(
        "mfc0 k0, CP0_STATUS\n\t"
        :
        :
        :"k0"
    );
*/
    // 2. Disable all interrupt
    //int new_STATUS=initial_STATUS& 0xffff00ff;
    //can not use the reg name directly,$26 for k0,$27 for k1,$13 for CP0_CAUSE
    //$11 for CP0_COMPARE, $9 for CP0_COUNT, $12 for CP0_STATUS
    asm(
        //"lw $26,%0\n\t"
        //"mtc0 $26, $12\n\t"
        //:
        //:"m"(new_STATUS)
        //:"k0"

        "mtc0 $0,$12"
    );
    //printk("%d\n",new_STATUS);
/*only use regs
    asm volatile(
        "or k1,$0,0x111100111\n\t"
        "or k0,k1,k0\n\t"
        "mtc0 k0, CP0_STATUS\n\t"
        :
        :
        :"k0","k1"
    );
*/
    // 3. Copy the level 2 exception handling code to 0x80000180
    printk("copying\n");
    printk("size:%d\n",exception_handler_begin-exception_handler_end);
    memcpy((void *)0x80000180,exception_handler_begin,exception_handler_end-exception_handler_begin);
    printk("finish copying\n");
    // 4. reset CP0_COMPARE & CP0_COUNT register
    //can not use the reg name directly,$26 for k0,$27 for k1,$13 for CP0_CAUSE
    //$11 for CP0_COMPARE, $9 for CP0_COUNT
    //150000 is TIMER_INTERVAL
    asm volatile(
        "add $26,$0,0x3a98\n\t"
        "mtc0 $26,$11\n\t"
        "mtc0 $0,$9\n\t"
    );
    //printk("setting CP0_COMPARE and CP0_COUNT");
    //restore the CP0_STATUS and open interrupt
    //int newest_STATUS=new_STATUS & 11110011;
    //int newest_STATUS=0xffffffff;
        asm(
        //"add $26,$0,0xffffffff\n\t"
        //"lw $26,%0\n\t"
        //"mtc0 $26, $12\n\t"
        //:
        //:"m"(newest_STATUS)
        //:"k0"
        "li $26,0xffffffff\n\t"
        "mtc0 $26, $12\n\t"
    );
    //printk("%d\n",newest_STATUS);
    printk("STATUS:%x,COMPARE:%x,COUNT:%x\n",get_CP0_STATUS(),get_CP0_COMPARE(),get_CP0_COUNT());
}

static void init_syscall(void)
{
	// init system call table.
} 

// jump from bootloader.
// The beginning of everything >_< ~~~~~~~~~~~~~~
void __attribute__((section(".entry_function"))) _start(void)
{
	// Close the cache, no longer refresh the cache 
	// when making the exception vector entry copy
	asm_start();
        printk("hello OS!\n");
	// init interrupt (^_^)
	init_exception();
	printk("> [INIT] Interrupt processing initialization succeeded.\n");

	// init system call table (0_0)
	init_syscall();
	printk("> [INIT] System call initialized successfully.\n");

	// init Process Control Block (-_-!)
	init_pcb();
	printk("> [INIT] PCB initialization succeeded.\n");

	// init screen (QAQ)
	init_screen();
	printk("> [INIT] SCREEN initialization succeeded.\n");

	// TODO Enable interrupt
	
	while (1)
	{
		// (QAQQQQQQQQQQQ)
		// If you do non-preemptive scheduling, you need to use it to surrender control
                //printk("looping...\n");
		do_scheduler();
	};
	return;
}
