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
static int huntersReach(DraculaView);
static int InArray(DraculaView, Player);
static void updateHuntersReach(DraculaView, Player);
static int DraculaAI(PlaceId *validMoves, PlaceId draculaReac[], int numReturnedMoves);
static void draculaStarting();
static void draculaTeleport();
static PlaceId DraculaStayAway(PlaceId *, int, int);
static void DraculaBestMove(PlaceId *, int);


void decideDraculaMove(DraculaView dv)
{
	// array will hold locations from away from hunters reach
	PlaceId draculaReach[DRAC_REACH];

	// initialise the huntersLoc array with NOWEHRE
	int i;	
	for(i = 0; i < MAX_LOCATION; i++)
		huntersLoc[i] = NOWHERE;
	size = 0;

	// initialise the draculaReach array with NOWHERE
	for(i = 0; i < DRAC_REACH; i++)
		draculaReach[i] = NOWHERE;

	// Fills the huntersLoc array with current and reachable locations of all hunters
	// Returns the len for current Locations of Hunters
	int currentLocation = huntersReach(dv);

	// If Round 0 Play anything for testing
	if(DvGetRound(dv) == 0) {
        draculaStarting(dv);
		return;
	}


	// Takes Valid Moves of Dracula and returns array that excludes locations of huntersReach
	int numReturnedLocs = -1;
	// Going to Sea costs dracula 2 pts, so avoid if can
	PlaceId *Locs = DvWhereCanIGo(dv, &numReturnedLocs);

	// Means teleported
	if(numReturnedLocs == 0) {
		draculaTeleport();
		free(Locs);
		return;
	}
	// gives the first element in best moves array
	int nLocs = DraculaAI(Locs, draculaReach, numReturnedLocs);
	// Go over the array and instead of chosing first element, choose loc which is not sea or CASTLE_DRACULA
	if(nLocs > 0) {
		DraculaBestMove(draculaReach, nLocs);
		free(Locs);	
		return;
	}
	// If no place in draculaReach then choose location not a current location.
	PlaceId Far = DraculaStayAway(Locs, numReturnedLocs, currentLocation);	
	registerBestPlay(placeIdToAbbrev(Far), "Mwahahahaha");
	free(Locs);

}

// Fills the array with current location of hunters and places reachable
static
int huntersReach(DraculaView dv)
{
	assert(dv != NULL);
	int len = 0;
	// insert hunters current location in the array
	huntersLoc[0] = DvGetPlayerLocation(dv, 0);
	size++;

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
	len = size;
	// Insert places reachable by the hunters in the array
	updateHuntersReach(dv, 0);
	updateHuntersReach(dv, 1);
	updateHuntersReach(dv, 2);
	updateHuntersReach(dv, 3);
	
	return len;
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
int DraculaAI(PlaceId *validMoves, PlaceId draculaReach[], int numReturnedMoves)
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

// Registers best play for when Dracula has not made a move
// Make sure to start where the Hunters not
static
void draculaStarting() 
{	
	// Start anywhere not sea or in Hunters Reach
	PlaceId start, secBest, Best = NOWHERE;
	int j;
	for(start = MIN_REAL_PLACE; start <= MAX_REAL_PLACE; start++) {
		for(j = 0; j < size; j++) {
			if(start == huntersLoc[j])
				break;
		}	
		// Means that start atleast not in Hunters Reach
		if(size == j) {
			secBest = start;
			// Check if not sea
			if(!placeIsSea(start))
				Best = secBest;
			if(Best == CASTLE_DRACULA)
				break;			
		}
	}
	if(Best != NOWHERE) {
		registerBestPlay(placeIdToAbbrev(Best), "Mwahahahaha");
		return;
	}
	registerBestPlay(placeIdToAbbrev(secBest), "Mwahahahaha");
}

// Registers Teleport when Dracula has no valid moves
static
void draculaTeleport() {
    registerBestPlay("TP", "Mwahahahaha");
    return;
}

// This function is used if Dracula doesnot have any moves outside huntersLoc
// Finds atleast a location not a current location of Hunter
// Else gives any in case if all leads to Hunters current location
static
PlaceId DraculaStayAway(PlaceId *Locs, int numReturnedLocs, int len)
{
	PlaceId secBest, Best = NOWHERE;
	int i, j;
	for(i = 0; i < numReturnedLocs; i++) {
		for(j = 0; j < len; j++) {
			if(Locs[i] == huntersLoc[j])
				break;
		}
		// means location not a current location of hunter
		if(j == len) {
			secBest = Locs[i];
			if(!placeIsSea(secBest))
				Best = Locs[i];
			if(Best == CASTLE_DRACULA)
				return Best;
		}
	}
	// Here. either Best or secBest or Nowhere
	if(Best != NOWHERE)
		return Best;
	// All locations current location so just return any
	return secBest == NOWHERE? Locs[0]: secBest;
}

static
void DraculaBestMove(PlaceId draculaReach[], int len)
{
	PlaceId secBest = NOWHERE;
	int i;
	for(i = 0; i < len; i++) {
		if(!placeIsSea(draculaReach[i]))
			secBest = draculaReach[i];
		if(secBest == CASTLE_DRACULA)
			break;
	}
	if(secBest != NOWHERE) {
		registerBestPlay(placeIdToAbbrev(secBest), "Mwahahahaha");
		return;
	}
	// All sea thats away from Hunters reach then no choice, take it
	registerBestPlay(placeIdToAbbrev(draculaReach[0]), "Mwahahahaha");
}


