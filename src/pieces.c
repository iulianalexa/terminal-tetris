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

// This function ensures piece correction. Especially useful with rotations, 
// it ensures that blocks begin starting x=0 and y=0.
static void correct_piece(Piece *piece) {
	int x_min = piece->blocks[0].position.x;
	int y_min = piece->blocks[0].position.y;

	for (int i = 1; i < piece->n_blocks; i++) {
		if (piece->blocks[i].position.x < x_min) {
			x_min = piece->blocks[i].position.x;
		}

		if (piece->blocks[i].position.y < y_min) {
			y_min = piece->blocks[i].position.y;
		}
	}

	// Subtract from each block's position (make zero)
	for (int i = 0; i < piece->n_blocks; i++) {
		piece->blocks[i].position.x -= x_min;
		piece->blocks[i].position.y -= y_min;
	}
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
	
	fscanf(file, "%d", &piece->n_blocks);
	for (int i = 0; i < piece->n_blocks; i++) {
		fscanf(file, "%d%d", &block.position.y, &block.position.x);
		fscanf(file, "%d", &block.colour);
		piece->blocks[i] = block;
	}
	
	fclose(file);
	correct_piece(piece);
}

static void get_rotated_piece(Piece piece, Piece rotated[ROTATIONS]) {
	// Copy
	for (int i = 0; i < ROTATIONS; i++) {
		rotated[i].n_blocks = piece.n_blocks;
		for (int j = 0; j < piece.n_blocks; j++) {
			rotated[i].blocks[j].position = piece.blocks[j].position;
			rotated[i].blocks[j].colour = piece.blocks[j].colour;
		}
	}

	for (int i = 0; i < ROTATIONS; i++) {
		// Rotate i + 1 times. (x, y) -> (y, -x)
		for (int j = 0; j < i + 1; j++) {
			for (int k = 0; k < rotated[i].n_blocks; k++) {
				swap(&rotated[i].blocks[k].position.x, 
					&rotated[i].blocks[k].position.y);
				rotated[i].blocks[k].position.y *= -1;
			}
		}

		correct_piece(&rotated[i]);
	}
}

// This function saves all possible pieces.
void set_pieces() {
	for (int i = 0; i < N_PIECES; i++) {
		get_piece(&PIECES[i], i);
		get_rotated_piece(PIECES[i], ROTATED_PIECES[i]);
	}
}
