########################################################################
# COMP2521 20T2 // the Fury of Dracula // the View
# view/Makefile: build tests for GameView/HunterView/DraculaView
#
# You can modify this if you add additional files
#
# 2018-12-31	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
# 2020-07-10    v2.0    Team Dracula <cs2521@cse.unsw.edu.au>
#
########################################################################

CC = gcc
CFLAGS = -Wall -g
BINS = Testing testMap

all: $(BINS)

Testing: Testing.o GameView.o Map.o Places.o Queue.o
Testing.o: Testing.c GameView.h Map.h Places.h Game.h

testMap: testMap.o Map.o Places.o
testMap.o: testMap.c Map.h Places.h 

Places.o: Places.c Places.h Game.h
Map.o: Map.c Map.h Places.h Game.h
GameView.o:	GameView.c GameView.h Game.h

.PHONY: clean
clean:
	-rm -f ${BINS} *.o core
