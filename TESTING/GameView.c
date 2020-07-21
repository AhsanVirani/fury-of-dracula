////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// GameView.c: GameView ADT implementation
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
#include <string.h>

#include "Game.h"
#include "GameView.h"
#include "Map.h"
#include "Places.h"

#define MAX_ROUNDS 366
#define ABBREVIATION_SIZE 3
#define MOVE_SIZE 8

// global declarations
char abbre[ABBREVIATION_SIZE];
char move[MOVE_SIZE];
char 	*locVamp;		// location of vampire. Should be freed after use
char 	*traps;			// location of all active traps. Dynamic

// Static Function Declarations
static void setGameView(GameView, char *);
static void set_abbreviation(char *, char *);
static void reset_move_abbreviation(char *, char *);
static void set_playerInfo(GameView, char *, char *);

// add your own #includes here


struct node {
	int health;		// health of a given player
	PlaceId *placeHist;	// Array of all the location history of a player 
} Node;


struct gameView {
	Map graph;
	struct node players[NUM_PLAYERS];	// array storing pointer to a node
	Round numRound;
	int GameScore;
};

static 
void set_abbreviation(char *move, char *abbre)
{
	assert(move != NULL);
	abbre[0] = move[1];
	abbre[1] = move[2];
}

static 
void reset_move_abbreviation(char *move, char *abbre)
{
	int i;
	for(i = 0; i < 8; i++)	move[i] = '\0';
	for(i = 0; i < 3; i++)	abbre[i] = '\0';
}

static
void set_playerInfo(GameView gv, char *move, char *abbre)
{
	switch(move[0]) {
	case 'G':
		// if starting at hospital increase health to full
		if(gv->players[0].health == 0)
			gv->players[0].health = GAME_START_HUNTER_LIFE_POINTS;
		gv->players[0].placeHist[gv->numRound] = placeAbbrevToId(abbre);
		// if rests then health +3 but not >9
		if(gv->players[0].placeHist[gv->numRound - 1] == gv->players[0].placeHist[gv->numRound])
			gv->players[0].health = gv->players[0].health > 5? GAME_START_HUNTER_LIFE_POINTS: gv->players[0].health + LIFE_GAIN_REST;
		
		// look at move[3] .. move[6]
		// encounter V T D and health points reduction
		// if V then locVamp == NULL
		for(int i = 3; i < MOVE_SIZE - 1; i++) {
			if(move[i] == 'V')
				locVamp = NULL;
			if(move[i] == 'T') {
				// Remember to destroy trap
				gv->players[0].health = gv->players[0].health < 3? 0: gv->players[0].health - LIFE_LOSS_TRAP_ENCOUNTER;
				break;
			}
			if(move[i] == 'D') {
				gv->players[0].health = gv->players[0].health < 5? 0: gv->players[0].health - LIFE_LOSS_DRACULA_ENCOUNTER;
				// Draculae health also reduces by 10
				gv->players[4].health = gv->players[4].health < 11? 0: gv->players[0].health - LIFE_LOSS_HUNTER_ENCOUNTER;
			}
				
		}

		// end of move[6] and round % 6 == 0 and not V and locVamp != NULL then gamescore -13
		// If health 0 goes to hospital and gamescore -6

		break;
	case 'S':
		gv->players[1].placeHist[gv->numRound] = placeAbbrevToId(abbre); 
		printf("%d\n",gv->players[1].placeHist[0]);
		break;
	case 'H':
		gv->players[2].placeHist[gv->numRound] = placeAbbrevToId(abbre); 
		printf("%d\n",gv->players[2].placeHist[0]);
		break;
	case 'M':
		gv->players[3].placeHist[gv->numRound] = placeAbbrevToId(abbre); 
		printf("%d\n",gv->players[3].placeHist[0]);
		break;
	case 'D':
		gv->players[4].placeHist[gv->numRound] = placeAbbrevToId(abbre); 
		printf("%d\n",gv->players[4].placeHist[0]);

		// if Draculae means end of round
		gv->numRound++;
		break;
	}

	// Break out of switch statement here. Now updates players health

/*
	if(player == 'G') {		
		gv->players[0].placeHist[gv->numRound] = placeAbbrevToId(abbre);
		printf("%d\n", gv->players[0].placeHist[0]);
	}
	if(player == 'S') { 		
		gv->players[1].placeHist[gv->numRound] = placeAbbrevToId(abbre); 
		printf("%d\n",gv->players[1].placeHist[0]);
	}
	if(player == 'H') { 		
		gv->players[2].placeHist[gv->numRound] = placeAbbrevToId(abbre); 
		printf("%d\n",gv->players[2].placeHist[0]);
	}
	if(player == 'M') { 		
		gv->players[3].placeHist[gv->numRound] = placeAbbrevToId(abbre); 
		printf("%d\n",gv->players[3].placeHist[0]);
	}
	if(player == 'D') { 		
		gv->players[4].placeHist[gv->numRound] = placeAbbrevToId(abbre); 
		printf("%d\n",gv->players[4].placeHist[0]);
	}
	*/
}

void setGameView(GameView gv, char *pastPlays)
{
	assert(gv != NULL && pastPlays != NULL);
		
	int i;
	int j = 0;
	// set move and abbre arrays to null terminators
	for(i = 0; i < MOVE_SIZE; i++)
		move[i] = '\0';
	for(i = 0; i < ABBREVIATION_SIZE; i++)
		abbre[i] = '\0';

	// Reading the pastPlays string and recording
	for(i = 0; pastPlays[i] != '\0'; i++) {
		if(pastPlays[i] == ' ') {					
			continue;
		}
		move[j] = pastPlays[i];
		j++;
		if(j == MOVE_SIZE - 1) {			
			set_abbreviation(move, abbre);
			// set array of moves of a player
			set_playerInfo(gv, move, abbre);

			//printf("%s\n", move);
			//printf("%s\n", abbre);
			reset_move_abbreviation(move, abbre);
			j = 0;
		}
	}

}

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

GameView GvNew(char *pastPlays, Message messages[])
{
	GameView gv = malloc(sizeof(*gv));
	if (gv == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}
	gv->graph = MapNew();
	gv->GameScore = GAME_START_SCORE;
	
	// setting up infoPlayers array
	for(int i = 0; i < 5; i++) {	
		gv->players[i].placeHist = malloc(MAX_ROUNDS * sizeof(PlaceId *));
		gv->players[i].health = GAME_START_HUNTER_LIFE_POINTS;
		assert(gv->players[i].placeHist != NULL);
	}
	// Setting traps dynamic array here
	traps = malloc(TRAIL_SIZE * sizeof(char *));

	// loop through pastPlays and edit round Numbers and all the info inside players	
	setGameView(gv, pastPlays);	

	return gv;
}

void GvFree(GameView gv)
{
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round GvGetRound(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

Player GvGetPlayer(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return PLAYER_LORD_GODALMING;
}

int GvGetScore(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

int GvGetHealth(GameView gv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return NOWHERE;
}

PlaceId GvGetVampireLocation(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return NOWHERE;
}

// trap global variable passed here
PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numTraps = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Game History

PlaceId *GvGetMoveHistory(GameView gv, Player player,
                          int *numReturnedMoves, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	*canFree = false;
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

void white_box() {
	
	char *pastPlays = "GMN.... SPL.... HAM.... MPA.... DCD.V.. GLV....";
	//setNode(pastPlays);
	GvNew(pastPlays, NULL);
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
