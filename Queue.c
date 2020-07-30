// Q.c ... implementation of Q ADT
// Written by John Shepherd, March 2013

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "Queue.h"

typedef struct Node {
    PlaceId place;
	struct Node *next;
} Node;

typedef struct Queue {
	Node *head;  // ptr to first node
	Node *tail;  // ptr to last node
} Queue;

// create new empty Q
Q newQueue()
{
	Q q;
	q = malloc(sizeof(Queue));
	assert(q != NULL);
	q->head = NULL;
	q->tail = NULL;
	return q;
}

// free memory used by Q
void dropQueue(Q Q)
{
	Node *curr, *next;
	assert(Q != NULL);
	// free list nodes
	curr = Q->head;
	while (curr != NULL) {
		next = curr->next;
		free(curr);
		curr = next;
	}
	// free Q rep
	free(Q);
}

// display as 3 > 5 > 4 > ...
void showQueue(Q Q)
{
	Node *curr;
	assert(Q != NULL);
	// free list nodes
	curr = Q->head;
	while (curr != NULL) {
		// ItemShow(curr->value);
		if (curr->next != NULL)
			printf(">");
		curr = curr->next;
	}
	printf("\n");
}

// add item at end of Q 
void QueueJoin(Q Q, PlaceId loc)
{
	assert(Q != NULL);
	Node *new = malloc(sizeof(Node));
	assert(new != NULL);
	new->place = loc;
	new->next = NULL;
	if (Q->head == NULL)
		Q->head = new;
	if (Q->tail != NULL)
		Q->tail->next = new;
	Q->tail = new;
}

// remove item from front of Q
PlaceId QueueLeave(Q Q)
{
	assert(Q != NULL);
	assert(Q->head != NULL);
	PlaceId loc = Q->head->place;
	Node *old = Q->head;
	Q->head = old->next;
	if (Q->head == NULL)
		Q->tail = NULL;
	free(old);
	return loc;
}

// check for no items
int QueueIsEmpty(Q Q)
{
	return (Q->head == NULL);
}

