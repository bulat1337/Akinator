#ifndef AKINATOR_HELPERS_H
#define AKINATOR_HELPERS_H

#include "akinator.h"

error_t mark_b_tree_nodes_as_free(struct B_tree_node *node, size_t size, size_t start_ID);
error_t create_node(int node_ID, struct node_charachteristics *nd_description,
					FILE *graphic_dump_code_file_ptr);
void print_regular_nodes(int ID, struct B_tree_node *btr,
						struct node_charachteristics *nd_description,
						FILE *graphic_dump_code_file_ptr);

void connect_nodes(int ID, struct B_tree_node *node, FILE *graphic_dump_code_file_ptr);
error_t b_tree_realloc(struct B_tree *btr);
void read_node(struct Lexemes_w_carriage *data_base_buf_w_info, int *current_node_ID,
				struct B_tree_node *node);
void free_memory(struct B_tree *btr);
void print_node(struct B_tree_node *node, int ID, FILE *data_base);
void node_delete(struct B_tree_node *node, int current_ID);

#endif
