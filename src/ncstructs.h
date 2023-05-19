// To be used for structs that rely on other ncurses structs

typedef struct {
		WINDOW *title, *body, *preboard, *board, *score_display, *hold_display;
} GameWindows;
