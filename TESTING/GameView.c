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
PlaceId **PlayersPlaceHist;



// Static Function Declarations
static void setGameView(GameView, char *);
static void set_abbreviation(char *, char *);
static void reset_move_abbreviation(char *, char *);
static void set_playerInfo(GameView, char *, char *);
static void restoreHHealth(GameView, int);
static void restHunterspts(GameView, int);
static void HunterEncounter(GameView, int, char *);
static void VampireMatures(GameView);

// add your own #includes here


struct node_hunters {
	int health;				// health of a given player
};

struct node_dracula {
	int bloodpts;			// health of a given player
	char 	*traps;			// location of all active traps. Dynamic
	PlaceId locVamp;		// location of vampire. Should be freed after use	
};


struct gameView {
	Map graph;
	struct node_hunters hunters[NUM_PLAYERS];		// array storing pointer to hunters node
	struct node_dracula dracula;						// array storing pointer to dracula node	
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
void restoreHHealth(GameView gv, int HunterIndex)
{
	assert(gv != NULL);
	gv->hunters[HunterIndex].health = GAME_START_HUNTER_LIFE_POINTS;
}

static
void restHunterspts(GameView gv, int HunterIndex)
{
	assert(gv != NULL);
	gv->hunters[HunterIndex].health = gv->hunters[HunterIndex].health > 5? GAME_START_HUNTER_LIFE_POINTS: gv->hunters[HunterIndex].health + LIFE_GAIN_REST;
}

static
void draculaRealPlace(GameView gv, PlaceId p, char *abbre)
{
	assert(gv != NULL && abbre != NULL);
	if(!placeIsSea(p)) {
		if(p != CASTLE_DRACULA)
			PlayersPlaceHist[4][gv->numRound] = placeAbbrevToId(abbre);
		else {
			PlayersPlaceHist[4][gv->numRound] = placeAbbrevToId(abbre);
			gv->dracula.bloodpts += LIFE_GAIN_CASTLE_DRACULA;
		}
	}			
	else {
		PlayersPlaceHist[4][gv->numRound] = placeAbbrevToId(abbre);
		gv->dracula.bloodpts -= 2;
	}
}

static
void draculaNotRealPlace(GameView gv, PlaceId p, char *abbre, char *move)
{
	if(p == TELEPORT) {
		PlayersPlaceHist[4][gv->numRound] = CASTLE_DRACULA;
		gv->dracula.bloodpts += LIFE_GAIN_CASTLE_DRACULA;
	}
	else if(p == HIDE) {
		PlayersPlaceHist[4][gv->numRound] = PlayersPlaceHist[4][gv->numRound - 1];
		if(PlayersPlaceHist[4][gv->numRound] == SEA_UNKNOWN || placeIsSea(PlayersPlaceHist[4][gv->numRound]))
			gv->dracula.bloodpts -= LIFE_LOSS_SEA;
		if(PlayersPlaceHist[4][gv->numRound] == CASTLE_DRACULA)
			gv->dracula.bloodpts += LIFE_GAIN_CASTLE_DRACULA;
	} 
	else if(p >= 103 && p <= 107) {
		char DN = move[2];
		int Dn = DN - 48;
		PlayersPlaceHist[4][gv->numRound] = PlayersPlaceHist[4][gv->numRound - Dn];
		if(PlayersPlaceHist[4][gv->numRound] == SEA_UNKNOWN || placeIsSea(PlayersPlaceHist[4][gv->numRound]))
			gv->dracula.bloodpts -= LIFE_LOSS_SEA;
		if(PlayersPlaceHist[4][gv->numRound] == CASTLE_DRACULA)
			gv->dracula.bloodpts += LIFE_GAIN_CASTLE_DRACULA;
	}
	else if(p == SEA_UNKNOWN) {
		PlayersPlaceHist[4][gv->numRound] = placeAbbrevToId(abbre);
		gv->dracula.bloodpts -= LIFE_LOSS_SEA;
	}
	else
		PlayersPlaceHist[4][gv->numRound] = placeAbbrevToId(abbre);
}

static
void HunterEncounter(GameView gv, int HunterIndex, char *move)
{
	for(int i = 3; i < MOVE_SIZE - 1; i++) {
		if(move[i] == 'V')
			gv->dracula.locVamp = NOWHERE;
		else if(move[i] == 'T') {
			// Remember to destroy trap (DO LATER PLS)
			gv->hunters[HunterIndex].health = gv->hunters[HunterIndex].health < 3? 0: gv->hunters[HunterIndex].health - LIFE_LOSS_TRAP_ENCOUNTER;
			// If health == 0 cant do anything, hence break out of loop			
			if(gv->hunters[HunterIndex].health == 0) {
				gv->GameScore -= 6;				
				break;
			}
		}
		else if(move[i] == 'D') {
			gv->hunters[HunterIndex].health = gv->hunters[HunterIndex].health < 5? 0: gv->hunters[HunterIndex].health - LIFE_LOSS_DRACULA_ENCOUNTER;
			// Draculae health also reduces by 10
			gv->dracula.bloodpts = gv->dracula.bloodpts < 11? 0: gv->dracula.bloodpts - LIFE_LOSS_HUNTER_ENCOUNTER;
			// If health == 0 cant do anything or bloodpts == 0 gameover, so break out in both cases				
			if(gv->hunters[HunterIndex].health == 0) {
				PlayersPlaceHist[HunterIndex][gv->numRound] = ST_JOSEPH_AND_ST_MARY; 
				gv->GameScore -= 6;				
				break;
			}
			else if(gv->dracula.bloodpts == 0)
				break;
		}		
	}
}



static
void VampireMatures(GameView gv)
{
	assert(gv != NULL);
	gv->GameScore -= 13;
	gv->dracula.locVamp = NOWHERE;
}

static
void set_playerInfo(GameView gv, char *move, char *abbre)
{
	switch(move[0]) {
	case 'G':
		// if Health = 0 then increase health to full new Round
		if(gv->hunters[0].health == 0)
			restoreHHealth(gv, 0);
		// fills the PlayersPlaceHist array	
		PlayersPlaceHist[0][gv->numRound] = placeAbbrevToId(abbre);
		// if rests then health +3
		if(PlayersPlaceHist[0][gv->numRound - 1] == PlayersPlaceHist[0][gv->numRound])
			restHunterspts(gv, 0);		
		// Trap, Vampire, Dracula encounters handling
		HunterEncounter(gv, 0, move);
		break;
	
	case 'S':
		if(gv->hunters[1].health == 0)
			restoreHHealth(gv, 1);	
		PlayersPlaceHist[1][gv->numRound] = placeAbbrevToId(abbre);
		if(PlayersPlaceHist[1][gv->numRound - 1] == PlayersPlaceHist[1][gv->numRound])
			restHunterspts(gv, 1);
		HunterEncounter(gv, 1, move);
		break;

	case 'H':
		if(gv->hunters[2].health == 0)
			restoreHHealth(gv, 2);		
		PlayersPlaceHist[2][gv->numRound] = placeAbbrevToId(abbre);
		if(PlayersPlaceHist[2][gv->numRound - 1] == PlayersPlaceHist[2][gv->numRound])
			restHunterspts(gv, 2);
		HunterEncounter(gv, 2, move);		
		break;

	case 'M':
		if(gv->hunters[3].health == 0)
			restoreHHealth(gv, 3);	
		PlayersPlaceHist[3][gv->numRound] = placeAbbrevToId(abbre);
		if(PlayersPlaceHist[3][gv->numRound - 1] == PlayersPlaceHist[3][gv->numRound])
			restHunterspts(gv, 3);
		HunterEncounter(gv, 3, move);		
		break;

	case 'D': ;
		// If Dracula makes move to a real place
		PlaceId p = placeAbbrevToId(abbre);
		if(placeIsReal(p))
			draculaRealPlace(gv, p, abbre);
		// If Dracula makes move to non-Real place
		else if(!placeIsReal(p)) {
			draculaNotRealPlace(gv, p, abbre, move);
		}  


		// check for vampire 
		if(move[4] == 'V')
			gv->dracula.locVamp = PlayersPlaceHist[4][gv->numRound];

		// check for trap and add to list trap. DO LATER PLS
		 
		// Vampire matures and flies away if after 6 rounds not vanquished
		if(move[5] == 'V')
			VampireMatures(gv);

		// Increase round number and decrease gamescore at end of Dracula's turn
		gv->numRound++;
		gv->GameScore -= SCORE_LOSS_DRACULA_TURN;
		break;
	
	default:
		printf("Not a Valid Player");
		return;
	}
	
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
		//	printf("%s ", abbre);
			// set array of moves of a player
			set_playerInfo(gv, move, abbre);
			//printf("%d ", placeAbbrevToId(abbre));

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
	gv->numRound = 0;

	// initialising points for players
	int i;	
	gv->dracula.bloodpts = GAME_START_BLOOD_POINTS;
	for(i = 0; i < (NUM_PLAYERS - 1); i++)
		gv->hunters[i].health = GAME_START_HUNTER_LIFE_POINTS;	

	// setting up Players Place History Dynamic Array
	PlayersPlaceHist = malloc(sizeof(PlaceId *) * NUM_PLAYERS);
	assert(PlayersPlaceHist != NULL);
	for(i = 0; i < NUM_PLAYERS; i++) {
		PlayersPlaceHist[i] = malloc(sizeof(PlaceId) * MAX_ROUNDS);
		assert(PlayersPlaceHist[i] != NULL);
	}
	// Setting all the Past moves to NoWhere initially
	for(i = 0; i < NUM_PLAYERS; i++) {
		for(int j = 0; j < MAX_ROUNDS; j++)
			PlayersPlaceHist[i][j] = -2;
	}

	// Set locVamp
	gv->dracula.locVamp = NOWHERE;

	// Setting traps dynamic array here
	gv->dracula.traps = malloc(TRAIL_SIZE * sizeof(char *));
	// initialise with null terminators
	for(i = 0; i < TRAIL_SIZE; i++)
		gv->dracula.traps[i] = '\0';

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
	
	char *pastPlays = 	"GGE.... SGE.... HGE.... MGE.... DCD.V.. GGE.... SGE.... HGE.... MGE.... DD1T...";
	//setNode(pastPlays);
	//printf("%d %d %d %d %d %d\n",placeAbbrevToId("MN"), placeAbbrevToId("PL"), placeAbbrevToId("AM"), placeAbbrevToId("PA"), placeAbbrevToId("CD"), placeAbbrevToId("LV"));
	GameView gv = GvNew(pastPlays, NULL); 
	

	//printf("%d\n", gv->numRound);
	//printf("%d\n", gv->GameScore);
		
	//printf("%s\n", placeIdToName(PlayersPlaceHist[1][gv->numRound-1]));
	//printf("%s\n", placeIdToName(PlayersPlaceHist[2][gv->numRound-1]));
	//printf("%s\n", placeIdToName(PlayersPlaceHist[3][gv->numRound-1]));
	//printf("%s\n", placeIdToName(PlayersPlaceHist[4][gv->numRound-1]));			
	//printf("%d\n", gv->hunters[0].health);
	printf("%d\n", gv->dracula.bloodpts);
	//printf("%s\n", placeIdToName(gv->dracula.locVamp));
	//printf("%s\n", placeIdToName(PlayersPlaceHist[0][gv->numRound]));
	printf("%s\n", placeIdToName(PlayersPlaceHist[4][gv->numRound-1]));	
	//printf("%d\n", gv->numRound);
	//printf("%s\n", placeIdToName(PlayersPlaceHist[0][gv->numRound+1]));

	//printf("%d", placeAbbrevToId("HI"));
	//PlaceId p= placeAbbrevToId("HI");
	//printf("%d\n", p);*/
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
