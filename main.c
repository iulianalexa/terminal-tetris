#include <stdio.h>

#include "src/logic.h"

int main() {
	int level;
	int score = begin(&level);
	printf("thanks for playing!\n");
	printf("your level: %d\n", level);
	printf("your score: %d\n", score);
	return 0;
}
