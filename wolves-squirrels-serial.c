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
	int ate_squirrel;
	int breed;
 } **worlds[N_COLORS]; 


void swap_matrix(){
  struct world** aux;
  aux = worlds[1];
  worlds[1] = worlds[0];
  worlds[0] = aux;  
}

inline void copy_matrix(struct world **src, struct world **dst) {
	memcpy(dst[0], src[0], sizeof(struct world) * max_size * max_size); 
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
 * move_to: Move an animal in position (src_row, src_col)
 * 	to cell number dest_ci. The animal breeds if it's breeding
 * period is 0
 */
void move_to(int src_row, int src_col, int dest_c, struct world **read_matrix, struct world **write_matrix) {
	int dest_row, dest_col, new_breed_flag, new_ate_squirrel;
	struct world *read_src_cell = &read_matrix[src_row][src_col];	
	struct world *read_dst_cell;
	struct world *write_src_cell = &write_matrix[src_row][src_col];
	struct world *write_dst_cell;
	get_world_coordinates(dest_c, &dest_row, &dest_col);
	read_dst_cell = &read_matrix[dest_row][dest_col];
	write_dst_cell = &write_matrix[dest_row][dest_col];

	/* What will be the content of source cell */
	if(!read_src_cell->breeding_period) {
		/* Breeds */
		*write_src_cell = *read_src_cell;
		if(read_src_cell->type & SQUIRREL) {
			write_src_cell->breeding_period = s_breeding_p;
			new_breed_flag = 1;
		}
		else {
			write_src_cell->breeding_period = w_breeding_p;
			write_src_cell->starvation_period = w_starvation_p;
			new_breed_flag = 1;
		}
	}
	else {
		/*doesn't breed*/
		write_src_cell->type = read_src_cell->type & TREE;
		write_src_cell->breeding_period = 0;
		write_src_cell->starvation_period = 0;
		new_breed_flag = 0;
	}

	/* What will be the content of destination cell */
	if(read_src_cell->type & WOLF) {
		/* Check if the wolf is competing against other wolf */
		if(write_dst_cell->type & WOLF) {
			if(read_dst_cell->type & SQUIRREL) {
				new_ate_squirrel = 1;
			}
			else {
				new_ate_squirrel = 0;
			}

			if(read_src_cell->starvation_period > write_dst_cell->starvation_period) {
				*write_dst_cell = *read_src_cell;
				write_dst_cell->breed = new_breed_flag;
			}
			else if(read_src_cell->starvation_period == write_dst_cell->starvation_period) {
				if(read_src_cell->breeding_period < write_dst_cell->breeding_period) {
					*write_dst_cell = *read_src_cell;
					write_dst_cell->breed = new_breed_flag;
				}
			}
			write_dst_cell->ate_squirrel = new_ate_squirrel;
		}

		else { 
			if(write_dst_cell->type & SQUIRREL)
				new_ate_squirrel = 1;
			else
				new_ate_squirrel = 0;
			
			*write_dst_cell = *read_src_cell;
			write_dst_cell->breed = new_breed_flag;
			write_dst_cell->ate_squirrel = new_ate_squirrel;

			/* Check if the wolf is eating a squirrel */
			if(read_dst_cell->type & SQUIRREL) {
				write_dst_cell->ate_squirrel = 1;				
			}
		}
	}

	else if(read_src_cell->type & SQUIRREL) {	
		/* Check if the squirrel is competing against a wolf */
		if(write_dst_cell->type & WOLF) {
			/* Suicide move */
			write_dst_cell->ate_squirrel = 1;
		}
		else if(write_dst_cell->type & SQUIRREL) {
		/* Check if the squirrel is competing against other squirrel */
			if(read_src_cell->breeding_period < write_dst_cell->breeding_period) {
				*write_dst_cell = *read_src_cell;
			   write_dst_cell->breed = new_breed_flag;	   
				/* Prevent moving trees or deleting existing ones */
				if (read_dst_cell->type & TREE)
					write_dst_cell->type = SQUIRRELnTREE;
				else 
					write_dst_cell->type = SQUIRREL;
			}
		} else {
			*write_dst_cell = *read_src_cell; 
			write_dst_cell->breed = new_breed_flag;
			/* Prevent moving trees or deleting existing ones */
			if (read_dst_cell->type & TREE)
				write_dst_cell->type = SQUIRRELnTREE;
			else 
				write_dst_cell->type = SQUIRREL;
		}
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
		if(cell.type == SQUIRREL) {
			squirrels[found] = cell_number(row, col);
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
 * Update rules for animals in the world
 */
void update_squirrel(struct world **read_matrix, struct world **write_matrix, int row, int col){
	int possibilities[N_ADJACENTS];
	int may_move[N_ADJACENTS];
	int n_possibilities, n_moves;
	int chosen;
	
	n_possibilities = get_adjacents(row, col, possibilities);
	n_moves = get_walkable_cells(read_matrix, possibilities, n_possibilities, may_move, ICE | WOLF);
	if(n_moves) {
		chosen = choose_position(row, col, n_moves);
		move_to(row, col, may_move[chosen], read_matrix, write_matrix);
	}
 }

inline void kill_wolf(struct world *wolf) {
    memset(wolf, 0, sizeof(struct world));
}
 
void update_wolf(struct world **read_matrix, struct world **write_matrix, int row, int col) {
	int possibilities[N_ADJACENTS];
	int may_move[N_ADJACENTS];
	int n_possibilities, n_squirrels, n_other;
	int chosen;
    struct world *wolf = &write_matrix[row][col];
	n_possibilities = get_adjacents(row, col, possibilities);

    if(!wolf->starvation_period) {
        kill_wolf(wolf);
        return;
    }
	/*If has adjacents to choose*/
	if(n_possibilities) {
		/*Check for squirrels*/
		n_squirrels = get_cells_with_squirrels(read_matrix, possibilities, n_possibilities, may_move);
		if(n_squirrels) {
			/*At least one squirrel has been found
				Choose one of them*/
			chosen = choose_position(row, col, n_squirrels);
			/* Eat the squirrel - Now inside move_to
			get_world_coordinates(may_move[chosen], &s_row, &s_col);
			eat_squirrel(wolf, &write_matrix[s_row][s_col]);*/
			
			/*Move to that position*/
			move_to(row, col, may_move[chosen], read_matrix, write_matrix);
		}

		else {
			/*No squirrels
				Let's another cell*/
			n_other = get_walkable_cells(read_matrix, possibilities, n_possibilities, may_move, ICE | TREE);
			if(n_other) {
				chosen = choose_position(row, col, n_other);
				/*Move to that position*/
				move_to(row, col, may_move[chosen], read_matrix, write_matrix);
			}
		}
	}
}

void update_periods(struct world **read_matrix, struct world **write_matrix) {
    int i, j;
    struct world /*read_cell,*/ *write_cell;

    for(i = 0; i < max_size; i++) {
        for(j = 0; j < max_size; j++) {
			/*read_cell = &read_matrix[i][j];*/
			write_cell = &write_matrix[i][j];

			if(write_cell->type & WOLF) {
				/* Check if the wolf ate a squirrel*/
				if(write_cell->ate_squirrel) {
					write_cell->ate_squirrel = 0;
					write_cell->starvation_period = w_starvation_p;
				}
				else {
					write_cell->starvation_period--;
				}
				if(write_cell->breed) {
					write_cell->breed = 0;
					write_cell->breeding_period = w_breeding_p;
				}
				else {
					write_cell->breeding_period--;
				}
			}

			else if(write_cell->type & SQUIRREL) {
				if(write_cell->breed) {
					write_cell->breed = 0;
					write_cell->breeding_period = s_breeding_p;
				}
				else {
					write_cell->breeding_period--;
				}
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
    struct world **read_matrix = worlds[0];
    struct world **write_matrix = worlds[1];

	for(i = 0; i < max_size; i++) {
        if(get_cell_color(i, 0) == color) {
            start_col = 0;
        }
        else {
            start_col = 1;
        }
    
		for(j = start_col; j < max_size; j += N_COLORS) {
            if(read_matrix[i][j].type & WOLF) {
				update_wolf(read_matrix, write_matrix, i,j);
            }
			else if(read_matrix[i][j].type & SQUIRREL) {
                update_squirrel(read_matrix, write_matrix, i,j);
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
            cell = worlds[1][i][j];

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


void initiate_worlds(int row, int col, int type, int s_p, int b_p){
  int i;
  for(i=0; i<N_COLORS;++i) {
	worlds[i][row][col].type = type;
	worlds[i][row][col].starvation_period = s_p; 
	worlds[i][row][col].breeding_period = b_p;
  }
}


void populate_world_from_file(char file_name[]) {
	FILE *fp;
	int i, j, size, row_size;
	char a;

	struct world *all_positions;

   /* struct world ***right_world; */
	fp = fopen(file_name,"r");

	if(fp == NULL) {
		printf("Error while opening the file.\n");
	} else {
		fscanf(fp, "%d", &max_size);
        row_size = max_size*sizeof(struct world*);
		size = max_size*sizeof(struct world);
		
		for(i = 0; i < N_COLORS; i++) {
			worlds[i] = (struct world**) malloc(row_size);
			all_positions = (struct world*) malloc(max_size * max_size * sizeof(struct world));
		
			for(j = 0; j < max_size; j++) {
				worlds[i][j] = all_positions + (j * max_size);
				/* Put zeros in each line of each world */
				memset((void*) worlds[i][j], 0, size);
			}
		}

		/*populating*/
		while(fscanf(fp, "%d %d %c", &i, &j, &a) != EOF) {
			if(a=='w')
			  initiate_worlds(i,j,WOLF,w_starvation_p, w_breeding_p);
			else if(a=='s')
			  initiate_worlds(i,j,SQUIRREL,0, s_breeding_p);
			else if(a=='i')
			  initiate_worlds(i,j,ICE,0, 0);
			else if(a=='t')
			  initiate_worlds(i,j,TREE,0, 0);
			else if(a=='$') 
			  initiate_worlds(i,j,SQUIRRELnTREE,0, s_breeding_p);
			else {
				printf("Error in input file\n");
				fclose(fp);
				exit(-1);
			}	
		}

		fclose(fp);
	}
}

/*  
 * process_generations: Process all the generations
 */
void process_generations() {
	int i, color;
	for (i = 0; i < num_gen; ++i) {
		for(color = 0; color < N_COLORS; color++) {	
			swap_matrix();
			copy_matrix(worlds[0], worlds[1]);
			iterate_subgeneration(color);
		}
		update_periods(worlds[0], worlds[1]);
	}
}

int main(int argc, char **argv) {
	if(argc >= N_ARGS) {
		w_breeding_p = atoi(argv[2]);
		s_breeding_p = atoi(argv[3]);
		w_starvation_p = atoi(argv[4]);		
		num_gen = atoi(argv[5]);
		populate_world_from_file(argv[1]);
	    process_generations();
		print_all_cells();
	}
	else {
		printf("Usage: wolves-squirrels-serial <input file name> <wolf_breeding_period> ");
		printf("<squirrel_breeding_period> <wolf_startvation_period> <# of generations>\n");
	}
	return 0;	
}
