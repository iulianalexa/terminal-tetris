#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

#include "structs.h"
#include "pieces.h"
#include "lists.h"
#include "render.h"

#define TICKRATE 20

extern Piece PIECES[N_PIECES];

// Sleep in milliseconds
static void sleep_ms(int ms) {
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = ms * 1000 * 1000;
	nanosleep(&ts, NULL);
}

// Find what y a list index translates to
static int list_index_to_y(int ind) {
	return BOARD_H - 1 - ind;
}

// This function updates the moving piece with a random one.
static void get_random_piece(MovingPiece *mp, List list) {
	srand(time(NULL));
	int type = rand() % N_PIECES;
	
	set_pieces();
	Piece piece = PIECES[type];
	mp->position.x = BOARD_W / 2 - 1;
	mp->position.y = 1;
	mp->structure = piece;
	mp->current = get_offset_node(NULL, list.end, mp->position.y - 
		list_index_to_y(list.last_index), &mp->prev);
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

		if (mp.position.y + offset_y < BOARD_H - 1 - list.last_index) {
			// This line does not exist in the list, no collision here.
			continue;
		}

		// This line exists in the list.
		Node *line = get_offset_node(mp.current, mp.prev, offset_y, NULL);
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
	upd->position.y++;
	if (upd->prev == NULL && 
		upd->position.y - 1 == list_index_to_y(list.last_index)) {
		upd->prev = list.end;
	} else if (upd->prev != NULL && upd->current == NULL) {
		upd->current = upd->prev;
		upd->prev = get_offset_node(NULL, upd->current, 2, NULL);
	} else if (upd->prev != NULL && upd->current != NULL) {
		upd->current = get_offset_node(upd->current, upd->prev, 1, &upd->prev);
	}
}

// This function updates the moving piece accordingly.
static void input_updater(MovingPiece *upd, int ch, List list) {
	switch (ch) {
		case KEY_LEFT:
			upd->position.x--;
			break;
		case KEY_RIGHT:
			upd->position.x++;
			break;
		case KEY_DOWN:
			move_down(upd, list);
			break;
	}
}

// This function places the moving piece into the list.
static void place_piece(MovingPiece *mp, List *list) {
	if (mp->current == NULL) {
		while (list_index_to_y(list->last_index) > mp->position.y) {
			mp->current = add_node(list);
		}

		mp->prev = get_offset_node(NULL, mp->current, 2, NULL);
	}
	
	for (int i = 0; i < mp->structure.n_blocks; i++) {
		Block block = mp->structure.blocks[i];
		Node *node = get_offset_node(mp->current, mp->prev, block.position.y, 
			NULL);
		node->value[mp->position.x + block.position.x] = block.colour;
	}
}

// This function starts the game.
void begin() {
	WINDOW *title, *body, *preboard, *board;
	MovingPiece mp, upd;
	List list = create_list();
	int frames_until_fall = TICKRATE / 2, frames_drawn = 0, ch;

	draw_begin(&title, &body, &preboard, &board);
	get_random_piece(&mp, list);

	wgetch(body); // debug

	while (1) {
		draw(title, body, preboard, board, mp, list);
		upd = mp;

		// Get input
		while ((ch = wgetch(board)) != -1) {
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
				get_random_piece(&mp, list);

			}
			frames_drawn = -1;
		}

		frames_drawn++;
		sleep_ms(1000 / TICKRATE);
	}

	draw_end();
}