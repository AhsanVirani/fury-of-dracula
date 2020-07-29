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
	return HvWhereCanIGoByType(hv, true, true, true, numReturnedLocs);
	
}

PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	Player currPlayer = GvGetPlayer(hv->gView);
	PlaceId currLoc = GvGetPlayerLocation(hv->gView,currPlayer);
	Round round = GvGetRound(hv->gView);
	
	if (currLoc == NOWHERE) {
		*numReturnedLocs = 0;
		return NULL;
	}
	PlaceId *results = GvGetReachableByType(hv->gView,currPlayer, round, 
											currLoc, road, rail, boat,
											numReturnedLocs);

	// *numReturnedLocs = 0;
	return results;
}

PlaceId *HvWhereCanTheyGo(HunterView hv, Player player,
                          int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	// *numReturnedLocs = 0;
	return HvWhereCanIGoByType(hv, true, true, true, numReturnedLocs);
}

PlaceId *HvWhereCanTheyGoByType(HunterView hv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION

	PlaceId currLoc = GvGetPlayerLocation(hv->gView, player);
	if (currLoc == NOWHERE) {
		*numReturnedLocs = 0;
		return NULL;
	}
	Round round = GvGetRound(hv->gView);
	PlaceId *results = GvGetReachableByType(hv->gView,player, round, 
											currLoc, road, rail, boat,
											numReturnedLocs);


	// *numReturnedLocs = 0;
	return results;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions
// PlaceId *findShortestPath(HunterView hv, Player hunter, Round round, PlaceId start, PlaceId dest) {
// 	PlaceId *visited;
// 	visited[0] = start;
// 	int i = 0;
// 	PlaceId *p;

// }



// TODO
