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

// Add node to list and return it. The node value will be alloc'd with zeros.
Node *add_node(List *list) {
	Node *node = malloc(sizeof(Node)), *prev;
	list->last_index++;
	
	if (node == NULL) {
		printf(OUT_OF_MEMORY);
		exit(-1);
	}
	
	node->value = malloc(sizeof(int) * BOARD_W);
	for (int i = 0; i < BOARD_W; i++) {
		node->value[i] = 0;
	}
	
	if (node->value == NULL) {
		printf(OUT_OF_MEMORY);
		exit(-1);
	}
	
	if (list->end == NULL) {
		list->start = node;
		list->end = node;
		node->link = NULL;
		return node;
	}

	node->link = XOR(list->end, NULL);
	prev = XOR(list->end->link, NULL);
	list->end->link = XOR(prev, node);
	list->end = node;
	
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
// This function will also, optionally, find the new near node. If newnear is 
// NULL, then it will not.
Node *get_offset_node(Node *node, Node *near, int offset, Node **newnear) {
	if (offset < 0) {
		if (newnear != NULL) {
			*newnear = NULL;
		}
		return NULL;
	}

	for (int i = 0; i < offset; i++) {
		if (near == NULL) {
			if (newnear != NULL) {
				*newnear = NULL;
			}
			return NULL;
		}

		if (near->link == NULL) {
			if (newnear != NULL) {
				*newnear = NULL;
			}
			return near;
		}

		Node *nearnear = XOR(near->link, node);
		node = near;
		near = nearnear;
	}

	if (newnear != NULL) {
		*newnear = near;
	}
	return node;
}