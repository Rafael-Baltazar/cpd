#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define N_TYPES 5

int main(int argc, char **argv) {
	int i, j, size, empty;
	char types[] = {'i', 't', 's', 'w', '$'};

	if(argc > 1)
		size = atoi(argv[1]);
	else printf("Correct usage: make_test <matrix's size>\n");

	srand(time(NULL));

	printf("%d\n", size);

	for(i = 0; i < size; i++) {
		for(j = 0; j < size; j++) {
   			empty = rand() % 100;

			if(empty < 75) {
				printf("%d %d %c\n", i, j, types[rand() % N_TYPES]);
			}
		}
	}
	return 0;
}
