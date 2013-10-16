/*
 * Wolves and squirrels
 *
 * Serial implementation
 */

#include <stdio.h>

#define MAX 20	

//Types (Defined as binnary masks to make the comparations easier to handle)
// Empty is 0

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
				;//update_wolf(i,j);
			else if(world[i][j].type & SQUIRREL)
				;//update_squirrel(i,j);
			
			color = (i+1 + color) % 2;
		}
	}
}

//functions for debug
//prints the world
void print_all_cells(){
	int i, j;
	char type;
	struct world cell;

	for(i = 0; i < MAX; i++) {
		for(j = 0; j < MAX; j++) {
			cell = world[i][j];

			if(cell.type) {
				if(cell.type & WOLF) {
					type = 'w';
				}
				else if(cell.type & SQUIRREL) {
				//Squirrel and a tree
					if(cell.type & TREE) {
						type = '$';
					}
					else {
						type = 's';
					}
				}
				else if(cell.type & ICE) {
					type = 'i';
				}
				else {
					type = 't';
				}

				printf("%d %d %c\n", i, j, type);
			}
			
		}
	}
}


void populate_world_from_file(char file_name[]){

	FILE *fp;
	fp = fopen(file_name,"r");
	if(fp == NULL) {
		printf("Error while opening the file.\n");
	} else {
		printf("File found. Now polulating...\n");
		int i, j, max_size;
		char a;
		while(fscanf(fp, "%d %d %c", &i, &j, &a) != EOF) {
			if(a=='w')
				world[j][i].type = WOLF;
			else if(a=='s')
				world[j][i].type = SQUIRREL;
			else if(a=='i')
				world[j][i].type = ICE;
			else if(a=='t')
				world[j][i].type = TREE;
		}
	}
}



 

int main(int argc, char **argv) {
	//Test world print

	world[0][0].type = WOLF;
	world[1][2].type = SQUIRREL | TREE;
	world[2][2].type = ICE;
	world[3][1].type = TREE;
	world[1][0].type = WOLF | SQUIRREL;	
	print_all_cells(world);

	int w_breeding_p, s_breeding_p, w_starvation_p, num_gen;
	if(argc>1) {
		populate_world_from_file(argv[1]);
		w_breeding_p = atoi(argv[2]);
		s_breeding_p = atoi(argv[3]);
		w_starvation_p = atoi(argv[4]);
		num_gen = atoi(argv[5]);	
	}
	print_all_cells();

	return 0;	
}
