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
#define SQUIRRELnTREE  (SQUIRREL | TREE)

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#define RED 0
#define BLACK 1

#define N_COLORS 2
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
	int current_subgeneration;
 } **world, **world2;

 
 /*
  * TODO: COMMENTS
  */
void clean_matrix(struct world** matrix) {
  int i;		
  int size = max_size*sizeof(struct world);
  for(i=0;i<max_size;++i){
    memset((void*)matrix[i], 0, size);
  }
}
 /*
  * TODO: COMMENTS
  */
void swap_matrix(){
  struct world** aux;
  aux = world2;
  world2 = world;
  world = aux;  
}

void copy_matrix(struct world** matrix0, struct world** matrix1 ){
  int i, size;
  size = max_size*sizeof(struct world);
  for(i=0;i<max_size;++i){
    memcpy(matrix1[i], matrix0[i], size);
  }
   
  
}


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
	int found = 0;
	/*Has up adjacent cell?*/
	if(row > 0) {
		adjacents[found++] = cell_number(row - 1, col);
	}

	/*Has right adjacent cell?*/
	if(col < max_size - 1) {
		adjacents[found++] = cell_number(row, col + 1);
	}

	/*Has down adjacent cell?*/
	if(row < max_size - 1) {
		adjacents[found++] = cell_number(row + 1, col);
	}

	/*Has left adjacent cell?*/
	if(col > 0) {
		adjacents[found++] = cell_number(row, col - 1);
	}

	return found;
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
 * check_breeding_period: Checks if an animal is going to breed
 */
int check_breeding_period(int row, int col){
	struct world *animal = &world2[row][col];
	int saved_type = animal->type;
	if(animal->breeding_period) {
		if(saved_type == SQUIRRELnTREE)
			animal->type = TREE;
		else
			animal->type = EMPTY;
		return 0;
	} else {
		/*load the breeding and starvation values into the new animal*/
		if(saved_type & SQUIRREL || saved_type == SQUIRRELnTREE)
			animal->breeding_period = s_breeding_p;
		else if(saved_type & WOLF) {
			animal->breeding_period = w_breeding_p;
			animal->starvation_period = w_starvation_p;
		}
		return 1; 
	}
}


/* TODO: STARVATION
 * move_to: Move an animal in position (src_row, src_col)
 * 	to cell number dest_c
 */
void move_to(int src_row, int src_col, int dest_c) {
	int dest_row, dest_col, saved_type;
	struct world *animal = &world[src_row][src_col];
	struct world *dest_cell;
	get_world_coordinates(dest_c, &dest_row, &dest_col);
	dest_cell = &world2[dest_row][dest_col];
	/*update the new values for the breeding and starvation*/
	saved_type = dest_cell->type;
	*dest_cell = *animal;
	if((saved_type & TREE) && ((animal->type & SQUIRREL) || (animal->type == SQUIRRELnTREE))) {
		/*squirrel entering a tree*/
		dest_cell->type = SQUIRRELnTREE;
	}
	else if((!saved_type) && (animal->type == SQUIRRELnTREE)) {
		/*squirrel exiting a tree*/
		dest_cell->type = SQUIRREL;
	}
	saved_type=dest_cell->type;
	if(check_breeding_period(src_row, src_col)) {
		/*reset breeding period on the mother*/
		if(saved_type & WOLF)
			dest_cell->breeding_period = w_breeding_p;
		else if(saved_type & SQUIRREL || saved_type == SQUIRRELnTREE) {
			dest_cell->breeding_period = s_breeding_p;
		}
	}
}
/*
 * get_cells_with_squirrels: Return the number of the cells
 * in possibilities with squirres
 *	squirriles: Array with cell numbers of cells that have squirrels
 */
int get_cells_with_squirrels(int *possibilities, int n_possibilities, int *squirrels) {
	int i;
	int found = 0;
	struct world cell;
	int row, col;

	for (i = 0; i < n_possibilities; ++i)
	{
		get_world_coordinates(possibilities[i], &row, &col);
		cell = world[row][col];
		if(cell.type & WOLF) {
			squirrels[found] = cell_number(row, col);
			found++;
		}
	}

	return found;
}
/*
 * get_empty_cells: Return the number of the cells
 * in possibilities that are empty
 *	empty_cells: Array with cell numbers of cells that are empty
 */
int get_empty_cells(int *possibilities, int n_possibilities, int *empty_cells) {
	int i;
	int found = 0;
	struct world cell;
	int row, col;

	for (i = 0; i < n_possibilities; ++i) {
		get_world_coordinates(possibilities[i], &row, &col);
		cell = world[row][col];
		if(!cell.type) {
			empty_cells[found] = cell_number(row, col);
			found++;
		}
	}

	return found;
}

/*
 * get_walkable_cells: Return the number of the cells
 * in possibilities that are empty or with trees (for squirrel movement)
 *	walkable_cells: Array with cell numbers of cells that are 'walk-able'
 */

int get_walkable_cells(int *possibilities, int n_possibilities, int *walkable_cells){
	int i;
	int found = 0;
	struct world cell;
	int row, col;

	for (i = 0; i < n_possibilities; ++i) {
		get_world_coordinates(possibilities[i], &row, &col);
		cell = world[row][col];
		if((!cell.type) | (cell.type & TREE)) {
			walkable_cells[found++] = cell_number(row, col);
		}
	}
	return found;
}

