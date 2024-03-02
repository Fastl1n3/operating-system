#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdatomic.h>
#include <semaphore.h>

#include <pthread.h>
#include <sched.h>

#include "queue.h"

#define RED "\033[41m"
#define NOCOLOR "\033[0m"

pthread_spinlock_t spin;
pthread_mutex_t mutex;
pthread_cond_t cond;
sem_t sem, sem1, sem2;

void set_cpu(int n) {
	int err;
	cpu_set_t cpuset;
	pthread_t tid = pthread_self();

	CPU_ZERO(&cpuset);
	CPU_SET(n, &cpuset);

	err = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset);
	if (err) {
		printf("set_cpu: pthread_setaffinity failed for cpu %d\n", n);
		return;
	}

	printf("set_cpu: set cpu %d\n", n);
}

void *reader(void *arg) {
	int expected = 0;
	queue_t *q = (queue_t *)arg;
	printf("reader [%d %d %d]\n", getpid(), getppid(), gettid());

	set_cpu(1);
	sem_wait(&sem1);
	while (1) {
		int val = -1;
		sem_wait(&sem);
		while (q->count == 0) {
			sem_post(&sem);
			sem_wait(&sem1);
			sem_wait(&sem);
		}
		int ok = queue_get(q, &val);
		sem_post(&sem2);

		sem_post(&sem);
		if (!ok)
			continue;

		if (expected != val)
			printf(RED"ERROR: get value is %d but expected - %d" NOCOLOR "\n", val, expected);

		expected = val + 1;
		usleep(1);
	}

	return NULL;
}

void *writer(void *arg) {
	int i = 0;
	queue_t *q = (queue_t *)arg;
	printf("writer [%d %d %d]\n", getpid(), getppid(), gettid());

	set_cpu(2);

	while (1) {			
		sem_wait(&sem);

		while (q->count == q->max_count) {
			sem_post(&sem);
			sem_wait(&sem2);
			sem_wait(&sem);
		}	
		int ok = queue_add(q, i);
		sem_post(&sem1);

		sem_post(&sem);
		if (!ok)
			continue;
		i++;
		usleep(1);
	}

	return NULL;
}

int main() {
	pthread_t tid;
	queue_t *q;
	int err;


	pthread_mutex_init(&mutex, NULL);	
	sem_init(&sem1, 0, 1);
	sem_init(&sem2, 0, 1);
	sem_init(&sem, 0, 1);
	printf("main [%d %d %d]\n", getpid(), getppid(), gettid());

	q = queue_init(1000000);

	err = pthread_create(&tid, NULL, reader, q);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

	//sched_yield();

	err = pthread_create(&tid, NULL, writer, q);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

	//sleep(6);
	pthread_exit(NULL);
	pthread_spin_destroy(&spin);
	pthread_mutex_destroy(&mutex);
	
	return 0;
}
