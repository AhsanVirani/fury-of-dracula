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
    GameView gV = hv->gView;
    Round turn = GvGetRound(gV);              
    PlaceId current = GvGetPlayerLocation(gV, hunter);
    //printf("%d\n", turn);
    //printf("%d\n", current);
    
    int numReturn;
        
    PlaceId *movePoss = GvGetReachable(gV, hunter, turn, current, &numReturn);  // this function only outputs madrid and itself as reachable places from lisbon and not 3 others (two neighbouring cities and the sea)
                       
    //printf("%d\n", movePoss[0]);                
    printf("%d\n", numReturn);
    
    int count = 0;
    while(count < numReturn){
        printf("%d\n", movePoss[count]);
        count++;
    }  
    
    // implementing Dijkstra's algorithm 
    int distance[MAX];
    count = 0;
    while(count < MAX){
        distance[count] = 10000;
        count++;
    }
    distance[current] = 0;
    printf("%d\n", distance[current]);
    
    count = 0;
    int prev[MAX];
    while(count < MAX){
        prev[count] = -1;
        count++;
    }
    printf("%d\n", prev[current]);
    
    /*insert(3);            //testing if the queue works
    insert(5);
    
    int num = removeData();
    printf("%d\n", num);
    
    num = removeData();
    printf("%d\n", num);*/
    
    insert(current);
    while(!isEmpty()) {    
        int n = removeData();           
        printf("%d\n",n);
        count = 0;
        PlaceId *moveNeighbours = GvGetReachable(gV, hunter, turn, n, &numReturn);
        while(count < numReturn){
            //distance[n] 
            PlaceId neighbour = moveNeighbours[count];
            printf("%d\n", neighbour);
            if (distance[n] + 1 < distance[neighbour]){            
                distance[neighbour] =  distance[n] + 1;
                prev[neighbour] = n;
                insert(neighbour);
            }            
            count++;
        }
    }
    // some pseudocode for the remaining part
    // the dist array, and the prev array all filled out
    // use the prev array to find the path from source to anywhere.  
    // Distance from source to destination = dist[dest]
	
	
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION

	// PlaceId start = GvGetPlayerLocation(hv->gView, hunter);
	// int num;
	// Round round = GvGetRound(hv->gView);
	// PlaceId *path;
	// int count = 0;
	// PlaceId *result;

	// bool found = false;
	// while (found == false) {
	// 	if (count == 0)
	// 		result = GvGetReachable(hv->gView,hunter,round,start, &num);
	// 	else
	// 		result = GvGetReachable(hv->gView,hunter,round,path[count], &num);
		
	// 	for (int i = 1; i < num; i++) {
	// 		if (result[i] == dest) {
	// 			path[count] = result[i];
	// 			found = true;
	// 		} 
	// 	}

	// 	if (found == false) {
	// 		for (int i = 1; i < num; i++) {
	// 			path[count] = result[i];
	// 			count++;
	// 		} 

	// 	}
	// }

	// PlaceId *result = findShortestPath(hv, hunter, round, start, dest);
	// PlaceId *result = GvGetReachable(hv->gView,hunter,round,start, &num);
	// for (int i = 0; i < num; i++) {
	// 	printf("The place is  %d\n", result[i]);
	// 	//;
	// }   
	// printf("***************\n");
	// round = GvGetRound(hv->gView);
	// round++;
	// PlaceId *result0 = GvGetReachable(hv->gView,hunter,round,result[1], &num);
	
	// for (int i = 0; i < num; i++) {
	// 	printf("The place is  %d\n", result0[i]);
	// 	//;
	// } 
	*pathLength = -1;
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
