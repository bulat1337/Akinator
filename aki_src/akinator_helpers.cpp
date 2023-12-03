#include <stdio.h>
#include <math.h>
#include <string.h>

#include "akinator_helpers.h"

extern FILE *log_file;

struct node_charachteristics
{
	char *name;
	char *color;
	char *label;
};

error_t mark_b_tree_nodes_as_free(struct B_tree_node *node, size_t size, size_t start_ID)
{
	error_t error_code = AKI_ALL_GOOD;

	node[start_ID].left = NULL;

	for(size_t ID = start_ID + 1; ID < size - 1; ID++)
	{
		node[ID].data = NULL;
		node[ID].left = NULL;
		node[ID].right = &(node[ID + 1]);
	}

	node[start_ID].right = NULL;
	node[size - 1].left = NULL;
	node[size - 1].right = NULL;
	node[size - 1].data = NULL;

	return error_code;
}

error_t dump_node(struct B_tree_node *node, struct node_charachteristics *nd_description,
					FILE *graphic_dump_code_file_ptr)
{
	fprintf(graphic_dump_code_file_ptr, "\t%ld [shape = Mrecord, "
		"fillcolor = \"%s\", label = \"%s\" ];\n",
			node, nd_description->color, nd_description->label);

	return AKI_ALL_GOOD;
}

void print_regular_nodes(struct B_tree_node *node,
						struct node_charachteristics *nd_description,
						FILE *graphic_dump_code_file_ptr)
{
	if(node == NULL)
	{
		return;
	}

	snprintf(nd_description->label, NODE_LABEL_STR_SIZE,
					"{ID: %p | val: %s |{L: %p | R: %p}}",
					node, node->data,
					node->left, node->right);

	if((node->left == NULL) || (node->right == NULL))
	{
		nd_description->color = "#16DFB2";
	}
	else
	{
		nd_description->color = "#CA6103";
	}
	dump_node(node, nd_description, graphic_dump_code_file_ptr);

	print_regular_nodes(node->left,  nd_description, graphic_dump_code_file_ptr);
	print_regular_nodes(node->right, nd_description, graphic_dump_code_file_ptr);
}

void connect_nodes(struct B_tree_node *node, FILE *graphic_dump_code_file_ptr)
{
	if(node == NULL)
	{
		return;
	}

	if(node->left != NULL)
	{
		fprintf(graphic_dump_code_file_ptr, "%ld -> %ld\n", node, node->left);
	}

	if(node->right != NULL)
	{
		fprintf(graphic_dump_code_file_ptr, "%ld -> %ld\n", node, node->right);
	}

	connect_nodes(node->left, graphic_dump_code_file_ptr);
	connect_nodes(node->right, graphic_dump_code_file_ptr);
}

error_t b_tree_realloc(struct B_tree *btr)
{
	error_t error_code = AKI_ALL_GOOD;

	size_t data_size = sizeof(struct B_tree_node) * btr->capacity;

	btr->node = (struct B_tree_node *)realloc(btr->node, data_size);
	if(btr->node == NULL)
	{
		WRITE_IN_LOG_FILE("Unable to allocate btr->node\n");
		return UNABLE_TO_ALLOCATE;
	}

	size_t new_allocated_areas_start_ID = (btr->capacity / REALLOC_COEFF) - 1;
	WRITE_IN_LOG_FILE("new_allocated_areas_start_ID == %lu\n", new_allocated_areas_start_ID);

	mark_b_tree_nodes_as_free(btr->node, btr->capacity, new_allocated_areas_start_ID);

	btr->current_free = &btr->node[new_allocated_areas_start_ID + 1];
	printf("btr->current_free_ID: %d\n", new_allocated_areas_start_ID + 1);
	error_code = b_tree_verifier(btr);
	b_tree_dump(btr, error_code, __func__);


	return error_code;
}

void read_node(struct Lexemes_w_carriage *data_base_buf_w_info, int *current_node_ID,
				struct B_tree_node *node)
{
	#define IS_CURRENT_LEXEM(lexem)\
		!strncmp(data_base_buf_w_info->buf[data_base_buf_w_info->carriage], lexem, strlen(lexem))

	#define MOVE_CARRIAGE (data_base_buf_w_info->carriage)++;

	WRITE_IN_LOG_FILE("*(current_node_ID): %d\n", *(current_node_ID));

