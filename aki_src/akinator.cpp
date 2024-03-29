#include <stdio.h>
#include <math.h>
#include <string.h>

#include "akinator.h"
#include "akinator_helpers.h"
#include "../buffer_process_src/buffer_process.h"

char line_to_say[AKINATOR_LINE_MAX_LEN];

struct node_charachteristics
{
	char *name;
	char *color;
	char *label;
};

struct Universal_ret b_tree_ctor(size_t starter_capacity)
{
	struct Universal_ret result =
	{
		.error_code = AKI_ALL_GOOD,
		.second_arg.btr = (struct B_tree *)calloc(1, sizeof(struct B_tree)),
	};

	struct B_tree *btr = result.second_arg.btr;

	if(btr == NULL)
	{
		result.error_code = UNABLE_TO_ALLOCATE;
		log("Unable to allocate result.second_arg.btr\n");

		return result;
	}

	btr->capacity = starter_capacity;

	btr->node = (struct B_tree_node *)calloc(btr->capacity, sizeof(struct B_tree_node));

	mark_b_tree_nodes_as_free(btr->node, btr->capacity, 0);
	btr->node[0].right = &(btr->node[1]);

	btr->root = FREE_NODE;

	btr->current_free = &(btr->node[0]);

	b_tree_dump(btr, result.error_code, __func__);

	return result;
}

struct Universal_ret generate_code_for_graphic_dump(struct B_tree *btr)
{
	struct Universal_ret result =
	{
		.error_code = AKI_ALL_GOOD,
		.second_arg.file_ptr = fopen("b_tree_graphic_dump.dot", "w"),
	};

	if(result.second_arg.file_ptr == NULL)
	{
		fprintf(stderr, "Unable to open list_graphic_dump.dot\n");

		result.error_code = AKI_ALL_GOOD;
		return result;
	}

	#define WRITE_TO_DUMP_FILE(...) fprintf(result.second_arg.file_ptr, __VA_ARGS__);

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

	if(btr->root != FREE_NODE)
	{
		print_regular_nodes(btr->root, &nd_description, result.second_arg.file_ptr);
	}

	connect_nodes(btr->root, result.second_arg.file_ptr);

	WRITE_TO_DUMP_FILE("}");

	#undef WRITE_TO_DUMP_FILE

	free(nd_description.label);
	fclose(result.second_arg.file_ptr);

	return result;
}

