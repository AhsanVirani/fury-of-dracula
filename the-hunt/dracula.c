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

#include <string.h>

#include "dracula.h"
#include "DraculaView.h"
#include "Game.h"

// Registers best play for when Dracula has not made a move
void draculaStarting(DraculaView dv);
// Registers Teleport when Dracula has no valid moves
void draculaTeleport();
// AI to register best play for Dracula
void draculaAI(DraculaView dv, PlaceId currentLocation, PlaceId *validMoves);


void decideDraculaMove(DraculaView dv)
{
    int numReturnedMoves;
    PlaceId *moves = DvGetValidMoves(dv, &numReturnedMoves);
    PlaceId currentLocation = DvGetPlayerLocation(dv, PLAYER_DRACULA);

    // if Dracula has not made a move
    if (currentLocation == NOWHERE) {
        draculaStarting(dv);
    // if Dracula has no valid moves, teleport
    } else if (numReturnedMoves == 0) {
        draculaTeleport();
    // otherwise register the first valid move to meet time-constraint
    // and implement AI to select the best move
    } else {
        char *name = strdup(placeIdToAbbrev(moves[0]));
	    registerBestPlay(name, "Mwahahahaha");
        draculaAI(dv, currentLocation, moves);
    }
    free(moves);

    return;
}

// Registers best play for when Dracula has not made a move
void draculaStarting(DraculaView dv) {
    // TODO: Replace this with something better!
	registerBestPlay("CD", "Mwahahahaha");
}

// Registers Teleport when Dracula has no valid moves
void draculaTeleport() {
    registerBestPlay("TP", "Mwahahahaha");
    return;
}

// AI to register best play for Dracula
void draculaAI(DraculaView dv, PlaceId currentLocation, PlaceId *validMoves) {

    return;    
}
