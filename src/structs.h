#define N_PIECES 7
#define MAX_PIECE_BLOCKS 4

#define BOARD_W 10
#define BOARD_H 24
#define BOARD_H_PAD 1 + 2 + 2  // title bar + inner padding + outer padding
#define BOARD_W_PAD 2 + 2 // inner padding + outer padding

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

// The value of each node is an array that saves the colour of the block.
typedef struct node {
	struct node *link;
	int *value;
} Node;

// The list needs a last_index in order to efficiently check the collisions 
// with the moving piece.
typedef struct {
	Node *start, *end;
	int last_index;
} List;
