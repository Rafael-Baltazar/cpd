/*
 * Wolves and squirrels
 *
 * Serial implementation
 */

#include <stdio.h>

#define MAX 20	

//Types (Defined as binnary masks to make the comparations easier to handle)
#define WOLF 1			//0001
#define SQUIRREL 2 		//0010
#define ICE 4			//0100
#define TREE 8			//1000

#define RED 0
#define BLACK 1

/*
 * World structure
 */

struct world {
	int type; /* Wolf, Squirrel, Ice, Tree (Empty is 0) */
 	int breeding_period;
 	int starvation_period;
 } world[MAX][MAX];
 
 
//functions for animal behavior
void compute_wolf(int row, int col){
	//TODO: wolf behavior
	printf("There's a wolf at: %d x %d!\n", row, col);
}

void compute_squirrel(int row, int col){
	//TODO: squirrel behavior
	printf("There's a squirrel at: %d x %d!\n", row, col);
}

/*
 * Updates only the wolf and squirrels that belong to a specific subgeneration.
 * color: the color of the subgeneration to be updated.
 */
void iterate_subgeneration(int color) {
	int i, j;
	
	for(i = 0; i < MAX; i++) {
		for(j = color; j < MAX; j += 2) {
			if(world[i][j].type & WOLF)
				//update_wolf(i,j);
			else if(world[i][j].type & SQUIRREL)
				//update_squirrel(i,j);
			
			color = (i+1 + color) % 2;
		}
	}
}

//functions for debug
//prints the world
void print_all_cells(){
	int i, j;
	for(i=0;i<MAX;++i) {
		for(j=0;j<MAX;++j) {
			printf("%d", world[i][j].type);
		}
		printf("\n");
	}
}

 

int main(int argc, char **argv) {

	return 0;	
}
