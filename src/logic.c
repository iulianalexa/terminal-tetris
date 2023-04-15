#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

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
// the static blocks (saved in the XOR linked list) or the boundary.
// - The value of current is the node associated with the line of the y 
// component of the moving piece.
// - The value of prev is the node preceding current.
int check_collisions(MovingPiece mp, List list, Node *current, Node *prev) {
	for (int i = 0; i < mp.structure.n_blocks; i++) {
		Block block = mp.structure.blocks[i];
		int offset_y = block.position.y;
		int offset_x = block.position.x;

		if (mp.position.x + offset_x < 0 || 
			mp.position.x + offset_x >= BOARD_W) {
			// Collision with the left-right boundary
			return 1;
		}

		if (mp.position.y + offset_y < BOARD_H - 1 - list.last_index) {
			// This line does not exist in the list, no collision here.
			continue;
		}

		// This line exists in the list.
		Node *line = get_offset_node(current, prev, offset_y);
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

// This function does a collision check on the updated moving piece. If there 
// are no collisions, it updates the moving piece.
// This will only work as intended with one update at a time!
int advance(MovingPiece *mp, MovingPiece *upd, List list, Node *current, 
			 Node *prev) {
	if (check_collisions(*upd, list, current, prev)) {
		// Collision -> advance not successful
		return 0;
	}

	*mp = *upd;
	return 1;
}

// This function updates the moving piece accordingly.
void input_updater(MovingPiece *upd, int ch) {
	switch (ch) {
		case KEY_LEFT:
			upd->position.x--;
			break;
		case KEY_RIGHT:
			upd->position.x++;
			break;
	}
}