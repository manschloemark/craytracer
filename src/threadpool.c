#include "threadpool.h"

#define DEFAULT_THREAD_COUNT 4

void *threadpool_worker(void *threadpool_ptr) {
	threadpool *pool = (threadpool *)threadpool_ptr;

	threadpool_job *job;
	while(1) {
		pthread_mutex_lock(&pool->job_mutex);

		// No jobs but pool is still running
		while(pool->job_head == NULL && !pool->stop)
			pthread_cond_wait(&pool->job_available, &pool->job_mutex);

		if(pool->stop) break;

		job = threadpool_job_get(pool);
		pool->active_count++;
		pthread_mutex_unlock(&pool->job_mutex);

		if(job != NULL) {
			job->job_function(job->arg);
			threadpool_job_free(job);
		}

		pthread_mutex_lock(&pool->job_mutex);
		pool->active_count--;
		if(!pool->stop && pool->active_count == 0 && pool->job_head == NULL) {
			pthread_cond_signal(&pool->job_active);
		}
		pthread_mutex_unlock(&pool->job_mutex);
	}
	pool->thread_count--;
	pthread_cond_signal(&pool->job_active);
	pthread_mutex_unlock(&pool->job_mutex);

	return NULL;
}

threadpool_job *threadpool_job_new(void *funcptr, void *arg) {
	if(funcptr == NULL) return NULL;
	threadpool_job *job = malloc(sizeof(threadpool_job));
	if (job == NULL) return NULL;

	job->job_function = funcptr;
	job->arg = arg;
	job->next_job = NULL;
	return job;
}

void threadpool_job_free(threadpool_job *job) {
	if(job != NULL) free(job);
}


threadpool *threadpool_new(int thread_count) {
	if(thread_count == 0) thread_count = DEFAULT_THREAD_COUNT; // NOTE : get threadcount from os
	threadpool *pool = (threadpool *)calloc(1, sizeof(threadpool));

	pool->thread_count = thread_count;
	pthread_mutex_init(&pool->job_mutex, NULL);
	pthread_cond_init(&pool->job_available, NULL);
	pthread_cond_init(&pool->job_active, NULL);

	pool->job_head = NULL;
	pool->job_tail = NULL;
	pool->stop = 0;

	pthread_t thread;
	for (int i = 0; i < pool->thread_count; ++i) {
		pthread_create(&thread, NULL, threadpool_worker, pool);
		pthread_detach(thread);
	}
	return pool;
}

void threadpool_free(threadpool *pool) {
	pthread_mutex_lock(&pool->job_mutex);
	threadpool_job *job = pool->job_head;
	threadpool_job *next_job = NULL;
	while (job!=NULL) {
		next_job = job->next_job;
		threadpool_job_free(job);
		job = next_job;
	}
	pool->stop = 1;
	pthread_cond_broadcast(&pool->job_available);
	pthread_mutex_unlock(&pool->job_mutex);

	threadpool_wait(pool);

	pthread_mutex_destroy(&pool->job_mutex);
	pthread_cond_destroy(&pool->job_available);
	pthread_cond_destroy(&pool->job_active);

	free(pool);
}

threadpool_job *threadpool_job_get(threadpool *pool) {
	threadpool_job *job = pool->job_head;
	if(job == NULL) return NULL;
	if(job->next_job == NULL) {
		pool->job_head = NULL;
		pool->job_tail = NULL;
	} else {
		pool->job_head = job->next_job;
	}
	return job;
}

int threadpool_add_job(threadpool *pool, void *funcptr, void *arg) {
	threadpool_job *job = threadpool_job_new(funcptr, arg);
	if(job == NULL) return 0;

	pthread_mutex_lock(&pool->job_mutex);
	if(pool->job_head == NULL) {
		pool->job_head = job;
		pool->job_tail = pool->job_head;
	} else {
		pool->job_tail->next_job = job;
		pool->job_tail = job;
	}

	pthread_cond_broadcast(&pool->job_available);
	pthread_mutex_unlock(&pool->job_mutex);
	return 1;
}
void threadpool_wait(threadpool *pool) {
	pthread_mutex_lock(&pool->job_mutex);
	while(1) {
		if((!pool->stop && pool->active_count != 0) || (pool->stop && pool->thread_count != 0)) {
			pthread_cond_wait(&(pool->job_active), &(pool->job_mutex));
		} else {
			break;
		}
	}
	pthread_mutex_unlock(&pool->job_mutex);
}

