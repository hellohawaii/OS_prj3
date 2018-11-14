/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *            Copyright (C) 2018 Institute of Computing Technology, CAS
 *               Author : Han Shukai (email : hanshukai@ict.ac.cn)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *                  The shell acts as a task running in user mode. 
 *       The main function is to make system calls through the user's output.
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

#include "test.h"
#include "stdio.h"
#include "screen.h"
#include "syscall.h"
#include "sched.h"

#define COMMAND_MAX_LEN 15

void cmd_inter();
/*
static void disable_interrupt()
{
    uint32_t cp0_status = get_cp0_status();
    cp0_status &= 0xfffffffe;
    set_cp0_status(cp0_status);
}

static void enable_interrupt()
{
    uint32_t cp0_status = get_cp0_status();
    cp0_status |= 0x01;
    set_cp0_status(cp0_status);
}
*/
static char read_uart_ch(void)
{
    char ch = 0;
    unsigned char *read_port = (unsigned char *)(0xbfe48000 + 0x00);
    unsigned char *stat_port = (unsigned char *)(0xbfe48000 + 0x05);

    while ((*stat_port & 0x01))
    {
        ch = *read_port;
    }
    return ch;
}

struct task_info task1 = {"task1", (uint32_t)&ready_to_exit_task, USER_PROCESS};
struct task_info task2 = {"task2", (uint32_t)&wait_lock_task, USER_PROCESS};
struct task_info task3 = {"task3", (uint32_t)&wait_exit_task, USER_PROCESS};

struct task_info task4 = {"task4", (uint32_t)&semaphore_add_task1, USER_PROCESS};
struct task_info task5 = {"task5", (uint32_t)&semaphore_add_task2, USER_PROCESS};
struct task_info task6 = {"task6", (uint32_t)&semaphore_add_task3, USER_PROCESS};

struct task_info task7 = {"task7", (uint32_t)&producer_task, USER_PROCESS};
struct task_info task8 = {"task8", (uint32_t)&consumer_task1, USER_PROCESS};
struct task_info task9 = {"task9", (uint32_t)&consumer_task2, USER_PROCESS};

struct task_info task10 = {"task10", (uint32_t)&barrier_task1, USER_PROCESS};
struct task_info task11 = {"task11", (uint32_t)&barrier_task2, USER_PROCESS};
struct task_info task12 = {"task12", (uint32_t)&barrier_task3, USER_PROCESS};

struct task_info task13 = {"SunQuan",(uint32_t)&SunQuan, USER_PROCESS};
struct task_info task14 = {"LiuBei", (uint32_t)&LiuBei, USER_PROCESS};
struct task_info task15 = {"CaoCao", (uint32_t)&CaoCao, USER_PROCESS};

static struct task_info *test_tasks[16] = {&task1, &task2, &task3,
                                           &task4, &task5, &task6,
                                           &task7, &task8, &task9,
                                           &task10, &task11, &task12,
                                           &task13, &task14, &task15};
