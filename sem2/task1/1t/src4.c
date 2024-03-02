#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

void cleanup_handler(void* arg) {
	free((char*)arg);
	printf("Called cleanup handler\n");
}

void* mythread(void* arg) {
	int i = 0;
	int oldtype;
	char* str = (char*)malloc(12);
	strcpy(str, "hello world");
	pthread_cleanup_push(cleanup_handler, (void*)str);
	while (1) {
		printf("%s\n", str);
	}
	pthread_cleanup_pop(0);
	return NULL;
}

int main() {
	printf("%d\n", getpid());
	pthread_t thread[5];
	printf("MAIN [%d %d %d]\n", getpid(), getppid(), gettid());
	pthread_attr_t thread_attr;
	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

	pthread_create(&thread[0], &thread_attr, mythread, NULL);
	sleep(2);
	pthread_cancel(thread[0]);
	
	pthread_attr_destroy(&thread_attr);
	pthread_exit(NULL);
	return 0;
}
