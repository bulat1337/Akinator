#include <stdio.h>
#include <math.h>
#include <string.h>

#include "akinator.h"
#include "akinator_helpers.h"
#include "../buffer_process_src/buffer_process.h"

FILE *log_file;

struct node_charachteristics
{
	char *name;
	char *color;
	char *label;
};

struct B_tree_ctor_result b_tree_ctor(size_t starter_capacity)
{
	struct B_tree_ctor_result result =
	{
		.error_code = AKI_ALL_GOOD,
		.new_btr = (struct B_tree *)calloc(1, sizeof(struct B_tree)), // проверка if NULL есть
	};

	log_file = fopen("log_file.txt", "w");
	if(log_file == NULL)
	{
		result.error_code = AKI_ALL_GOOD;
		log_file = stdout;
		WRITE_IN_LOG_FILE("Unable to open log_file.txt\n");
		return result;
	}

	struct B_tree *btr = result.new_btr;

	if(btr == NULL)
	{
		result.error_code = UNABLE_TO_ALLOCATE;
		WRITE_IN_LOG_FILE("Unable to allocate result.new_btr\n");

		return result;
	}

	btr->capacity = starter_capacity;

	btr->node = (struct B_tree_node *)calloc(btr->capacity, sizeof(struct B_tree_node));

	mark_b_tree_nodes_as_free(btr->node, btr->capacity, 0);
	btr->node[0].right = &(btr->node[1]);

	btr->root = NULL;

	btr->current_free = &(btr->node[0]);

	b_tree_dump(btr, result.error_code, __func__);

	return result;
}

struct Generate_code_for_graphic_dump_result generate_code_for_graphic_dump(struct B_tree *btr)
{
	struct Generate_code_for_graphic_dump_result result =
	{
		.error_code = AKI_ALL_GOOD,
		.graphic_dump_code_file_ptr = fopen("b_tree_graphic_dump.dot", "w"),
	};

	if(result.graphic_dump_code_file_ptr == NULL)
	{
		fprintf(stderr, "Unable to open list_graphic_dump.dot\n");

		result.error_code = AKI_ALL_GOOD;
		return result;
	}

	#define WRITE_TO_DUMP_FILE(...) fprintf(result.graphic_dump_code_file_ptr, __VA_ARGS__);

	WRITE_TO_DUMP_FILE("digraph BinaryTree {\n"
	"bgcolor = \"#BAF0EC\";\n"
	"edge[minlen = 3, penwidth = 3; color = \"black\"];\n"
	"node[shape = \"rectangle\", style = \"rounded, filled\",\n"
	"\tfillcolor = \"#F08080\",\n"
	"\tfontsize = 30,\n"
	"\theight = 3,\n"
	"\tpenwidth = 5, color = \"#F0E68C\"];\n");

	struct node_charachteristics nd_description =
	{
		.color = "#F08080",
		.name = "node_",
	};

	nd_description.label = (char *)calloc((NODE_LABEL_STR_SIZE), sizeof(char));
	if(nd_description.label == NULL)
	{
		fprintf(stderr, "Unable to allocate nd_description.label\n");
		result.error_code = UNABLE_TO_ALLOCATE;
		return result;
	}

	//	print_list_manager
	WRITE_TO_DUMP_FILE("{rank = min;\n"
    	"\tlist_manager [shape = Mrecord, fillcolor = \"#FFF825\", "
		"label = \"{ROOT: %p| CUR_FREE: %p | CAPACITY: %lu}\"];\n"
	"}\n", btr->root, btr->current_free, btr->capacity);

	if(btr->root != NULL)
	{
		print_regular_nodes(btr->root, &nd_description, result.graphic_dump_code_file_ptr);
	}

	connect_nodes(btr->root, result.graphic_dump_code_file_ptr);

	WRITE_TO_DUMP_FILE("}");

	#undef WRITE_TO_DUMP_FILE

	free(nd_description.label);
	fclose(result.graphic_dump_code_file_ptr);

	return result;
}

