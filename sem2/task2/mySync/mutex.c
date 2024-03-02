#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/syscall.h> 
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#include <linux/futex.h>
#include "mutex.h"

void mutex_init(mutex_t *m) {
    m->lock = 1;
}

int futex(int* uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3) {
    
    return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}

void mutex_lock(mutex_t * m) {
    int err;
    while(1) {
        int one = 1;
        if (atomic_compare_exchange_strong(&m->lock, &one, 0)) {
            break;
        }
        
        err = futex(&m->lock, FUTEX_WAIT, 0, NULL, NULL, 0); // перейдет в wait только если lock = 0 
        if (err == -1 && errno != EAGAIN) {
            printf("futex WAIT failed\n");
            abort();
        }
    }
}

void mutex_unlock(mutex_t * m) {
    int err;
    int zero = 0;
        
    if (atomic_compare_exchange_strong(&m->lock, &zero, 1)) {
        err = futex(&m->lock, FUTEX_WAKE, 1, NULL, NULL, 0);
        if (err == -1) {
            printf("futex WAKE failed\n");
            abort();
        }
    }
}
