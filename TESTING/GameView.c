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
#define MAX_RAIL_CONNECTIONS 50
#define MAX_CONNECTION 100

// global declarations
char abbre[ABBREVIATION_SIZE];
char move[MOVE_SIZE];
PlaceId **PlayersPlaceHist;
PlaceId *moveHistory;
PlaceId *trap; // work done after trap function. Free this
PlaceId *visited;
PlaceId *connectionArr;

// Static Function Declarations
static void setGameView(GameView, char *);
static void set_abbreviation(char *, char *);
static void reset_move_abbreviation(char *, char *);
static void set_playerInfo(GameView, char *, char *);
static void FillPlayerLocationArr(GameView, Player, char *);
static void PlaceDraculaEncounter(GameView gv, char encounter);
static void PlaceDraculaAction(GameView gv, char action);
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
static int *findnumReturnedMoves(GameView, Player, int, int *);
static int distanceRail(int, Player);
static void makeVisited(PlaceId);
static int checkVisited(PlaceId);
static int InArray(PlaceId);
static int addRailConnection(GameView, PlaceId, int, int);
static int addRailConnection_wrapper(GameView, PlaceId, int, int);
static void addRoadConnection(PlaceId, int);
static void addBoatConnection(PlaceId, int);


struct node_hunters {
	int health;									// health of a given player
	PlaceId moves[MAX_ROUNDS];
};

struct node_dracula {
	int bloodpts;								// health of a given player
	int ntrail;									// size of trail	
	PlaceId moves[MAX_ROUNDS];
	PlaceId 	trail[TRAIL_SIZE];			// location of trail.	
	PlaceId locVamp;							// location of vampire
};

