#include "string.h"
#include "mailbox.h"

#define MAX_NUM_BOX 5

static mailbox_t mboxs[MAX_NUM_BOX];//a lot of mail box

void mbox_init()
{
    int i;
    int j;
    for(i=0;i<MAX_NUM_BOX;i++){
        *(mboxs[i].name)='0';
        for(j=0;j<MAIL_CAPACITY;j++){
            mboxs[i].buffer[j]='0';
        }
        condition_init(&(mboxs[i].not_full));
        condition_init(&(mboxs[i].not_empty));
        mutex_lock_init(&(mboxs[i].lock_on_box));
        mboxs[i].num_users=0;//users=0 <=> mailbox not_used <=> mailbox not exist
        mboxs[i].space_used=0;
        mboxs[i].capacity=MAIL_CAPACITY;
    }
}

mailbox_t *mbox_open(char *name)
{
    //first, try to find in the existing mboxs
    int i;
    for(i=0;i<MAX_NUM_BOX;i++){
        if(streq(mboxs[i].name,name))
            break;
    }
    if(streq(mboxs[i].name,name)){//find or not find
        ;
    }else{
        //find a spare space
        for(i=0;i<MAX_NUM_BOX;i++){
            if(mboxs[i].num_users == 0){
                strcpy(mboxs[i].name/*dest*/,name/*src*/);
                //do not do other initializations here, because we plan to restore to initial status when destroy(close) a mailbox
                break;
            }
        }
        if(i==MAX_NUM_BOX){
            printf("can not open more mailbox");
            while(1);
        }
    }
    //no matter which situation, mboxs[i] is what we want(what we found or created)
    //change num_users
    (mboxs+i)->num_users++;//don't assign it to 1
    //debugging
    //printf("finish open mail box named %s",name);
    return mboxs+i;
}

void mbox_close(mailbox_t *mailbox)
{
    int j;
    (mailbox->num_users)--;
    if((mailbox->num_users)==0){
        *(mailbox->name)='0';
        for(j=0;j<MAIL_CAPACITY;j++){
            mailbox->buffer[j]='0';
        }
        condition_init(&(mailbox->not_full));
        condition_init(&(mailbox->not_empty));
        mutex_lock_init(&(mailbox->lock_on_box));
        mailbox->space_used=0;
        mailbox->capacity=MAIL_CAPACITY;
    }
}

void mbox_send(mailbox_t *mailbox, void *msg, int msg_length)
{
    //debugging
    //printf("want to send msg\n");
    mutex_lock_acquire(&(mailbox->lock_on_box));
    //debugging
    //printf("acquire lock(send)\n");
    int len_sent=0;//how long have been sent
    while(len_sent<msg_length){
        if(mailbox->space_used < mailbox->capacity){
            //printf("sending");
            //do not forget to signal not_empty
            (mailbox->buffer)[mailbox->space_used++/*using 'space_used' as the index*/] = *(((char *)msg+len_sent));//using space_used&len_sent as the index
            //printf("\n.%c.",*(((char *)msg+len_sent)));
            //do not forget to change space_used
            len_sent++;
            if(mailbox->space_used==1){//just not empty
                condition_signal(&(mailbox->not_empty));
                //do not worry about giving lock to the unblocked process, while acquire lock at the end 
            }
        }else if(mailbox->space_used == mailbox->capacity){
            //debugging
            //printf("too full to send");
            condition_wait(&(mailbox->lock_on_box), &(mailbox->not_full));
            //after waiting, will own the lock
            //do not sent here, because after wait, may still be full, recheck in the next while loop
        }else{
            printf("there is sth wrong with the mailbox's attribute(send)\n");
            while(1);
        }
    }
    mutex_lock_release(&(mailbox->lock_on_box));
    //debugging
    //printf("finish sending msg\n");
}

void mbox_recv(mailbox_t *mailbox, void *msg, int msg_length)
{
    //debugging
    //printf("want to recv msg\n");
    mutex_lock_acquire(&(mailbox->lock_on_box));
    int len_recv=0;//how long have been sent
    while(len_recv<msg_length){
        if(mailbox->space_used > 0){
            //do not forget to signal not_empty
            *(((char *)msg+len_recv)) = (mailbox->buffer)[--(mailbox->space_used)/*using 'space_used-1' as the index*/] ;//using space_used&len_sent as the index
            //do not forget to change space_used
            len_recv++;
            if(mailbox->space_used==(mailbox->capacity)-1){//just not full
                condition_signal(&(mailbox->not_full));
                //do not worry about giving lock to the unblocked process, while acquire lock at the end 
            }
        }else if(mailbox->space_used == 0){
            //debugging
            //printf("too empty to recv");
            condition_wait(&(mailbox->lock_on_box), &(mailbox->not_empty));
            //after waiting, will own the lock
            //do not sent here, because after wait, may still be full, recheck in the next while loop
        }else{
            printf("there is sth wrong with the mailbox's attribute(recv)\n");
            while(1);
        }
    }
    mutex_lock_release(&(mailbox->lock_on_box));
    //debugging
    //printf("finish recieving msg\n");
}