void eat_squirrel(int wolf_row, int wolf_col, int squirrel_cell) {
	int row, col;
	struct world *cell;
	get_world_coordinates(squirrel_cell, &row, &col);
	cell = &world[row][col];
	cell->type = EMPTY;
	cell->starvation_period = 0;
	cell->breeding_period = 0;
	world[wolf_row][wolf_col].starvation_period = w_starvation_p;
}
/*
 * Update rules for animals in the world
 */
 void update_squirrel(int row, int col){
	int possibilities[N_ADJACENTS];
	int may_move[N_ADJACENTS];
	int n_possibilities, n_moves;
	int chosen;
	
	n_possibilities = get_adjacents(row, col, possibilities);
	if(n_possibilities) {
		n_moves = get_walkable_cells(possibilities, n_possibilities, may_move);
		chosen = choose_position(row, col, n_moves);
		move_to(row, col, may_move[chosen]);
	}	
 }
 
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
			/*At least one squirrel has been found
				Choose one of them*/
			choosed = choose_position(row, col, n_squirrels);
			/* Eat the squirrel */
			eat_squirrel(row, col, may_move[choosed]);
			/*Move to that position*/
			move_to(row, col, may_move[choosed]);
		}

		else {
			/*No squirrels
				Let's check if there's any empty cell*/
			n_empty = get_empty_cells(possibilities, n_possibilities, may_move);
			if(n_empty) {
				choosed = choose_position(row, col, n_empty);
				/*Move to that position*/
				move_to(row, col, may_move[choosed]);
			}
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

/*prints the world for debug*/
void print_for_debug(){
	int i, j;
	for(i = 0; i < max_size; i++) {
		for(j = 0; j < max_size; j++) {
			printf("%d", world[i][j].type);
		}
		printf("\n");
	}
	printf("\n");
}

void populate_world_from_file(char file_name[]) {
	FILE *fp;
	int i, j, k, size;
	char a;
	fp = fopen(file_name,"r");

	if(fp == NULL) {
		printf("Error while opening the file.\n");
	} else {
		fscanf(fp, "%d", &max_size);
		world = (struct world**) malloc(max_size*sizeof(*world));
		world2 = (struct world**) malloc(max_size*sizeof(*world));
		for(k=0;k<max_size;++k) {
			size = max_size*sizeof(struct world);
			world[k] = (struct world*) malloc(size);
			memset((void*)world[k], 0, size);
			world2[k] = (struct world*) malloc(size);
			memset((void*)world2[k], 0, size);
		}
		/*populating*/
		while(fscanf(fp, "%d %d %c", &i, &j, &a) != EOF) {
			if(a=='w') {
				world[i][j].type = WOLF;
				world[i][j].starvation_period = w_starvation_p;
				world[i][j].breeding_period = w_breeding_p;
			}
			else if(a=='s') {
				world[i][j].type = SQUIRREL;
				world[i][j].breeding_period = s_breeding_p;
			}
			else if(a=='i')
				world[i][j].type = ICE;
			else if(a=='t')
				world[i][j].type = TREE;
			else if(a=='$') {
				world[i][j].type = SQUIRRELnTREE;
				world[i][j].breeding_period = s_breeding_p;
			}
			else 
				world[i][j].type = EMPTY;	
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
				if(!color) {
					world[i][j].current_subgeneration = color;
				}
				if(!world[i][j].current_subgeneration || world[i][j].current_subgeneration != color) {
					world[i][j].current_subgeneration = (color + 1) % N_COLORS;
					world[i][j].breeding_period--;
				}
			else if( (world[i][j].type & SQUIRREL) || (world[i][j].type == SQUIRRELnTREE) ) {
				update_squirrel(i,j);
			}
			color = (i+1 + color) % 2;
		}	
	}
}


/* FIXME: the new functions aren't working 
 * process_generations: Process all the generations
 */
void process_generations() {
	int i, color;
	for (i = 0; i < num_gen; ++i) {
		copy_matrix(world, world2);
		for(color = 0; color < N_COLORS; color++) {
			iterate_subgeneration(color);
			swap_matrix();
		}
		swap_matrix();
		clean_matrix(world2);
	}
}

int main(int argc, char **argv) {
	if(argc == N_ARGS) {
		w_breeding_p = atoi(argv[2]);
		s_breeding_p = atoi(argv[3]);
		w_starvation_p = atoi(argv[4]);		
		num_gen = atoi(argv[5]);
		populate_world_from_file(argv[1]);
		process_generations();
		print_all_cells();
		
		/*
		printf("vals: %d\n", world[1][1].breeding_period);
		copy_matrix(world, world2);
		move_to(1,1, cell_number(0,0));
		printf("Antiga:\n");
		print_for_debug();
		swap_matrix();
		printf("Nova:\n");
		print_for_debug();
		clean_matrix(world2);
		world[0][0].breeding_period-=1;
		
		
		
		copy_matrix(world, world2);
		move_to(0,0, cell_number(0,1));
		swap_matrix();
		print_for_debug();
		clean_matrix(world2);
		world[0][1].breeding_period-=1;
		
		copy_matrix(world, world2);
		move_to(0,1, cell_number(0,2));
		swap_matrix();
		printf("Nova:\n");
		print_for_debug();
		clean_matrix(world2);
		world[0][2].breeding_period-=1;
		
		copy_matrix(world, world2);
		move_to(0,2, cell_number(0,3));
		swap_matrix();
		printf("Nova:\n");
		print_for_debug();
		clean_matrix(world2);
		world[0][2].breeding_period-=1;*/
		
		
	}
	else {
		printf("Usage: wolves-squirrels-serial <input file name> <wolf_breeding_period> ");
		printf("<squirrel_breeding_period> <wolf_startvation_period> <# of generations>\n");
	}
	return 0;	
}
