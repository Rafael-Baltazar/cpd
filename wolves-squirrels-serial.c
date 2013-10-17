/*
 * Wolves and squirrels
 *
 * Serial implementation
 */

#include <stdio.h>
#include <stdlib.h>	

//Types (Defined as binnary masks to make the comparations easier to handle)
// Empty is 0

#define WOLF 1			//0001
#define SQUIRREL 2 		//0010
#define ICE 4			//0100
#define TREE 8			//1000

#define RED 0
#define BLACK 1
/*
 * VERY DIRTY HACK, GLOBAL VARIABLES
 */
 //size of the world
 int max_size;

/*
 * World structure
 */
struct world {
	int type; /* Wolf, Squirrel, Ice, Tree (Empty is 0) */
 	int breeding_period;
 	int starvation_period;
 } **world;
 
 
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
	for(i = 0; i < max_size; i++) {
		for(j = color; j < max_size; j += 2) {
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

	for(i = 0; i < max_size; i++) {
		for(j = 0; j < max_size; j++) {
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
		printf("File found. Now populating...\n");
		int i, j, k;
		char a;
		fscanf(fp, "%d", &max_size);
		world = (struct world**) malloc(max_size*sizeof(*world));
		for(k=0;k<max_size;++k) {
			world[k] = (struct world*) malloc(max_size*sizeof(struct world));
		}
		for(i=0;i<max_size;++i) {
			for(j=0;j<max_size;++j){
				world[i][j].type = 0;
			}
		}
		
		while(fscanf(fp, "%d %d %c", &i, &j, &a) != EOF) {
			if(a=='w')
				world[i][j].type = WOLF;
			else if(a=='s')
				world[i][j].type = SQUIRREL;
			else if(a=='i')
				world[i][j].type = ICE;
			else if(a=='t')
				world[i][j].type = TREE;
			else if(a=='$')
				world[i][j].type = SQUIRREL | TREE;
			else 
				world[i][j].type = 0;	
		}
	}
}



 

int main(int argc, char **argv) {
	//Test world print


	int w_breeding_p, s_breeding_p, w_starvation_p, num_gen;
	if(argc>1) {
		populate_world_from_file(argv[1]);
		w_breeding_p = atoi(argv[2]);
		s_breeding_p = atoi(argv[3]);
		w_starvation_p = atoi(argv[4]);
		num_gen = atoi(argv[5]);	
	}
	print_all_cells(world);
	return 0;	
}
