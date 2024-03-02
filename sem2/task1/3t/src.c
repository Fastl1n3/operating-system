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


int g = 0;
ucontext_t uctx;

void f(void) {
    int err;
    g++;

    printf("f1: g: %d\n", g);
    err = setcontext(&uctx); //возьмет эту структуру и занесет в процессор
    printf("base: setcontext returned: %d\n", err);
}

int base(void) {
    int err;
    char str[] = "hello";
    err = getcontext(&uctx); //заполняет структуру
    printf("base: getcontext returned: %d\n", err);
    if (g == 0) {
        printf("g: %d\n", g);
        f();
    }
    else {
        printf("g: %d\n", g);
    }
}

ucontext_t uctx_main, uctx_func1, uctx_func2;

void func1(void) {
    printf("func1: started\n");
    printf("func1: 1 swapcontext\n");
    swapcontext(&uctx_func1, &uctx_func2);
    printf("func1: 2 swapcontext\n");
    swapcontext(&uctx_func1, &uctx_func2);
    printf("func1: 3 swapcontext\n");
    swapcontext(&uctx_func1, &uctx_func2);
    printf("func1: returned\n");
}

void func2(void) {
    printf("func2: started\n");
    printf("func2: 1 swapcontext\n");
    swapcontext(&uctx_func2, &uctx_func1);
    printf("func2: 2 swapcontext\n");
    swapcontext(&uctx_func2, &uctx_func1);
    printf("func2: returned\n");
  //  swapcontext(&uctx_func2, &uctx_main);
}


int main() {
    char func1_stack[16384];
    char func2_stack[16384];

    printf("sizeof(ucontext) = %ld\n", sizeof(ucontext_t));
    getcontext(&uctx_func1);
    uctx_func1.uc_stack.ss_sp = func1_stack; //кладем стек на котором будет выполнение
    uctx_func1.uc_stack.ss_size = sizeof(func1_stack); //
    uctx_func1.uc_link = &uctx_main; //если завершится функция, то это некст контекст который будет вызван 
    
    makecontext(&uctx_func1, func1, 0); //на подготовленной структуре регистрируем функцию которую будем исполнять на стеке(func1_stack)

    getcontext(&uctx_func2);
    uctx_func2.uc_stack.ss_sp = func2_stack;
    uctx_func2.uc_stack.ss_size = sizeof(func2_stack);
   // uctx_func2.uc_link =  &uctx_main;
    
    makecontext(&uctx_func2, func2, 0);
    
    printf("main: swapcontext\n");
    swapcontext(&uctx_main, &uctx_func2);
    printf("main: exiting\n");
    
	return 0;
}