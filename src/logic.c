#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "structs.h"
#include "pieces.h"

extern Piece PIECES[N_PIECES];

void get_random_piece(MovingPiece *mp) {
	srand(time(NULL));
	int type = rand() % N_PIECES;
	
	set_pieces();
	Piece piece = PIECES[type];
	mp->position.x = BOARD_W / 2 - 1 ;
	mp->position.y = 1;
	mp->structure = piece;	
}
