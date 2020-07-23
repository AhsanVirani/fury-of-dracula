////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// DraculaView.c: the DraculaView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "DraculaView.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h"
// add your own #includes here

// TODO: ADD YOUR OWN STRUCTS HERE

struct draculaView {
	GameView gv;
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

DraculaView DvNew(char *pastPlays, Message messages[])
{
	DraculaView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate DraculaView\n");
		exit(EXIT_FAILURE);
	}
    new->gv = GvNew(pastPlays, messages);

	return new;
}

void DvFree(DraculaView dv)
{
	GvFree(dv->gv);
	free(dv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round DvGetRound(DraculaView dv)
{
	
	return GvGetRound(dv->gv);
}

int DvGetScore(DraculaView dv)
{
	return GvGetScore(dv->gv);
}

int DvGetHealth(DraculaView dv, Player player)
{
	return GvGetHealth(dv->gv, player);
}

PlaceId DvGetPlayerLocation(DraculaView dv, Player player)
{
	return GvGetPlayerLocation(dv->gv, player);
}

PlaceId DvGetVampireLocation(DraculaView dv)
{
	return GvGetVampireLocation(dv->gv);
}

PlaceId *DvGetTrapLocations(DraculaView dv, int *numTraps)
{
	return GvGetTrapLocations(dv->gv, numTraps);
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *DvGetValidMoves(DraculaView dv, int *numReturnedMoves)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	return NULL;
}

PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
    // Get current location
    PlaceId currentLocation = GvGetPlayerLocation(dv->gv, PLAYER_DRACULA);
    
    // Check that the player has made a move, else return 0
	if (currentLocation == NOWHERE) {
        *numReturnedLocs = 0;
        return 0;
    }

    // List of all locations reachable by Dracula (ignoring trail)
	PlaceId *reachable = GvGetReachable(dv->gv, PLAYER_DRACULA, GvGetRound(dv->gv), 
                        currentLocation, numReturnedLocs);

    // List of last 5 moves (trail of dracula)
    int *numReturnedMoves = malloc(sizeof(int));
    assert(numReturnedMoves != NULL);
    PlaceId *trail = GvGetLastMoves(dv->gv, PLAYER_DRACULA, 5, numReturnedMoves, false);

    // Check for hide and double-backs
    int hide = false;
    int doubleBack = false;
    for (int i = 0; i < numReturnedMoves; i++) {
        if (trail[i] == HIDE) {
            hide = true;
        } else if (DOUBLE_BACK_1 <= trail[i] && trail[i] <= DOUBLE_BACK_5) {
            doubleBack = true;
        }
    }

    // Remove current if illegal
        // if already used hide or at sea
    placeType locationType = placeIdToType(GvGetPlayerLocation(dv->gv, PLAYER_DRACULA))
    if (hide == true || locationType == SEA) {
        // Remove current location from reachable
        for (int i = 0; i < numReturnedLocs; i++) {
            if (reachable[i] == currentLocation) {
                // REMOVING...

                // FREEING... last element
                realloc();
                break;
            }
        }
    }

    // Remove double-backs if illegal
    if (doubleBack == true) {
        for (int trailingMoves = 0; i < numReturnedMoves; i++) {
            for (int i = 0; i < numReturnedLocs)
        }
    }
    }

    // Free
    free(numReturnedMoves);

    return reachable;

}

PlaceId *DvWhereCanIGoByType(DraculaView dv, bool road, bool boat,
                             int *numReturnedLocs)
{
	if (GvGetPlayerLocation(dv->gv, PLAYER_DRACULA) == NOWHERE) {
        *numReturnedLocs = 0;
        return 0;
    }
	return GvGetReachableByType(dv->gv, PLAYER_DRACULA, GvGetRound(dv->gv),
                                GvGetPlayerLocation(dv->gv, PLAYER_DRACULA), 
                                road, false, boat, numReturnedLocs);
}

PlaceId *DvWhereCanTheyGo(DraculaView dv, Player player,
                          int *numReturnedLocs)
{
    if (GvGetPlayerLocation(dv->gv, player) == NOWHERE) {
        *numReturnedLocs = 0;
        return 0;
    }
	return GvGetReachable(dv->gv, player, GvGetRound(dv->gv), 
                        GvGetPlayerLocation(dv->gv, player), 
                        numReturnedLocs);
}

PlaceId *DvWhereCanTheyGoByType(DraculaView dv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	if (GvGetPlayerLocation(dv->gv, player) == NOWHERE) {
        *numReturnedLocs = 0;
        return 0;
    }
	return GvGetReachableByType(dv->gv, player, GvGetRound(dv->gv),
                                GvGetPlayerLocation(dv->gv, player), 
                                road, rail, boat, numReturnedLocs);
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
