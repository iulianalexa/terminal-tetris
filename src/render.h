void draw_begin(WINDOW **title, WINDOW **body, 
				WINDOW **preboard, WINDOW **board, WINDOW **score_display);
void draw_end();
void draw(WINDOW *title, WINDOW *body, WINDOW *preboard, WINDOW *board, 
		  WINDOW *score_display, MovingPiece mp, List list, int score, 
		  int level);
void draw_board(WINDOW *board, MovingPiece mp, List list);
