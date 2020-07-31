////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// Queue.h: For finding the shortest path between two place
//  
// Written by Team Ahsan-Aryaman-Michael-Vishnu 
// July 2020
//
// Adapted from Queue.h: Implementation of Queue ADT
// Written by John Sheperd, March 2013
////////////////////////////////////////////////////////////////////////

#include "Places.h"

typedef struct Queue *Q;

Q newQueue(); // create an empty Queue
void dropQueue(Q); // free memory used by queue
void QueueJoin(Q,PlaceId); // add item on queue
PlaceId QueueLeave(Q); // remove item from queue
int QueueIsEmpty(Q); // check for no items