static int num_test_tasks = 15;
char command_buffer[COMMAND_MAX_LEN]={0};
void test_shell()
{
    //initial it manually
    struct task_info task1 = {"task1", (uint32_t)&ready_to_exit_task, USER_PROCESS};
    struct task_info task2 = {"task2", (uint32_t)&wait_lock_task, USER_PROCESS};
    struct task_info task3 = {"task3", (uint32_t)&wait_exit_task, USER_PROCESS};

    struct task_info task4 = {"task4", (uint32_t)&semaphore_add_task1, USER_PROCESS};
    struct task_info task5 = {"task5", (uint32_t)&semaphore_add_task2, USER_PROCESS};
    struct task_info task6 = {"task6", (uint32_t)&semaphore_add_task3, USER_PROCESS};

    struct task_info task7 = {"task7", (uint32_t)&producer_task, USER_PROCESS};
    struct task_info task8 = {"task8", (uint32_t)&consumer_task1, USER_PROCESS};
    struct task_info task9 = {"task9", (uint32_t)&consumer_task2, USER_PROCESS};

    struct task_info task10 = {"task10", (uint32_t)&barrier_task1, USER_PROCESS};
    struct task_info task11 = {"task11", (uint32_t)&barrier_task2, USER_PROCESS};
    struct task_info task12 = {"task12", (uint32_t)&barrier_task3, USER_PROCESS};

    struct task_info task13 = {"SunQuan",(uint32_t)&SunQuan, USER_PROCESS};
    struct task_info task14 = {"LiuBei", (uint32_t)&LiuBei, USER_PROCESS};
    struct task_info task15 = {"CaoCao", (uint32_t)&CaoCao, USER_PROCESS};
    test_tasks[0]=&task1;
    test_tasks[1]=&task2;
    test_tasks[2]=&task3;
    test_tasks[3]=&task4;
    test_tasks[4]=&task5;
    test_tasks[5]=&task6;
    test_tasks[6]=&task7;
    test_tasks[7]=&task8;
    test_tasks[8]=&task9;
    test_tasks[9]=&task10;
    test_tasks[10]=&task11;
    test_tasks[11]=&task12;
    test_tasks[12]=&task13;
    test_tasks[13]=&task14;
    test_tasks[14]=&task15;
    test_tasks[15]=NULL;
    //initializition for test_sanguo.c
    //I think this should be done by test_sanguo.c
    mbox_init();
    //printk("\n\n\n\nfinish init mboxs");
    //command_buffer[COMMAND_MAX_LEN];//initialize to avoid strange bugs
    int i;
    for(i=0;i<COMMAND_MAX_LEN;i++){
        command_buffer[i]=0;
    }
    char *p=command_buffer;//point at free space
    //intialize the dividing line
    sys_move_cursor(0, SHELL_START_LINE);
    printf("--------------COMMAND--------------\n");//this will stay there as long as not covered
    printf(">root@UCAS_OS:");
    while (1)
    {
        // read command from UART port
        close_int();
        char ch = read_uart_ch();
        open_int();
        
        // read & show command
        // To show the command, need to write the command into new_screen;
        if(ch==127){//my backspace seems to be 127(delete)
            if(p>command_buffer){
                p--;
                *p=0;
                screen_write_ch(ch);//support '\n' and backspace
                                    //modify the screen_array
                                    //TODO, package this func as a syscall
            }
        }else if(ch!=0){//when not typying, will get ch=0 constantly
            if(ch!=13){
                *p=ch;//not put '\n' into buffer, this wil influence the implement of cmd_inter()
                p++;
            }
            screen_write_ch(ch);
        }
        //interprete command
        if(ch==13){
            cmd_inter();//do this after finishing print '\n'
            //clear buffer after interpreting cmd
            for(i=0;i<COMMAND_MAX_LEN;i++){
                command_buffer[i]=0;
            }
            p=command_buffer;
            printf(">root@UCAS_OS:");
        }
    }
}

int get_para(char *str){
    //printf("%s\n",str);
    int result=0;
    int digit;
    char *p=str;
    for(;*p!=0;p++){
        //printf("%d",*p);
        digit=*p-48;
        if(digit<0 || digit>9){//invalid input, return -1
            return -1;
        }else{
            result=result*10+digit;
        }
    }
    return result;
}

void cmd_inter(){
    //printk("interpreting\n");
    int i;
    char cmd4_without_para[6];//take part of command, used for recognize kill/exec
    for(i=0;i<5;i++){
        cmd4_without_para[i]=command_buffer[i];
    }
    cmd4_without_para[5]=0;
    //printf("%s",cmd4_without_para);
    //printf("%s",command_buffer);
    //printf("%d",i);
    int para=get_para(command_buffer+i);//convert from string to integer
    if(streq(command_buffer,"ps")==1){
        sys_process_show();
        return;
    }else if(streq(command_buffer,"clear")){
        screen_clear(SHELL_START_LINE+1,SCREEN_HEIGHT-1);
        //SHELL_START_LINE is the cursor location of "-----command----"
        //cursor location start from 0, and line number used by screen clear start from 0, too.
        sys_move_cursor(0, SHELL_START_LINE+1);
        return;
    }else if(streq(cmd4_without_para,"exec ")){
        if(para==-1){
            printf("invalid parameter!\n");
            return;
        }else{
            //printk("hi\n");
            printf("exec process[%d]\n",para);
            //printk("bye");
            sys_spawn(test_tasks[para]);
            return;
        }
    }else if(streq(cmd4_without_para,"kill ")){
        if(para==-1){
            printf("invalid parameter!\n");
            return;
        }else{
            printf("kill process pid=%d\n",para);
            sys_kill(para);
            return;
        }
    }else if(streq(command_buffer,"help")){
        printf("'ps' show all processes\n");
        printf("'clear' clear the command area\n");
        return;
    }else if(streq(command_buffer,"")){//just type enter, will reach here. do nothing.
        return;
    }else{
        printf("Unknown command, using 'help' for help\n");
    }
}

