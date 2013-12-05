/*
 * Wolves and squirrels
 *
 * Serial implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>	
#include <mpi.h>

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
int id, nprocs, num_lines;
int ghost_lines_start, ghost_lines_end, start_cell;
int start_computation_line = 0, total_lines;
MPI_Datatype mpi_world_type;
struct world *buffer_start, *buffer_end;

#define TAG 101
#define NITEMS 6
#define N_ADJACENTS	4

#define GHOST_LINES 1

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
	int cell_number;
} **worlds[N_COLORS]; 


/*
 * return 0, if different. Not zero, otherwise.
 */
inline int equals(struct world w1, struct world w2) {
	return (w1.type == w2.type) &&
		(w1.breeding_period == w2.breeding_period) &&
		(w1.starvation_period == w2.starvation_period) &&
		(w1.ate_squirrel == w2.ate_squirrel) &&
		(w1.breed == w2.breed) &&
		(w1.cell_number == w2.cell_number);
}

inline void swap_matrix(){
  struct world** aux;
  aux = worlds[1];
  worlds[1] = worlds[0];
  worlds[0] = aux;  
}

inline void copy_matrix(struct world **src, struct world **dst) {
	memcpy(dst[0], src[0], sizeof(struct world) * (num_lines + ghost_lines_start + ghost_lines_end) * max_size); 
}
 
/*
 * cell_number: Return the cell number of a given
 *	matrix position (row, col)
 */ 
inline int cell_number(int row, int col) {
	return (row * max_size + col) + start_cell;
}

/*
 * choose_position: Apply the rule to choose a position
 *	even when we have more than one to choose
 */
inline int choose_position(int row, int col, int p) {
	int c = cell_number(row, col);
	return  c % p;
 }

void init_ghost_buffers() {	
	buffer_start = (struct world*) malloc(buffer_start_size(id) * max_size * sizeof(struct world));	
	buffer_end = (struct world*) malloc(buffer_end_size(id) * max_size * sizeof(struct world));
	memset(buffer_start, 0, sizeof(buffer_start_size(id) * max_size * sizeof(struct world)));
	memset(buffer_end, 0, sizeof(buffer_start_size(id) * max_size * sizeof(struct world)));
}

inline int buffer_start_size(int nid) {
	return ghost_lines_at_start(nid) + ghost_lines_at_end(nid - 1);
}

inline int buffer_end_size(int nid) {
	return ghost_lines_at_end(nid) + ghost_lines_at_start(nid + 1);
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
	if(col < max_size-1) {
		adjacents[found++] = cell_number(row, col + 1);
	}

	/*Has down adjacent cell?*/
	if(row < total_lines - 1) {
		adjacents[found++] = cell_number(row + 1, col);
	}

	/*Has left adjacent cell?*/
	if(col > 0) {
		adjacents[found++] = cell_number(row, col - 1);
	}

	return found;
}



inline void get_global_world_coordinates(int cell_number, int *row, int *col) {	
	/* Adjust the line to the process matrix */
	*col = cell_number % max_size;
	*row = (cell_number - *col) / max_size;
}

/*
 * get_world_coordinates: Return in row and col the right
 *	coordinates of the matric of a given cell number
 */
inline void get_world_coordinates(int cell_number, int *row, int *col) {
	get_global_world_coordinates(cell_number, row, col);
	*row = *row - start_computation_line;
}

/*
 * The strong kill the weak. Wolf > Squirrel > Empty
 * Higher starvation period > Lower starvation period 
 * and if equal higher breeding period > lower breeding period.
 * Also wolves cannot climb trees and no breed_flag is being checked.
 * Only writes in dst and is only being used in solve_ghost_conflicts.
 */
