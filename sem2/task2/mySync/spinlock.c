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
#include "spinlock.h"

void spinlock_init(spinlock_t* s) {
	s->lock = 1;
}

void spinlock_lock(spinlock_t* s) {
	while(1) {
		int one = 1;
		if (atomic_compare_exchange_strong(&s->lock, &one, 0)) {
			break;
		}
	}
}

void spinlock_unlock(spinlock_t* s) {
	int zero = 0;
	atomic_compare_exchange_strong(&s->lock, &zero, 1);
}