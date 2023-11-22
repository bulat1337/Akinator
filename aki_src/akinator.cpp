#include <stdio.h>

#include "akinator.h"
#include "akinator_helpers.h"

FILE *log_file;

struct Op_new_result op_new(size_t starter_capacity)
{
	struct Op_new_result result =
	{
		.error_code = ALL_GOOD,
		.new_btr = (struct B_tree *)calloc(1, sizeof(struct B_tree)), // проверка if NULL есть
	};

	log_file = fopen("log_file.txt", "w");
	if(log_file == NULL)
	{
		result.error_code = UNABLE_TO_OPEN_FILE;
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

	btr->root = FREE_ELEM_MARKER;

	btr->current_free = 0;

	b_tree_dump(btr, result.error_code, __func__);

	return result;
}

struct Generate_code_for_graphic_dump_result generate_code_for_graphic_dump(struct B_tree *btr)
{
	struct Generate_code_for_graphic_dump_result result =
	{
		.error_code = ALL_GOOD,
		.graphic_dump_code_file_ptr = fopen("b_tree_graphic_dump.dot", "w"),
	};

	if(result.graphic_dump_code_file_ptr == NULL)
	{
		fprintf(stderr, "Unable to open list_graphic_dump.dot\n");

		result.error_code = UNABLE_TO_OPEN_FILE;
		return result;
	}

	#define WRITE_TO_DUMP_FILE(...) fprintf(result.graphic_dump_code_file_ptr, __VA_ARGS__);

	WRITE_TO_DUMP_FILE("digraph BinaryTree {\n"
	"bgcolor = \"#FFDAB9\";\n"
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
    	"\tlist_manager [shape = Mrecord, fillcolor = \"#1E90FF\", "
		"label = \"{ROOT: %d| CUR_FREE: %d | CAPACITY: %lu}\"];\n"
	"}\n", btr->root, btr->current_free, btr->capacity);

	//print

// 	if(btr->root >= 0)
// 	{
// 		//	print root
// 		nd_description.color = "#00FA9A";	// LIGHT_GREEN
//
// 		snprintf(nd_description.label, NODE_LABEL_STR_SIZE,
// 					"{ID: %d | val: %.2lf |{L: %d | R: %d}}",
// 					btr->root, btr->node[btr->root].data,
// 					btr->node[btr->root].left, btr->node[btr->root].right);
//
// 		create_node(btr->root, &nd_description, result.graphic_dump_code_file_ptr);
// 	}

	if(btr->root >= 0)
	{
		print_regular_nodes(btr->root, btr->node,
							&nd_description, result.graphic_dump_code_file_ptr);
	}

	connect_nodes(btr->root, btr->node, result.graphic_dump_code_file_ptr);

	WRITE_TO_DUMP_FILE("}");

	#undef WRITE_TO_DUMP_FILE

	fclose(result.graphic_dump_code_file_ptr);
	return result;
}

struct B_tree_insert_result b_tree_insert(struct B_tree *btr, b_tree_elem_t value)
{
	struct B_tree_insert_result result =
	{
		.error_code = ALL_GOOD,
	};

	if(btr->current_free == FREE_ELEM_MARKER)
	{
		WRITE_IN_LOG_FILE("btr->current_free[%d] == FREE_ELEM_MARKER so im reallocing\n",
							btr->current_free);
		btr->capacity *= REALLOC_COEFF;
		b_tree_realloc(btr);
	}

	if(btr->current_free == 0)
	{
		btr->root = btr->current_free;

		btr->node[btr->root].data = value;


		btr->current_free = btr->node[btr->current_free].right;
		printf("btr->current_free is now %d\n", btr->current_free);

		btr->node[btr->root].left = FREE_ELEM_MARKER;
		btr->node[btr->root].right = FREE_ELEM_MARKER;

		result.inserted_elem_ID = btr->root;

		return result;
	}

