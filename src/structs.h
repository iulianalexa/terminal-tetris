#define N_PIECES 7
#define MAX_PIECE_BLOCKS 4
#define ROTATIONS 3

#define BOARD_W 10
#define BOARD_H 24
#define BOARD_H_PAD 1 + 2 + 2  // title bar + inner padding + outer padding
#define BOARD_W_PAD 2 + 2 // inner padding + outer padding

// The value of each node is an array that saves the colour of the block.
// 0 -> no block
typedef struct node {
	struct node *link;
	int *value;
} Node;

// The list needs a last_index in order to efficiently check the collisions 
// with the moving piece.
typedef struct {
	Node *start, *end;
	int count;
} List;

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
// Each moving piece has a line associated with it in a list -> current is 
// the associated node, next is the next node. (NULL if out of bounds)
// Rotation: -1 (default); 0-2 -> rotations
typedef struct {
	Point position;
	Piece structure;
	Node *current, *next;
	int type, rotation;
} MovingPiece;
