#define N_PIECES 7
#define MAX_PIECE_BLOCKS 4

typedef struct {
	int x, y;
} Point;

// This structure defines the coordinates of a block and its colour.

typedef struct {
	Point position;
	int colour;
} Block;

// This structure describes a piece. The position of each block is relative.

typedef struct {
	Block blocks[MAX_PIECE_BLOCKS];
	int n_blocks;
} Piece;

// This structure defines the moving piece.
typedef struct {
	Point position;
	Piece structure;
} MovingPiece;
