#include <ncurses.h>
#include <string.h>
#include <stdio.h>

#include "structs.h"
#include "logic.h"
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

void draw_board(WINDOW *board, MovingPiece mp) {
	wclear(board);

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
		  MovingPiece mp) {
	draw_title(title);

	if (!draw_body(body)) {
		return;
	}

	wclear(preboard);
	box(preboard, 0, 0);
	wrefresh(preboard);
	draw_board(board, mp);
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

void main_draw() {
	WINDOW *title, *body, *preboard, *board;
	MovingPiece mp, upd;
	List list = create_list();

	initscr();
	init_pairs();
	curs_set(0);
	noecho();

	title = newwin(1, COLS, 0, 0);
	body = newwin(LINES - 1, COLS, 1, 0);
	preboard = subwin(body, BOARD_H + 2, 2 * BOARD_W + 2, 2, 2);
	// For some reason you can't create a subwin within a subwin
	board = subwin(body, BOARD_H, 2 * BOARD_W, 3, 3); 

	get_random_piece(&mp);

	draw(title, body, preboard, board, mp);
	upd = mp;

	do {
		draw(title, body, preboard, board, mp);
		upd.position.y++;
		wgetch(preboard);
	} while (advance(&mp, &upd, list, NULL, NULL));

	endwin();
}
