void get_random_piece(MovingPiece *mp);
int check_collisions(MovingPiece mp, List list, Node *current, Node *prev);
int advance(MovingPiece *mp, MovingPiece *upd, List list, Node *current, 
    Node *prev);