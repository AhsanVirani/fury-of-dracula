////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// HunterView.c: the HunterView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10   v3.0    Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-31	v4.0	Team Ahsan-Aryaman-Michael-Vishnu 
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


// add your own #includes here

// TODO: ADD YOUR OWN STRUCTS HERE

struct hunterView {

	GameView gView;
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

HunterView HvNew(char *pastPlays, Message messages[])
{
	
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

	GvFree(hv->gView);
	free(hv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round HvGetRound(HunterView hv)
{

	return GvGetRound(hv->gView);
}

Player HvGetPlayer(HunterView hv)
{

	return GvGetPlayer(hv->gView);
	
}

int HvGetScore(HunterView hv)
{

	return GvGetScore(hv->gView);
}

int HvGetHealth(HunterView hv, Player player)
{

	return GvGetHealth(hv->gView, player);
}

PlaceId HvGetPlayerLocation(HunterView hv, Player player)
{

	return GvGetPlayerLocation(hv->gView, player);
	
}

PlaceId HvGetVampireLocation(HunterView hv)
{

	return GvGetVampireLocation(hv->gView);
}

////////////////////////////////////////////////////////////////////////
// Utility Functions

PlaceId HvGetLastKnownDraculaLocation(HunterView hv, Round *round)
{
	*round = 0;
	int result;
	int count = -1;
	bool locBool;

	PlaceId *placeResult = GvGetLocationHistory(hv->gView,PLAYER_DRACULA,
												&result,&locBool);

	PlaceId finalPlace = NOWHERE;

	for (int i = 0; i < result; i++) {
		if ((placeResult[i] != CITY_UNKNOWN) && 
			(placeResult[i] != SEA_UNKNOWN)) {
			finalPlace = placeResult[i];
			count++;
		}	
	}

		if (count > 0)
			*round = count;

		if (locBool) {
			free(placeResult);
		}
	
	return finalPlace;
}

// This function implements BFS to get the shortest path
// from a starting point to the destination
// This function implements a Queue ADT 
PlaceId *HvGetShortestPathTo(HunterView hv, Player hunter, PlaceId dest,
                             int *pathLength)
{
	Map tempMap = MapNew();
    int connNum = MapNumPlaces(tempMap);
    int *dist = calloc (connNum, sizeof(int));

	int *visited = calloc (connNum, sizeof(int));
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

	visited[start] = 1;
	bool found = false;


    while (!QueueIsEmpty(q)) {
		
        PlaceId curr = QueueLeave(q);
		int num;
		PlaceId *next = NULL;

		if (curr == start) {
			if (found) break;
			next = GvGetReachable(hv->gView,hunter,round,curr,&num);
		} else {
			if (found) break;

			int currRound = round + dist[curr];
        	next = GvGetReachable(hv->gView,hunter,currRound,curr,&num);
		}
        
        // const char *namecurr = placeIdToName(curr);
        // printf("$$$$The current place is %s$$$$$$$\n", namecurr);

        for (int i = 0; i < num; i++) {
			if (next[i] == curr) i++;
            if (visited[next[i]] == 0) {
                visited[next[i]] = 1;
				// const char *temp = placeIdToName(next[i]);
				// printf("****The Place is %s******\n", temp);
                QueueJoin(q,next[i]);
                dist[next[i]] = dist[curr] + 1;
                prevPlace[next[i]] = curr;
                if (next[i] == dest) {
                    found = true;
					
                    break;
					
                } 
            }
			else {;}
        }
        free(next);
    }

	// creates the dynamic array which holds the shortest path
	if (dist[dest] > 0) {
        int index = dist[dest] - 1;
        
        PlaceId *path = calloc(index, sizeof(PlaceId));
        path[index] = dest;
        // const char *temp = placeIdToName(path[index]);
		// 		printf("****The Place is %s******\n", temp);
        
        index--;
        PlaceId destcpy = dest;
        
        while (index >= 0) {
            path[index] = prevPlace[destcpy];
			// temp = placeIdToName(path[index]);
			// 	printf("****The Place is %s******\n", temp);
            destcpy = prevPlace[destcpy];
            index--;
        }
		

        free(dist); free(prevPlace); free(visited); dropQueue(q);
        *pathLength = dist[dest];
        return path;
    }
    
    free(dist); free(visited); free(prevPlace); dropQueue(q);
    *pathLength = 0;
    return NULL;

}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
{
	return HvWhereCanIGoByType(hv,true,true,true,numReturnedLocs);	
}

PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	
	Player hunter = GvGetPlayer(hv->gView);
	PlaceId currentLocation = GvGetPlayerLocation(hv->gView, hunter); 
	if (currentLocation == NOWHERE) {
		return NULL;
	}	
	Round round = GvGetRound(hv->gView);

	PlaceId *result = GvGetReachableByType(hv->gView,hunter,round,
										   currentLocation,road,rail,
										   boat,numReturnedLocs);

	return result;
}

PlaceId *HvWhereCanTheyGo(HunterView hv, Player player,
                          int *numReturnedLocs)
{

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
		results = GvGetReachableByType(hv->gView,player,round,currentLocation, 
									   road,rail,boat,numReturnedLocs);
	} else {
		results = GvGetReachableByType(hv->gView,player,round,currentLocation, 
									   road,false,boat,numReturnedLocs);
	}
	return results;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions


