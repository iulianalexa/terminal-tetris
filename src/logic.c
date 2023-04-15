#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "structs.h"
#include "pieces.h"
#include "lists.h"

extern Piece PIECES[N_PIECES];

// This function updates the moving piece with a random one.
void get_random_piece(MovingPiece *mp) {
	srand(time(NULL));
	int type = rand() % N_PIECES;
	
	set_pieces();
	Piece piece = PIECES[type];
	mp->position.x = BOARD_W / 2 - 1;
	mp->position.y = 1;
	mp->structure = piece;	
}

// This function checks if there is a collision between the moving piece and 
// the static blocks (on the board, saved in the XOR linked list). 
// - The value of current is the node associated with the line of the y 
// component of the moving piece.
// - The value of prev is the node preceding current.
int check_collisions(MovingPiece mp, List list, Node *current, Node *prev) {
	for (int i = 0; i < mp.structure.n_blocks; i++) {
		Block block = mp.structure.blocks[i];
		int offset = block.position.y;
		if (mp.position.y + offset < BOARD_H - 1 - list.last_index) {
			// This line does not exist in the list, no collision here.
			continue;
		}

		// This line exists in the list.
		Node *line = get_offset_node(current, prev, offset);
		if (line == NULL) {
			// Collision with the ground
			return 1;
		}
		if (line->value[mp.position.x] != 0) {
			// Collision with a block
			return 1;
		}
	}

	return 0;
}
