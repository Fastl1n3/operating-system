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

#include "lib.h"

void* mythread_routine(void* arg) {    
    int i = 0;
    while (i < 5) {
        printf("hello from thread: %s; i = %d\n", (char*)arg, i);
        i++;
        usleep(500000);
        scheduler();
    }
    printf("thread finished: %s\n", (char*)arg);
}

int main(int argc, char* argv[]) {
    uthread_t *ut[3];
    uthread_t main;
    set_main(&main);

    printf("main: started\n");
    
    uthread_create(&ut[0], mythread_routine, "arg 11111");
    uthread_create(&ut[1], mythread_routine, "arg 22222");
    uthread_create(&ut[2], mythread_routine, "arg 33333");

    while (1) {
        printf("main\n");
        usleep(500000);
        scheduler();
    }
    printf("main: returned\n");

	return 0;
}