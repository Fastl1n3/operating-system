#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void sig_handler(int signum) {
	printf("Message from sig_handler\n");
	//signal(signum, SIG_IGN);
	//pthread_exit(NULL);
}
void* thread_func1(void* arg) {
	sigset_t set;
    sigfillset(&set);
    pthread_sigmask(SIG_SETMASK, &set, NULL);

	while(1) {
		printf("THREAD 1\n");
		sleep(1);
	}
	return NULL;
}

void* thread_func2(void* arg) {
	signal(SIGINT, sig_handler);
	while(1) {
		printf("THREAD 2\n");
		sleep(1);
	}
	return NULL;
}

void* thread_func3(void* arg) {
	sigset_t set;
    //sigfillset(&set);
	sigemptyset(&set);
	sigaddset(&set, SIGQUIT);
	int sig;
    //pthread_sigmask(SIG_SETMASK, &set, NULL);
	printf("THREAD 3\n");
	sigwait(&set, &sig);
	printf("SIGNAL FROM THREAD 3 = %d\n", sig);
	return NULL;
}


int main() {
	printf("%d\n", getpid());
	pthread_t thread[3];
	printf("MAIN [%d %d %d]\n", getpid(), getppid(), gettid());
	pthread_attr_t thread_attr;
	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

	pthread_create(&thread[0], &thread_attr, thread_func1, NULL);
    pthread_create(&thread[1], &thread_attr, thread_func2, NULL);
    pthread_create(&thread[2], &thread_attr, thread_func3, NULL);
	
	
//	sleep(2);
	//pthread_kill(thread[2], SIGQUIT);
	//pthread_kill(thread[1], SIGINT);
//	pthread_kill(thread[0], SIGINT);
		
	//sleep(20);
	pthread_attr_destroy(&thread_attr);
//	pthread_kill(thread[0], SIGKILL);
	pthread_exit(NULL);
	return 0;
}
