#include <stdio.h>
#include <stdlib.h>

typedef enum placeId {
	// real places
	ADRIATIC_SEA,
	ALICANTE,
	AMSTERDAM,
} placeId;


typedef struct node *Link;
struct node {
	placeId *place;
} Node;

typedef struct gameview *GameView;
struct gameview {
	struct node info[10]
};


int main(void)
{
	//placeId *place = malloc(100*sizeof(*place));
	//place[0] = ALICANTE;
	//place[1] = AMSTERDAM;
	//printf("%d %d", place[0], place[1]);

	GameView gv = malloc(sizeof(*gv));
	//gv->info = malloc(5 * sizeof(int));
	//Link l = malloc(sizeof(Link));
	//l->place = malloc(100*sizeof(placeId *));
	//l->place[0] = 	AMSTERDAM;
	gv->info[0].place = malloc(100*sizeof(placeId *));
	gv->info[0].place[0] = ADRIATIC_SEA;
	
	//placeId p = ALICANTE;
	//if(p == 1)
		//printf("YES");
	placeId **g;
	g = malloc(sizeof(placeId *)*4);
	for(int i = 0; i < 4; i++) {
		g[i] = malloc(sizeof(placeId) * 10);
	}
	
	g[0][1] = ADRIATIC_SEA;
	printf("%d\n", g[0][1]);

	//Link s = malloc(sizeof(struct node));
	//s->info = malloc(5 * sizeof(Exam));	
	//s->info[0]->d = 1; 
	//printf("%d", gv->info[0].place[0]);
	return 0;
}
