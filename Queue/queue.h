#ifndef MYQUEUE_H_
#define MYQUEUE_H_

#ifndef MYQUEUE_TYPE
    #define MYQUEUE_TYPE int
#endif

struct myqueue_node{
    MYQUEUE_TYPE value;
    struct myqueue_node *next;
};

typedef struct myqueue_head{
    struct myqueue_node *head;
} myqueue;

#include <stdbool.h>
#include <stdlib.h>


//available Methods to outsiders
//set MYQUEUE_TYPE makro to a preferred data type

void myqueue_init(myqueue* q);

bool myqueue_is_empty(myqueue* q);

void myqueue_enqueue(myqueue* q, MYQUEUE_TYPE value);

MYQUEUE_TYPE myqueue_dequeue(myqueue* q);

void myqueue_destroy(myqueue* q);



void myqueue_init(myqueue* q) {
	q->head = NULL;
}

bool myqueue_is_empty(myqueue* q) {
	return q->head == NULL;
}

void myqueue_enqueue(myqueue* q, MYQUEUE_TYPE value) {
	struct myqueue_node** next;
	next = &(q->head);
	while(*next != NULL) {
		next = &((*next)->next);
	}
	*next =(struct myqueue_node *) malloc(sizeof **next);
	(*next)->value = value;
	(*next)->next = NULL;
}

MYQUEUE_TYPE myqueue_dequeue(myqueue* q) {
	if(myqueue_is_empty(q)) {
		// error
		return 0;
	}
	struct myqueue_node* oldest_node = q->head;
	MYQUEUE_TYPE ret_val = oldest_node->value;
	q->head = q->head->next;
	free(oldest_node);
	return ret_val;
}

void myqueue_destroy(myqueue* q) {
	struct myqueue_node *last_node, *node = q->head;
	if(myqueue_is_empty(q)) return;

	while(node->next != NULL) {
		last_node = node;
		node = node->next;
		free(last_node);
	}
	free(node);
}

#endif