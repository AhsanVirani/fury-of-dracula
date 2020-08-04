////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// hunter.c: your "Fury of Dracula" hunter AI.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include "Game.h"
#include "hunter.h"
#include "HunterView.h"
#include <stdio.h>

void decideHunterMove(HunterView hv)
{
	// Hunter walks around in a circle
    // Pending hunterview to be fixed to implement AI

    int player = HvGetPlayer(hv);
    int round = HvGetRound(hv);
    PlaceId currentLocation = HvGetPlayerLocation(hv, player);

    if (round == 0) {
        registerBestPlay("BD", "None");
        return;
    }

    switch (currentLocation) {
        case VIENNA:
            registerBestPlay("BD", "None");
        break;
        case BUDAPEST:
            registerBestPlay("SZ", "None");
        break;
        case SZEGED:
            registerBestPlay("KL", "None");
        break;
        case KLAUSENBURG:
            registerBestPlay("CD", "None");
        break;
        case CASTLE_DRACULA:
            registerBestPlay("GA", "None");
        break;
        case GALATZ:
            registerBestPlay("CN", "None");
        break;
        case CONSTANTA:
            registerBestPlay("VR", "None");
        break;
        case VARNA:
            registerBestPlay("BS", "None");
        break;
        case BLACK_SEA:
            registerBestPlay("IO", "None");
        break;
        case IONIAN_SEA:
            registerBestPlay("VA", "None");
        break;
        case VALONA:
            registerBestPlay("SJ", "None");
        break;
        case SARAJEVO:
            registerBestPlay("ZA", "None");
        break;
        case ST_JOSEPH_AND_ST_MARY: //////////////////// IF DEAD
            registerBestPlay("ZA", "None");
        break;
        case ZAGREB:
            registerBestPlay("MU", "None");
        break;
        case MUNICH:
            registerBestPlay("VE", "None");
        break;
        case VENICE:
            registerBestPlay("FL", "None");
        break;
        case FLORENCE:
            registerBestPlay("RO", "None");
        break;
        case ROME:
            registerBestPlay("NP", "None");
        break;
        case NAPLES:
            registerBestPlay("TS", "None");
        break;
        case TYRRHENIAN_SEA:
            registerBestPlay("MS", "None");
        break;
        case MEDITERRANEAN_SEA:
            registerBestPlay("MR", "None");
        break;
        case MARSEILLES:
            registerBestPlay("CF", "None");
        break;
        case CLERMONT_FERRAND:
            registerBestPlay("TO", "None");
        break;
        case TOULOUSE:
            registerBestPlay("SR", "None");
        break;
        case SARAGOSSA:
            registerBestPlay("MA", "None");
        break;
        case MADRID:
            registerBestPlay("SN", "None");
        break;
        case SANTANDER:
            registerBestPlay("BB", "None");
        break;
        case BAY_OF_BISCAY:
            registerBestPlay("AO", "None");
        break;
        case ATLANTIC_OCEAN:
            registerBestPlay("IR", "None");
        break;
        case IRISH_SEA:
            registerBestPlay("SW", "None");
        break;
        case SWANSEA:
            registerBestPlay("LV", "None");
        break;
        case LIVERPOOL:
            registerBestPlay("MN", "None");
        break;
        case MANCHESTER:
            registerBestPlay("ED", "None");
        break;
        case EDINBURGH:
            registerBestPlay("NS", "None");
        break;
        case NORTH_SEA:
            registerBestPlay("AM", "None");
        break;
        case AMSTERDAM:
            registerBestPlay("CO", "None");
        break;
        case COLOGNE:
            registerBestPlay("HA", "None");
        break;
        case HAMBURG:
            registerBestPlay("BR", "None");
        break;
        case BERLIN:
            registerBestPlay("PR", "None");
        break;
        case PRAGUE:
            registerBestPlay("VI", "None");
        break;
        default:
            registerBestPlay(placeIdToAbbrev(currentLocation), "None");
        break;
    }
}
