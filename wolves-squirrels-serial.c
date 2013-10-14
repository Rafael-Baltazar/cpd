/*
 * Wolves and squirrels
 *
 * Serial implementation
 */

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

int main(int argc, char **argv) {

	return 0;	
}
