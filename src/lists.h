List create_list();
Node *add_node(List *list);
void remove_node(List *list, Node *node, Node *prev);
Node *get_offset_node(Node *node, Node *near, int offset);