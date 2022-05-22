#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
	void (*job_function)(void *arg);
	void *arg;
	void *next_job;

} threadpool_job;

typedef struct {
	threadpool_job *job_head;
	threadpool_job *job_tail;
	pthread_mutex_t job_mutex;
	pthread_cond_t job_available, job_active;
	int active_count, thread_count;
	int stop;
} threadpool;

threadpool_job *threadpool_job_new(void *funcptr, void *arg);
void threadpool_job_free(threadpool_job *job);

threadpool *threadpool_new(int thread_count);
void threadpool_free(threadpool *pool);

threadpool_job *threadpool_job_get(threadpool *pool);

int threadpool_add_job(threadpool *pool, void *funcptr, void *arg);
void threadpool_wait(threadpool *pool);

#endif
