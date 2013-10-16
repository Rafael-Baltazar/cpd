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

//functions for generation computations
//checks all black checks for WOLFS or SQUIRRELS. If it finds any of those, it simulates their behavior
void compute_black_cells(){
	int i, j;
	for(i=0;i<MAX;i+=2){
		for(j=0;j<MAX;j+=2) {
			if(world[i][j+1].type == WOLF)
				compute_wolf(i,j+1);
			else if(world[i][j+1].type == SQUIRREL)
				compute_squirrel(i,j+1);
			if(world[i+1][j].type== WOLF)
				compute_wolf(i+1,j);
			else if(world[i+1][j].type== SQUIRREL)
				compute_squirrel(i+1,j);
		}
	}
}

void compute_red_cells(){
	int i, j;
	for(i=0;i<MAX;i+=2){
		for(j=0;j<MAX;j+=2) {
			if(world[i][j].type == WOLF)
				compute_wolf(i,j);
			else if(world[i][j].type == SQUIRREL)
				compute_squirrel(i,j);
			if(world[i+1][j+1].type== WOLF)
				compute_wolf(i+1,j+1);
			else if(world[i+1][j+1].type== SQUIRREL)
				compute_wolf(i+1,j+1);
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
