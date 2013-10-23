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
#define N_WORLDS 2

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
 } **worlds[2]; 
 
 /*
  * TODO: COMMENTS
  */
/*void clean_matrix(struct world** matrix) {
  int i;		
  int size = max_size*sizeof(struct world);
  for(i=0;i<max_size;++i){
    memset((void*)matrix[i], 0, size);
  }
}*/
 /*
  * TODO: COMMENTS
  */
/*
void swap_matrix(){
  struct world** aux;
  aux = world2;
  world2 = world;
  world = aux;  
}*/

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

int get_cell_color(int row, int col) {
    /* Odd line: Start with a black cell */
    if(row % 2) {
        if(col % 2) {
            return RED;
        }
        else {
            return BLACK;
        }
    }

    /* Even line: Start with a red cell */
    else {
        if(col % 2) {
            return BLACK;
        }
        else {
            return RED;
        }
    }
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
inline void get_world_coordinates(int cell_number, int *row, int *col) {
	*col = cell_number % max_size;
	*row = (cell_number - *col) / max_size;
}

/* 
 * check_breeding_period: Checks if an animal is going to breed
 */
int check_breeding_period(int row, int col, struct world **world) {
	struct world *animal = &world[row][col];
	if(animal->breeding_period) {
		if(animal->type == SQUIRRELnTREE)
			animal->type = TREE;
		else
			animal->type = EMPTY;
		return 0;
	} else {
		animal->breeding_period = w_breeding_p;
		animal->starvation_period = w_starvation_p;
		return 1; 
	}
}

/*
 * Squirrel is eaten by wolf, squirrel/wolf with higher starvation period kills the other squirrel/wolf.
 */
void solve_conflict(struct world *src, struct world *dest) {
	if(src->type & SQUIRREL) {
		if(dest->type & WOLF)
			;/*eat_squirrel*/
		else {
			;
		}
	}
}

/* TODO: STARVATION
 * move_to: Move an animal in position (src_row, src_col)
 * 	to cell number dest_c
 */
void move_to(int src_row, int src_col, int dest_c, struct world **src, struct world **dest) {
	int dest_row, dest_col;
	struct world *animal = &src[src_row][src_col];
	struct world *dest_cell;
	get_world_coordinates(dest_c, &dest_row, &dest_col);
	dest_cell = &dest[dest_row][dest_col];

	if(dest_cell->type)
		solve_conflict(animal, dest_cell);

	if((dest_cell->type & TREE) && ((animal->type & SQUIRREL) || (animal->type == SQUIRRELnTREE))) {
		/*squirrel entering a tree*/
		*dest_cell = *animal;
		dest_cell->type = SQUIRRELnTREE;
	}
	else if((!dest_cell->type) && (animal->type == SQUIRRELnTREE)) {
		/*squirrel exiting a tree*/
		*dest_cell = *animal;
		dest_cell->type = SQUIRREL;
	}
	if(check_breeding_period(src_row, src_col, src)) {
		/*Reset breeding period */
		dest_cell->breeding_period = w_breeding_p;
	}

}
/*
 * get_cells_with_squirrels: Return the number of the cells
 * in possibilities with squirres
 *	squirriles: Array with cell numbers of cells that have squirrels
 */
int get_cells_with_squirrels(struct world **world, int *possibilities, int n_possibilities, int *squirrels) {
	int i;
	int found = 0;
	struct world cell;
	int row, col;

	for (i = 0; i < n_possibilities; ++i)
	{
		get_world_coordinates(possibilities[i], &row, &col);
		cell = world[row][col];
		if(cell.type & SQUIRREL) {
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
int get_empty_cells(struct world **world, int *possibilities, int n_possibilities, int *empty_cells) {
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
 * types_to_exclude: cells with one of these types aren't walkable
 */

int get_walkable_cells(struct world **world, int *possibilities, int n_possibilities, int *walkable_cells, int types_to_exclude){
	int i;
	int found = 0;
	struct world cell;
	int row, col;

	for (i = 0; i < n_possibilities; ++i) {
		get_world_coordinates(possibilities[i], &row, &col);
		cell = world[row][col];
		if(!(cell.type & types_to_exclude)) {
			walkable_cells[found++] = cell_number(row, col);
		}
	}
	return found;
}

/*
   eat_squirrel: Put empty content in the squirrel cell (in squirrel_world))
     and reset starvation period
   */
void eat_squirrel(struct world *wolf, struct world *squirrel) {
	squirrel->type = EMPTY;
	squirrel->starvation_period = 0;
	squirrel->breeding_period = 0;
	wolf->starvation_period = w_starvation_p;
}
/*
 * Update rules for animals in the world
 */
void update_squirrel(struct world **world, struct world **to_move, int row, int col){
	int possibilities[N_ADJACENTS];
	int may_move[N_ADJACENTS];
	int n_possibilities, n_moves;
	int chosen;
	
	n_possibilities = get_adjacents(row, col, possibilities);
	if(n_possibilities) {
		n_moves = get_walkable_cells(to_move, possibilities, n_possibilities, may_move, ICE);
		chosen = choose_position(row, col, n_moves);
		move_to(row, col, may_move[chosen], world, to_move);
	}	
 }

inline void kill_wolf(struct world *wolf) {
    memset(wolf, 0, sizeof(struct world));
}
 
void update_wolf(struct world **world, struct world **to_move, int row, int col) {
	int possibilities[N_ADJACENTS];
	int may_move[N_ADJACENTS];
	int n_possibilities, n_squirrels, n_empty;
	int chosen, s_row, s_col;
    struct world *wolf = &world[row][col];
	n_possibilities = get_adjacents(row, col, possibilities);

    if(!wolf->starvation_period) {
        kill_wolf(wolf);
        return;
    }
	/*If has adjacents to choose*/
	if(n_possibilities) {
		/*Check for squirrels*/
		n_squirrels = get_cells_with_squirrels(to_move, possibilities, n_possibilities, may_move);
		if(n_squirrels) {
			/*At least one squirrel has been found
				Choose one of them*/
			chosen = choose_position(row, col, n_squirrels);
			/* Eat the squirrel */
			get_world_coordinates(may_move[chosen], &s_row, &s_col);
			eat_squirrel(wolf, &to_move[s_row][s_col]);
			
			/*Move to that position*/
			move_to(row, col, may_move[chosen], world, to_move);
		}

		else {
			/*No squirrels
				Let's check if there's any empty cell*/
			n_empty = get_walkable_cells(to_move, possibilities, n_possibilities, may_move, ICE | TREE);
			if(n_empty) {
				chosen = choose_position(row, col, n_empty);
				/*Move to that position*/
				move_to(row, col, may_move[chosen], world, to_move);
			}
		}
	}
}

void update_periods() {
    int i, j, color;
    struct world *cell;

    for(i = 0; i < max_size; i++) {
        for(j = 0; j < max_size; j++) {
            color = get_cell_color(i, j);
            cell = &worlds[color][i][j];
            if(cell->type & WOLF) {
                cell->breeding_period--;
                cell->starvation_period--;
            }
            else if(cell->type & SQUIRREL) {
                cell->breeding_period--;
            }
        }
    }
}

/*
 * Updates only the wolf and squirrels that belong to a specific subgeneration.
 * color: the color of the subgeneration to be updated.
 */
void iterate_subgeneration(int color) {
	int i, j, start_col;
    struct world **current = worlds[color];
    struct world **to_move = worlds[(color + 1) % N_WORLDS];

	for(i = 0; i < max_size; i++) {
        if(get_cell_color(i, 0) == color) {
            start_col = 0;
        }
        else {
            start_col = 1;
        }
    
		for(j = start_col; j < max_size; j += 2) {
            if(current[i][j].type & WOLF) {
				update_wolf(current, to_move, i,j);
            }
			else if(current[i][j].type & SQUIRREL) {
                update_squirrel(current, to_move, i,j);
			}
			
			/*color = (i+1 + color) % 2;*/
		}
	}
}

/*prints the world*/
void print_all_cells(){
	int i, j, color;
	char type;
	struct world cell;

	for(i = 0; i < max_size; i++) {
		for(j = 0; j < max_size; j++) {
			color = get_cell_color(i, j);
            cell = worlds[color][i][j];

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
void print_for_debug(struct world **world){
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
	int i, j, k, size, row_size, color;
	char a;
   /* struct world ***right_world; */
	fp = fopen(file_name,"r");

	if(fp == NULL) {
		printf("Error while opening the file.\n");
	} else {
		fscanf(fp, "%d", &max_size);
        row_size = max_size*sizeof(struct world*);
		worlds[0] = (struct world**) malloc(row_size);
		worlds[1] = (struct world**) malloc(row_size);
		for(k=0;k<max_size;++k) {
			size = max_size*sizeof(struct world);
			worlds[0][k] = (struct world*) malloc(size);
			memset((void*)worlds[0][k], 0, size);
			worlds[1][k] = (struct world*) malloc(size);
			memset((void*)worlds[1][k], 0, size);
		}

		/*populating*/
		while(fscanf(fp, "%d %d %c", &i, &j, &a) != EOF) {
            color = get_cell_color(i, j);
			if(a=='w') {
				worlds[color][i][j].type = WOLF;
				worlds[color][i][j].starvation_period = w_starvation_p;
                worlds[color][i][j].breeding_period = w_breeding_p;
			}
			else if(a=='s') {
				worlds[color][i][j].type = SQUIRREL;
                worlds[color][i][j].breeding_period = w_breeding_p;
            }
                
			else if(a=='i')
				worlds[color][i][j].type = ICE;
			else if(a=='t')
				worlds[color][i][j].type = TREE;
			else if(a=='$') {
				worlds[color][i][j].type = SQUIRRELnTREE;
                worlds[color][i][j].breeding_period = w_breeding_p;
            }
			else 
				worlds[color][i][j].type = EMPTY;	
		}
	}
}

/*
 * process_generations: Process all the generations
 */
void process_generations() {
	int i, color;
	for (i = 0; i < num_gen; ++i) {
		for(color = 0; color < N_COLORS; color++) {
			iterate_subgeneration(color);
			print_all_cells();
			printf("----\n");
		}
        update_periods();
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
		/*process_generations();
		print_all_cells();*/
		/*print_for_debug();
		printf("-----------\n");
		process_generations();
		move_to(1,1, cell_number(0,0));
		print_for_debug();
		move_to(0,0, cell_number(2,2));
		print_for_debug();*/
		/*copy_matrix(world, world2);
		move_to(1,1, cell_number(0,0));
		printf("Antiga:\n");
		print_for_debug();
		swap_matrix();
		printf("Nova:\n");
		print_for_debug();
		clean_matrix(world2);
		
		
		copy_matrix(world, world2);
		move_to(0,0, cell_number(1,1));
		printf("Antiga:\n");
		print_for_debug();
		swap_matrix();
		printf("Nova:\n");
		print_for_debug();
		clean_matrix(world2);*/
		
		
		
		
	}
	else {
		printf("Usage: wolves-squirrels-serial <input file name> <wolf_breeding_period> ");
		printf("<squirrel_breeding_period> <wolf_startvation_period> <# of generations>\n");
	}
	return 0;	
}
