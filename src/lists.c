#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "structs.h"

#define OUT_OF_MEMORY "out of memory\n"
#define XOR(a, b) (Node *)((intptr_t)(a)^(intptr_t)(b))

// Create a list
List create_list() {
	List list;
	list.start = NULL;
	list.end = NULL;
	list.last_index = -1;
	
	return list;
}

// Add node to list and return it. The node value will be alloc'd.
Node *add_node(List *list) {
	Node *node = malloc(sizeof(Node)), *prev;
	
	if (node == NULL) {
		printf(OUT_OF_MEMORY);
		exit(-1);
	}
	
	node->value = malloc(sizeof(Block) * BOARD_W);
	
	if (node->value == NULL) {
		printf(OUT_OF_MEMORY);
		exit(-1);
	}
	
	node->link = XOR(list->end, NULL);
	prev = XOR(list->end->link, NULL);
	list->end->link = XOR(prev, node);
	list->end = node;
	if (list->start == NULL) {
		list->start = node;
	}
	
	list->last_index++;
	return node;
}

// Remove node from list. This requires knowing what the previous node is.
void remove_node(List *list, Node *node, Node *prev) {
	Node *next = XOR(node->link, prev);
	
	if (next != NULL) {
		Node *nextnext = XOR(next->link, node);
		next->link = XOR(prev, nextnext);
	} else {
		list->end = prev;
	}
	
	if (prev != NULL) {
		Node *prevprev = XOR(prev->link, node);
		prev->link = XOR(prevprev, next);
	} else {
		list->start = next;
	}
	
	free(node->value);
	free(node);
	list->last_index--;
}	

// This function returns the node offset positions away from the current node.
// If near is the succeding node, then the function will return the node at
// offset positions to the right.
// If near is the preceding node, then the function will return the node at 
// offset positions to the left.
// If the offset is greater than the number of nodes that can be discovered 
// in that direction, the function will return NULL.
Node *get_offset_node(Node *node, Node *near, int offset) {
	for (int i = 0; i < offset; i++) {
		if (near == NULL) {
			return NULL;
		}

		if (near->link == NULL) {
			return near;
		}

		Node *nearnear = XOR(near->link, node);
		node = near;
		near = nearnear;
	}

	return node;
}