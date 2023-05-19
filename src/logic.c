#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

#include "structs.h"
#include "ncstructs.h"
#include "pieces.h"
#include "lists.h"
#include "render.h"

#define TICKRATE 25.0

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

// This function updates the moving piece with a specific one.
static int get_specific_piece(MovingPiece *mp, List list, int type) {
	Piece piece = PIECES[type];
	mp->position.x = BOARD_W / 2 - piece.center.x;
	mp->position.y = 0;
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

// This function gets the next piece and updates the new next.
// If type is -1, it also randomises the first piece (used for initiating)
static int get_next_piece(MovingPiece *mp, List list, int type, 
						  int *next_type) {
	if (type == -1) {
		type = rand() % N_PIECES;
	}

	*next_type = rand() % N_PIECES;

	return get_specific_piece(mp, list, type);
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
		// C treated separately in begin
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
static int check_break_lines(List *list, Node *node, Node *next,
							  int check_upto, int level) {

	int base_score = 0;
	int lines_cleared = 0;

	while (node != NULL && check_upto > 0) {
		if (line_complete(node)) {
			// Find prev node
			Node *prev = get_offset_node(node, next, 1, NULL);
			remove_node(list, node, prev);
			node = prev;
			lines_cleared++;
		} else {
			// Keep going
			node = get_offset_node(node, next, 1, &next);
		}
		check_upto--;
	}

	// Add base score depending on how many lines were broken
	switch (lines_cleared) {
		case 1:
			base_score = 100;
			break;
		case 2:
			base_score = 300;
			break;
		case 3:
			base_score = 500;
			break;
		case 4:
			base_score = 800;
			break;
	}

	return base_score * level;
}

// This function places the moving piece into the list. It returns the points 
// awarded after placing the piece.
static int place_piece(MovingPiece *mp, List *list, int level) {
	int score = 0;

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
		score += check_break_lines(list, node, near, 4, level);
	}

	return score;
}

// Advance level if needed.
const void level_advancer(int score, int *level, float *frames_until_fall) {
	int cond;

	while (1) {
		cond = (score > *level * 1000);
		if (*level >= 10) {
			cond = (score > 10000);
		}

		if (*level == 15) {
			cond = 0;
		}

		if (!cond) {
			break;
		}

		*level = *level + 1;
		*frames_until_fall = *frames_until_fall - 0.8;
	}
}

// This function starts the game. Returns the score.
int begin() {
	GameWindows gw;
	MovingPiece mp, upd;
	List list = create_list();
	float frames_until_fall = TICKRATE / 2, frames_drawn = 0.0;
	int ch, type_of_held_piece = -1, has_held = 0, type_of_next_piece = -1;
	int score = 0, level = 1;

	srand(time(NULL));

	draw_begin(&gw);
	wgetch(gw.body); // debug
	set_pieces();
	get_next_piece(&mp, list, type_of_next_piece, &type_of_next_piece);

	draw(gw, mp, list);
	draw_next_display(gw.next_display, &PIECES[type_of_next_piece]);

	while (1) {
		draw_board(gw.board, mp, list);
		upd = mp;

		// Get input
		while ((ch = wgetch(gw.board)) != -1) {
			if (ch == ' ') {
				fall(&mp, list);
				// force place
				frames_drawn = frames_until_fall;
				upd = mp;
				break;
			} else if (!has_held && (ch == 'c' || ch == 'C')) {
				has_held = 1;
				if (type_of_held_piece == -1) {
					// No currently held piece, get from next.
					type_of_held_piece = mp.type;
					get_next_piece(&mp, list, type_of_next_piece, 
						&type_of_next_piece);
					draw_next_display(gw.next_display, 
						&PIECES[type_of_next_piece]);
				} else {
					int tmp = mp.type;
					get_specific_piece(&mp, list, type_of_held_piece);
					type_of_held_piece = tmp;
				}

				draw_hold_display(gw.hold_display, &PIECES[type_of_held_piece]);
				frames_drawn = -1.0;
				upd = mp;
				break;
			}
			
			input_updater(&upd, ch, list);
			advance(&mp, &upd, list);
			upd = mp;
		}

		// Tick down after a certain amount of passes
		if (frames_drawn >= frames_until_fall) {
			move_down(&upd, list);
			if (advance(&mp, &upd, list) == 0) {
				// Could not advance piece further: place and regenerate
				int changes = place_piece(&mp, &list, level);
				// Allow player to hold pieces again
				has_held = 0;

				if (changes > 0) {
					score += changes;
					level_advancer(score, &level, &frames_until_fall);
					draw_score_display(gw.score_display, score, level);
				}

				if (!get_next_piece(&mp, list, type_of_next_piece, 
					&type_of_next_piece)) {
					// Lose condition
					break;
				} else {
					draw_next_display(gw.next_display, 
						&PIECES[type_of_next_piece]);
				}

			}
			frames_drawn = -1.0;
		}

		frames_drawn++;
		sleep_ms((int) (1000 / TICKRATE));
	}

	draw_end();

	return score;
}
