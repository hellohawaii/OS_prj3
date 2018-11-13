/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *            Copyright (C) 2018 Institute of Computing Technology, CAS
 *               Author : Han Shukai (email : hanshukai@ict.ac.cn)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *                                   Thread Lock
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
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

#ifndef INCLUDE_LOCK_H_
#define INCLUDE_LOCK_H_

#define MAX_MUTEX_OWN 5

#include "queue.h"

typedef enum {
    UNLOCKED,
    LOCKED,
} lock_status_t;

typedef struct spin_lock
{
    lock_status_t status;
} spin_lock_t;

typedef struct mutex_lock
{
    lock_status_t status;
    pid_t owner;//can not declare to be pcb_t *, because in the definition of pcb_t, they need the definition of mutex lock too
              //If using pcb_t *, there will be a loop in type define.
    queue_t block_queue;
} mutex_lock_t;

/* init lock */
void spin_lock_init(spin_lock_t *lock);
void spin_lock_acquire(spin_lock_t *lock);
void spin_lock_release(spin_lock_t *lock);

void do_mutex_lock_init(mutex_lock_t *lock);
void do_mutex_lock_acquire(mutex_lock_t *lock);
void do_mutex_lock_release(mutex_lock_t *lock);

#endif
