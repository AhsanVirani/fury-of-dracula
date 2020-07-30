////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// HunterView.c: the HunterView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10   v3.0    Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "Game.h"
#include "GameView.h"
#include "HunterView.h"
#include "Map.h"
#include "Places.h"
#include "Queue.h"

#define MAX 70

int intArray[MAX];
int front = 0;
int rear = -1;
int itemCount = 0;

int peek();
bool isEmpty(); 
bool isFull(); 
int size();
void insert(int data);
int removeData();

// add your own #includes here

// TODO: ADD YOUR OWN STRUCTS HERE

struct hunterView {
	// TODO: ADD FIELDS HERE
	//DONE
	GameView gView;
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

HunterView HvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	HunterView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate HunterView!\n");
		exit(EXIT_FAILURE);
	}

	new->gView = GvNew(pastPlays, messages);

	return new;
}

void HvFree(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	//DONE
	GvFree(hv->gView);
	free(hv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round HvGetRound(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	//DONE
	return GvGetRound(hv->gView);
}

Player HvGetPlayer(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	// DONE
	return GvGetPlayer(hv->gView);
	
}

int HvGetScore(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	// DONE	
	return GvGetScore(hv->gView);
}

int HvGetHealth(HunterView hv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	//DONE
	return GvGetHealth(hv->gView, player);
}

PlaceId HvGetPlayerLocation(HunterView hv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	//DONE
	return GvGetPlayerLocation(hv->gView, player);
	
}

PlaceId HvGetVampireLocation(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	// DONE
	return GvGetVampireLocation(hv->gView);
}

////////////////////////////////////////////////////////////////////////
// Utility Functions

PlaceId HvGetLastKnownDraculaLocation(HunterView hv, Round *round)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*round = 0;
	int result;
	int count = -1;
	bool locBool;
	PlaceId *placeResult = GvGetLastLocations(hv->gView, PLAYER_DRACULA, 5,
											  &result, &locBool);
	PlaceId finalPlace = NOWHERE;

	for (int i = 0; i < result; i++) {
		// printf("the result is %d\n", placeResult[i]);
		
		if (placeResult[i] != CITY_UNKNOWN) {
			finalPlace = placeResult[i];
			count++;
		}	
	}

		if (count > 0)
			*round = count;
	
	return finalPlace;
}

PlaceId *HvGetShortestPathTo(HunterView hv, Player hunter, PlaceId dest,
                             int *pathLength)
{
	
	Map tempMap = MapNew();
	int connNum = MapNumPlaces(tempMap);
	int *dist = calloc (connNum, sizeof(int));


	PlaceId *visited = calloc (connNum, sizeof(PlaceId));
	PlaceId *prevPlace = malloc (connNum * sizeof(PlaceId));
	int i;
	for (i = 0; i < connNum; i++) {
		prevPlace[i] = NOWHERE;
	}
	PlaceId start = GvGetPlayerLocation(hv->gView, hunter);
	Q q = newQueue();
	QueueJoin(q, start);
	prevPlace[start] = NOWHERE;
	Round round = GvGetRound(hv->gView);

	bool found = false;
	while (!QueueIsEmpty(q)) {
		PlaceId curr = QueueLeave(q);
		if (found) break;
		int num;
		//  Hey Ahsan, this is where i think the bug is.
		PlaceId *next = GvGetReachableByType(hv->gView,hunter,round,curr,true,true,true,&num);
		round++;
		const char *namecurr = placeIdToName(curr);
		printf("$$$$The current place is %s$$$$$$$\n", namecurr);
		for (int i = 0; i < num; i++) {
			const char *temp = placeIdToName(next[i]);
			printf("****The Place is %s******\n", temp);
			if (visited[next[i]] == 0) {
				visited[next[i]] = 1;
				QueueJoin(q,next[i]);
				dist[next[i]] = dist[curr] + 1;
				prevPlace[next[i]] = curr;
				if (next[i] == dest) {
					found = true;
					break;
				} 
			}

		}
		free(next);

	}
	
	// For example from MARSEILLES the possible connections should include 
	// Milan or Genoa
	// but it only returns itself

	// ConnList list = MapGetConnections(tempMap, EDINBURGH);
	// while (list != NULL) {
	// 	const char *name = placeIdToName(list->p);
	// 	printf("from Edinburgh0: %s\n", name);
	// 	list = list->next;
	// }

	// 	Round testround = GvGetRound(hv->gView);
	// 	int testNum;
	// 	PlaceId *test = GvGetReachableByType(hv->gView,hunter,testround,EDINBURGH,true,true,true,&testNum);
	// 	const char *myname;
	// 	for (i = 0; i < testNum; i++) {
	// 		my
	// 		printf("from Edinburgh1 %d\n", test[i]);
	// 	}
	// 	free(test);

	printf("the pathlength is %d\n", dist[dest]);

	if (dist[dest] > 0) {
		int index = dist[dest] - 2;
		
		PlaceId *path = calloc(index, sizeof(PlaceId));
		path[index] = dest;
		// printf("**********The place is %d*********\n", path[index]);
		// printf("Hello\n");
		
		index--;
		PlaceId destcpy = dest;
		
		while (index >= 0) {
			path[index] = prevPlace[destcpy];
			// printf("**********The place is %d*********\n", path[index]);
			destcpy = prevPlace[destcpy];
			index--;
		}

		free(dist); free(prevPlace); free(visited); dropQueue(q);
		

		*pathLength = dist[dest] - 1;

		// for (int i = 0; i < *pathLength; i++) {
		// 	printf("The path is %d\n", path[i]);
		// }
		
		return path;
	}
	
	free(dist); free(visited); free(prevPlace); dropQueue(q);
	// free(next);
	*pathLength = 0;
	return NULL;

}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	// *numReturnedLocs = 0;
	return HvWhereCanIGoByType(hv,true,true,true,numReturnedLocs);
	
}

PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	
	Player hunter = GvGetPlayer(hv->gView);
	PlaceId currentLocation = GvGetPlayerLocation(hv->gView, hunter); 
	if (currentLocation == NOWHERE) {
		return NULL;
	}	
	Round round = GvGetRound(hv->gView);

	PlaceId *result = GvGetReachableByType(hv->gView,hunter,round,currentLocation, road,rail,boat,numReturnedLocs);

	return result;
}

PlaceId *HvWhereCanTheyGo(HunterView hv, Player player,
                          int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	// *numReturnedLocs = 0;
	return HvWhereCanTheyGoByType(hv,player, true,true,true,numReturnedLocs);
}

PlaceId *HvWhereCanTheyGoByType(HunterView hv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	
	
	*numReturnedLocs = 0;
	PlaceId currentLocation = GvGetPlayerLocation(hv->gView, player);

	if (currentLocation == NOWHERE) {
		return NULL;
	}
	PlaceId *results;

	Round round = GvGetRound(hv->gView);
	round++;
	if (player != PLAYER_DRACULA) {
		results = GvGetReachableByType(hv->gView,player,round,currentLocation, road,rail,boat,numReturnedLocs);
	} else {
		results = GvGetReachableByType(hv->gView,player,round,currentLocation, road,false,boat,numReturnedLocs);
	}


	return results;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO

int peek() {
   return intArray[front];
}

bool isEmpty() {
   return itemCount == 0;
}

bool isFull() {
   return itemCount == MAX;
}

int size() {
   return itemCount;
}  

void insert(int data) {

   if(!isFull()) {
	
      if(rear == MAX-1) {
         rear = -1;            
      }       

      intArray[++rear] = data;
      itemCount++;
   }
}

int removeData() {
   int data = intArray[front++];
	
   if(front == MAX) {
      front = 0;
   }
	
   itemCount--;
   return data;  
}
