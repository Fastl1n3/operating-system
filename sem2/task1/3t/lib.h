#pragma once

typedef void* (*start_routine_t)(void*); //определяем указатель на функцию, возвращающую void* и принимающую void*

typedef struct _uthread {
    ucontext_t ctx;
    start_routine_t thread_func;
    void* arg;
    void* stack;
    void* begin_stack;
} uthread_t;

void scheduler();
int uthread_create(uthread_t **thread, start_routine_t start_routine, void* arg);
void set_main(uthread_t* main_th);