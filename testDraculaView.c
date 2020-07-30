////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// testDraculaView.c: test the DraculaView ADT
//
// As supplied, these are very simple tests.  You should write more!
// Don't forget to be rigorous and thorough while writing tests.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-02	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DraculaView.h"
#include "GameView.h"
#include "Game.h"
#include "Places.h"
#include "testUtils.h"

#define green() printf("\033[1;32m")
#define purple() printf("\033[0;35m")
#define resetColour() printf("\033[0m")

void passed();

int main(void)
{
	{///////////////////////////////////////////////////////////////////
	
		printf("Test for basic functions, "
			   "just before Dracula's first move\n");

		char *trail =
			"GST.... SAO.... HZU.... MBB....";
		
		Message messages[] = {
			"Hello", "Goodbye", "Stuff", "..."
		};
		
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 0);
		assert(DvGetScore(dv) == GAME_START_SCORE);
		assert(DvGetHealth(dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert(DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(DvGetPlayerLocation(dv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert(DvGetPlayerLocation(dv, PLAYER_VAN_HELSING) == ZURICH);
		assert(DvGetPlayerLocation(dv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(DvGetPlayerLocation(dv, PLAYER_DRACULA) == NOWHERE);
		assert(DvGetVampireLocation(dv) == NOWHERE);
		int numTraps = -1;
		PlaceId *traps = DvGetTrapLocations(dv, &numTraps);
		assert(numTraps == 0);
		free(traps);

		printf("Test passed!\n");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for encountering Dracula\n");

		char *trail =
			"GST.... SAO.... HCD.... MAO.... DGE.V.. "
			"GGEVD.. SAO.... HCD.... MAO....";
		
		Message messages[] = {
			"Hello", "Goodbye", "Stuff", "...", "Mwahahah",
			"Aha!", "", "", ""
		};
		
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 1);
		assert(DvGetScore(dv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN);
		assert(DvGetHealth(dv, PLAYER_LORD_GODALMING) == 5);
		assert(DvGetHealth(dv, PLAYER_DRACULA) == 30);
		assert(DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING) == GENEVA);
		assert(DvGetPlayerLocation(dv, PLAYER_DRACULA) == GENEVA);
		assert(DvGetVampireLocation(dv) == NOWHERE);

		printf("Test passed!\n");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for Dracula leaving minions 1\n");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DED.V.. "
			"GST.... SST.... HST.... MST.... DMNT... "
			"GST.... SST.... HST.... MST.... DLOT... "
			"GST.... SST.... HST.... MST.... DHIT... "
			"GST.... SST.... HST.... MST....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 4);
		assert(DvGetVampireLocation(dv) == EDINBURGH);
		int numTraps = -1;
		PlaceId *traps = DvGetTrapLocations(dv, &numTraps);
		assert(numTraps == 3);
		sortPlaces(traps, numTraps);
		assert(traps[0] == LONDON);
		assert(traps[1] == LONDON);
		assert(traps[2] == MANCHESTER);
		free(traps);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for Dracula's valid moves 1\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DCD.V.. "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[9] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
		assert(numMoves == 4);
		sortPlaces(moves, numMoves);
		assert(moves[0] == GALATZ);
		assert(moves[1] == KLAUSENBURG);
		assert(moves[2] == HIDE);
		assert(moves[3] == DOUBLE_BACK_1);
		free(moves);
		
		printf("Test passed!\n");
		DvFree(dv);
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("Test for DvWhereCanIGo 1\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DKL.V.. "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "
			"GGE.... SGE.... HGE.... MGE.... DBCT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGo(dv, &numLocs);
		assert(numLocs == 4);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BELGRADE);
		assert(locs[1] == CONSTANTA);
		assert(locs[2] == GALATZ);
		assert(locs[3] == SOFIA);
		free(locs);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

    printf("\n");
    printf("\n");
    printf("////////////////////////////////////////////////////////////////////\n");
    printf("//////////////////////////    MY TESTS    //////////////////////////\n");
    printf("////////////////////////////////////////////////////////////////////\n");
    printf("\n");

    printf("NOTES:");
    printf(" --- All game state functions are identical to their GameView.c counterparts\n");
    printf(" --- DvGetValidMoves bootstraps and modifies DvWhereCanIGo\n");
    printf(" --- DvWhereCanIGo is identical to DvWhereCanIGoByType for Dracula\n");
    printf(" --- DvWhereCanTheyGo and DvWhereCanTheyGoByType for Dracula are identical\n");
    printf("     to their Dracula specific counterparts above\n");
    printf(" --- DvWhereCanTheyGo and DvWhereCanTheyGoByType for Hunters are identical\n");
    printf("     to GvGetReachable and GvGetReachableByType in GameView.c\n");
    printf(" --- Unlike the functions in GameView.c, these will take into account Dracula's trail\n");
    printf("\n");
    printf("With these assumptions in mind, we are testing for DvGetValidMoves and DvWhereCanIGo\n");
    printf("\n");

    {///////////////////////////////////////////////////////////////////
    purple();
    printf("Test for when HIDE and DB are valid\n");
    resetColour();
    char *trail =
        "GGE.... SGE.... HGE.... MGE.... DCD.V.. "
        "GGE.... SGE.... HGE.... MGE....";
    
    Message messages[9] = {};
    DraculaView dv = DvNew(trail, messages);
    int num = -1;

    // DvGetValidMoves
    printf("... testing DvGetValidMoves\n");
    PlaceId *moves = DvGetValidMoves(dv, &num);
    assert(num == 4);
    sortPlaces(moves, num);
    assert(moves[0] == GALATZ);
    assert(moves[1] == KLAUSENBURG);
    assert(moves[2] == HIDE);
    assert(moves[3] == DOUBLE_BACK_1);
    free(moves);

    // DvWhereCanIGo
    printf("... testing DvWhereCanIGo\n");
    PlaceId *locations = DvWhereCanIGo(dv, &num);
    assert(num == 3);
    sortPlaces(locations, num);
    assert(locations[0] == CASTLE_DRACULA);
    assert(locations[1] == GALATZ);
    assert(locations[2] == KLAUSENBURG);
    free(locations);

    DvFree(dv);
    passed();
    }

    {///////////////////////////////////////////////////////////////////
    purple();
    printf("Test for when DB is valid but HIDE is invalid (already used)\n");
    printf(" --- Also checking that DB can refer to a HIDE\n");
    resetColour();
    char *trail =
        "GGE.... SGE.... HGE.... MGE.... DCD.V.. "
        "GGE.... SGE.... HGE.... MGE.... DHIT... "
        "GGE.... SGE.... HGE.... MGE....";
    
    Message messages[14] = {};
    DraculaView dv = DvNew(trail, messages);
    int num = -1;

    // DvGetValidMoves
    printf("... testing DvGetValidMoves\n");
    PlaceId *moves = DvGetValidMoves(dv, &num);
    assert(num == 4);
    sortPlaces(moves, num);
    assert(moves[0] == GALATZ);
    assert(moves[1] == KLAUSENBURG);
    assert(moves[2] == DOUBLE_BACK_1);
    assert(moves[3] == DOUBLE_BACK_2);
    free(moves);

    // DvWhereCanIGo
    printf("... testing DvWhereCanIGo\n");
    PlaceId *locations = DvWhereCanIGo(dv, &num);
    assert(num == 3);
    sortPlaces(locations, num);
    assert(locations[0] == CASTLE_DRACULA);
    assert(locations[1] == GALATZ);
    assert(locations[2] == KLAUSENBURG);
    free(locations);

    DvFree(dv);
    passed();
    }

    {///////////////////////////////////////////////////////////////////
    purple();
    printf("Test for when HIDE is valid but DB is invalid (already used)\n");
    resetColour();
    char *trail =
        "GGE.... SGE.... HGE.... MGE.... DCD.V.. "
        "GGE.... SGE.... HGE.... MGE.... DD1T... "
        "GGE.... SGE.... HGE.... MGE....";
    
    Message messages[14] = {};
    DraculaView dv = DvNew(trail, messages);
    int num = -1;

    // DvGetValidMoves
    printf("... testing DvGetValidMoves\n");
    PlaceId *moves = DvGetValidMoves(dv, &num);
    assert(num == 3);
    sortPlaces(moves, num);
    assert(moves[0] == GALATZ);
    assert(moves[1] == KLAUSENBURG);
    assert(moves[2] == HIDE);
    free(moves);

    // DvWhereCanIGo
    printf("... testing DvWhereCanIGo\n");
    PlaceId *locations = DvWhereCanIGo(dv, &num);
    assert(num == 3);
    sortPlaces(locations, num);
    assert(locations[0] == CASTLE_DRACULA);
    assert(locations[1] == GALATZ);
    assert(locations[2] == KLAUSENBURG);
    free(locations);

    DvFree(dv);
    passed();
    }

    {///////////////////////////////////////////////////////////////////
    purple();
    printf("Test for when both HIDE and DB are invalid (already used)\n");
    resetColour();
    char *trail =
        "GGE.... SGE.... HGE.... MGE.... DCD.V.. "
        "GGE.... SGE.... HGE.... MGE.... DD1T... "
        "GGE.... SGE.... HGE.... MGE.... DHIT... "
        "GGE.... SGE.... HGE.... MGE....";
    
    Message messages[19] = {};
    DraculaView dv = DvNew(trail, messages);
    int num = -1;

    // DvGetValidMoves
    printf("... testing DvGetValidMoves\n");
    PlaceId *moves = DvGetValidMoves(dv, &num);
    assert(num == 2);
    sortPlaces(moves, num);
    assert(moves[0] == GALATZ);
    assert(moves[1] == KLAUSENBURG);
    free(moves);

    // DvWhereCanIGo
    printf("... testing DvWhereCanIGo\n");
    PlaceId *locations = DvWhereCanIGo(dv, &num);
    assert(num == 2);
    sortPlaces(locations, num);
    assert(locations[0] == GALATZ);
    assert(locations[1] == KLAUSENBURG);
    free(locations);

    DvFree(dv);
    passed();
    }

    {///////////////////////////////////////////////////////////////////
    purple();
    printf("Test for when Dracula is at sea: HIDE is invalid but DB is valid\n");
    resetColour();
    char *trail =
        "GGE.... SGE.... HGE.... MGE.... DCD.V.. "
        "GGE.... SGE.... HGE.... MGE.... DGAT... "
        "GGE.... SGE.... HGE.... MGE.... DCNT... "
        "GGE.... SGE.... HGE.... MGE.... DBS.... "
        "GGE.... SGE.... HGE.... MGE....";
    
    Message messages[24] = {};
    DraculaView dv = DvNew(trail, messages);
    int num = -1;

    // DvGetValidMoves
    printf("... testing DvGetValidMoves\n");
    PlaceId *moves = DvGetValidMoves(dv, &num);
    assert(num == 4);
    sortPlaces(moves, num);
    assert(moves[0] == IONIAN_SEA);
    assert(moves[1] == VARNA);
    assert(moves[2] == DOUBLE_BACK_1);
    assert(moves[3] == DOUBLE_BACK_2);
    free(moves);

    // DvWhereCanIGo
    printf("... testing DvWhereCanIGo\n");
    PlaceId *locations = DvWhereCanIGo(dv, &num);
    assert(num == 4);
    sortPlaces(locations, num);
    assert(locations[0] == BLACK_SEA);
    assert(locations[1] == CONSTANTA);
    assert(locations[2] == IONIAN_SEA);
    assert(locations[3] == VARNA);
    free(locations);

    DvFree(dv);
    passed();
    }

    {///////////////////////////////////////////////////////////////////
    purple();
    printf("Test for when Dracula is at sea: HIDE is invalid and DB is used\n");
    resetColour();
    char *trail =
        "GGE.... SGE.... HGE.... MGE.... DCD.V.. "
        "GGE.... SGE.... HGE.... MGE.... DGAT... "
        "GGE.... SGE.... HGE.... MGE.... DCNT... "
        "GGE.... SGE.... HGE.... MGE.... DBS.... "
        "GGE.... SGE.... HGE.... MGE.... DD1.... "
        "GGE.... SGE.... HGE.... MGE....";
    
    Message messages[29] = {};
    DraculaView dv = DvNew(trail, messages);
    int num = -1;

    // DvGetValidMoves
    printf("... testing DvGetValidMoves\n");
    PlaceId *moves = DvGetValidMoves(dv, &num);
    assert(num == 2);
    sortPlaces(moves, num);
    assert(moves[0] == IONIAN_SEA);
    assert(moves[1] == VARNA);
    free(moves);

    // DvWhereCanIGo
    printf("... testing DvWhereCanIGo\n");
    PlaceId *locations = DvWhereCanIGo(dv, &num);
    assert(num == 2);
    sortPlaces(locations, num);
    assert(locations[0] == IONIAN_SEA);
    assert(locations[1] == VARNA);
    free(locations);

    DvFree(dv);
    passed();
    }

    {///////////////////////////////////////////////////////////////////
    purple();
    printf("Test for when HIDE and DB are the only valid moves\n");
    resetColour();
    char *trail =
        "GGE.... SGE.... HGE.... MGE.... DGA.V.. "
        "GGE.... SGE.... HGE.... MGE.... DKLT... "
        "GGE.... SGE.... HGE.... MGE.... DCDT... "
        "GGE.... SGE.... HGE.... MGE....";
    
    Message messages[19] = {};
    DraculaView dv = DvNew(trail, messages);
    int num = -1;

    // DvGetValidMoves
    printf("... testing DvGetValidMoves\n");
    PlaceId *moves = DvGetValidMoves(dv, &num);
    assert(num == 4);
    sortPlaces(moves, num);
    assert(moves[0] == HIDE);
    assert(moves[1] == DOUBLE_BACK_1);
    assert(moves[2] == DOUBLE_BACK_2);
    assert(moves[3] == DOUBLE_BACK_3);
    free(moves);

    // DvWhereCanIGo
    printf("... testing DvWhereCanIGo\n");
    PlaceId *locations = DvWhereCanIGo(dv, &num);
    assert(num == 3);
    sortPlaces(locations, num);
    assert(locations[0] == CASTLE_DRACULA);
    assert(locations[1] == GALATZ);
    assert(locations[2] == KLAUSENBURG);
    free(locations);

    DvFree(dv);
    passed();
    }

    {///////////////////////////////////////////////////////////////////
    purple();
    printf("Test for when there are no valid moves\n");
    resetColour();
    char *trail =
        "GGE.... SGE.... HGE.... MGE.... DGA.V.. "
        "GGE.... SGE.... HGE.... MGE.... DKLT... "
        "GGE.... SGE.... HGE.... MGE.... DCDT... "
        "GGE.... SGE.... HGE.... MGE.... DD1T... "
        "GGE.... SGE.... HGE.... MGE.... DHIT... "
        "GGE.... SGE.... HGE.... MGE....";
    
    Message messages[29] = {};
    DraculaView dv = DvNew(trail, messages);
    int num = -1;

    // DvGetValidMoves
    printf("... testing DvGetValidMoves\n");
    PlaceId *moves = DvGetValidMoves(dv, &num);
    assert(num == 0);
    assert(moves == NULL);
    free(moves);

    // DvWhereCanIGo
    printf("... testing DvWhereCanIGo\n");
    PlaceId *locations = DvWhereCanIGo(dv, &num);
    assert(num == 0);
    assert(locations == NULL);
    free(locations);

    DvFree(dv);
    passed();
    }

    {///////////////////////////////////////////////////////////////////
    purple();
    printf("Test for when Dracula has not moved\n");
    resetColour();
    char *trail =
        "GGE.... SGE.... HGE.... MGE....";
    
    Message messages[4] = {};
    DraculaView dv = DvNew(trail, messages);
    int num = -1;

    // DvGetValidMoves
    printf("... testing DvGetValidMoves\n");
    PlaceId *moves = DvGetValidMoves(dv, &num);
    assert(num == 0);
    assert(moves == NULL);
    free(moves);

    // DvWhereCanIGo
    printf("... testing DvWhereCanIGo\n");
    PlaceId *locations = DvWhereCanIGo(dv, &num);
    assert(num == 0);
    assert(locations == NULL);
    free(locations);

    DvFree(dv);
    passed();
    }

    {///////////////////////////////////////////////////////////////////
    purple();
    printf("Test for when HIDE is about to fall off trail\n");
    resetColour();
    char *trail =
        "GGE.... SGE.... HGE.... MGE.... DCD.V.. "
        "GGE.... SGE.... HGE.... MGE.... DHIT... "
        "GGE.... SGE.... HGE.... MGE.... DKLT... "
        "GGE.... SGE.... HGE.... MGE.... DBET... "
        "GGE.... SGE.... HGE.... MGE.... DBCT... "
        "GGE.... SGE.... HGE.... MGE.... DGAT... "
        "GGE.... SGE.... HGE.... MGE....";
    
    Message messages[34] = {};
    DraculaView dv = DvNew(trail, messages);
    int num = -1;

    // DvGetValidMoves
    printf("... testing DvGetValidMoves\n");
    PlaceId *moves = DvGetValidMoves(dv, &num);
    assert(num == 5);
    sortPlaces(moves, num);
    assert(moves[0] == CONSTANTA);
    assert(moves[1] == DOUBLE_BACK_1);
    assert(moves[2] == DOUBLE_BACK_2);
    assert(moves[3] == DOUBLE_BACK_4);
    assert(moves[4] == DOUBLE_BACK_5);
    free(moves);

    // DvWhereCanIGo
    printf("... testing DvWhereCanIGo\n");
    PlaceId *locations = DvWhereCanIGo(dv, &num);
    assert(num == 5);
    sortPlaces(locations, num);
    assert(locations[0] == BUCHAREST);
    assert(locations[1] == CASTLE_DRACULA);
    assert(locations[2] == CONSTANTA);
    assert(locations[3] == GALATZ);
    assert(locations[4] == KLAUSENBURG);
    free(locations);

    DvFree(dv);
    passed();
    }

    printf("\033[4;33m"); 
    printf("All tests passed.\n");
    printf("\n");


	return EXIT_SUCCESS;
}

void passed() {
    green();
    printf("Test passed!\n");
    printf("\n");
    resetColour();
}
