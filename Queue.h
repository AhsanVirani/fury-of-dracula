////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// Queue.h: For finding the shortest path between two place
//  
//  Team: Ahsan,Aryaman,Michael,Vishnu    
//  Vishnu Birudavolu(z5280782)
//
// Adapted from 
////////////////////////////////////////////////////////////////////////

#include "Places.h"

typedef struct Queue *Q;

Q newQueue(); // create an empty Queue
void dropQueue(Q); // free memory used by queue
void showQueue(Q); // display as 3 > 5 > 4 > ...
void QueueJoin(Q,PlaceId); // add item on queue
PlaceId QueueLeave(Q); // remove item from queue
int QueueIsEmpty(Q); // check for no items



