void draw_begin(GameWindows *gw);
void draw_end();
void draw(GameWindows gw, MovingPiece mp, List list);
void draw_board(WINDOW *board, MovingPiece mp, List list);
void draw_hold_display(WINDOW *hold_display, Piece *piece);
void draw_score_display(WINDOW *score_display, int score, int level);
