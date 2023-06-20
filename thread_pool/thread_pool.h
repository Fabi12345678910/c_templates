/* Disclosure: The code provided herein contains a code template
that was derived from the repository "Fabi12345678910/c_templates." */
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <stddef.h> // for size_t
#include <pthread.h>
#include <stdbool.h>

typedef void* (*job_function)(void*);
typedef void* job_arg;

/***
 * This is the stub of a simple job queue.
 */

struct job_queue_entry {
	struct job_queue_entry *next;
	int state;
	pthread_mutex_t mutexState;
	pthread_cond_t condFinished;
	
	job_function start_routine;
	job_arg arg;
};

typedef struct job_queue_entry *job_id;

/***
 * This is the stub for the thread pool that uses the queue.
 * Implement at LEAST the Prototype functions below.
 */

typedef struct {
	bool exit_requested;
	struct job_queue_entry *queue_head;
	struct job_queue_entry *queue_waiting_head;
	struct job_queue_entry *queue_tail;
	pthread_mutex_t mutexAccessQueue;
	pthread_cond_t condWorkAvailable;
	size_t ammount_threads;
	pthread_t *threads;
} thread_pool;

// Prototypes for REQUIRED functions
void pool_create(thread_pool* pool, size_t size);
job_id pool_submit(thread_pool* pool, job_function start_routine,
                   job_arg arg); // You need to define a datatype for the job_id (chose wisely).
void pool_await(job_id id);
void pool_destroy(thread_pool* pool);

#endif
