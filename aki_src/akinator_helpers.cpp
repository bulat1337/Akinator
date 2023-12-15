#include <stdio.h>
#include <math.h>
#include <string.h>

#include "akinator_helpers.h"

#define NODE_PATH_ID current_pos->node_path_ID
#define FIRST_TURN cmp_leafs[FIRST].node_path.data[NODE_PATH_ID]
#define SECOND_TURN cmp_leafs[SECOND].node_path.data[NODE_PATH_ID]
#define CURRENT_NODE current_pos->current_node

extern char line_to_say[AKINATOR_LINE_MAX_LEN];

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
		log("Unable to allocate btr->node\n");
		return UNABLE_TO_ALLOCATE;
	}

	size_t new_allocated_areas_start_ID = (btr->capacity / REALLOC_COEFF) - 1;
	log("new_allocated_areas_start_ID == %lu\n", new_allocated_areas_start_ID);

	mark_b_tree_nodes_as_free(btr->node, btr->capacity, new_allocated_areas_start_ID);

	btr->current_free = &btr->node[new_allocated_areas_start_ID + 1];
	printf("btr->current_free_ID: %d\n", new_allocated_areas_start_ID + 1);
	error_code = b_tree_verifier(btr);
	b_tree_dump(btr, error_code, __func__);


	return error_code;
}

#define IS_CURRENT_LEXEM(lexem)\
!strncmp(data_base_buf_w_info->buf[data_base_buf_w_info->carriage], lexem, strlen(lexem))

#define MOVE_CARRIAGE (data_base_buf_w_info->carriage)++;

void read_node(struct Lexemes_w_carriage *data_base_buf_w_info, int *current_node_ID,
				struct B_tree_node *node)
{
	log("*(current_node_ID): %d\n", *(current_node_ID));

	int current_function_node_ID = *(current_node_ID);

	log("current_function_node_ID: %d\n", current_function_node_ID);

