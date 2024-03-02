#define _GNU_SOURCE
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include "cache.h"


node_t* add_cache(cache_t* cache, char* url, char* http_resp, size_t bytes) {
   // printf("Adding cache...\n");
    pthread_rwlock_wrlock(&cache->sync);
    
    node_t* old_node = get_exists_node(cache, url); //если есть нода то обновляем
    if (old_node != NULL) {
        //pthread_mutex_lock(&old_node->sync);
        pthread_rwlock_wrlock(&old_node->sync);
        size_t new_size = old_node->size + bytes;
        char* tmp_ptr = old_node->http_response;
       
        old_node->http_response = (char*)realloc(tmp_ptr, new_size);
        memcpy(old_node->http_response + old_node->size, http_resp, bytes);
        old_node->size += bytes;
        
        pthread_rwlock_unlock(&old_node->sync);
        //pthread_mutex_unlock(&old_node->sync);
        pthread_rwlock_unlock(&cache->sync);
        //printf("Updated cache\n");
        return old_node;    
    }
        
    if (cache->curr_size == cache->max_size) {
        printf("Cache is full, delete node...\n");
        node_t* del = cache->first;
        cache->first = cache->first->next;
        free(del->http_response);
        free(del);
        cache->curr_size--;
    }

    node_t* node = cache->first;
    if (node == NULL) {
        cache->first = (node_t*)malloc(sizeof(node_t));
        strcpy(cache->first->url, url);
        cache->first->http_response = (char*)malloc(bytes);
        memcpy(cache->first->http_response, http_resp, bytes);
        cache->first->next = NULL;
        cache->first->size = bytes;
        cache->first->is_all = 0;
        //pthread_mutex_init(&cache->first->sync, NULL);
        pthread_rwlock_init(&cache->first->sync, NULL);
        cache->curr_size++;
        node = cache->first;
    }
    else {
        while (node->next != NULL) {
            node = node->next;
        }

        node_t* tmp = (node_t*)malloc(sizeof(node_t));
        strcpy(tmp->url,url);
        tmp->http_response = (char*)malloc(bytes);
        memcpy(tmp->http_response, http_resp, bytes);
        tmp->next = NULL;
        tmp->size = bytes;
        tmp->is_all = 0;
        //pthread_mutex_init(&tmp->sync, NULL);
        pthread_rwlock_init(&tmp->sync, NULL);
        node->next = tmp;
        cache->curr_size++;
        node = node->next;
    }

    pthread_rwlock_unlock(&cache->sync);
    printf("Added to cache\n");
    return node;
}

node_t* search_cache(cache_t* cache, char* url) {
    pthread_rwlock_rdlock(&cache->sync);  
    node_t* node = cache->first;
    while (node != NULL) {
        if(strcmp(node->url, url) == 0) {
            pthread_rwlock_unlock(&cache->sync);
            return node;
        }
        node = node->next;
    }  
    printf("URL: %s is not found\n", url);
    pthread_rwlock_unlock(&cache->sync);
    return NULL;
}

void init_cache(cache_t* cache_list, int max_size) {
    cache_list->first = NULL;
    cache_list->curr_size = 0;
    cache_list->max_size = max_size;
    pthread_rwlock_init(&cache_list->sync, NULL);
}

node_t* get_exists_node(cache_t* cache, char* url) {  
    node_t* node = cache->first;
    while (node != NULL) {
        if(strcmp(node->url, url) == 0) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

ssize_t get_response_cache(node_t* node, char* buffer, ssize_t* itr, int buf_size) {
    //pthread_mutex_lock(&node->sync);
    pthread_rwlock_rdlock(&node->sync); 
    if (*itr == node->size && node->is_all) {
        //pthread_mutex_unlock(&node->sync);
        pthread_rwlock_unlock(&node->sync);
        return -1;
    }
    ssize_t writed_len;
    if (node->size - *itr > buf_size) {
        memcpy(buffer, node->http_response + (*itr), buf_size);
        writed_len = buf_size;
        *itr += buf_size;
    }
    else {
        memcpy(buffer, node->http_response + (*itr), node->size - *itr);
        writed_len = node->size - (*itr);
        *itr = node->size;
    }
 
    pthread_rwlock_unlock(&node->sync);
   // pthread_mutex_unlock(&node->sync);
    return writed_len;
}

void set_all(node_t* node, int value) {
    //pthread_mutex_lock(&node->sync);
    pthread_rwlock_wrlock(&node->sync);
    node->is_all = value;
    pthread_rwlock_unlock(&node->sync);
    //pthread_mutex_unlock(&node->sync);
}