struct gameView {
	Map graph;
	struct node_hunters hunters[NUM_PLAYERS];		// array storing pointer to hunters node
	struct node_dracula dracula;						// array storing pointer to dracula node	
	Round numRound;
	int GameScore;
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

GameView GvNew(char *pastPlays, Message messages[])
{
	GameView gv = malloc(sizeof(*gv));
	if (gv == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}

	// GameView Struct initialisation
	gv->graph = MapNew();
	gv->GameScore = GAME_START_SCORE;
	gv->numRound = 0;
	
	int i; int j;
	//Dracula Struct initialisation
	for(i = 0; i < MAX_ROUNDS; i++)
		gv->dracula.moves[i] = NOWHERE;
	gv->dracula.ntrail = 0;
	gv->dracula.bloodpts = GAME_START_BLOOD_POINTS;
	gv->dracula.locVamp = NOWHERE;

	//Hunters Struct initialisation
	for(i = 0; i < (NUM_PLAYERS - 1); i++)
		gv->hunters[i].health = GAME_START_HUNTER_LIFE_POINTS;
	for(i = 0; i < NUM_PLAYERS - 1; i++) {
		for(j = 0; j < MAX_ROUNDS; j++)
			gv->hunters[i].moves[j] = NOWHERE;
	}

	// Initialising Players Location Dynamic Array
	PlayersPlaceHist = malloc(sizeof(PlaceId *) * NUM_PLAYERS);
	assert(PlayersPlaceHist != NULL);
	for(i = 0; i < NUM_PLAYERS; i++) {
		PlayersPlaceHist[i] = malloc(sizeof(PlaceId) * MAX_ROUNDS);
		assert(PlayersPlaceHist[i] != NULL);
	}
	for(i = 0; i < NUM_PLAYERS; i++) {
		for(int j = 0; j < MAX_ROUNDS; j++)
			PlayersPlaceHist[i][j] = NOWHERE;
	}
	
	// Process PastPlays String	
	setGameView(gv, pastPlays);	

	return gv;
}

void setGameView(GameView gv, char *pastPlays)
{
	assert(gv != NULL && pastPlays != NULL);
		
	int i;
	int j = 0;
	// Initialise move & abbrevations (abbre) arrays
	for(i = 0; i < MOVE_SIZE; i++)
		move[i] = '\0';
	for(i = 0; i < ABBREVIATION_SIZE; i++)
		abbre[i] = '\0';

	// Setting moves & abbreviations (abbre) array
	for(i = 0; pastPlays[i] != '\0'; i++) {
		if(pastPlays[i] == ' ') {					
			continue;
		}
		move[j] = pastPlays[i];
		j++;
		if(j == MOVE_SIZE - 1) {			
			set_abbreviation(move, abbre);
			set_playerInfo(gv, move, abbre);
			// Reset arrays after use
			reset_move_abbreviation(move, abbre);
			j = 0;
		}
	}
}

static
void set_playerInfo(GameView gv, char *move, char *abbre)
{
	if(gv->numRound == -1)
		gv->numRound = 0;	
	switch(move[0]) {
	case 'G':
		// if Health = 0 then increase health to full new Round
		if(gv->hunters[0].health == 0)
			restoreHHealth(gv, 0);
		// fills the PlayersPlaceHist array	 
		FillPlayerLocationArr(gv, 0, abbre);
		// if rests then health +3
		if(PlayersPlaceHist[0][gv->numRound - 1] == PlayersPlaceHist[0][gv->numRound])
			restHunterspts(gv, 0);		
		// Trap, Vampire, Dracula encounters handling
		HunterEncounter(gv, 0, move);
		break;
	
	case 'S':
		if(gv->hunters[1].health == 0)
			restoreHHealth(gv, 1);	
		FillPlayerLocationArr(gv, 1, abbre);
		if(PlayersPlaceHist[1][gv->numRound - 1] == PlayersPlaceHist[1][gv->numRound])
			restHunterspts(gv, 1);
		HunterEncounter(gv, 1, move);
		break;

	case 'H':
		if(gv->hunters[2].health == 0)
			restoreHHealth(gv, 2);		
		FillPlayerLocationArr(gv, 2, abbre);
		if(PlayersPlaceHist[2][gv->numRound - 1] == PlayersPlaceHist[2][gv->numRound])
			restHunterspts(gv, 2);
		HunterEncounter(gv, 2, move);		
		break;

	case 'M':
		if(gv->hunters[3].health == 0)
			restoreHHealth(gv, 3);	
		FillPlayerLocationArr(gv, 3, abbre);
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
		else if(!placeIsReal(p))
			draculaNotRealPlace(gv, p, abbre, move);
		// check for trap and add to list if exists
		PlaceDraculaEncounter(gv, move[3]);
		PlaceDraculaEncounter(gv, move[4]);
		PlaceDraculaAction(gv, move[5]);
		PlaceDraculaAction(gv, move[6]);
		// Increase round number and decrease gamescore at end of Dracula's turn
		incrementRound(gv);
		decrementGameScore(gv);
		break;
	
	default:
		printf("Not a Valid Player");
		return;
	}
}

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
void FillPlayerLocationArr(GameView gv, Player player, char *abbre)
{
	if(player >=0 && player <= 3) {
		PlayersPlaceHist[player][gv->numRound] = placeAbbrevToId(abbre);
		gv->hunters[player].moves[gv->numRound] = PlayersPlaceHist[player][gv->numRound];
	}
	else {
		PlayersPlaceHist[player][gv->numRound] = placeAbbrevToId(abbre);
		gv->dracula.moves[gv->numRound] = PlayersPlaceHist[player][gv->numRound];
	}

}

static
void HunterEncounter(GameView gv, int HunterIndex, char *move)
{
	for(int i = 3; i < MOVE_SIZE - 1; i++) {
		if(move[i] == 'V')
			removeTrail(gv, gv->dracula.locVamp, true);
		else if(move[i] == 'T') {
			// Destroy trap
			gv->hunters[HunterIndex].health = gv->hunters[HunterIndex].health < 3? 0: gv->hunters[HunterIndex].health - LIFE_LOSS_TRAP_ENCOUNTER;
			removeTrail(gv, PlayersPlaceHist[HunterIndex][gv->numRound], false);			
			// If health == 0 Hunter cant do anything until next round			
			if(gv->hunters[HunterIndex].health == 0) {
				gv->hunters[HunterIndex].moves[gv->numRound] = ST_JOSEPH_AND_ST_MARY;
				gv->GameScore -= 6;				
				break;
			}
		}
		else if(move[i] == 'D') {
			gv->hunters[HunterIndex].health = gv->hunters[HunterIndex].health < 5? 0: gv->hunters[HunterIndex].health - LIFE_LOSS_DRACULA_ENCOUNTER;
			gv->dracula.bloodpts = gv->dracula.bloodpts < 11? 0: gv->dracula.bloodpts - LIFE_LOSS_HUNTER_ENCOUNTER;				
			if(gv->hunters[HunterIndex].health == 0) {
				gv->hunters[HunterIndex].moves[gv->numRound] = ST_JOSEPH_AND_ST_MARY;
				gv->GameScore -= 6;				
				break;
			}
			else if(gv->dracula.bloodpts == 0)
				break;
		}		
	}
}

static
void PlaceDraculaEncounter(GameView gv, char encounter)
{
	if(encounter == 'T')
		addTrail(gv, PlayersPlaceHist[4][gv->numRound], false); 
	else if(encounter == 'V')
		addTrail(gv, PlayersPlaceHist[4][gv->numRound], true);
}

static
void PlaceDraculaAction(GameView gv, char action)
{
	if(action == 'M')
		removeTrail(gv, gv->dracula.locVamp, false);
	else if(action == 'V') {
		VampireMatures(gv);
		removeTrail(gv, gv->dracula.locVamp, true);
	}
}

static
void draculaRealPlace(GameView gv, PlaceId p, char *abbre)
{
	assert(gv != NULL && abbre != NULL);
	if(!placeIsSea(p)) {
		if(p != CASTLE_DRACULA)
			FillPlayerLocationArr(gv, 4, abbre);
		else {
			FillPlayerLocationArr(gv, 4, abbre);
			gv->dracula.bloodpts += LIFE_GAIN_CASTLE_DRACULA;
		}
	}			
	else {
		FillPlayerLocationArr(gv, 4, abbre);
		gv->dracula.bloodpts -= 2;
	}
}

static
void draculaNotRealPlace(GameView gv, PlaceId p, char *abbre, char *move)
{
	assert(gv != NULL);

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
	else {
		PlayersPlaceHist[4][gv->numRound] = placeAbbrevToId(abbre);
		}
	gv->dracula.moves[gv->numRound] = p;
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

void GvFree(GameView gv)
{
	assert(gv != NULL);

	MapFree(gv->graph);
	free(gv);
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
	assert(gv != NULL);
	int pNum;
	for(pNum = 0; pNum < NUM_PLAYERS; pNum++) {
		if(PlayersPlaceHist[pNum][gv->numRound] == NOWHERE)
			break;
	}

	return pNum;
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

	if(PlayersPlaceHist[player][gv->numRound] != NOWHERE) {
		if(gv->hunters[player].moves[gv->numRound] == ST_JOSEPH_AND_ST_MARY)
			return ST_JOSEPH_AND_ST_MARY;
		return PlayersPlaceHist[player][gv->numRound];
	}
	else if((gv->numRound - 1) >= 0) {
		if(gv->hunters[player].moves[gv->numRound -1] == ST_JOSEPH_AND_ST_MARY)
			return ST_JOSEPH_AND_ST_MARY;
		return PlayersPlaceHist[player][gv->numRound - 1];
	}
	return NOWHERE;
}

PlaceId GvGetVampireLocation(GameView gv)
{
	assert(gv != NULL);
	return gv->dracula.locVamp;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	assert(gv != NULL);	

	trap = malloc(sizeof(PlaceId) * TRAIL_SIZE);
	int counter = 0;
	int vampInTrail;

	// remove Vampire from trail if exists
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
	return trap;
}

////////////////////////////////////////////////////////////////////////
// Game History
PlaceId *GvGetMoveHistory(GameView gv, Player player,
                          int *numReturnedMoves, bool *canFree)
{
	// Create a dynamically allocated array of size MAX_ROUND
	moveHistory = malloc(sizeof(PlaceId) *MAX_ROUNDS);
	assert(moveHistory != NULL);
	for(int i = 0; i < MAX_ROUNDS; i++)
		moveHistory[i] = NOWHERE;
	
	// Setting up MoveHistory array for PLAYER_DRACULA
	int i;
	if(player == PLAYER_DRACULA) {
		for(i = 0; i < MAX_ROUNDS; i++) {
			if(gv->dracula.moves[i] == NOWHERE) {
				break;
			}
			moveHistory[i] = gv->dracula.moves[i];
		}
	}

	// Setting up MoveHistory array for HUNTERS
	else {	
		for(i = 0; i < MAX_ROUNDS; i++) {
			if(gv->hunters[player].moves[i] == NOWHERE)
					break;
			if(gv->hunters[player].moves[i] == ST_JOSEPH_AND_ST_MARY) {
				moveHistory[i] = PlayersPlaceHist[player][i];
				continue;
			}
			moveHistory[i] = gv->hunters[player].moves[i];
		}
	}

	*numReturnedMoves = i;
	*canFree = true;
	return moveHistory;
}

static
int *findnumReturnedMoves(GameView gv, Player player, int numMoves, int *numReturnedMoves)
{
	// Finding numReturnedMoves
	if(numMoves <= gv->numRound)
		*numReturnedMoves = numMoves;

	else if(numMoves > gv->numRound) {
		if((player >= 0 && player <= 3) && (gv->dracula.moves[gv->numRound] == NOWHERE))
			*numReturnedMoves = gv->numRound + 1;
		else {
			*numReturnedMoves = gv->numRound;
		}
	}
	return numReturnedMoves;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	assert(gv != NULL);

	// Finds numReturnedMoves
	numReturnedMoves = findnumReturnedMoves(gv, player, numMoves, numReturnedMoves);
	moveHistory = malloc(sizeof(PlaceId) * (*numReturnedMoves));
	assert(moveHistory != NULL);
	for(int i = 0; i < *numReturnedMoves; i++)
		moveHistory[i] = NOWHERE;
	int lastElem = ((player >= 0 && player <= 3) && (gv->dracula.moves[gv->numRound] == NOWHERE))? gv->numRound: gv->numRound-1;

	// Setting up lastMoves array for PLAYER_DRACULA
	int i;
	if(player == PLAYER_DRACULA) {
		for(i = *numReturnedMoves - 1; i >= 0 && lastElem >= 0; i--) {
			moveHistory[i] = gv->dracula.moves[lastElem];		
			lastElem--;
		}
	}
	// Setting up lastMoves array for HUNTERS
	else {
		for(i = *numReturnedMoves - 1; i >= 0 && lastElem >= 0; i--) {
			if(gv->hunters[player].moves[lastElem] == ST_JOSEPH_AND_ST_MARY)
				moveHistory[i] = PlayersPlaceHist[player][lastElem];
			else
				moveHistory[i] = gv->hunters[player].moves[lastElem];				
			lastElem--;
		}
	}

	*canFree = true;
	return moveHistory;
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
	assert(gv != NULL);
	
	int i;
	for(i = 0; i <= gv->numRound; i++) {
		if(PlayersPlaceHist[player][i] == NOWHERE)
			break;
	}
	*numReturnedLocs = i;
	*canFree = true;
	return PlayersPlaceHist[player];
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	assert(gv != NULL);
	
	if((gv->numRound) >= numLocs) {
		*numReturnedLocs = numLocs;
		*canFree = true;
		int newLocHead = (player >=0 && player <= 3) && (PlayersPlaceHist[4][gv->numRound] == NOWHERE)? (gv->numRound+1-numLocs):(gv->numRound-numLocs);
		return &PlayersPlaceHist[player][newLocHead];
	}
	*numReturnedLocs = (player >=0 && player <= 3) && (PlayersPlaceHist[4][gv->numRound] == NOWHERE)? (gv->numRound+1): gv->numRound;
	*canFree = true;
	return PlayersPlaceHist[player];
}

////////////////////////////////////////////////////////////////////////
// Making a Move


static 
int distanceRail(int roundNum, Player player)
{
	int sum = roundNum + player;
	if(sum % 4 == 0)
		return 0;
	if(sum % 4 == 1)
		return 1;
	if(sum % 4 == 2)
		return 2;
	return 3;
}


static
void makeVisited(PlaceId from)
{
	int i;
	for(i = 0; i < MAX_RAIL_CONNECTIONS; i++) {
		if(visited[i] == NOWHERE)
			break;	
	}
	visited[i] = from;
}


static 
int checkVisited(PlaceId from)
{	
	int i;
	for(i = 0; i < MAX_RAIL_CONNECTIONS; i++) {
		if(visited[i] == from)
			return 1;
	}
	return 0;
}

static
int InArray(PlaceId p)
{
	int i;
	for(i = 0; i < MAX_CONNECTION; i++) {
		if(connectionArr[i] == p)
			return 1;
	}
	return 0;
}

static
int addRailConnection(GameView gv, PlaceId from, int RailMoves, int pos)
{
	if(!InArray(from)) {
		connectionArr[pos] = from;
<<<<<<< HEAD
		// printf("%s\n", placeIdToName(connectionArr[pos]));
=======
>>>>>>> 452b0b5dce1372590a73b2b1d03011748672f9ae
		makeVisited(from);
		pos++;
	}

	if(RailMoves == 1) {	
		return pos;
	}

	ConnList w = MapGetConnections(gv->graph, from);
	while(w != NULL) {
		if(w->type != RAIL) {
			w = w->next;
			continue;
		}	
		if(checkVisited(w->p)) {
			w = w->next;		
			continue;
		}
		pos = addRailConnection(gv, w->p, RailMoves-1, pos);
		w = w->next;
	}
	return pos;
}

// returns total number of connections added
static
int addRailConnection_wrapper(GameView gv, PlaceId from, int RailMoves, int pos)
{
	assert(gv != NULL);

	visited = malloc(sizeof(PlaceId) * MAX_RAIL_CONNECTIONS);
	for(int i = 0; i < MAX_RAIL_CONNECTIONS; i++)
		visited[i] = NOWHERE;
	pos = addRailConnection(gv, from, RailMoves, pos);
	free(visited);

	return pos;
}

// Add Road Connection to the Array
static
void addRoadConnection(PlaceId p, int pos)
{
	connectionArr[pos] = p;
}

// Add Boat Connection to the Array
static
void addBoatConnection(PlaceId p, int pos)
{
	connectionArr[pos] = p;
}


PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{
	assert(gv != NULL);

	return GvGetReachableByType(gv, player, round, from, true, true, true, numReturnedLocs);
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	assert(gv != NULL);

	connectionArr = malloc(sizeof(PlaceId) * MAX_CONNECTION);
	int i; int j = 1;	
	for(i = 0; i < MAX_CONNECTION; i++)
		connectionArr[i] = NOWHERE;

	connectionArr[0] = from;
	ConnList c = MapGetConnections(gv->graph, connectionArr[0]);
	// If Player is PLAYER_DRACULA
	if(player == PLAYER_DRACULA) {
		while(c != NULL) {
			if(c->type == RAIL) {
				c = c->next;
				continue;
			}
			if(c->p == ST_JOSEPH_AND_ST_MARY) {
				c = c->next;
				continue;
			}
			if(road) {
				addRoadConnection(c->p, j);
				j++;		
			}

			else if(boat) {
				addBoatConnection(c->p, j);
				j++;	
			}
			c = c->next;
		}
	}
	// If Player is HUNTER
	else {
		int RailMoves = distanceRail(round, player); 
		while(c != NULL) {
			
			if(c->type == RAIL && rail) {
				if(RailMoves > 0) {
					j = addRailConnection_wrapper(gv, c->p, RailMoves, j);
				}
				else {
				c = c->next;
				continue;
				}
			}
			
			else if(!InArray(c->p)){
				if(boat) {
					addBoatConnection(c->p, j);
					j++;
				}
				else if(road) {
					addRoadConnection(c->p, j);
					j++;
				}
			}
			c = c->next;
		}
	}

	*numReturnedLocs = j;
	return connectionArr;
}

void white_box() {
	
	char *pastPlays =	         "GGE.... SGE.... HGE.... MGE.... DCD.V.. "
        "GGE.... SGE.... HGE.... MGE....";
    
	//printf("%d %d %d %d %d %d\n",placeAbbrevToId("MN"), placeAbbrevToId("PL"), placeAbbrevToId("AM"), placeAbbrevToId("PA"), placeAbbrevToId("CD"), placeAbbrevToId("LV"));
	GameView gv = GvNew(pastPlays, NULL);

	int *numReturnedLocs = malloc(sizeof(int));
	GvGetReachable(gv, 0, 0,BUCHAREST, numReturnedLocs);
	printf("%d\n", *numReturnedLocs);
	printf("%s\n", placeIdToName(connectionArr[3]));	
	printf("%s\n", placeIdToName(connectionArr[4]));
	printf("%s\n", placeIdToName(connectionArr[5]));

}


