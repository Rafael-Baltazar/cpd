/*
 * Wolves and squirrels
 *
 * Serial implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>	

/*Types (Defined as binnary masks to make the comparations easier to handle) */
/* Empty is 0 */

#define EMPTY 0			/*0000*/
#define WOLF 1			/*0001*/
#define SQUIRREL 2 		/*0010*/
#define ICE 4			/*0100*/
#define TREE 8			/*1000*/

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#define RED 0
#define BLACK 1
/*
 * VERY DIRTY HACK, GLOBAL VARIABLES, RUN
 */
 /*size of the world*/
 int max_size;

 int w_breeding_p, s_breeding_p, w_starvation_p, num_gen;

#define N_ADJACENTS	4

 /* Main arguments */
#define N_ARGS 6

/*
 * World structure
 */
struct world {
	int type; /* Wolf, Squirrel, Ice, Tree (Empty is 0) */
 	int breeding_period;
 	int starvation_period;
 } **world;
 
/*
 * cell_number: Return the cell number of a given
 *	matrix position (row, col)
 */ 
inline int cell_number(int row, int col) {
	return row * max_size + col;
}

/*
 * choose_position: Apply the rule to choose a position
 *	even when we have more than one to choose
 */
inline int choose_position(int row, int col, int p) {
	int c = cell_number(row, col);
	return  c % p;
 }

/*
 * get_adjacents: Return the number possible adjacent positions
 *	of a given position (row, col)
 *	adjacents: array with the cell numbers of all
 *		adjacent positions
 */
int get_adjacents(int row, int col, int *adjacents) {
	int i = 0;
	int founded = 0;
	/*Has up adjacent cell?*/
	if(row > 0) {
		adjacents[i++] = cell_number(row - 1, col);
		founded++;
	}

	/*Has right adjacent cell?*/
	if(col < max_size - 1) {
		adjacents[i++] = cell_number(row, col + 1);
		founded++;
	}

	/*Has down adjacent cell?*/
	if(row < max_size - 1) {
		adjacents[i++] = cell_number(row + 1, col);
		founded++;
	}

	/*Has left adjacent cell?*/
	if(col > 0) {
		adjacents[i++] = cell_number(row, col - 1);
		founded++;
	}

	return founded;
}

/*
 * get_world_coordinates: Return in row and col the right
 *	coordinates of the matric of a given cell number
 */
void get_world_coordinates(int cell_number, int *row, int *col) {
	*col = cell_number % max_size;
	*row = (cell_number - *col) / max_size;
}

 
/*
 * move_to: Move an animal in position (src_row, src_col)
 * 	to cell number dest_c
 */
void move_to(int src_row, int src_col, int dest_c) {
	/* TODO: Implement this... */
}

/*
 * functions for animal behavior
 */
/*void compute_wolf(int row, int col){
	int pos[4];
	int p = 0;
	if((row>0) && (world[row-1][col].type==EMPTY)) {
		pos[p++] = UP;
	}
	TODO: REST
	if(col<max_size)
		world[row][col+1].type = EMPTY;
	if(row<max_size)
		world[row+1][col].type = EMPTY;
	if(col>0)
		world[row][col-1].type = EMPTY;
	
	
	printf("There's a wolf at: %d x %d!\n", row, col);
}*/

void compute_squirrel(int row, int col){
	/*TODO: squirrel behavior*/
	printf("There's a squirrel at: %d x %d!\n", row, col);
}

/*
 * get_cells_with_squirrels: Return the number of the cells
 * in possibilities with squirres
 *	squirriles: Array with cell numbers of cells that have squirrels
 */
int get_cells_with_squirrels(int *possibilities, int n_possibilities, int *squirrels) {
	int i;
	int founded = 0;
	struct world cell;
	int row, col;

	for (i = 0; i < n_possibilities; ++i)
	{
		get_world_coordinates(possibilities[i], &row, &col);
		cell = world[row][col];
		if(cell.type & WOLF) {
			squirrels[founded] = cell_number(row, col);
			founded++;
		}
	}

	return founded;
}

/*
 * get_empty_cells: Return the number of the cells
 * in possibilities that are empty
 *	empty_cells: Array with cell numbers of cells that are empty
 */
int get_empty_cells(int *possibilities, int n_possibilities, int *empty_cells) {
	int i;
	int founded = 0;
	struct world cell;
	int row, col;

	for (i = 0; i < n_possibilities; ++i) {
		get_world_coordinates(possibilities[i], &row, &col);
		cell = world[row][col];
		if(!cell.type) {
			empty_cells[founded] = cell_number(row, col);
			founded++;
		}
	}

	return founded;
}

/*
 * Update rules for animals in the world
 */
void update_wolf(int row, int col) {
	int possibilities[N_ADJACENTS];
	int may_move[N_ADJACENTS];
	int n_possibilities, n_squirrels, n_empty;
	int choosed;

	n_possibilities = get_adjacents(row, col, possibilities);

	/*If has adjacents to choose*/
	if(n_possibilities) {
		/*Check for squirrels*/
		n_squirrels = get_cells_with_squirrels(possibilities, n_possibilities, may_move);
		if(n_squirrels) {
			/*At least one squirrel as been founded
				Choose one of them*/
			choosed = choose_position(row, col, n_squirrels);
			/*Move to that position*/
			move_to(row, col, choosed);
		}

		else {
			/*No squirrels
				Let's check if there's any empty cell*/
			n_empty = get_empty_cells(possibilities, n_possibilities, may_move);
			if(n_empty) {
				choosed = choose_position(row, col, n_empty);
				/*Move to that position*/
				move_to(row, col, choosed);
			}
		}
	}
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
				update_wolf(i,j);
			else if(world[i][j].type & SQUIRREL)
				;/*update_squirrel(i,j);*/
			
			color = (i+1 + color) % 2;
		}
	}
}

/*prints the world*/
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
				/*Squirrel and a tree*/
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

void populate_world_from_file(char file_name[]) {
	FILE *fp;
	int i, j, k, size;
	char a;
	fp = fopen(file_name,"r");

	if(fp == NULL) {
		printf("Error while opening the file.\n");
	} else {
		/*printf("File found. Now populating...\n");*/
		fscanf(fp, "%d", &max_size);
		world = (struct world**) malloc(max_size*sizeof(*world));
		for(k=0;k<max_size;++k) {
			size = max_size*sizeof(struct world);
			world[k] = (struct world*) malloc(size);
			memset((void*)world[k], 0, size);
		}
		/*world initialization*/
		/*for(i=0;i<max_size;++i) {
			for(j=0;j<max_size;++j){
				world[i][j].type = EMPTY;
			}
		}*/
		/*populating*/
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
				world[i][j].type = EMPTY;	
		}
	}
}

int main(int argc, char **argv) {
	if(argc == N_ARGS) {
		populate_world_from_file(argv[1]);
		w_breeding_p = atoi(argv[2]);
		s_breeding_p = atoi(argv[3]);
		w_starvation_p = atoi(argv[4]);
		num_gen = atoi(argv[5]);
		print_all_cells(world);	
	}
	else {
		printf("Usage: wolves-squirrels-serial <input file name> <wolf_breeding_period> ");
		printf("<squirrel_breeding_period> <wolf_startvation_period> <# of generations>\n");
	}
	return 0;	
}
