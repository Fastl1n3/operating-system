#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

typedef struct _Node {
	char* http_response;
    size_t size;
    int is_all;
    char url[256];
   // pthread_mutex_t sync;
	pthread_rwlock_t sync;
    struct _Node *next;
} node_t;

typedef struct _Storage {
	node_t *first;
    int max_size;
    int curr_size;
    pthread_rwlock_t sync;
} cache_t;

node_t* search_cache(cache_t* cache, char* url);
node_t* add_cache(cache_t* cache, char* url, char* http_resp, size_t bytes);
void init_cache(cache_t* cache_list, int max_size);
node_t* get_exists_node(cache_t* cache, char* url);
ssize_t get_response_cache(node_t* node, char* buffer, ssize_t* itr, int buf_size);
void set_all(node_t* node, int value);