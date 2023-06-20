/* Disclosure: The code provided herein contains a code template
that was derived from the repository "Fabi12345678910/c_templates." */
#define _DEFAULT_SOURCE 200809L
#include "thread_pool.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define JOB_STATE_WAITING  0
#define JOB_STATE_ACTIVE   1
#define JOB_STATE_FINISHED 2


//#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

void *worker_thread(void *);

void pool_create(thread_pool* pool, size_t size){
    pool->exit_requested = 0;
    pool->mutexAccessQueue = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    pool->condWorkAvailable = (pthread_cond_t) PTHREAD_COND_INITIALIZER;

    pool->queue_waiting_head = NULL;

    //create initial finished job to reduce edge cases
    if((pool->queue_head = malloc(sizeof(*pool->queue_head))) == NULL){
        fprintf(stderr, "errr creating queueHead");
        return;
    }
    pool->queue_head->state = 1;
//    pool->queue_head->id = 0;
    pool->queue_head->next = NULL;

    pool->queue_tail = pool->queue_head;

    //spawn threads
    if((pool->threads = malloc(size * sizeof(pthread_t))) == NULL){
        fprintf(stderr, "error getting space for the worker threads");
        return;
    }

    pool->ammount_threads = size;
    for(size_t i = 0; i < size; i++){
        if(pthread_create(&pool->threads[i], NULL, worker_thread, pool)){
            fprintf(stderr, "error creating workier_thread %lu\n", i);
            exit(EXIT_FAILURE);
        }
    }
}

//enqueue job
job_id pool_submit(thread_pool* pool, job_function start_routine, job_arg arg) {
    //create new queueEntry
    struct job_queue_entry *newQueueEntry = malloc(sizeof(*pool->queue_tail->next));
    newQueueEntry->start_routine = start_routine;
    newQueueEntry->arg = arg;
    newQueueEntry->state = JOB_STATE_WAITING;
    newQueueEntry->next = NULL;
    newQueueEntry->mutexState = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_lock(&pool->mutexAccessQueue);
    DEBUG_PRINT("enqueing job\n");
    
    //actually insert new Entry
    pool->queue_tail->next = newQueueEntry;
    pool->queue_tail = newQueueEntry;

    //update waiting head
    if(pool->queue_waiting_head == NULL){
        pool->queue_waiting_head = newQueueEntry;
    }
    pthread_mutex_unlock(&pool->mutexAccessQueue);
    pthread_cond_signal(&pool->condWorkAvailable);

    DEBUG_PRINT("enqued job\n");
    return newQueueEntry;
}

void pool_await(job_id id) {
    pthread_mutex_lock(&id->mutexState);
    while(id->state != JOB_STATE_FINISHED){
        DEBUG_PRINT("awaiting job end\n");
        pthread_cond_wait(&id->condFinished, &id->mutexState);
    }
    pthread_mutex_unlock(&id->mutexState);
}

void pool_destroy(thread_pool* pool) {

    pthread_mutex_lock(&pool->mutexAccessQueue);

    pool->exit_requested = 1;

    pthread_mutex_unlock(&pool->mutexAccessQueue);
    pthread_cond_broadcast(&pool->condWorkAvailable);

    for(size_t i = 0; i < pool->ammount_threads; i++){
        pthread_join(pool->threads[i], NULL);
    }

    free(pool->threads);
    struct job_queue_entry *lastEntry, *entry;
    entry = pool->queue_head;
    while(entry != NULL){
        lastEntry = entry;
        entry = entry->next;
        free(lastEntry);
    }
}

void *worker_thread(void * arg){
    thread_pool* pool = arg;
    
    while(1){
        pthread_mutex_lock(&pool->mutexAccessQueue);
        while(pool->queue_waiting_head == NULL && !pool->exit_requested){
            pthread_cond_wait(&pool->condWorkAvailable, &pool->mutexAccessQueue);
        }
        //critical stuff = get new work;
        if(pool->exit_requested){
            DEBUG_PRINT("exit requested, exiting\n");
            pthread_mutex_unlock(&pool->mutexAccessQueue);
            return EXIT_SUCCESS;
        }


        struct job_queue_entry *workingJob = pool->queue_waiting_head;
        DEBUG_PRINT("took job\n");
        workingJob->state = JOB_STATE_ACTIVE;

        //move queue forward 
        pool->queue_waiting_head = pool->queue_waiting_head->next;
        pthread_mutex_unlock(&pool->mutexAccessQueue);

        workingJob->start_routine(workingJob->arg);
        
        pthread_mutex_lock(&workingJob->mutexState);
        //update job Details
        workingJob->state = JOB_STATE_FINISHED;
        pthread_mutex_unlock(&workingJob->mutexState);
        pthread_cond_broadcast(&workingJob->condFinished);
    }

    return NULL;
}