int check_if_fits();
void del_main_wins(GameWindows gw);
void del_game_wins(GameWindows gw);
void set_main_wins(GameWindows *gw);
void set_game_wins(GameWindows *gw);
void resize_game(GameWindows *gw, MovingPiece mp, List list, 
				Piece *next_piece, Piece *held_piece);
void draw_begin(GameWindows *gw);
void draw_end(GameWindows gw);
void draw(GameWindows gw, MovingPiece mp, List list, Piece *next_piece, 
          Piece *held_piece);
void draw_board(WINDOW *board, MovingPiece mp, List list);
void draw_next_display(WINDOW *next_display, Piece *piece);
void draw_hold_display(WINDOW *hold_display, Piece *piece);
void draw_score_display(WINDOW *score_display, int score, int level);
void draw_small_error(GameWindows gw);