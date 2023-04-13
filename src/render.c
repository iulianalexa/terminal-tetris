#include <ncurses.h>

#define BOARD_W 10
#define BOARD_H 24

// This function will be used as an error message if draw_board fails because 
// of a terminal that is too small.
void draw_small_error() {
	int min_w = BOARD_W + 2;
	int min_h = BOARD_H + 1 + 1 + 2;
	
	printw("too small\n");
	printw("allow at least %dx%d\n", min_w, min_h);
	refresh();
	getch();
	endwin();
}

// This function attempts to draw the game board. If terminal sizes are too 
// small, it will fail and draw an error message instead.
void draw_board() {
	initscr();
	
	// Minimum height: Board Height + 1 (title bar) + 2 (board padding) + 1
	// (outer padding)
	if (LINES < BOARD_H + 1 + 1 + 2) {
		draw_small_error();
		return;
	}
	
	// Minimum width: Board Width + 2 (board padding)
	if (COLS < BOARD_W + 2) {
		draw_small_error();
		return;
	}
	
	printw("Succes");
	refresh();
	getch();
	endwin();
}