	WRITE_IN_LOG_FILE("\nSEARCHING PLACE FOR %.2lf\n", value);
	WRITE_IN_LOG_FILE("CURRENT FREE IS: %d\n", btr->current_free);
	int current = btr->root;
	WRITE_IN_LOG_FILE("current: %d\n", current);
	while(true)
	{
		// Сравниваем значение нового узла с текущим узлом в дереве
		if (value < btr->node[current].data)
		{
			// Если значение нового узла меньше текущего, идем влево
			WRITE_IN_LOG_FILE( "value[%.2lf] < btr->node[current].data[%.2lf]\n",
								value, btr->node[current].data);

			if (btr->node[current].left == FREE_ELEM_MARKER)
			{
				// Если нет левого потомка, вставляем новый узел здесь
				WRITE_IN_LOG_FILE( "btr->node[current].left[%d] == FREE_ELEM_MARKER\n",
								   btr->node[current].left);

				btr->node[current].left = btr->current_free;
				break;
			} else
			{
				// Иначе, переходим к левому потомку
				WRITE_IN_LOG_FILE( "btr->node[current].left[%d] != FREE_ELEM_MARKER\n",
								   btr->node[current].left);

				current = btr->node[current].left;
				WRITE_IN_LOG_FILE("current is now: %d\n", current);
			}
		} else
		{
			// Если значение нового узла больше или равно текущему, идем вправо
			WRITE_IN_LOG_FILE( "value[%.2lf] >= btr->node[current].data[%.2lf]\n",
								value, btr->node[current].data);

			if (btr->node[current].right == FREE_ELEM_MARKER)
			{
				// Если нет правого потомка, вставляем новый узел здесь
				WRITE_IN_LOG_FILE( "btr->node[current].right[%d] == FREE_ELEM_MARKER\n",
								   btr->node[current].right);

				btr->node[current].right = btr->current_free;

				WRITE_IN_LOG_FILE("btr->node[%d].right [%d] = btr->current_free [%d]\n",
					current, btr->node[current].right, btr->current_free);

				break;
			} else
			{
				// Иначе, переходим к правому потомку
				WRITE_IN_LOG_FILE( "btr->node[current].right[%d] != FREE_ELEM_MARKER\n",
								   btr->node[current].right);

				current = btr->node[current].right;
				WRITE_IN_LOG_FILE("current is now: %d\n", current);
			}
		}
	}

	int next_current_free = btr->node[btr->current_free].right;

	btr->node[btr->current_free].data = value;
	btr->node[btr->current_free].left  = FREE_ELEM_MARKER;
	btr->node[btr->current_free].right = FREE_ELEM_MARKER;

	result.inserted_elem_ID = btr->current_free;

	btr->current_free = next_current_free;

	b_tree_dump(btr, result.error_code, __func__);
	
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
	error_t dump_function_error_code = ALL_GOOD;

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
	DUMP_W_COND(btr->node->data, "        data:");
	DUMP_W_COND(btr->node->left, "        left:");
	DUMP_W_COND(btr->node->right, "        right:\n");
	for(size_t ID = 0; ID < btr->capacity; ID++)
	{
		DUMP_W_COND(&ID &&  &((btr->node)[ID].data),"[%lu]%13.3lf", ID, (btr->node)[ID].data);
		DUMP_W_COND(&((btr->node)[ID].left), "%13.d", (btr->node)[ID].left);
		DUMP_W_COND(&((btr->node)[ID].right), "%13.d", (btr->node)[ID].right);

		WRITE_IN_LOG_FILE("\n");

	}

	// size_t current_free_ID = get_current_free_ID(btr);

	DUMP_W_COND(&(btr->current_free), "current free: %d\n", btr->current_free);
	DUMP_W_COND(&(btr->capacity), "capacity: %lu\n", btr->capacity);

	ERROR_CHECK(UNABLE_TO_ALLOCATE);
	ERROR_CHECK(UNABLE_TO_OPEN_FILE);
	ERROR_CHECK(BTR_NULL_PTR);

	#undef ERROR_CHECK
	#undef DUMP_W_COND

	return dump_function_error_code;
}
