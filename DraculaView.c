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

// Given an array of locations, removes specifies location and decrements n
// If location does not exist, do nothing
static PlaceId *removeLocationFromArray(PlaceId *places, PlaceId location, int *n);

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
    // List of locations Dracula can move to
	PlaceId *moves = DvWhereCanIGo(dv, numReturnedMoves);

    // If teleport is only option or Dracula has not moved, return NULL
    if (moves == NULL) {
        return NULL;
    }

    // Get current location
    PlaceId currentLocation = GvGetPlayerLocation(dv->gv, PLAYER_DRACULA);

    // List of last 5 moves (trail of dracula)
    int numTrailMoves;
    bool trailBool;
    PlaceId *trail = GvGetLastMoves(dv->gv, PLAYER_DRACULA, 5, &numTrailMoves, &trailBool);

    // Check for hide and double-backs on trail
    int hide = false;
    int doubleBack = false;
    for (int i = 0; i < numTrailMoves; i++) {
        if (trail[i] == HIDE) {
            hide = true;
        } else if (DOUBLE_BACK_1 <= trail[i] && trail[i] <= DOUBLE_BACK_5) {
            doubleBack = true;
        }
    }
    
    // If Dracula has not used HIDE, replace currentLocation with HIDE
    if (hide == false) {
        for (int i = 0; i < *numReturnedMoves; i++) {
            if (currentLocation == moves[i]) {
                moves[i] = HIDE;
                break;
            }
        }
    }

    // If Dracula has not used DB, replace moves on trail with corresponding double-back
    if (doubleBack == false) {
        // Find locations on trail
        int tempNum;
        bool tempBool;
        PlaceId *lastLocations = GvGetLastLocations(dv->gv, PLAYER_DRACULA, 5, &tempNum, &tempBool);

        for (int nTrail = 0; nTrail < tempNum; nTrail++) {
            for (int i = 0; i < *numReturnedMoves; i++) {
                if (lastLocations[nTrail] == moves[i]) {
                    moves[i] = DOUBLE_BACK_1 + nTrail;
                    break;
                }
            }
        }
        
        // Free lastLocations
        if (tempBool) {
            free(lastLocations);
        }
    }

    // If Dracula can HIDE or DB, then add DB1 (missing)
    if (doubleBack == false && hide == false) {
        *numReturnedMoves = *numReturnedMoves + 1;
        moves = realloc(moves, *numReturnedMoves * sizeof(int));
        moves[*numReturnedMoves - 1] = DOUBLE_BACK_1;
    }


    return moves;

}

PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
    return DvWhereCanIGoByType(dv, true, true, numReturnedLocs);
}

PlaceId *DvWhereCanIGoByType(DraculaView dv, bool road, bool boat,
                             int *numReturnedLocs)
{
	// Get current location
    PlaceId currentLocation = DvGetPlayerLocation(dv, PLAYER_DRACULA);

    // Check that the player has made a move, else return 0
	if (currentLocation == NOWHERE) {
        *numReturnedLocs = 0;
        return NULL;
    }

    // List of all locations reachable by Dracula filtering type (ignores trail)
	PlaceId *reachable = GvGetReachableByType(dv->gv, PLAYER_DRACULA, GvGetRound(dv->gv),
                                            GvGetPlayerLocation(dv->gv, PLAYER_DRACULA), 
                                            road, false, boat, numReturnedLocs);

    // List of last 5 moves (trail of dracula)
    int numTrailMoves;
    bool trailBool;
    PlaceId *trail = GvGetLastMoves(dv->gv, PLAYER_DRACULA, 5, &numTrailMoves, &trailBool);

    // Check for hide and double-backs on trail
    int hide = false;
    int doubleBack = false;
    for (int i = 0; i < numTrailMoves; i++) {
        if (trail[i] == HIDE) {
            hide = true;
        } else if (DOUBLE_BACK_1 <= trail[i] && trail[i] <= DOUBLE_BACK_5) {
            doubleBack = true;
        }
    }

    // Remove currentLocation if illegal (already used hide or at sea + already used DB)
    PlaceType locationType = placeIdToType(GvGetPlayerLocation(dv->gv, PLAYER_DRACULA));
    if (hide == true || locationType == SEA) {
        if (doubleBack == true) {
            // Remove current location from reachable
            reachable = removeLocationFromArray(reachable, currentLocation, numReturnedLocs);
        }
    }

    // Remove double-back locations (trail locations) if illegal (already used double-back)
    if (doubleBack == true) {
        // Find locations on trail
        int tempNum;
        bool tempBool;
        PlaceId *lastLocations = GvGetLastLocations(dv->gv, PLAYER_DRACULA, numTrailMoves, &tempNum, &tempBool);
        
        for (int i = 0; i < tempNum; i++) {
            if (lastLocations[i] == currentLocation && hide == false) {
                continue;
            }
            reachable = removeLocationFromArray(reachable, lastLocations[i], numReturnedLocs);
        }

        if (tempBool) {
            free(lastLocations);
        }
    }
    // Free

    // If teleport is only move, return NULL
    if (*numReturnedLocs == 0) {
        return NULL;
    }

    return reachable;
}

PlaceId *DvWhereCanTheyGo(DraculaView dv, Player player,
                          int *numReturnedLocs)
{
    return DvWhereCanTheyGoByType(dv, player, true, 
                                true, true, numReturnedLocs);
}

PlaceId *DvWhereCanTheyGoByType(DraculaView dv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	// Get current location
    PlaceId currentLocation = GvGetPlayerLocation(dv->gv, PLAYER_DRACULA);

    // Check that the player has made a move, else return 0
	if (currentLocation == NOWHERE) {
        *numReturnedLocs = 0;
        return 0;
    }

    // If player is Dracula then return DvWhereCanIGoByType
    if (player == PLAYER_DRACULA) {
        return DvWhereCanIGoByType(dv, road, boat, numReturnedLocs);

    // If player is hunter then return GvGetReachableByType
    } else {
        return GvGetReachableByType(dv->gv, player, GvGetRound(dv->gv),
                                GvGetPlayerLocation(dv->gv, player), 
                                road, rail, boat, numReturnedLocs);
    }
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO

// Given an array of locations, removes specifies location and decrements n
// If location does not exist, do nothing
static PlaceId *removeLocationFromArray(PlaceId *places, PlaceId location, int *n) {
    for (int i = 0; i < *n; i++) {
        if (places[i] == location) {
            // Remove by replacing with last element and deleting last element
            places[i] = places[*n - 1];
            *n = *n - 1;
            // Deleting... last element (using realloc)
            places = realloc(places, *n * sizeof(int));
            break;
        }
    }
    return places;
}
