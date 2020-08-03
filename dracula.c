////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "dracula.h"
#include "DraculaView.h"
#include "Game.h"

#define MAX_LOCATION 100
#define DRAC_REACH 10

PlaceId huntersLoc[MAX_LOCATION];
int size;

// Static Function Declarations
static void huntersReach(DraculaView);
static int InArray(DraculaView, Player);
static void updateHuntersReach(DraculaView, Player);
static int DraculaBestMove(PlaceId *validMoves, PlaceId draculaReac[], int numReturnedMoves);



void decideDraculaMove(DraculaView dv)
{
	PlaceId draculaReach[DRAC_REACH];

	// initialise the huntersLoc array with NOWEHRE
	int i;	
	for(i = 0; i < MAX_LOCATION; i++)
		huntersLoc[i] = NOWHERE;
	size = 0;

	huntersReach(dv);

	// Takes Valid Moves of Dracula and returns array that excludes locations of huntersReach
	int numReturnedMoves = -1;
	PlaceId *dracValidMoves = DvGetValidMoves(dv, &numReturnedMoves);

	// Means teleported
	if(numReturnedMoves == 0) {
		registerBestPlay("CD", "Mwahahahaha");
		free(dracValidMoves);
		return;
	}
	// gives the first element in best moves array
	int nMoves = DraculaBestMove(dracValidMoves, draculaReach, numReturnedMoves);
	char *abbre;
	if(nMoves > 0) {
		abbre = strdup(placeIdToAbbrev(draculaReach[0]));
		registerBestPlay(abbre, "Mwahahahaha");
		free(dracValidMoves);
		free(abbre);		
		return;
	}		
	abbre = strdup(placeIdToAbbrev(dracValidMoves[0]));
	registerBestPlay(abbre, "Mwahahahaha");
	free(abbre);
	free(dracValidMoves);
}

// Fills the array with current location of hunters and places reachable
static
void huntersReach(DraculaView dv)
{
	assert(dv != NULL);

	// insert hunters current location in the array
	huntersLoc[0] = DvGetPlayerLocation(dv, 0);
	if(!InArray(dv, 1)) {
		huntersLoc[1] = DvGetPlayerLocation(dv, 1);
		size++;
	}
	if(!InArray(dv, 2)) {
		huntersLoc[2] = DvGetPlayerLocation(dv, 2);
		size++;
	}
	if(!InArray(dv, 3)) {
		huntersLoc[3] = DvGetPlayerLocation(dv, 3);
		size++;
	}
	// Insert places reachable by the hunters in the array
	updateHuntersReach(dv, 0);
	updateHuntersReach(dv, 1);
	updateHuntersReach(dv, 2);
	updateHuntersReach(dv, 3);
}

static
int InArray(DraculaView dv, Player player)
{
	assert(dv != NULL);
		
	if(player == 1) {
		if(huntersLoc[0] != DvGetPlayerLocation(dv, 1))
			return 0;
		return 1;
	}	

	else if(player == 2) {
		if(huntersLoc[0] != DvGetPlayerLocation(dv, 2) && 
			huntersLoc[1] != DvGetPlayerLocation(dv, 2))
			return 0;
		return 1;
	}
	
	if(huntersLoc[0] != DvGetPlayerLocation(dv, 3) && 
		huntersLoc[1] != DvGetPlayerLocation(dv, 3) && 
		huntersLoc[2] != DvGetPlayerLocation(dv, 3))
		return 0;
	return 1;
}

static
void updateHuntersReach(DraculaView dv, Player player)
{
	int numReturnedLocs = 0;
	PlaceId *reachable = DvWhereCanTheyGo(dv, player, &numReturnedLocs);
	
	int i, j;
	for(i = 0; i < numReturnedLocs; i++) {
		for(j = 0; j < size; j++) {
			if(huntersLoc[j] == reachable[i])
				break;
		}
		if(size == j) {
			huntersLoc[size] = reachable[i];
			size++;
		}
	}
	free(reachable);
}

// Takes valid moves for the Dracula and excludes those in Hunters Reach
// If no such move i.e. Dracula trapped or no valid moves then returns 0
static
int DraculaBestMove(PlaceId *validMoves, PlaceId draculaReach[], int numReturnedMoves)
{
	int len = 0;
	int i, j;
	for(i = 0; i < numReturnedMoves; i++) {
		for(j = 0; j < size; j++) {
			if(huntersLoc[j] == validMoves[i])
				break;
		}
		if(size == j) {
			draculaReach[len] = validMoves[i];
			len++;
		}		
	}
		
	return len;
}

