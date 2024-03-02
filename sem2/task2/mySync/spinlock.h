#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdatomic.h>

#include <pthread.h>
#include <sched.h>

typedef struct {
	int lock;
} spinlock_t;

void spinlock_init(spinlock_t* s);
void spinlock_lock(spinlock_t* s);
void spinlock_unlock(spinlock_t* s);