#define DUMP_W_COND(cond, ... )\
	if(cond)\
	{\
		log(__VA_ARGS__);\
	}\
	else\
	{\
		log("FAILED CONDITION: ");\
		log(#cond);\
		log("\n");\
	}

error_t b_tree_dump(const struct B_tree *btr, error_t error_code, const char *func_name)
{
	error_t dump_function_error_code = AKI_ALL_GOOD;

	DUMP_W_COND(func_name, "dump called from %s function:\n", func_name);

	if(btr == NULL)
	{
		log("FATAL_ERROR: pointer to the list structure is NULL\n");
		log("\tbtr[%p]\n", btr);

		return BTR_NULL_PTR;
	}

	#define ERROR_CHECK(err_type)\
		if(error_code & err_type)\
		{\
			log("-");\
			log(#err_type);\
			log("\n");\
		}\

	DUMP_W_COND(btr->node, "ID:");
	DUMP_W_COND(&(btr->node[0].data), "   data:");
	DUMP_W_COND(&(btr->node[0].left), "               left:");
	DUMP_W_COND(&(btr->node[0].right), "        right:");
	DUMP_W_COND(true, "        addresses:\n");
	for(size_t ID = 0; ID < btr->capacity; ID++)
	{
		log("%lu     ", ID);
		log("%s", btr->node[ID].data);
		DUMP_W_COND(&((btr->node)[ID].left), "%20.p", (btr->node)[ID].left);
		DUMP_W_COND(&((btr->node)[ID].right), "%13.p", (btr->node)[ID].right);
		DUMP_W_COND(&(btr->node[ID]), "%20.p", &(btr->node[ID]));

		log("\n");

	}
	DUMP_W_COND(true, "root: %p\n", btr->root);
	DUMP_W_COND(&(btr->current_free), "current free: %p\n", btr->current_free);
	DUMP_W_COND(&(btr->capacity), "capacity: %lu\n", btr->capacity);

	ERROR_CHECK(UNABLE_TO_ALLOCATE);
	ERROR_CHECK(AKI_ALL_GOOD);
	ERROR_CHECK(BTR_NULL_PTR);

	return dump_function_error_code;
}

#undef ERROR_CHECK
#undef DUMP_W_COND

struct Universal_ret construct_b_tree(const char *data_base_file_name)
{
	log("construct_b_tree log:\n");
	struct Universal_ret result =
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



	result.second_arg.btr = b_tree_ctor(amount_of_nodes + SPACE_FOR_NEW_NODES).second_arg.btr;

	int current_node_ID = 0;
	result.second_arg.btr->root = &(result.second_arg.btr->node[current_node_ID]);
	result.second_arg.btr->capacity = amount_of_nodes + SPACE_FOR_NEW_NODES;
	read_node(&data_base_lexemes, &(current_node_ID), result.second_arg.btr->node);

	result.second_arg.btr->current_free = &(result.second_arg.btr->node[current_node_ID + 1]);

	free(data_base_lexemes.buf);
	free(data_base_buf_w_info.buf);

	fclose(data_base);

	return result;
}

void op_del(struct B_tree *btr)
{
	btr->capacity = 0;
	btr->current_free = FREE_NODE;
	btr->root = FREE_NODE;

	free_memory(btr);

	log("binary_tree has been deleted\n");
}

struct Universal_ret create_data_base(struct B_tree *btr, const char *file_name)
{
	struct Universal_ret result =
	{
		.error_code = AKI_ALL_GOOD,
		.second_arg.file_ptr = fopen(file_name, "w"),
	};

	if(result.second_arg.file_ptr == NULL)
	{
		perror("ERROR:");
		result.error_code = AKI_ALL_GOOD;

		return result;
	}

	print_node(btr->root, result.second_arg.file_ptr);

	fclose(result.second_arg.file_ptr);

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
	if(parent_node == FREE_NODE)
	{
		return PARENT_NODE_IS_FREE;
	}
	if(is_left_child)
	{
		node_delete(parent_node->left);
		parent_node->left = FREE_NODE;
	}
	else if(!is_left_child)
	{
		node_delete(parent_node->right);
		parent_node->right = FREE_NODE;
	}
	else
	{
		return INVALID_VALUE;
	}

	return AKI_ALL_GOOD;
}

struct Universal_ret create_node(struct B_tree *btr, const b_tree_elem_t value)
{
	struct Universal_ret result =
	{
		.error_code = AKI_ALL_GOOD,
		.second_arg.node = FREE_NODE,
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

    result.second_arg.node = btr->current_free;
    btr->current_free = result.second_arg.node->right;

	error_t assigning_error = assign_value(&(result.second_arg.node->data), value);
	if(assigning_error != AKI_ALL_GOOD)
	{
		result.error_code = assigning_error;
		result.second_arg.node = FREE_NODE;
		return result;
	}
    result.second_arg.node->left = FREE_NODE;
    result.second_arg.node->right = FREE_NODE;

    return result;
}

error_t add_child(struct B_tree_node *parent, struct B_tree_node *child, bool is_right_child)
{
	if(parent == FREE_NODE || child == FREE_NODE)
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
	error_t error_code = AKI_ALL_GOOD;

	while(true)
	{
		enum Menu_option player_answer = QUIT;

		struct Universal_ret ctor_result = construct_b_tree(data_base_file_name);

		struct B_tree *btr = ctor_result.second_arg.btr;

		printf("\t\tAKINATOR MENU\n");
		printf("\t[s] - to start the game\n");
		printf("\t[b] - to show data base\n");
		printf("\t[d] - to describe a certain leaf\n");
		printf("\t[c] - to comapre leafs\n");
		printf("\t[q] - to quit\n");

		player_answer = get_menu_option_answer();

		switch(player_answer)
		{
			case QUIT:
			{
				SAY_TO_PLAYER("Okay bye then...");

				return AKI_ALL_GOOD;
			}
			case START:
			{
				start_akinator(btr);

				break;
			}
			case SHOW_DATA_BASE:
			{
				show_data_base(btr);

				break;
			}
			case DESCRIBE:
			{
				describe_desired_leaf(btr);

				break;
			}
			case COMPARE:
			{
				compare_leafs(btr);

				break;
			}
			default:
			{
				SAY_TO_PLAYER("Unknown menu option![%c] "
					   "Please chose from options given in square brackets.\n", player_answer);
				break;
			}
		}

		printf("[m] - to go to the main menu\n");
		printf("[q] - to quit\n");

		player_answer = get_menu_option_answer();

		switch(player_answer)
		{
			case 'm':
			{
				break;
			}
			case 'q':
			{
				SAY_TO_PLAYER("Okay bye then...");

				return AKI_ALL_GOOD;
			}
			default:
			{
				printf("Unknown menu option![%c] "
				"But im going to the main menu anyways lol.\n", player_answer);
				break;
			}
		}
	}


	//dead_zone
	printf("Something is definitly very wrong "
		   "because the program is on %lu line\n", __LINE__);
	exit(EXIT_FAILURE);
}

struct B_tree_node *search_for_node(struct B_tree_node *node, struct Leaf_w_path *leaf_w_path)
{

	if(node == FREE_NODE)
	{
		STACK_POP(&leaf_w_path->node_path);
		return FREE_NODE;
	}

	if(!strncmp(node->data, leaf_w_path->name, strlen(leaf_w_path->name)))
	{
		return node;
	}

	struct B_tree_node *found_node = FREE_NODE;

	STACK_PUSH(&leaf_w_path->node_path, 0);
	found_node = search_for_node(node->left, leaf_w_path);
	if(found_node != FREE_NODE)
	{
		return found_node;
	}

	STACK_PUSH(&leaf_w_path->node_path, 1);
	found_node = search_for_node(node->right, leaf_w_path);
	if(found_node != FREE_NODE)
	{
		return found_node;
	}

	STACK_POP(&leaf_w_path->node_path);
	return FREE_NODE;
}

void log(const char *fmt, ...)
{
    static FILE *log_file = fopen("log.txt", "w");

    if (log_file == NULL)
	{
        perror("Error opening log_file");
        return;
    }

    va_list args = NULL;

    va_start(args, fmt);

    vfprintf(log_file, fmt, args);

    va_end(args);
}
