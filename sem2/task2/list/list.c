#define _GNU_SOURCE
#include <pthread.h>
#include <string.h>
#include "list.h"

list_t* list_init(int max_count) {
    list_t* list = (list_t*)malloc(sizeof(list_t));
    
    list->first = (node_t*)malloc(sizeof(node_t)); 
    strcpy(list->first->value, "0");
    pthread_mutex_init(&list->first->sync, NULL);
    list->first->next = NULL;
    node_t* nd = NULL;
    node_t* tmp = list->first;
    for (int i = 1; i < max_count; i++) {
        nd = (node_t*)malloc(sizeof(node_t)); 
        for (int j = 0 ; j < i % 100; j++) {    
            nd->value[j] = 'a';
        }
        
        pthread_mutex_init(&nd->sync, NULL);
        nd->next = NULL;
        tmp->next = nd;
        tmp = nd; 
    }
    return list;
}

void list_destroy(list_t* list) {
    node_t* nd = list->first;
    while (nd != NULL) {
        node_t* tmp = nd->next;
        free(nd);
        nd = tmp;
    }
    free(list);
}