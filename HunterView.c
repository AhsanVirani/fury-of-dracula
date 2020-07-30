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
