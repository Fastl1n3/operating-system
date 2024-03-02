//#pragma once
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

typedef struct _Node {
	char value[100];
	struct _Node *next;
    pthread_rwlock_t sync;
} node_t;

typedef struct _Storage {
	node_t *first;
} list_t;

list_t* list_init(int max_count);
void list_destroy(list_t *list);

