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

    int round = HvGetRound(hv);
    int player = HvGetPlayer(hv);
    printf("round: %d\n", round);
    // Check if player is at ST_JOSEPH_AND_ST_MARY (killed) stay
    if (HvGetPlayerLocation(hv, player) == ST_JOSEPH_AND_ST_MARY){
        registerBestPlay("JM", "None");
        return;
    }

    PlaceId currentLocation = HvGetPlayerLocation(hv, player);

    switch (round % 39) {
        case 0:
            registerBestPlay("BD", "None");
        break;
        case 1:
            registerBestPlay("SZ", "None");
        break;
        case 2:
            registerBestPlay("KL", "None");
        break;
        case 3:
            registerBestPlay("CD", "None");
        break;
        case 4:
            registerBestPlay("GA", "None");
        break;
        case 5:
            registerBestPlay("CN", "None");
        break;
        case 6:
            registerBestPlay("VR", "None");
        break;
        case 7:
            registerBestPlay("BS", "None");
        break;
        case 8:
            registerBestPlay("IO", "None");
        break;
        case 9:
            registerBestPlay("VA", "None");
        break;
        case 10:
            registerBestPlay("SJ", "None");
        break;
        case 11:
            registerBestPlay("ZA", "None");
        break;
        case 12:
            registerBestPlay("MU", "None");
        break;
        case 13:
            registerBestPlay("VE", "None");
        break;
        case 14:
            registerBestPlay("FL", "None");
        break;
        case 15:
            registerBestPlay("RO", "None");
        break;
        case 16:
            registerBestPlay("NP", "None");
        break;
        case 17:
            registerBestPlay("TS", "None");
        break;
        case 18:
            registerBestPlay("MS", "None");
        break;
        case 19:
            registerBestPlay("MR", "None");
        break;
        case 20:
            registerBestPlay("CF", "None");
        break;
        case 21:
            registerBestPlay("TO", "None");
        break;
        case 22:
            registerBestPlay("SR", "None");
        break;
        case 23:
            registerBestPlay("MA", "None");
        break;
        case 24:
            registerBestPlay("SN", "None");
        break;
        case 25:
            registerBestPlay("BB", "None");
        break;
        case 26:
            registerBestPlay("AO", "None");
        break;
        case 27:
            registerBestPlay("IR", "None");
        break;
        case 28:
            registerBestPlay("SW", "None");
        break;
        case 29:
            registerBestPlay("LV", "None");
        break;
        case 30:
            registerBestPlay("MN", "None");
        break;
        case 31:
            registerBestPlay("ED", "None");
        break;
        case 32:
            registerBestPlay("NS", "None");
        break;
        case 33:
            registerBestPlay("AM", "None");
        break;
        case 34:
            registerBestPlay("CO", "None");
        break;
        case 35:
            registerBestPlay("HA", "None");
        break;
        case 36:
            registerBestPlay("BR", "None");
        break;
        case 37:
            registerBestPlay("PR", "None");
        break;
        case 38:
            registerBestPlay("VI", "None");
        break;
    }
}
