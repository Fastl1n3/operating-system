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

#include "list_spin.h"

int iter_first = 0;
int iter_second = 0;
int iter_third = 0;
int swap_count = 0;

void* check_increasing(void* arg) {
    list_t *list = (list_t*)arg;
    while(1) {
        int count = 0;
        node_t* nd = list->first;
        //////////////////
        pthread_spin_lock(&nd->sync);
        while (nd->next != NULL) {
            node_t* nd2 = nd->next;
            pthread_spin_lock(&nd2->sync);
            if (strlen(nd->value) < strlen(nd2->value)) {
                count++;
            }
            pthread_spin_unlock(&nd->sync);
            nd = nd2;
        }
        pthread_spin_unlock(&nd->sync);
        // printf("count_increasing = %d\n", count);
        // sleep(1);
        iter_first++;
    }
} 

void* check_decreasing(void* arg) {
    list_t *list = (list_t*)arg;
    while(1) {
        int count = 0;
        node_t* nd = list->first;
        //////////////////
        pthread_spin_lock(&nd->sync);
        while (nd->next != NULL) {
            node_t* nd2 = nd->next;
            pthread_spin_lock(&nd2->sync);
            if (strlen(nd->value) > strlen(nd2->value)) {
                count++;
            }
            pthread_spin_unlock(&nd->sync);
            nd = nd2;
        }
        pthread_spin_unlock(&nd->sync);
        //printf("count_decreasing = %d\n", count);
        // sleep(1);
        iter_second++;
    }
}

void* check_equals(void* arg) {
    list_t *list = (list_t*)arg;
    while(1) {
        int count = 0;
        node_t* nd = list->first;
        //////////////////
        pthread_spin_lock(&nd->sync);
        while (nd->next != NULL) {
            node_t* nd2 = nd->next;
            pthread_spin_lock(&nd2->sync);
            if (strlen(nd->value) == strlen(nd2->value)) {
                count++;
            }
            pthread_spin_unlock(&nd->sync);
            nd = nd2;
        }
        pthread_spin_unlock(&nd->sync);
        //  printf("count_equals = %d\n", count);
        //////////////////
        //  sleep(1);
        iter_third++;
    }
}

void* swap(void* arg) {
    list_t *list = (list_t*)arg;
    
    while(1) {    
        node_t* nd = list->first;
        pthread_spin_lock(&nd->sync);
        node_t* nd2 = nd->next;
        pthread_spin_lock(&nd2->sync);
        
        while (nd2->next != NULL) {
        
            node_t* nd3 = nd2->next;
            pthread_spin_lock(&nd3->sync);
            if (rand() % 2 == 1) {       
                nd2->next = nd3->next;
                nd3->next = nd2;
                nd->next = nd3;
                pthread_spin_unlock(&nd->sync);
                swap_count++;   
                nd = nd3; 
            }
            else {
                pthread_spin_unlock(&nd->sync);
                nd = nd2;
                nd2 = nd3;
                
            }
        }
        pthread_spin_unlock(&nd->next->sync);
        pthread_spin_unlock(&nd->sync);     
   }
}

int main() {
    srand(time(NULL));
    list_t* list = list_init(123456);
    
    // node_t* nd = list->first;
    // while(nd != NULL) {
    //     printf("%s\n", nd->value);
    //     nd = nd->next;
    // }
    pthread_t tid[6];
    pthread_create(&tid[0], NULL, check_increasing, list);
    pthread_create(&tid[1], NULL, check_decreasing, list);
    pthread_create(&tid[2], NULL, check_equals, list);
    
    pthread_create(&tid[3], NULL, swap, list);
    pthread_create(&tid[4], NULL, swap, list);
    pthread_create(&tid[5], NULL, swap, list);
    
    while(1) {
        printf("stat: iter_inc = %d, iter_dec = %d, iter_eq = %d, swap_count = %d\n", iter_first, iter_second, iter_third, swap_count);
        sleep(1);
    }

    pthread_exit(NULL);
    list_destroy(list);
    return 0;
}