void solve_conflict(struct world *src, struct world *dst) {
	struct world temp = *dst;
	if(dst->type == EMPTY && src->type != EMPTY) {
		*dst = *src;
		return;
	}
	
	/* Eats squirrel or competes with another wolf */
	/* WOLF is moving*/
	if(src->type & WOLF) {
		if(dst->type == SQUIRREL) {
			*dst = *src;
			dst->ate_squirrel = 1;
		}
		else if(dst->type & WOLF) {
			if(src->starvation_period > dst->starvation_period)
				*dst = *src;
			else if(src->starvation_period == dst->starvation_period) {
				if(src->breeding_period > dst->breeding_period) {
					*dst = *src;
				}
			}
		}
	}
	/* Is eaten by a wolf or competes with another squirrel */
	/* SQUIRREL is moving*/
	else if(src->type & SQUIRREL) {
		if(dst->type & WOLF)
			dst->ate_squirrel = 1;
		else if(dst->type & SQUIRREL) {
			if(src->breeding_period > dst->breeding_period) {
				*dst = *src;
			}
		}
	}
	
	/* Fixes possible errors */
	if(temp.type & TREE)
		dst->type |= TREE;
	else if(dst->type == SQUIRRELnTREE)
		dst->type = SQUIRREL;	
	if(temp.ate_squirrel > 0)
		dst->ate_squirrel = 1;
	dst->cell_number = temp.cell_number;
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
		}
		else {
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

    for(i = 0; i < total_lines; i++) {
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
 * Sets ghost lines in write matrix at zero. To ease the conflict solve at the
 * end of each subgeneration. 
 */
void blank_write_ghost_lines() {
	int i, ghost_lines;

	ghost_lines = ghost_lines_at_start(id);
	for(i = 0; i < ghost_lines; i++) {
		memset(worlds[1][i], 0, max_size);
	}

	ghost_lines = ghost_lines_at_end(id);
	for(i = 0; i < ghost_lines; i++) {
		memset(worlds[1][ghost_lines_at_start(id) + num_lines + i], 0, max_size);
	}
}

/*
 * Sends corresponding ghost lines to the previous and next processes.
 */
void send_ghost_lines() {
	int ghost_lines = buffer_end_size(id - 1);

	if(id > 0) {
	 	MPI_Send(worlds[1][0], ghost_lines * max_size, mpi_world_type, id - 1, TAG, MPI_COMM_WORLD);
	}

	ghost_lines = buffer_start_size(id + 1);
	if(id < (nprocs - 1)) {
		MPI_Send(worlds[1][ghost_lines_at_start(id) + num_lines - ghost_lines_at_start(id + 1)], ghost_lines * max_size, mpi_world_type, id + 1, TAG, MPI_COMM_WORLD);
	}
}

/*
 * Receives num ghost lines from process other_id.
 */
void receive_ghost_lines() {
	if(id > 0) {
		MPI_Recv(buffer_start, buffer_start_size(id) * max_size, mpi_world_type, id - 1, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	if(id < (nprocs - 1)) {
		MPI_Recv(buffer_end, buffer_end_size(id) * max_size, mpi_world_type, id + 1, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
}

/*
 * Solve conflicts by moving the buffer to the write matrix
 */
void solve_ghost_conflict(int nlines, int index, struct world *buffer) {
	int i, j;

	for(i = 0; i < nlines; i++) {
		for(j = 0; j < max_size; j++) {
			if(equals(worlds[0][index + i][j], buffer[i * max_size + j])) {//TESTING
				continue;
			}
			else if(equals(worlds[0][index + i][j], worlds[1][index + i][j])) {
				worlds[1][index + i][j] = buffer[i * max_size + j];
			}
			else {
				 solve_conflict(&buffer[i * max_size + j], &worlds[1][index + i][j]);
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

	/* Don't process the ghost lines */
	for(i = ghost_lines_start; i < num_lines + ghost_lines_start; i++) {
        if(get_cell_color(i + start_computation_line, 0) == color) {
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
	
	/* Trade ghost lines */
	send_ghost_lines();
	receive_ghost_lines();
	
	/* And solve conflicts */
	solve_ghost_conflict(buffer_start_size(id), 0, buffer_start);
	solve_ghost_conflict(buffer_end_size(id), ghost_lines_at_start(id) + num_lines - ghost_lines_at_start(id + 1), buffer_end);
}

char get_type_char(struct world *cell) {
	char type;

	if(cell->type & WOLF) {
		type = 'w';
	}
	else if(cell->type & SQUIRREL) {
		/*Squirrel and a tree*/
		if(cell->type & TREE) {
			type = '$';
		}
		else {
			type = 's';
		}
	}
	else if(cell->type & ICE) {
		type = 'i';
	}
	else if(cell->type & TREE){
		type = 't';
	}

	else {
		type = ' ';
	}

	return type;
}

inline void print_cell(int l, int c, int prev_proc_last_line) {
	struct world cell = worlds[1][l][c];

	if(cell.type) {
		printf("%d %d %c\n", l + prev_proc_last_line, c, get_type_char(&cell));
	}
}

/*prints the world*/
void print_all_cells(){
	int i, j, p, process_lines, prev_proc_lines;

	/* First: Print the master process's lines */
	for(i = 0, prev_proc_lines = 0; i < num_lines; i++, prev_proc_lines++) {
		for(j = 0; j < max_size; j++) {
			print_cell(i, j, 0);
		}
	}

	/* Receive the lines of each process and print them */
	for(p = 1; p < nprocs; p++) {
		process_lines = get_num_lines(max_size, nprocs, p);

		MPI_Recv(worlds[1][0], process_lines * max_size, mpi_world_type, p, 
				TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	
		for(i = 0; i < process_lines; i++) {
			for(j = 0; j < max_size; j++) {
				print_cell(i, j, prev_proc_lines);
			}
		}

		prev_proc_lines += process_lines;
	}
}

void initiate_worlds(int row, int col, int type, int s_p, int b_p){
  int i;
  for(i=0; i<N_COLORS;++i) {
	worlds[i][row][col].type = type;
	worlds[i][row][col].starvation_period = s_p; 
	worlds[i][row][col].breeding_period = b_p;
  }
}

/*
 * returns the number of lines process id computes
 * @param size: size of the entire matrix
 * @param nprocs: number of processes
 * @param id: current process' id
 */
int get_num_lines(int size, int nprocs, int id) {
	int n = size / nprocs;
	int r = size % nprocs;

	if(id < r)
		return n + 1;
	else
		return n;
}

/*
 * Creates MPI_Datatype and commits it
 */
void create_mpi_datatype() {
	int blocklengths[NITEMS] = {1, 1, 1, 1, 1, 1};
	MPI_Datatype types[NITEMS] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT};
	MPI_Aint offsets[NITEMS];

	offsets[0] = offsetof(struct world, type);
 	offsets[1] = offsetof(struct world, breeding_period);
 	offsets[2] = offsetof(struct world, starvation_period);
	offsets[3] = offsetof(struct world, ate_squirrel);
	offsets[4] = offsetof(struct world, breed);
	offsets[5] = offsetof(struct world, cell_number);

	MPI_Type_create_struct(NITEMS, blocklengths, offsets, types, &mpi_world_type);
	MPI_Type_commit(&mpi_world_type);
}

/*
 * Alloc memory for worlds
 */
void init_worlds(int ghost_lines) {
	int  i, j, row_size;
	struct world *all_positions;
	int total_lines = num_lines + ghost_lines;
	row_size = total_lines * sizeof(struct world*);

	for(i = 0; i < N_COLORS; i++) {
		worlds[i] = (struct world**) malloc(row_size);
		all_positions = (struct world*) malloc(total_lines * max_size * sizeof(struct world));

		for(j = 0; j < num_lines + ghost_lines; j++) {
			worlds[i][j] = all_positions + (j * max_size);
		}
	}
}

/*
 * start_ghost_lines: Get how many ghost lines the process
 *	will receive before the real ones
 */
int ghost_lines_at_start(int process_id) {
	/* Only the first process will not have ghost lines at the start */
	if(process_id>0 && process_id<nprocs) {
		return GHOST_LINES;
	}
	else {
		return 0;
	}
}

int ghost_lines_at_end(int process_id) {
	/* Only the last process will not have ghost lines at the end */
	if(process_id<(nprocs - 1) && (process_id>-1)) {
		return GHOST_LINES;
	}
	else {
		return 0;
	}
}

/*
 * Distributes the worlds by an aproximate number of lines to each process
 */
void scatter_matrix() {
	int i, size, row_size, proc_num_lines, begin_index = 0;
	int ghosts_start, ghosts_end, total_ghosts, col;
	int process_total_lines;	

	num_lines = get_num_lines(max_size, nprocs, id);

	ghost_lines_start = ghost_lines_at_start(id);
	ghost_lines_end = ghost_lines_at_end(id);

	if(!id) {
		total_ghosts = ghost_lines_start + ghost_lines_end;
		total_lines = num_lines + total_ghosts;
		/* Send some lines of the matrix to the other processors*/
		for(i = 1, begin_index = num_lines; i < nprocs; i++, begin_index += proc_num_lines) {
			proc_num_lines = get_num_lines(max_size, nprocs, i);
			ghosts_start = ghost_lines_at_start(i);
			ghosts_end = ghost_lines_at_end(i);
			total_ghosts = ghosts_start + ghosts_end;
			process_total_lines = proc_num_lines + total_ghosts;
			MPI_Send(worlds[0][begin_index - ghosts_start], process_total_lines * max_size, mpi_world_type, i, TAG, MPI_COMM_WORLD);
		}
	} else {
		total_ghosts = ghost_lines_start + ghost_lines_end;
		total_lines = num_lines + total_ghosts;
		init_worlds(total_ghosts);
		MPI_Recv(worlds[0][0], total_lines * max_size, mpi_world_type, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		start_cell = worlds[0][0][0].cell_number;
		copy_matrix(worlds[0], worlds[1]);
		get_global_world_coordinates(start_cell, &start_computation_line, &col);
	}
}

void init_process() {
	num_lines = get_num_lines(max_size, nprocs, id);

	ghost_lines_start = ghost_lines_at_start(id);
	ghost_lines_end = ghost_lines_at_end(id);

	total_lines = num_lines + ghost_lines_start + ghost_lines_end;
}

struct world **create_world(int nlines) {
	struct world *all_positions;
	struct world **new_world;
	int i;

	new_world = (struct world**) malloc(nlines * sizeof(struct world*));
	all_positions = (struct world*) malloc(nlines * max_size * sizeof(struct world));
	
	for(i = 0; i < total_lines; i++) {
		new_world[i] = all_positions + (i * max_size);
		/* Put zeros in each line of each world */
		memset((void*) new_world[i], 0, max_size * sizeof(struct world));
		/* Later each process will know the index of start processing line
		   in the original matrix */
		new_world[i][0].cell_number = cell_number(i, 0);
	}
}

void alloc_worlds() {
	int i, j;
	struct world *all_positions;
	
	for(i = 0; i < N_COLORS; i++) {
		worlds[i] = create_world(total_lines);	

	}
}

void populate_cell(int line, int col, char type, struct world **matrix) {
	struct world *cell = &matrix[line][col]; 
	
	if(type == 'w') {
		cell->type = WOLF;
		cell->starvation_period = w_starvation_p;
	   	cell->breeding_period =	w_breeding_p;
	}
	else if(type=='s') {
		cell->type = SQUIRREL;
		cell->breeding_period = s_breeding_p;
	}
	else if(type=='i') {
		cell->type = ICE;
	}
	else if(type=='t') {
		cell->type = TREE;
	}
	else if(type=='$') {
		cell->type = SQUIRRELnTREE;
		cell->breeding_period = s_breeding_p;
	}
	else {
		printf("Error in input file\n");
		exit(-1);
	}	
}

void scatter_matrix_from_file(char *file_name) {
	FILE *file;
	int i, j, p = 1, read_lines, process_lines, read;;
	char c = '\0';
	struct world **buffer;

	file = fopen(file_name, "r");
	if(file == NULL) {
		printf("Error while opening the file %s.\n", file_name);
		exit(-1);
	}

	/* Read max_size */
	fscanf(file, "%d", &max_size);
	MPI_Bcast(&max_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
	init_process();
	alloc_worlds();

	buffer = create_world(num_lines + 2 * GHOST_LINES + 1);

	read_lines = total_lines;
	
	/* Read the first lines and check if it's for the master process */
	while((fscanf(file, "%d %d %c", &i, &j, &c)) != EOF) {
		if(i < total_lines) {
			populate_cell(i, j, c, worlds[0]);
		}
		else {
			/* Find which process is */
			process_lines = get_num_lines(max_size, nprocs, p);
			while(i > read_lines - 1 && i < read_lines + process_lines) {
				read_lines += get_num_lines(max_size, nprocs, p++);
			}
			break;
		}
	}
	
	for(p = 1; p < nprocs; p++) {
		if(c) {

		}
		while(fscanf(file, "%d %d %c", &i, &j, &c) != EOF) {
			//if()
		}
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
				/* Later each process will know the index of start processing line
				   in the original matrix */
				worlds[i][j][0].cell_number = cell_number(j, 0);
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

void gather_matrix() {
        int i, begin_index = num_lines;

        /*Receive all matrixes*/
        if(!id) {
                for(i = 1; i < nprocs; i++) {
                        MPI_Recv(worlds[1][begin_index], get_num_lines(max_size, nprocs, i) * max_size,
                                 mpi_world_type, i, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        begin_index += get_num_lines(max_size, nprocs, i);
                }
        }
        /*Send local matrix to master process*/
        else {
                MPI_Send(worlds[1][ghost_lines_at_start(id)], num_lines * max_size, mpi_world_type, 0, TAG, MPI_COMM_WORLD);
        }
}

void print_matrix(int generation, int subgeneration) {
	int i, j, p;

	for(p = 0; p < nprocs; p++) {
		MPI_Barrier(MPI_COMM_WORLD);
		if(p == id) {
			printf("Process %d Generation: %d Subgeneration: %d\n", id, generation, subgeneration);
			for(i = 0; i < total_lines; i++) {
				printf("Line %d |", i + start_computation_line);
				for(j = 0; j < max_size; j++) {
					printf("%c|", get_type_char(&worlds[1][i][j]));
				}
				printf("\n");
				printf("       ");
				for(j = 0; j < max_size * 2 + 1; j++) {
					printf("-");
				}
				printf("\n");
				fflush(stdout);
			}
		}
	}
}
/*
   * send_lines_to_master: Send computed lines to
   * process master that is waiting for them to
   * print
   */
void send_lines_to_master() {
	MPI_Send(worlds[1][ghost_lines_start], num_lines * max_size,
		   	mpi_world_type, 0, TAG, MPI_COMM_WORLD);
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

	if(id) {
		send_lines_to_master();
	}
}

int main(int argc, char **argv) {
	if(argc >= N_ARGS) {
		w_breeding_p = atoi(argv[2]);
		s_breeding_p = atoi(argv[3]);
		w_starvation_p = atoi(argv[4]);		
		num_gen = atoi(argv[5]);

		MPI_Init(&argc, &argv);
		MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
		MPI_Comm_rank(MPI_COMM_WORLD, &id);
		/*Only the master thread has the entire matrix*/
		if(!id)
			populate_world_from_file(argv[1]);
		
		MPI_Bcast(&max_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
		init_ghost_buffers();		
		
		/* More processes than lines */
		if(nprocs > max_size) {
			/* Limit the number of processes */
			nprocs = max_size;
		}
		
		if(id < nprocs) {
			create_mpi_datatype();
			scatter_matrix();
			process_generations();
		/*	gather_matrix();*/
		}

		if(!id) {
			print_all_cells();
		}
		

		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Finalize();
	}
	else {
		printf("Usage: wolves-squirrels-mpi <input file name> <wolf_breeding_period> ");
		printf("<squirrel_breeding_period> <wolf_startvation_period> <# of generations>\n");
	}
	return 0;	
}
