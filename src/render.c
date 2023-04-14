#include <ncurses.h>
#include <string.h>

#include "structs.h"
#include "logic.h"

#define BOARD_W 2 * 10 // tetris bit = 2x1
#define BOARD_H 24
#define BOARD_H_PAD 1 + 2 + 2  // title bar + inner padding + outer padding
#define BOARD_W_PAD 2 + 2 // inner padding + outer padding

#define TITLE "Terminal Tetris"

// Colour pairs (2-8 are reserved for piece colours)

#define TITLE_PAIR 1

// This function will be used as an error message if draw_board fails because 
// of a terminal that is too small.
void draw_small_error() {
	clear();
	int min_w = BOARD_W + BOARD_W_PAD;
	int min_h = BOARD_H + BOARD_H_PAD;
	
	printw("too small\n");
	printw("allow at least %dx%d\n", min_w, min_h);
	refresh();
	getch();
}

// This function attempts to draw the game board. If terminal sizes are too 
// small, it will fail and draw an error message instead.
void draw_body(WINDOW *body) {
	clear();
	
	// Minimum height
	if (LINES < BOARD_H + BOARD_H_PAD) {
		draw_small_error();
		return;
	}
	
	// Minimum width
	if (COLS < BOARD_W + BOARD_W_PAD) {
		draw_small_error();
		return;
	}
	
	wrefresh(body);
}

void draw_board(WINDOW *board, MovingPiece mp) {
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
	WINDOW *title, *body, *board;
	MovingPiece mp;
	
	initscr();
	init_pairs();
	
	title = newwin(1, COLS, 0, 0);
	body = newwin(LINES - 1, COLS, 1, 0);
	board = subwin(body, BOARD_H, BOARD_W, 2, 2);
	box(board, 0, 0);
	
	wbkgd(title, COLOR_PAIR(TITLE_PAIR));
	mvwaddstr(title, 0, (COLS - strlen(TITLE)) / 2, TITLE); 
	refresh();
	wrefresh(title);
	
	curs_set(0);
	noecho();
	draw_body(body);
	get_random_piece(&mp);
	draw_board(board, mp);
	wgetch(body);
	
	endwin();
}
