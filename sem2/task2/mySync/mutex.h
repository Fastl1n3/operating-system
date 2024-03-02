#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdatomic.h>

#include <linux/futex.h>

typedef struct {
    int lock;
} mutex_t;

int futex(int* uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3);
void mutex_init(mutex_t *m);
void mutex_lock(mutex_t * m);
void mutex_unlock(mutex_t * m);