	//	Current_node_process
	if(IS_CURRENT_LEXEM("("))
	{
		log("data_base_buf_w_info->buf[data_base_buf_w_info->carriage]: %s\n",
						   data_base_buf_w_info->buf[data_base_buf_w_info->carriage]);

		MOVE_CARRIAGE;

		// Write_in_data
		assign_value(&(node[current_function_node_ID].data),
			data_base_buf_w_info->buf[data_base_buf_w_info->carriage]);

		log("node[%d].data = %s\n", current_function_node_ID,
						   node[current_function_node_ID].data);

		MOVE_CARRIAGE;

		// Left_node_process
		if(IS_CURRENT_LEXEM("nil"))
		{
			log("data_base_buf_w_info->buf[data_base_buf_w_info->carriage]: %s\n",
							   data_base_buf_w_info->buf[data_base_buf_w_info->carriage]);

			node[current_function_node_ID].left = NULL;

			log("node[%d].left = %p\n",
							   current_function_node_ID, node[current_function_node_ID].left);

			MOVE_CARRIAGE;
		}
		else if(IS_CURRENT_LEXEM("("))
		{
			log("data_base_buf_w_info->buf[data_base_buf_w_info->carriage]: %s\n",
							   data_base_buf_w_info->buf[data_base_buf_w_info->carriage]);

			node[current_function_node_ID].left = &node[++(*current_node_ID)];

			log("node[%d].left = %d\n",
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
			log("data_base_buf_w_info->buf[data_base_buf_w_info->carriage]: %s\n",
							   data_base_buf_w_info->buf[data_base_buf_w_info->carriage]);

			node[current_function_node_ID].right = NULL;

			log("node[%d].right = %p\n",
							   current_function_node_ID, node[current_function_node_ID].right);

			MOVE_CARRIAGE;
		}
		else if(IS_CURRENT_LEXEM("("))
		{
			log("data_base_buf_w_info->buf[data_base_buf_w_info->carriage]: %s\n",
							   data_base_buf_w_info->buf[data_base_buf_w_info->carriage]);
			node[current_function_node_ID].right = &node[++(*current_node_ID)];

			log("node[%d].right = %p\n",
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
			log("data_base_buf_w_info->buf[data_base_buf_w_info->carriage]: %s\n",
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
}

#undef IS_CURRENT_LEXEM
#undef MOVE_CARRIAGE

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

struct Guess_result ask_question(struct B_tree_node *parent, bool is_right_child)
{
	struct Guess_result result =
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

	struct Guess_result final_answer = ask_question(parent, is_right_child);

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

enum Menu_option get_menu_option_answer(void)
{
	enum Menu_option answer = QUIT;
	printf("> ");
	scanf("%c", &answer);
	clear_buffer();

	return answer;
}

error_t update_current_node_according_to_path(int current_turn,
											  struct B_tree_node *current_node)
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

struct B_tree_node *get_final_guess(const struct Guess_result *ask_result)
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

struct Guess_result guess_leaf(struct B_tree *btr)
{
	SAY_TO_PLAYER("You already guessed someone?");
	SAY_TO_PLAYER("Answer the following questions and I will tell you who it is.");

	struct B_tree_node finctitious_root_parent =
	{
		.data = "dont_look_at_me",
		.left = btr->root,
		.right = btr->root,
	};

	return ask_question(&finctitious_root_parent, RIGHT_CHILD);
}

void process_guess_validation(char player_answer,
							  const struct Guess_result *guess_result,
							  struct B_tree *btr)
{
	if(player_answer == 'y')
	{
		SAY_TO_PLAYER("Too slow, too weak, too easy");
	}
	else
	{
		char new_leaf_data[MAX_NEW_NODE_DATA_STRLEN] = {};
		char new_question[MAX_NEW_NODE_DATA_STRLEN]  = {};

		SAY_TO_PLAYER("damn... what or who was it then?");

		get_string(new_leaf_data);

		struct B_tree_node *final_guess = get_final_guess(guess_result);

		SAY_TO_PLAYER("And how does he differ from %s?", final_guess->data);

		get_string(new_question);

		SAY_TO_PLAYER("U wanna add new leaf "
						"with the corresponding question to the data base?");
		printf("New leaf: %s\n", new_leaf_data);
		printf("New question: %s\n", new_question);

		player_answer = get_menu_option_answer();

		if(player_answer == 'y')
		{
			struct B_tree_node *new_leaf =
				create_node(btr, new_leaf_data).second_arg.node;
			struct B_tree_node *new_question_node =
				create_node(btr, new_question).second_arg.node;

			add_child(new_question_node, final_guess, LEFT_CHILD);
			add_child(new_question_node, new_leaf, RIGHT_CHILD);
			if(guess_result->is_right_child)
			{
				add_child(guess_result->parent, new_question_node, RIGHT_CHILD);
			}
			else
			{
				add_child(guess_result->parent, new_question_node, LEFT_CHILD);
			}

			create_data_base(btr, "data_base.txt");

			SAY_TO_PLAYER("New leaf has been added to the tree");
		}
	}
}

struct Leaf_w_path get_Leaf_w_path(struct B_tree_node *root)
{
	struct Leaf_w_path cmp_leaf =
	{
		.node_path = {},
		.name = {},
	};
	printf("%p\n", &cmp_leaf.node_path);

	get_string(cmp_leaf.name);

	STACK_CTOR(&cmp_leaf.node_path, DEFAULT_STARTER_CAPACITY);

	struct B_tree_node *found_node_1 =
		search_for_node(root, &cmp_leaf); // struct param

	if(found_node_1 == NULL)
	{
		SAY_TO_PLAYER("There is no such leaf as %s.", cmp_leaf.name);
		cmp_leaf = {};
		return cmp_leaf;
	}

	return cmp_leaf;
}

error_t tell_similarities(struct Leaf_w_path *cmp_leafs,
					      struct Current_tree_position *current_pos)
{
	if(FIRST_TURN == SECOND_TURN)
	{
		SAY_TO_PLAYER("Similarities:");
	}
	while(FIRST_TURN == SECOND_TURN)
	{
		if(FIRST_TURN == 0)
		{
			SAY_TO_PLAYER("NOT %s", CURRENT_NODE->data);
			CURRENT_NODE = CURRENT_NODE->left;
		}
		else if(FIRST_TURN == 1)
		{
			SAY_TO_PLAYER("%s", CURRENT_NODE->data);
			CURRENT_NODE = CURRENT_NODE->right;
		}
		else
		{
			SAY_TO_PLAYER("Invalid stack value: %d", FIRST_TURN);

			return INVALID_NODE_PATH_TURN;
		}

		NODE_PATH_ID++;
	}

	return AKI_ALL_GOOD;
}

void tell_difference(struct Leaf_w_path *cmp_leafs,
					 struct Current_tree_position *current_pos)
{
	SAY_TO_PLAYER("Difference:");
	if(FIRST_TURN > SECOND_TURN)
	{
		SAY_TO_PLAYER("%s is %s but %s is not",
			cmp_leafs[FIRST].name, CURRENT_NODE->data,
			cmp_leafs[SECOND].name);
	}
	else
	{
		SAY_TO_PLAYER("%s is %s but %s is not",
			cmp_leafs[SECOND].name, CURRENT_NODE->data,
			cmp_leafs[FIRST].name);
	}

}

#define NODE_PATH cmp_leaf->node_path

error_t tell_additional_info(const struct Leaf_w_path *cmp_leaf,
										struct Current_tree_position *current_pos)
{
	struct Current_tree_position old_current_pos =
	{
		.current_node = CURRENT_NODE,
		.node_path_ID = NODE_PATH_ID,
	};

	if(NODE_PATH.data[NODE_PATH_ID] == (int)RIGHT_TURN)
	{
		CURRENT_NODE = CURRENT_NODE->right;
	}
	else if(NODE_PATH.data[NODE_PATH_ID] == (int)LEFT_TURN)
	{
		CURRENT_NODE = CURRENT_NODE->left;
	}
	else
	{
		return INVALID_NODE_PATH_TURN;
	}

	NODE_PATH_ID++;

	if(NODE_PATH_ID < NODE_PATH.size)
	{
		SAY_TO_PLAYER("In addition %s is:", cmp_leaf->name);
	}

	describe_leaf(cmp_leaf, current_pos); //handle error

	CURRENT_NODE = old_current_pos.current_node;
	NODE_PATH_ID = old_current_pos.node_path_ID;

	return AKI_ALL_GOOD;
}

error_t start_akinator(struct B_tree *btr)
{
	struct Guess_result guess_result = guess_leaf(btr);

	struct B_tree_node *final_guess = get_final_guess(&guess_result);

	SAY_TO_PLAYER("Is your guess %s?", final_guess->data);

	char player_answer = get_menu_option_answer();

	process_guess_validation(player_answer, &guess_result, btr);

	return AKI_ALL_GOOD;
}

error_t show_data_base(struct B_tree *btr)
{
	SAY_TO_PLAYER("Generating data base...");

	system("rm b_tree_graphic_dump.dot");
	system("rm b_tree_graphic_dump.png");

	generate_code_for_graphic_dump(btr);

	system("dot -Tpng b_tree_graphic_dump.dot"
			" -o b_tree_graphic_dump.png -Gdpi=100");
	system("open b_tree_graphic_dump.png");

	return AKI_ALL_GOOD;
}

error_t describe_leaf(const struct Leaf_w_path *cmp_leaf,
					  struct Current_tree_position *current_pos)
{
	while(NODE_PATH_ID < NODE_PATH.size)
	{
		if(NODE_PATH.data[NODE_PATH_ID] == 0)
		{
			SAY_TO_PLAYER("NOT %s", CURRENT_NODE->data);
			CURRENT_NODE = CURRENT_NODE->left;
		}
		else if(NODE_PATH.data[NODE_PATH_ID] == 1) // get_current_turn
		{
			SAY_TO_PLAYER("%s", CURRENT_NODE->data);
			CURRENT_NODE = CURRENT_NODE->right;
		}
		else
		{
			SAY_TO_PLAYER("Invalid stack value: %d",
			NODE_PATH.data[NODE_PATH_ID]);
			return INVALID_NODE_PATH_TURN;
		}

		NODE_PATH_ID++;
	}

	return AKI_ALL_GOOD;
}

error_t describe_desired_leaf(struct B_tree *btr)
{
	struct Leaf_w_path leaf_w_path = {};

	SAY_TO_PLAYER("Who you wanna describe?");

	get_string(leaf_w_path.name);

	STACK_CTOR(&(leaf_w_path.node_path), DEFAULT_STARTER_CAPACITY);

	struct B_tree_node *found_node =
		search_for_node(btr->root, &leaf_w_path);

	if(found_node != FREE_NODE)
	{
		struct Current_tree_position current_pos =
		{
			.node_path_ID = 0,
			.current_node = btr->root,
		};

		describe_leaf(&leaf_w_path, &current_pos);
	}
	else
	{
		SAY_TO_PLAYER("There is no such leaf as %s.\n", leaf_w_path.name);
	}

	stack_dtor(&leaf_w_path.node_path);

	return AKI_ALL_GOOD;
}

error_t compare_leafs(struct B_tree *btr)
{
	SAY_TO_PLAYER("Who you wanna compare?");

	struct Leaf_w_path cmp_leafs[2] = {};

	printf("First compared leaf: \n");
	cmp_leafs[FIRST]  =  get_Leaf_w_path(btr->root);

	printf("Second compared leaf: \n");
	cmp_leafs[SECOND] = get_Leaf_w_path(btr->root);



	struct Current_tree_position current_pos =
	{
		.node_path_ID = 0,
		.current_node = btr->root,
	};

	error_t error_code = tell_similarities(cmp_leafs, &current_pos);
	HANDLE_ERROR

	tell_difference(cmp_leafs, &current_pos);

	error_code = tell_additional_info(&cmp_leafs[FIRST],  &current_pos);
	HANDLE_ERROR

	error_code = tell_additional_info(&cmp_leafs[SECOND], &current_pos);
	HANDLE_ERROR

	return AKI_ALL_GOOD;
}

#undef NODE_PATH
#undef NODE_PATH_ID
#undef FIRST_TURN
#undef SECOND_TURN
#undef CURRENT_NODE
