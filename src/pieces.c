#include "structs.h"
#include <stdio.h>

#define MAX_FILEPATH 35

Piece PIECES[N_PIECES];

// This function returns a piece with a certain type. It is only used 
// internally.
// Each piece structure is saved in a file in the pieces folder. Each file 
// contains the number of blocks, followed by that many lines containing 
// the x, y coordinates and the colour of each block.
static void get_piece(Piece *piece, int type) {
	FILE *file;
	Block block;
	char filepath[MAX_FILEPATH];
	
	sprintf(filepath, "pieces/piece_%d.txt", type);
	file = fopen(filepath, "r");
	
	fscanf(file, "%d", &piece->n_blocks);
	for (int i = 0; i < piece->n_blocks; i++) {
		fscanf(file, "%d%d", &block.position.y, &block.position.x);
		fscanf(file, "%d", &block.colour);
		piece->blocks[i] = block;
	}
	
	fclose(file);
}

// This function saves all possible pieces.
void set_pieces() {
	for (int i = 0; i < N_PIECES; i++) {
		get_piece(&PIECES[i], i);
	}
}
