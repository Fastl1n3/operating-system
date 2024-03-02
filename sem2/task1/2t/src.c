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

#define STACK_SIZE 4096 * 8

typedef void* (*start_routine_t)(void*); //определяем указатель на функцию, возвращающую void* и принимающую void*

typedef struct _mythread {
    int             mythread_id;
    start_routine_t start_routine;
    void*           arg;
    void*           retval;
    volatile int    joined;
    volatile int    exited;
} mythread_struct_t;

typedef mythread_struct_t* mythread_t;


int mythread_start(void* arg) {
    mythread_struct_t *mythread = (mythread_struct_t*) arg;

    mythread->retval = mythread->start_routine(mythread->arg);
    mythread->exited = 1;
    //wait join
    while(mythread->joined) {
        sleep(1);
    }
    free(mythread);
    return 0;
}

int mythread_join(mythread_t mytid, void** retval) {
    mythread_struct_t* mythread = mytid;
    //wait thread ends
    while(!mythread->exited) {
        sleep(1);
    }
    *retval = mythread->retval;
    mythread->joined = 1;
    return 0;
}

void* create_stack(off_t size, int thread_id) {
    void* stack = mmap(NULL, size, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
    return stack;
}

int mythread_create(mythread_t *mytid, start_routine_t start_routine, void* arg) {
    static int thread_id = 0;
    thread_id++;
    mythread_struct_t *mythread = (mythread_struct_t*)malloc(sizeof(mythread_struct_t));
    
    void* child_stack = create_stack(STACK_SIZE, thread_id);
    mprotect(child_stack + 4096, STACK_SIZE - 2 * 4096, PROT_READ | PROT_WRITE);
    
    mythread->start_routine = start_routine;
    mythread->arg = arg;
    mythread->mythread_id = thread_id;
    mythread->exited = 0;
    mythread->joined = 0;

    int child_pid = clone(mythread_start, child_stack + STACK_SIZE - 4096, CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_THREAD | CLONE_SIGHAND | SIGCHLD, (void*) mythread);
    if (child_pid == -1) {
        printf("clone failed: %s\n" ,strerror(errno));
        return -1;
    }
    *mytid = mythread;
    return 0;
}


void* mythread_routine(void* arg) {    
    int i = 0;
    while (i < 15) {
        printf("hello from thread: %s\n", (char*)arg);
        i++;
    } 
    return "SUCCESS";
}

int main() {
    mythread_t tid[5];
    void* retval;
    printf("pid: %d\n", getpid());
    mythread_create(&tid[0], mythread_routine, "hello from 1");
    mythread_create(&tid[1], mythread_routine, "hello from 2");
    mythread_create(&tid[2], mythread_routine, "hello from 3");
    mythread_create(&tid[3], mythread_routine, "hello from 4");
    mythread_create(&tid[4], mythread_routine, "hello from 5");
    mythread_join(tid[0], &retval);
    mythread_join(tid[1], &retval);
    mythread_join(tid[2], &retval);
    mythread_join(tid[3], &retval);
    mythread_join(tid[4], &retval);
   // printf("thread returned: %s\n", (char*)retval);
	return 0;
}