error_t b_tree_dump(const struct B_tree *btr, error_t error_code, const char *func_name)
{
	#define DUMP_W_COND(cond, ... )\
	if(cond)\
	{\
		fprintf(log_file, __VA_ARGS__);\
	}\
	else\
	{\
		fprintf(log_file, "FAILED CONDITION: ");\
		fprintf(log_file, #cond);\
		fprintf(log_file, "\n");\
	}
	error_t dump_function_error_code = AKI_ALL_GOOD;

	DUMP_W_COND(func_name, "dump called from %s function:\n", func_name);

	if(btr == NULL)
	{
		WRITE_IN_LOG_FILE("FATAL_ERROR: pointer to the list structure is NULL\n");
		WRITE_IN_LOG_FILE("\tbtr[%p]\n", btr);

		return BTR_NULL_PTR;
	}

	#define ERROR_CHECK(err_type)\
		if(error_code & err_type)\
		{\
			WRITE_IN_LOG_FILE("-");\
			WRITE_IN_LOG_FILE(#err_type);\
			WRITE_IN_LOG_FILE("\n");\
		}\

	DUMP_W_COND(btr->node, "ID:");
	DUMP_W_COND(&(btr->node[0].data), "   data:");
	DUMP_W_COND(&(btr->node[0].left), "               left:");
	DUMP_W_COND(&(btr->node[0].right), "        right:");
	DUMP_W_COND(true, "        addresses:\n");
	for(size_t ID = 0; ID < btr->capacity; ID++)
	{
		WRITE_IN_LOG_FILE("%lu     ", ID);
		WRITE_IN_LOG_FILE("%s", btr->node[ID].data);
		DUMP_W_COND(&((btr->node)[ID].left), "%20.p", (btr->node)[ID].left);
		DUMP_W_COND(&((btr->node)[ID].right), "%13.p", (btr->node)[ID].right);
		DUMP_W_COND(&(btr->node[ID]), "%20.p", &(btr->node[ID]));

		WRITE_IN_LOG_FILE("\n");

	}
	DUMP_W_COND(true, "root: %p\n", btr->root);
	DUMP_W_COND(&(btr->current_free), "current free: %p\n", btr->current_free);
	DUMP_W_COND(&(btr->capacity), "capacity: %lu\n", btr->capacity);

	ERROR_CHECK(UNABLE_TO_ALLOCATE);
	ERROR_CHECK(AKI_ALL_GOOD);
	ERROR_CHECK(BTR_NULL_PTR);

	#undef ERROR_CHECK
	#undef DUMP_W_COND

	return dump_function_error_code;
}

struct Construct_b_tree_result construct_b_tree(const char *data_base_file_name)
{
	WRITE_IN_LOG_FILE("construct_b_tree log:\n");
	Construct_b_tree_result result =
	{
		.error_code = AKI_ALL_GOOD,
	};

	FILE *data_base = fopen(data_base_file_name, "r");
	if (data_base == NULL)
	{
		fprintf(stderr, "I cant open ur data base(%s)!\n", data_base_file_name);
		result.error_code = AKI_UNABLE_TO_OPEN_FILE;

		return result;
	}

	struct Buf_w_carriage_n_len data_base_buf_w_info = {};

	data_base_buf_w_info.length = get_file_length(data_base);
	data_base_buf_w_info.buf = (char *)calloc(data_base_buf_w_info.length, sizeof(char));
	if(data_base_buf_w_info.buf == NULL)
	{
		perror("ERROR:");
		result.error_code = UNABLE_TO_ALLOCATE;
		return result;
	}

	size_t read_elems_amount =
		fread(data_base_buf_w_info.buf, sizeof(char), data_base_buf_w_info.length, data_base);
	if(read_elems_amount != data_base_buf_w_info.length)
	{
		if(ferror(data_base))
		{
			perror("ERROR:");

			result.error_code = UNEXPECTED_WRITTEN_ELEMS;

			return result;
		}
		else if(feof(data_base))
		{
			fprintf(stderr ,
			"read_elems_amount != data_base_buf_w_info.length because end of the file "
			"EOF was reached.\n");
		}
	}

	size_t amount_of_lexems = count_buf_lexemes(data_base_buf_w_info);
	struct Lexemes_w_carriage data_base_lexemes =
	{
		.buf = (char * *)calloc(amount_of_lexems, sizeof(char *)),
		.carriage = 0,
	};

	if(data_base_lexemes.buf == NULL)
	{
		perror("ERROR:");
		result.error_code = UNABLE_TO_ALLOCATE;
		return result;
	}

	lexemes_ptr_arranger(data_base_lexemes.buf, data_base_buf_w_info);

	size_t amount_of_nodes = get_amount_of_nodes(data_base_lexemes.buf, amount_of_lexems);



	result.btr = b_tree_ctor(amount_of_nodes + SPACE_FOR_NEW_NODES).new_btr;

	int current_node_ID = 0;
	result.btr->root = &(result.btr->node[current_node_ID]);
	result.btr->capacity = amount_of_nodes + SPACE_FOR_NEW_NODES;
	read_node(&data_base_lexemes, &(current_node_ID), result.btr->node);

	result.btr->current_free = &(result.btr->node[current_node_ID + 1]);

	free(data_base_lexemes.buf);
	free(data_base_buf_w_info.buf);

	fclose(data_base);

	return result;
}

void op_del(struct B_tree *btr)
{
	btr->capacity = 0;
	btr->current_free = NULL;
	btr->root = NULL;
	fclose(log_file);

	free_memory(btr);

	WRITE_IN_LOG_FILE("binary_tree has been deleted\n");
}

struct Create_data_base_result create_data_base(struct B_tree *btr, const char *file_name)
{
	struct Create_data_base_result result=
	{
		.error_code = AKI_ALL_GOOD,
		.data_base = fopen(file_name, "w"),
	};

	if(result.data_base == NULL)
	{
		perror("ERROR:");
		result.error_code = AKI_ALL_GOOD;

		return result;
	}

	print_node(btr->root, result.data_base);

	fclose(result.data_base);

	return result;
}

error_t b_tree_verifier(struct B_tree *btr)
{
	int error_code = AKI_ALL_GOOD;
	if(btr == NULL)
	{
		error_code |= B_TREE_NULL_PTR;
	}

	if(btr->node == NULL)
	{
		error_code |= B_TREE_NODE_NULL_PTR;
	}

	return (error_t)error_code;
}

error_t destroy_subtree(struct B_tree_node *parent_node, bool is_left_child)
{
	if(parent_node == NULL)
	{
		return PARENT_NODE_IS_FREE;
	}
	if(is_left_child)
	{
		node_delete(parent_node->left);
		parent_node->left = NULL;
	}
	else if(!is_left_child)
	{
		node_delete(parent_node->right);
		parent_node->right = NULL;
	}
	else
	{
		return INVALID_VALUE;
	}

	return AKI_ALL_GOOD;
}

struct Create_node_result create_node(struct B_tree *btr, const b_tree_elem_t value)
{
	struct Create_node_result result =
	{
		.error_code = AKI_ALL_GOOD,
		.created_node = NULL,
	};

    if((result.error_code = b_tree_verifier(btr)) != AKI_ALL_GOOD)
	{
		b_tree_dump(btr, result.error_code, __func__);
	}

    if (btr->current_free == FREE_NODE)
    {
		btr->capacity *= REALLOC_COEFF;
		printf("New capacity: %lu\n", btr->capacity);
        result.error_code = b_tree_realloc(btr);
        if (result.error_code != AKI_ALL_GOOD)
        {
            return result;
        }
    }

    result.created_node = btr->current_free;
    btr->current_free = result.created_node->right;

	error_t assigning_error = assign_value(&(result.created_node->data), value);
	if(assigning_error != AKI_ALL_GOOD)
	{
		result.error_code = assigning_error;
		result.created_node = NULL;
		return result;
	}
    result.created_node->left = NULL;
    result.created_node->right = NULL;

    return result;
}

error_t add_child(struct B_tree_node *parent, struct B_tree_node *child, bool is_right_child)
{
	if(parent == NULL || child == NULL)
	{
		return B_TREE_NODE_NULL_PTR;
	}

	if(is_right_child)
	{
		printf("%s's right child is now %s\n", parent->data, child->data);
		parent->right = child;
	}
	else
	{
		printf("%s's left child is now %s\n", parent->data, child->data);
		parent->left = child;
	}

	return AKI_ALL_GOOD;
}

error_t set_root(struct B_tree *btr, int root_ID)
{
	if(root_ID > btr->capacity || root_ID < 0)
	{
		return AKI_ALL_GOOD;
	}

	btr->root = &(btr->node[root_ID]);

	return AKI_ALL_GOOD;
}

error_t play_akinator(const char *data_base_file_name)
{
	#define AFTER_TASK_CHOISE														\
		printf("[m] - to go to the main menu\n"); 									\
		printf("[q] - to quit\n");													\
																					\
		player_answer = get_menu_option_answer();		    						\
																					\
		switch(player_answer)														\
		{																			\
			case 'm':																\
			{																		\
				break;  								    						\
			}																		\
			case 'q':																\
			{																		\
				return AKI_ALL_GOOD;												\
				break;																\
			}																		\
			default:																\
			{																		\
				printf("Unknown menu option![%c] "									\
				"But im going to the main menu anyways lol.\n", player_answer);		\
				break;																\
			}																		\
		}

	while(true)
	{
		char player_answer = 'q';

		struct Construct_b_tree_result ctor_result = construct_b_tree("data_base.txt");

		if(ctor_result.error_code != AKI_ALL_GOOD)
		{
			b_tree_dump(ctor_result.btr, ctor_result.error_code, __func__);

			return ctor_result.error_code;
		}

		struct B_tree *btr = ctor_result.btr;

		printf("\t\tAKINATOR MENU\n");
		printf("\t[s] - to start the game\n");
		printf("\t[b] - to show data base\n");
		printf("\t[d] - to describe a certain leaf\n");
		printf("\t[c] - to comapre leafs\n");
		printf("\t[q] - to quit\n");

		player_answer = get_menu_option_answer();

		switch(player_answer)
		{
			case 'q':
			{
				return AKI_ALL_GOOD;
				break;
			}
			case 's':
			{
				printf("You already guessed someone? "
					   "Answer the following questions and I'll tell you who it is!\n");


				struct B_tree_node finctitious_root_parent =
				{
					.data = "dont_look_at_me",
					.left = btr->root,
					.right = btr->root,
				};

				struct Ask_question_result ask_result =
					ask_question(&finctitious_root_parent, true);


				struct B_tree_node *final_guess = get_final_guess(&ask_result);
				printf("Is your guess %s?\n", final_guess->data);

				player_answer = get_menu_option_answer();

				if(player_answer == 'y')
				{
					printf("Too slow, too weak, too easy\n");

					AFTER_TASK_CHOISE;
				}
				else
				{
					char new_leaf_data[MAX_NEW_NODE_DATA_STRLEN] = {};
					char new_question[MAX_NEW_NODE_DATA_STRLEN]  = {};

					printf("damn... what or who was it then?\n");

					get_string(new_leaf_data);

					printf("And how does he differ from %s?\n", final_guess->data);

					get_string(new_question);

					printf("U wanna add new leaf with the corresponding question to data base?\n");
					printf("New leaf: %s\n", new_leaf_data);
					printf("New question: %s\n", new_question);

					player_answer = get_menu_option_answer();

					if(player_answer == 'y')
					{
						struct B_tree_node *new_leaf =
							create_node(btr, new_leaf_data).created_node;
						struct B_tree_node *new_question_node =
							create_node(btr, new_question).created_node;

						add_child(new_question_node, final_guess, false);
						add_child(new_question_node, new_leaf, true);
						if(ask_result.is_right_child)
						{
							add_child(ask_result.parent, new_question_node, true);
						}
						else
						{
							add_child(ask_result.parent, new_question_node, false);
						}

						create_data_base(btr, "data_base.txt");

						printf("New leaf has been added to the tree\n");
					}
				}

				break;
			}
			case 'b':
			{
				printf("Generating data base...\n");
				system("rm b_tree_graphic_dump.dot");
				system("rm b_tree_graphic_dump.png");
				generate_code_for_graphic_dump(btr);
				system("dot -Tpng b_tree_graphic_dump.dot -o b_tree_graphic_dump.png -Gdpi=100");

				AFTER_TASK_CHOISE;

				break;
			}
			case 'd':
			{
				char desired_data[MAX_NEW_NODE_DATA_STRLEN] = {};

				printf("Who you wanna describe?\n");

				get_string(desired_data);

				struct Stack node_path = {};
				STACK_CTOR(&node_path, DEFAULT_STARTER_CAPACITY);

				struct B_tree_node *found_node =
					search_for_node(btr->root, desired_data, &node_path);

				if(found_node != NULL)
				{
					int current_turn = INDEX_POISON;
					struct B_tree_node *current_node = btr->root;
					for(size_t ID = 0; ID < node_path.size; ID++)
					{
						current_turn = node_path.data[ID];
						if(current_turn == 0)
						{
							printf("NOT ");
						}
						else if(current_turn == 1)
						{
							;
						}
						else
						{
							return INVALID_NODE_PATH_TURN;
						}

						printf("%s\n", current_node->data);

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

					}
				}
				else
				{
					printf("There is no such leaf as %s.\n", desired_data);
				}

				AFTER_TASK_CHOISE;

				break;
			}
			case 'c':
			{
				printf("Who you wanna compare?\n");


				char compared_leaf_1[MAX_NEW_NODE_DATA_STRLEN] = {};
				printf("First compared leaf: \n");

				get_string(compared_leaf_1);

				struct Stack node_path_1 = {};
				STACK_CTOR(&node_path_1, DEFAULT_STARTER_CAPACITY);

				struct B_tree_node *found_node_1 =
					search_for_node(btr->root, compared_leaf_1, &node_path_1);

				if(found_node_1 == NULL)
				{
					printf("There is no such leaf as %s.\n", compared_leaf_1);
					break;
				}

				char compared_leaf_2[MAX_NEW_NODE_DATA_STRLEN] = {};

				printf("Second compared leaf: \n");

				get_string(compared_leaf_2);

				struct Stack node_path_2 = {};
				STACK_CTOR(&node_path_2, DEFAULT_STARTER_CAPACITY);

				struct B_tree_node *found_node_2 =
					search_for_node(btr->root, compared_leaf_2, &node_path_2);

				if(found_node_2 == NULL)
				{
					printf("There is no such leaf as %s.\n", compared_leaf_2);
					break;
				}

				//Similarities
				size_t stack_ID = 0;
				struct B_tree_node *current_node = btr->root;
				while(node_path_1.data[stack_ID] == node_path_2.data[stack_ID])
				{
					printf("Similatities:\n");
					if(node_path_1.data[stack_ID] == 0)
					{
						printf("NOT ");
					}
					else if(node_path_1.data[stack_ID] == 1)
					{
						;
					}
					else
					{
						printf("Invalid stack value: %d\n", node_path_1.data[stack_ID]);
						return INVALID_NODE_PATH_TURN;
					}

					printf("%s\n", current_node->data);


					if(node_path_1.data[stack_ID] == 0)
					{
						current_node = current_node->left;
					}
					else if(node_path_1.data[stack_ID] == 1)
					{
						current_node = current_node->right;
					}
					else
					{
						printf("Invalid stack value: %d\n", node_path_1.data[stack_ID]);
						return INVALID_NODE_PATH_TURN;
					}

					stack_ID++;
				}
				//

				//Difference
				struct B_tree_node *current_node_1 = current_node;
				struct B_tree_node *current_node_2 = current_node;
				printf("Difference:\n");

				if(node_path_1.data[stack_ID] > node_path_2.data[stack_ID])
				{
					printf("%s is %s but %s is not\n",
						compared_leaf_1, current_node->data, compared_leaf_2);
					current_node_1 = current_node_1->right;
					current_node_2 = current_node_2->left;
				}
				else
				{
					printf("%s is %s but %s is not\n",
						compared_leaf_2, current_node->data, compared_leaf_1);
					current_node_1 = current_node_1->left;
					current_node_2 = current_node_2->right;
				}
				stack_ID++;
				//

				//Additional_info_1
				size_t stack_ID_1 = stack_ID;
				size_t stack_ID_2 = stack_ID;

				if(stack_ID_1 < node_path_1.size)
				{
					printf("In addition %s is:\n", compared_leaf_1);
				}
				while(stack_ID_1 < node_path_1.size)
				{
					if(node_path_1.data[stack_ID_1] == 0)
					{
						printf("NOT ");
					}
					else if(node_path_1.data[stack_ID_1] == 1)
					{
						;
					}
					else
					{
						printf("Invalid stack value: %d\n", node_path_1.data[stack_ID_1]);
						return INVALID_NODE_PATH_TURN;
					}

					printf("%s\n", current_node_1->data);

					if(node_path_1.data[stack_ID_1] == 0)
					{
						current_node_1 = current_node_1->left;
					}
					else if(node_path_1.data[stack_ID_1] == 1)
					{
						current_node_1 = current_node_1->right;
					}
					else
					{
						return INVALID_NODE_PATH_TURN;
					}

					stack_ID_1++;
				}

				AFTER_TASK_CHOISE;
				break;
			}
			default:
			{
				printf("Unknown menu option![%c] "
					   "Please chose from options given in square brackets.\n", player_answer);
				break;
			}
		}
	}

	#undef AFTER_TASK_CHOISE

	//dead_zone
	exit(EXIT_FAILURE);
}

struct B_tree_node *search_for_node(struct B_tree_node *node, const char *data,
									struct Stack *node_path_stk)
{

	if(node == NULL)
	{
		STACK_POP(node_path_stk);
		return NULL;
	}

	if(!strncmp(node->data, data, strlen(data)))
	{
		return node;
	}

	struct B_tree_node *found_node = NULL;

	STACK_PUSH(node_path_stk, 0);
	found_node = search_for_node(node->left, data, node_path_stk);
	if(found_node != NULL)
	{
		return found_node;
	}

	STACK_PUSH(node_path_stk, 1);
	found_node = search_for_node(node->right, data, node_path_stk);
	if(found_node != NULL)
	{
		return found_node;
	}

	STACK_POP(node_path_stk);
	return NULL;
}
