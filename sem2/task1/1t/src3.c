#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
typedef struct args {
	int a;
	char* str;
	int returned;
}thread_data;

void* mythread(void* arg) {
	thread_data* data = (thread_data*)arg;
	printf("data: %d, %s\n", data->a, data->str);
	data->returned = 5;
	return NULL;
}

int main() {
	pthread_t tid;
	int err;
	printf("%d\n", getpid());
	pthread_t thread[5];
	printf("MAIN [%d %d %d]\n", getpid(), getppid(), gettid());
	pthread_attr_t thread_attr;
	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);
	
	thread_data* data = (thread_data*)malloc(sizeof(thread_data));
	data->a = 4;
	data->str = "hello";
	printf("data: %d, %s\n", data->a, data->str);
	pthread_create(&thread[0], &thread_attr, mythread, (void*)data);
	void* status;
	pthread_join(thread[0], &status);
	printf("%d\n", data->returned);
	free(data);
	pthread_attr_destroy(&thread_attr);

	pthread_exit(NULL);

	return 0;
}
