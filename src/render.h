void draw_begin(WINDOW **title, WINDOW **body, 
				WINDOW **preboard, WINDOW **board);
void draw_end();
void draw(WINDOW *title, WINDOW *body, WINDOW *preboard, WINDOW *board, 
		  MovingPiece mp, List list);
void draw_board(WINDOW *board, MovingPiece mp, List list);
