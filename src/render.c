#include <ncurses.h>
#include <string.h>
#include <stdio.h>

#include "structs.h"
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
	if (LINES < BOARD_H + BOARD_H_PAD) {
		draw_small_error(body);
		return 0;
	}
	
	// Minimum width
	if (COLS < 2 * BOARD_W + BOARD_W_PAD) {  // tetris block: 2x1
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

void draw(WINDOW *title, WINDOW *body, WINDOW *preboard, WINDOW *board, 
		  MovingPiece mp, List list) {
	draw_title(title);

	if (!draw_body(body)) {
		return;
	}

	wclear(preboard);
	box(preboard, 0, 0);
	wrefresh(preboard);
	draw_board(board, mp, list);
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

void draw_begin(WINDOW **title, WINDOW **body, 
				WINDOW **preboard, WINDOW **board) {
	initscr();
	init_pairs();
	curs_set(0);
	noecho();

	*title = newwin(1, COLS, 0, 0);
	*body = newwin(LINES - 1, COLS, 1, 0);
	*preboard = subwin(*body, BOARD_H + 2, 2 * BOARD_W + 2, 2, 2);
	// For some reason you can't create a subwin within a subwin
	*board = subwin(*body, BOARD_H, 2 * BOARD_W, 3, 3); 
	keypad(*board, 1);
	wtimeout(*board, 0);
}

void draw_end() {
	endwin();
}
