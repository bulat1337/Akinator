#ifndef AKINATOR_HELPERS_H
#define AKINATOR_HELPERS_H

#include "akinator.h"

struct Ask_question_result
{
	struct B_tree_node *parent;
	bool is_right_child;
};

error_t mark_b_tree_nodes_as_free(struct B_tree_node *node, size_t size, size_t start_ID);
error_t dump_node(struct B_tree_node *node, struct node_charachteristics *nd_description,
					FILE *graphic_dump_code_file_ptr);
void print_regular_nodes(struct B_tree_node *node,
						struct node_charachteristics *nd_description,
						FILE *graphic_dump_code_file_ptr);

void connect_nodes(struct B_tree_node *node, FILE *graphic_dump_code_file_ptr);
error_t b_tree_realloc(struct B_tree *btr);
void read_node(struct Lexemes_w_carriage *data_base_buf_w_info, int *current_node_ID,
				struct B_tree_node *node);
void free_memory(struct B_tree *btr);
void print_node(struct B_tree_node *node, FILE *data_base);
void node_delete(struct B_tree_node *node);
error_t assign_value(b_tree_elem_t *data, const b_tree_elem_t assigned_value);
struct Ask_question_result ask_question(struct B_tree_node *parent, bool is_right_child);
void clear_buffer(void);
size_t get_amount_of_nodes(char * *lexemes, size_t amount_of_lexemes);
char get_menu_option_answer(void);
error_t update_current_node_according_to_path(int current_turn, struct B_tree_node *current_node);
size_t min_size_t(size_t num_1, size_t num_2);
struct B_tree_node *get_final_guess(struct Ask_question_result *ask_result);
void get_string(char *answer);

#endif
