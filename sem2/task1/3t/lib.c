#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sched.h>  

#include <ucontext.h>
#define STACK_SIZE 4096 * 10

#include "lib.h"


typedef struct _meta_thread_inf {
    int cur_num; //= 0;
    int thread_num;// = 1;
} meta_thread;

uthread_t* uthreads[100];
meta_thread meta_info;

void scheduler() {
    ucontext_t *cur_ctx, *next_ctx;

    int cur_num = meta_info.cur_num; 
    
    cur_ctx = &(uthreads[cur_num]->ctx);
    cur_num = (cur_num + 1) % meta_info.thread_num;
    next_ctx = &(uthreads[cur_num]->ctx);
    meta_info.cur_num = cur_num;

    swapcontext(cur_ctx, next_ctx);

}

void uthread_start(void* arg) {
    uthread_t *uthread = (uthread_t*) arg;
    
    uthread->thread_func(uthread->arg);
    
    printf("thread exited\n");
    free(uthread);
    scheduler();
}

void* create_stack(off_t size) {
    void* stack = mmap(NULL, size, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
    mprotect(stack + 4096, STACK_SIZE - 2 * 4096, PROT_READ | PROT_WRITE);
    
    return stack + 4096;
}

ucontext_t uctx_main;
int uthread_create(uthread_t **thread, start_routine_t start_routine, void* arg) {    
    if (meta_info.thread_num == 0) {
        meta_info.thread_num++;
    }
    void* thread_stack = create_stack(STACK_SIZE);
    uthread_t *my_uthread = (uthread_t*)malloc(sizeof(uthread_t));
    my_uthread->stack = thread_stack;
    my_uthread->begin_stack = my_uthread->stack - 4096;
   // printf("MUNMAP %d\n", munmap(my_uthread->stack, STACK_SIZE));
    my_uthread->thread_func = start_routine;
    my_uthread->arg = arg;
    
    getcontext(&my_uthread->ctx);
    my_uthread->ctx.uc_stack.ss_sp = thread_stack; //кладем стек на котором будет выполнение
    my_uthread->ctx.uc_stack.ss_size = STACK_SIZE - 2 * 4096; //
  //  my_uthread->ctx.uc_link = &uctx_main; 
    makecontext(&my_uthread->ctx, (void(*)())uthread_start, 1, (void*)my_uthread); //на подготовленной структуре регистрируем функцию которую будем исполнять на стеке
    
    uthreads[meta_info.thread_num] = my_uthread;
    
    meta_info.thread_num++;

    *thread = my_uthread;
}

void set_main(uthread_t* main_th) {
    uthreads[0] = main_th;
}