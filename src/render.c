#include <ncurses.h>
#include <string.h>

#define BOARD_W 2 * 10 // tetris bit = 2x1
#define BOARD_H 24
#define BOARD_H_PAD 1 + 2 + 2  // title bar + inner padding + outer padding
#define BOARD_W_PAD 2 + 2 // inner padding + outer padding

#define TITLE "Terminal Tetris"

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
	
	//wprintw(body, "Test\n");
	wrefresh(body);
	wgetch(body);
}

void init_pairs() {
	start_color();
	init_pair(TITLE_PAIR, COLOR_BLACK, COLOR_WHITE);
}

void main_draw() {
	WINDOW *title, *body, *board;
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
	endwin();
}
