#include "structs.h"

#include <stdio.h>

#define MAX_FILEPATH 35

Piece PIECES[N_PIECES];
Piece ROTATED_PIECES[N_PIECES][ROTATIONS];

void swap(int *x, int *y) {
	// bitwise swapping!!
	*x = *x ^ *y;
	*y = *x ^ *y;
	*x = *x ^ *y;
}

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
	
	fscanf(file, "%*[^\n]");			// skip first line (a comment)
	fscanf(file, "%d", &piece->n_blocks);
	fscanf(file, "%d%d", &piece->center.x, &piece->center.y); // center of grid
	fscanf(file, "%d", &piece->even);
	for (int i = 0; i < piece->n_blocks; i++) {
		fscanf(file, "%d%d", &block.position.y, &block.position.x);
		fscanf(file, "%d", &block.colour);
		piece->blocks[i] = block;
	}
	
	fclose(file);
}

static void get_rotated_piece(Piece piece, Piece rotated[ROTATIONS]) {
	// Copy
	for (int i = 0; i < ROTATIONS; i++) {
		rotated[i].n_blocks = piece.n_blocks;
		rotated[i].center = piece.center;
		rotated[i].even = piece.even;
		for (int j = 0; j < piece.n_blocks; j++) {
			rotated[i].blocks[j].position = piece.blocks[j].position;
			rotated[i].blocks[j].colour = piece.blocks[j].colour;
		}
	}

	for (int i = 0; i < ROTATIONS; i++) {
		// Rotate i + 1 times. (x, y) -> (y, -x)
		for (int j = 0; j < i + 1; j++) {
			for (int k = 0; k < rotated[i].n_blocks; k++) {
				// Subtract center before rotating (origin is center)
				rotated[i].blocks[k].position.x -= 
					rotated[i].center.x;
				rotated[i].blocks[k].position.y -=
					rotated[i].center.y;

				swap(&rotated[i].blocks[k].position.x, 
					&rotated[i].blocks[k].position.y);
				rotated[i].blocks[k].position.x *= -1;
				rotated[i].blocks[k].position.x -= rotated[i].even;

				// Add center back in
				rotated[i].blocks[k].position.x += 
					rotated[i].center.x;
				rotated[i].blocks[k].position.y +=
					rotated[i].center.y;
			}
		}
	}
}

// This function saves all possible pieces.
void set_pieces() {
	for (int i = 0; i < N_PIECES; i++) {
		get_piece(&PIECES[i], i);
		get_rotated_piece(PIECES[i], ROTATED_PIECES[i]);
	}
}