	int current_function_node_ID = *(current_node_ID);

	WRITE_IN_LOG_FILE("current_function_node_ID: %d\n", current_function_node_ID);

// 	if(IS_CURRENT_LEXEM("nil"))
// 	{
// 		WRITE_IN_LOG_FILE("data_base_buf_w_info->buf[data_base_buf_w_info->carriage]: %s\n",
// 				data_base_buf_w_info->buf[data_base_buf_w_info->carriage]);
// 		WRITE_IN_LOG_FILE("*returning*\n");
//
// 		MOVE_CARRIAGE;
//
// 		return;
// 	}

	//	Current_node_process
	if(IS_CURRENT_LEXEM("("))
	{
		WRITE_IN_LOG_FILE("data_base_buf_w_info->buf[data_base_buf_w_info->carriage]: %s\n",
						   data_base_buf_w_info->buf[data_base_buf_w_info->carriage]);

		MOVE_CARRIAGE;

		// Write_in_data
		assign_value(&(node[current_function_node_ID].data),
			data_base_buf_w_info->buf[data_base_buf_w_info->carriage]);
		// sscanf(data_base_buf_w_info->buf[data_base_buf_w_info->carriage],
		// 		"%s", node[current_function_node_ID].data);

		WRITE_IN_LOG_FILE("node[%d].data = %s\n", current_function_node_ID,
						   node[current_function_node_ID].data);

		MOVE_CARRIAGE;

		// Left_node_process
		if(IS_CURRENT_LEXEM("nil"))
		{
			WRITE_IN_LOG_FILE("data_base_buf_w_info->buf[data_base_buf_w_info->carriage]: %s\n",
							   data_base_buf_w_info->buf[data_base_buf_w_info->carriage]);

			node[current_function_node_ID].left = NULL;

			WRITE_IN_LOG_FILE("node[%d].left = %p\n",
							   current_function_node_ID, node[current_function_node_ID].left);

			MOVE_CARRIAGE;
		}
		else if(IS_CURRENT_LEXEM("("))
		{
			WRITE_IN_LOG_FILE("data_base_buf_w_info->buf[data_base_buf_w_info->carriage]: %s\n",
							   data_base_buf_w_info->buf[data_base_buf_w_info->carriage]);

			node[current_function_node_ID].left = &node[++(*current_node_ID)];

			WRITE_IN_LOG_FILE("node[%d].left = %d\n",
							   current_function_node_ID, node[current_function_node_ID].left);

			read_node(data_base_buf_w_info, current_node_ID, node);
		}
		else
		{
			fprintf(stderr, "Syntax error:\n");
			fprintf(stderr, "Node cannot start with %s:\n",
					data_base_buf_w_info->buf[data_base_buf_w_info->carriage]);

			exit(EXIT_FAILURE);	// а как иначе?
		}

		// Right_node_process
		if(IS_CURRENT_LEXEM("nil"))
		{
			WRITE_IN_LOG_FILE("data_base_buf_w_info->buf[data_base_buf_w_info->carriage]: %s\n",
							   data_base_buf_w_info->buf[data_base_buf_w_info->carriage]);

			node[current_function_node_ID].right = NULL;

			WRITE_IN_LOG_FILE("node[%d].right = %p\n",
							   current_function_node_ID, node[current_function_node_ID].right);

			MOVE_CARRIAGE;
		}
		else if(IS_CURRENT_LEXEM("("))
		{
			WRITE_IN_LOG_FILE("data_base_buf_w_info->buf[data_base_buf_w_info->carriage]: %s\n",
							   data_base_buf_w_info->buf[data_base_buf_w_info->carriage]);
			node[current_function_node_ID].right = &node[++(*current_node_ID)];

			WRITE_IN_LOG_FILE("node[%d].right = %p\n",
							   current_function_node_ID, node[current_function_node_ID].right);

			read_node(data_base_buf_w_info, current_node_ID, node);
		}
		else
		{
			fprintf(stderr, "Syntax error:\n");
			fprintf(stderr, "Node cannot start with %s:\n",
					data_base_buf_w_info->buf[data_base_buf_w_info->carriage]);

			exit(EXIT_FAILURE);	// а как иначе?
		}

		// Check_if_brecket_closes
		if(IS_CURRENT_LEXEM(")"))
		{
			WRITE_IN_LOG_FILE("data_base_buf_w_info->buf[data_base_buf_w_info->carriage]: %s\n",
				data_base_buf_w_info->buf[data_base_buf_w_info->carriage]);

			MOVE_CARRIAGE;
		}
		else
		{
			fprintf(stderr, "Syntax error:\n");
			fprintf(stderr, "Node cannot start with %s:\n",
					data_base_buf_w_info->buf[data_base_buf_w_info->carriage]);

			exit(EXIT_FAILURE);	// а как иначе?
		}
	}

