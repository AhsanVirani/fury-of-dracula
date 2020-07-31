////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// Queue.c: Implementation of Queue ADT
//
//   
// Written by Team Ahsan-Aryaman-Michael-Vishnu 
// July 2020
//  
//
// Adapted from Queue.c: Implementation of Queue ADT
// Written by John Sheperd, March 2013
////////////////////////////////////////////////////////////////////////


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
	assert(Q != NULL && Q->head != NULL);
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

