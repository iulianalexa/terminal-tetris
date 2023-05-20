#include <ncurses.h>
#include <string.h>
#include <stdio.h>

#include "structs.h"
#include "ncstructs.h"
#include "lists.h"

#define TITLE "Terminal Tetris"

// Colour pairs (2-8 are reserved for piece colours)
#define TITLE_PAIR 1

// This function draws the title.
void draw_title(WINDOW *title) {
	wclear(title);
	wresize(title, 1, COLS);

	wbkgd(title, COLOR_PAIR(TITLE_PAIR));
	mvwaddstr(title, 0, (COLS - strlen(TITLE)) / 2, TITLE); 
	wrefresh(title);
}

// This function will be used as an error message if draw_board fails because 
// of a terminal that is too small.
void draw_small_error(WINDOW *body) {
	int min_w = 2 * BOARD_W + BOARD_W_PAD; // tetris block: 2x1
	int min_h = BOARD_H + BOARD_H_PAD;
	
	wprintw(body, "too small\n");
	wprintw(body, "allow at least %dx%d\n", min_w, min_h);
	wrefresh(body);
}

// This function attempts to draw the game board. If terminal sizes are too 
// small, it will fail and draw an error message instead.
int draw_body(WINDOW *body) {
	wclear(body);
	wresize(body, LINES - 1, COLS);
	
	// Minimum height
	if (LINES < BOARD_H + BOARD_H_PAD + SCORE_PAD_H) {
		draw_small_error(body);
		return 0;
	}
	
	// Minimum width
	if (COLS < 2 * BOARD_W + BOARD_W_PAD + HOLD_PAD_W) {  // tetris block: 2x1
		draw_small_error(body);
		return 0;
	}
	
	wrefresh(body);
	return 1;
}

void draw_board(WINDOW *board, MovingPiece mp, List list) {
	wclear(board);

	// Rendering the static pieces

	int y = BOARD_H - 1;
	Node *current = list.start, *prev = NULL;
	while (current != NULL) {
		for (int i = 0; i < BOARD_W; i++) {
			wattron(board, COLOR_PAIR(current->value[i]));
			mvwaddstr(board, y, 2 * i, "  ");
			wattroff(board, COLOR_PAIR(current->value[i]));
		}
		y--;
		current = get_offset_node(current, prev, 1, &prev);
	}

	// Rendering the projection
	for (int i = 0; i < mp.structure.n_blocks; i++) {
		Block block = mp.structure.blocks[i];
		int x = 2 * (mp.projection.x + block.position.x);
		int y = mp.projection.y + block.position.y;
		mvwaddstr(board, y, x, "xx");	
	}

	// Rendering the dynamic piece

	for (int i = 0; i < mp.structure.n_blocks; i++) {
		Block block = mp.structure.blocks[i];
		int x = 2 * (mp.position.x + block.position.x);
		int y = mp.position.y + block.position.y;
		wattron(board, COLOR_PAIR(block.colour));
		mvwaddstr(board, y, x, "  ");
		wattroff(board, COLOR_PAIR(block.colour));
	}

	wrefresh(board);
}

// Draw the next display. If piece points to NULL, don't draw anything inside.
void draw_next_display(WINDOW *next_display, Piece *piece) {
	wclear(next_display);
	box (next_display, 0, 0);
	mvwprintw(next_display, 0, 1, "Next");

	if (piece != NULL) {
		for (int i = 0; i < piece->n_blocks; i++) {
			Block block = piece->blocks[i];
			int x = 1 + 2 * block.position.x;
			int y = 1 + block.position.y;
			wattron(next_display, COLOR_PAIR(block.colour));
			mvwaddstr(next_display, y, x, "  ");
			wattroff(next_display, COLOR_PAIR(block.colour));
		}
	}

	wrefresh(next_display);
}

// Draw the hold display. If piece points to NULL, don't draw anything inside.
void draw_hold_display(WINDOW *hold_display, Piece *piece) {
	wclear(hold_display);
	box (hold_display, 0, 0);
	mvwprintw(hold_display, 0, 1, "Held");

	if (piece != NULL) {
		for (int i = 0; i < piece->n_blocks; i++) {
			Block block = piece->blocks[i];
			int x = 1 + 2 * block.position.x;
			int y = 1 + block.position.y;
			wattron(hold_display, COLOR_PAIR(block.colour));
			mvwaddstr(hold_display, y, x, "  ");
			wattroff(hold_display, COLOR_PAIR(block.colour));
		}
	}

	wrefresh(hold_display);
}

// Draw the score and level
void draw_score_display(WINDOW *score_display, int score, int level) {
	wclear(score_display);

	wprintw(score_display, "Score: %d\nLevel: %d\n", score, level);
	wrefresh(score_display);
}

int draw(GameWindows gw, MovingPiece mp, List list, Piece *next_piece, 
		  Piece *held_piece) {
	draw_title(gw.title);

	if (!draw_body(gw.body)) {
		return 0;
	}

	wclear(gw.preboard);
	box(gw.preboard, 0, 0);
	wrefresh(gw.preboard);
	draw_board(gw.board, mp, list);
	draw_score_display(gw.score_display, 0, 1);
	draw_next_display(gw.next_display, next_piece);
	draw_hold_display(gw.hold_display, held_piece);

	return 1;
}
	
void init_pairs() {
	start_color();
	init_pair(TITLE_PAIR, COLOR_BLACK, COLOR_WHITE);
	init_pair(2, COLOR_BLACK, COLOR_CYAN);
	init_pair(3, COLOR_BLACK, COLOR_BLUE);
	init_pair(4, COLOR_BLACK, COLOR_WHITE);
	init_pair(5, COLOR_BLACK, COLOR_YELLOW);
	init_pair(6, COLOR_BLACK, COLOR_GREEN);
	init_pair(7, COLOR_BLACK, COLOR_MAGENTA);
	init_pair(8, COLOR_BLACK, COLOR_RED);
}

void del_wins(GameWindows gw) {
	delwin(gw.preboard);
	delwin(gw.board);
	delwin(gw.score_display);
	delwin(gw.next_display);
	delwin(gw.hold_display);
	delwin(gw.title);
	delwin(gw.body);
	refresh();
}

void set_wins(GameWindows *gw) {
	gw->title = newwin(1, COLS, 0, 0);
	gw->body = newwin(LINES - 1, COLS, 1, 0);
	gw->preboard = subwin(gw->body, BOARD_H + 2, 2 * BOARD_W + 2, 2, 2);
	// For some reason you can't create a subwin within a subwin
	gw->board = subwin(gw->body, BOARD_H, 2 * BOARD_W, 3, 3); 
	gw->score_display = subwin(gw->body, 2, COLS, BOARD_H + 5, 0);
	gw->next_display = subwin(gw->body, 6, 10, 2, 2 * BOARD_W + 2 + 2 + 2);
	gw->hold_display = subwin(gw->body, 6, 10, 10, 2 * BOARD_W + 2 + 2 + 2);
	keypad(gw->board, 1);
	wtimeout(gw->board, 0);
	refresh();
}

int resize_game(GameWindows *gw, MovingPiece mp, List list, 
				Piece *next_piece, Piece *held_piece) {
	// Complete redraw
	del_wins(*gw);
	set_wins(gw);
	return draw(*gw, mp, list, next_piece, held_piece);	
}

void draw_begin(GameWindows *gw) {
	initscr();
	init_pairs();
	curs_set(0);
	noecho();

	set_wins(gw);
}

void draw_end() {
	endwin();
}
