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
static void draculaRealPlace(GameView, PlaceId, char *);
static void draculaNotRealPlace(GameView, PlaceId, char *, char *);
static void incrementRound(GameView);
static void decrementGameScore(GameView);
static void addTrail(GameView gv, PlaceId loc, bool isVamp);
static void removeTrail(GameView gv, PlaceId loc, bool isVamp);


struct node_hunters {
	int health;				// health of a given player
};

struct node_dracula {
	int bloodpts;								// health of a given player
	int ntrail;									// size of trail	
	PlaceId 	trail[TRAIL_SIZE];			// location of trail. Dynamic	
	PlaceId locVamp;							// location of vampire. Should be freed after use	
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
			removeTrail(gv, gv->dracula.locVamp, true);
		else if(move[i] == 'T') {
			// Remember to destroy trap (DO LATER PLS)
			gv->hunters[HunterIndex].health = gv->hunters[HunterIndex].health < 3? 0: gv->hunters[HunterIndex].health - LIFE_LOSS_TRAP_ENCOUNTER;
			removeTrail(gv, PlayersPlaceHist[HunterIndex][gv->numRound], false);			
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
}

static
void incrementRound(GameView gv)
{
	assert(gv != NULL);
	gv->numRound++;
}

static
void decrementGameScore(GameView gv)
{
	assert(gv != NULL);
	gv->GameScore -= SCORE_LOSS_DRACULA_TURN;
}

static
void addTrail(GameView gv, PlaceId loc, bool isVamp)
{
	assert(gv != NULL);
	
	if(isVamp)
		gv->dracula.locVamp = loc;
	if(gv->dracula.ntrail < 6) {
		gv->dracula.trail[gv->dracula.ntrail] = loc;
		gv->dracula.ntrail++;
	}
	else {
		int i;
		for(i = 1; i < TRAIL_SIZE; i++) {
			gv->dracula.trail[i-1] = gv->dracula.trail[i];  
		}
		gv->dracula.trail[TRAIL_SIZE - 1] = loc;
	}
}

static
void removeTrail(GameView gv, PlaceId loc, bool isVamp)
{
	assert(gv != NULL);
	int i; int locNum = 0;
	for(i = 0; i < TRAIL_SIZE; i++) {
		if(gv->dracula.trail[i] == loc)
			break;
		locNum++;
	}
	if(isVamp)
		gv->dracula.locVamp = NOWHERE;
	for(i = locNum; i < TRAIL_SIZE; i++)
		gv->dracula.trail[i] = gv->dracula.trail[i+1];
	gv->dracula.ntrail--; 
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
		// check for trap and add to list if exists
		if(move[3] == 'T')
			addTrail(gv, PlayersPlaceHist[4][gv->numRound], false);
		// check and add Vampire Location if exist 
		if(move[4] == 'V')
			addTrail(gv, PlayersPlaceHist[4][gv->numRound], true);
		// Handling Vampire Mature
		if(move[5] == 'V') {
			VampireMatures(gv);
			removeTrail(gv, gv->dracula.locVamp, true);
		}
		// Increase round number and decrease gamescore at end of Dracula's turn
		incrementRound(gv);
		decrementGameScore(gv);
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
			// set array of moves of a player
			set_playerInfo(gv, move, abbre);
			// Reset arrays to null
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
	gv->dracula.ntrail = 0;
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
			PlayersPlaceHist[i][j] = NOWHERE;
	}
	// Set locVamp
	gv->dracula.locVamp = NOWHERE;
	
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
	assert(gv != NULL);
	return gv->numRound;
}

Player GvGetPlayer(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return PLAYER_LORD_GODALMING;
}

int GvGetScore(GameView gv)
{
	assert(gv != NULL);
	return gv->GameScore;
}

int GvGetHealth(GameView gv, Player player)
{
	assert(gv != NULL);
	if(player >= 0 && player <= 3)
		return gv->hunters[player].health;
	return gv->dracula.bloodpts;
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	assert(gv != NULL);

	if(PlayersPlaceHist[player][gv->numRound] != NOWHERE)
		return PlayersPlaceHist[player][gv->numRound];
	else if((gv->numRound - 1) >= 0)
		return PlayersPlaceHist[player][gv->numRound - 1];
	
	return NOWHERE;
}

PlaceId GvGetVampireLocation(GameView gv)
{
	assert(gv != NULL);
	return gv->dracula.locVamp;
}

PlaceId *trap;
// trap global variable passed here
PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{	
	trap = malloc(sizeof(PlaceId) * TRAIL_SIZE);
	int counter = 0;
	// if gv->dracula.locVamp != NOWHERE then remove from ntrails
	int vampInTrail;
	if(gv->dracula.locVamp != NOWHERE)
		vampInTrail = 1;
	else
		vampInTrail = 0;

	for(int i = 0; i < gv->dracula.ntrail; i++) {
		if(vampInTrail && (gv->dracula.trail[i] == gv->dracula.locVamp)) {
			vampInTrail = 0;
			continue;
		}
		trap[counter] = gv->dracula.trail[i];
		counter++;
	}

	*numTraps = counter;
	// checking whether numTraps is correct
	//printf("%d\n", *numTraps);
	return trap;
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
	
	char *pastPlays =	"GLS.... SGE.... HGE.... MGE.... DST.V.. GCA.... SGE.... HGE.... MGE.... DC?T... GGR.... SGE.... HGE.... MGE.... DC?T... GAL.... SGE.... HGE.... MGE.... DD3T... GSR.... SGE.... HGE.... MGE.... DHIT... GSN.... SGE.... HGE.... MGE.... DC?T... GMA.... SSTTTV.";
	//setNode(pastPlays);
	//printf("%d %d %d %d %d %d\n",placeAbbrevToId("MN"), placeAbbrevToId("PL"), placeAbbrevToId("AM"), placeAbbrevToId("PA"), placeAbbrevToId("CD"), placeAbbrevToId("LV"));
	GameView gv = GvNew(pastPlays, NULL); 
	

	//printf("%d\n", gv->numRound);
	printf("%d\n", GvGetHealth(gv, PLAYER_DR_SEWARD));
		
	//printf("%s\n", placeIdToName(PlayersPlaceHist[1][gv->numRound-1]));
	//printf("%s\n", placeIdToName(PlayersPlaceHist[2][gv->numRound-1]));
	//printf("%s\n", placeIdToName(PlayersPlaceHist[3][gv->numRound-1]));
	//printf("%d\n", gv->hunters[0].health);
	printf("%s\n", placeIdToName(GvGetPlayerLocation(gv, PLAYER_DRACULA)));			
	printf("%s\n", placeIdToName(GvGetVampireLocation(gv)));
	//printf("%d\n", gv->dracula.bloodpts);
	//int *numTraps = malloc(sizeof(int));
	//GvGetTrapLocations(gv, numTraps);
	//printf("%d\n", *numTraps);
	//printf("%s %s\n", placeIdToName(trap[0]), placeIdToName(trap[1]));
	//printf("%s\n", placeIdToName(PlayersPlaceHist[0][gv->numRound]));
	//printf("%s\n", placeIdToName(PlayersPlaceHist[4][gv->numRound-2]));	
	//printf("%s\n", placeIdToName(gv->dracula.locVamp));
	//printf("%s %s\n", placeIdToName(gv->dracula.trail[2]), placeIdToName(gv->dracula.trail[1]));
	//printf("%s\n", placeIdToName(PlayersPlaceHist[0][gv->numRound+1]));

	//printf("%d", placeAbbrevToId("HI"));
	//PlaceId p= placeAbbrevToId("HI");
	//printf("%d\n", p);*/
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
