#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

#include "structs.h"
#include "pieces.h"
#include "lists.h"
#include "render.h"

#define TICKRATE 25

extern Piece PIECES[N_PIECES];
extern Piece ROTATED_PIECES[N_PIECES][ROTATIONS];

// Sleep in milliseconds
static void sleep_ms(int ms) {
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = ms * 1000 * 1000;
	nanosleep(&ts, NULL);
}

// Find what list index a y coordinate would translate to
static int list_index_from_y(int y) {
	return BOARD_H - 1 - y;
}

// This function checks if there is a collision between the moving piece and 
// the static blocks (saved in the XOR linked list) or the boundary.
static int check_collisions(MovingPiece mp, List list) {
	for (int i = 0; i < mp.structure.n_blocks; i++) {
		Block block = mp.structure.blocks[i];
		int offset_y = block.position.y;
		int offset_x = block.position.x;

		if (mp.position.x + offset_x < 0 || 
			mp.position.x + offset_x >= BOARD_W) {
			// Collision with the left-right boundary
			return 1;
		}

		if (list_index_from_y(mp.position.y + offset_y) >= list.count) {
			// This line does not exist in the list, no collision here.
			continue;
		}

		// This line exists in the list.
		Node *line = get_oob_offset_node(mp.current, mp.next, offset_y, NULL, 
			list_index_from_y(mp.position.y), list);
		if (line == NULL) {
			// Collision with the ground
			return 1;
		}
		if (line->value[mp.position.x + offset_x] != 0) {
			// Collision with a block
			return 1;
		}
	}

	return 0;
}

// This function does a collision check on the updated moving piece. If there 
// are no collisions, it updates the moving piece.
// This will only work as intended with one update at a time!
static int advance(MovingPiece *mp, MovingPiece *upd, List list) {
	if (check_collisions(*upd, list)) {
		// Collision -> advance not successful
		return 0;
	}

	*mp = *upd;
	return 1;
}

static void move_down(MovingPiece *upd, List list) {
	upd->current = get_oob_offset_node(upd->current, upd->next, 1, &upd->next, 
		list_index_from_y(upd->position.y), list);
	upd->position.y++;
}

// Forcefully make a piece fall (equivalent to spacebar on most implementations)
static void fall(MovingPiece *mp, List list) {
	MovingPiece upd;
	do {
		upd = *mp;
		move_down(&upd, list);
	} while (advance(mp, &upd, list));
}

// Update the projection coordinates of a piece. A projection is a preview of 
// the piece position if the user were to force fall.
static void get_projection(MovingPiece *mp, List list) {
	MovingPiece upd;
	upd = *mp;
	fall(&upd, list);
	mp->projection = upd.position;
}

// This function updates the moving piece with a random one.
static int get_random_piece(MovingPiece *mp, List list) {
	srand(time(NULL));
	int type = rand() % N_PIECES;
	
	Piece piece = PIECES[type];
	mp->position.x = BOARD_W / 2 - 1;
	mp->position.y = 1;
	mp->rotation = -1;
	mp->type = type;
	mp->structure = piece;
	mp->current = get_oob_offset_node(NULL, NULL, mp->position.y, &mp->next,
		BOARD_H, list);

	if (check_collisions(*mp, list)) {
		// Collided on generation. That means you lose :)
		return 0;
	}

	get_projection(mp, list);

	return 1;
}

// This function attempts the rotation of the moving piece, according to SRS.
// https://tetris.wiki/Super_Rotation_System
static void rotate(MovingPiece *mp, List list) {
	int tries = 0;
	do {
		tries++;
		mp->rotation++;
		if (mp->rotation == ROTATIONS) {
			mp->rotation = -1;
		}

		if (mp->rotation == -1) {
			mp->structure = PIECES[mp->type];
		} else {
			mp->structure = ROTATED_PIECES[mp->type][mp->rotation];
		}
	} while(check_collisions(*mp, list) && tries < ROTATIONS);
}

// This function updates the moving piece accordingly.
static void input_updater(MovingPiece *upd, int ch, List list) {
	switch (ch) {
		case KEY_LEFT:
			upd->position.x--;
			get_projection(upd, list);
			break;
		case KEY_RIGHT:
			upd->position.x++;
			get_projection(upd, list);
			break;
		case KEY_DOWN:
			move_down(upd, list);
			break;
		case KEY_UP:
			rotate(upd, list);
			get_projection(upd, list);
			break;
		// Space treated separately in begin
	}
}

// This function checks if a line is complete.
static int line_complete(Node *node) {
	for (int i = 0; i < BOARD_W; i++) {
		if (node->value[i] == 0) {
			return 0;
		}
	}
	
	return 1;
}

// This function checks for completed lines starting from the given node, up to 
// check_upto lines. If any completed line is found, break it.
static void check_break_lines(List *list, Node *node, Node *next,
							  int check_upto) {
	while (node != NULL && check_upto > 0) {
		if (line_complete(node)) {
			// Find prev node
			Node *prev = get_offset_node(node, next, 1, NULL);
			remove_node(list, node, prev);
			node = prev;
		} else {
			// Keep going
			node = get_offset_node(node, next, 1, &next);
		}
		check_upto--;
	}
}

// This function places the moving piece into the list.
static void place_piece(MovingPiece *mp, List *list) {
	if (mp->current == NULL) {
		while (list_index_from_y(mp->position.y) > list->count - 1) {
			mp->current = add_node(list);
		}

		mp->next = NULL;
	}
	
	for (int i = 0; i < mp->structure.n_blocks; i++) {
		Block block = mp->structure.blocks[i];
		Node *near;
		Node *node = get_offset_node(mp->current, mp->next, block.position.y, 
			&near);
		node->value[mp->position.x + block.position.x] = block.colour;
		check_break_lines(list, node, near, 4);
	}
}

// This function starts the game.
void begin() {
	WINDOW *title, *body, *preboard, *board;
	MovingPiece mp, upd;
	List list = create_list();
	int frames_until_fall = TICKRATE / 2, frames_drawn = 0, ch;

	draw_begin(&title, &body, &preboard, &board);
	wgetch(body); // debug
	set_pieces();
	get_random_piece(&mp, list);

	draw(title, body, preboard, board, mp, list);

	while (1) {
		draw_board(board, mp, list);
		upd = mp;

		// Get input
		while ((ch = wgetch(board)) != -1) {
			if (ch == ' ') {
				fall(&mp, list);
				// force place
				frames_drawn = frames_until_fall;
				upd = mp;
				break;
			}
			
			input_updater(&upd, ch, list);
			advance(&mp, &upd, list);
			upd = mp;
		}

		// Tick down after a certain amount of passes
		if (frames_drawn == frames_until_fall) {
			move_down(&upd, list);
			if (advance(&mp, &upd, list) == 0) {
				// Could not advance piece further: place and regenerate
				place_piece(&mp, &list);
				if (!get_random_piece(&mp, list)) {
					// Lose condition
					break;
				}

			}
			frames_drawn = -1;
		}

		frames_drawn++;
		sleep_ms(1000 / TICKRATE);
	}

	draw_end();
}