	#undef IS_CURRENT_LEXEM
	#undef MOVE_CARRIAGE
}

void free_memory(struct B_tree *btr)
{
    free(btr->node);
    free(btr);
}

void print_node(struct B_tree_node *node, FILE *data_base)
{
    if(node == NULL)
    {
        fprintf(data_base, "nil ");
        return;
    }

    fprintf(data_base, "( ");
    fprintf(data_base, "%s ", node->data);
    print_node(node->left, data_base);
    print_node(node->right, data_base);
    fprintf(data_base, ") ");
}

void node_delete(struct B_tree_node *node)
{
	if(node == NULL)
	{
		return;
	}

	node->data = DATA_POISON;
	node_delete(node->left);
	node_delete(node->right);

	node->left  = NULL;
	node->right = NULL;
}

error_t assign_value(b_tree_elem_t *data, const b_tree_elem_t assigned_value)
{
	*(data) = (b_tree_elem_t)calloc(strlen(assigned_value) + 1, sizeof(char));
	if(*(data) == NULL)
	{
		return UNABLE_TO_ALLOCATE;
	}

	strncpy(*(data), assigned_value, strlen(assigned_value));

	return AKI_ALL_GOOD;
}

struct Ask_question_result ask_question(struct B_tree_node *parent, bool is_right_child)
{
	char line_to_say[AKINATOR_LINE_MAX_LEN] = {};
	struct Ask_question_result result =
	{
		.parent = parent,
		.is_right_child = is_right_child,
	};

	struct B_tree_node *current_node = NULL;

	if(is_right_child)
	{
		current_node = parent->right;
	}
	else
	{
		current_node = parent->left;
	}

	//IS_LEAF
	if((current_node->left == NULL) || (current_node->right == NULL))
	{
		return result;
	}

	char player_answer = 'n';

	SAY_TO_PLAYER("%s?", current_node->data);

	player_answer = get_menu_option_answer();

	parent = current_node;

	if(player_answer == 'y')
	{
		is_right_child = true;
	}
	else
	{
		is_right_child = false;
	}

	struct Ask_question_result final_answer = ask_question(parent, is_right_child);

	return final_answer;
}

void clear_buffer()
{
	while(getchar() != '\n')
		;
}

size_t get_amount_of_nodes(char * *lexemes, size_t amount_of_lexemes)
{
	size_t node_counter = 0;

	for(size_t lexem_ID = 0; lexem_ID < amount_of_lexemes; lexem_ID++)
	{
		if(!strncmp(lexemes[lexem_ID], "(", strlen("(")))
		{
			node_counter++;
		}
	}

	return node_counter;
}

char get_menu_option_answer(void)
{
	char answer = 'x';
	printf("> ");
	scanf("%c", &answer);
	clear_buffer();

	return answer;
}

error_t update_current_node_according_to_path(int current_turn, struct B_tree_node *current_node)
{
	if(current_turn == 0)
	{
		current_node = current_node->left;
	}
	else if(current_turn == 1)
	{
		current_node = current_node->right;
	}
	else
	{
		return INVALID_NODE_PATH_TURN;
	}

	return AKI_ALL_GOOD;
}

size_t min_size_t(size_t num_1, size_t num_2)
{
	if(num_1 >= num_2)
	{
		return num_1;
	}
	else
	{
		return num_2;
	}
}

struct B_tree_node *get_final_guess(struct Ask_question_result *ask_result)
{
	struct B_tree_node *final_guess = NULL;
	if(ask_result->is_right_child)
	{

		final_guess = ask_result->parent->right;
	}
	else
	{
		final_guess = ask_result->parent->left;
	}

	return final_guess;
}

void get_string(char *answer)
{
	printf("> ");
	scanf("%s", answer);
	clear_buffer();
}


