#include <ncurses.h>

int main() {
	initscr();
	printw("TETRIS");
	refresh();
	getch();
	endwin();

	return 0;
}
