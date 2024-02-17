#ifndef AKINATOR_H
#define AKINATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>

#include "../stack_src/stack.h"

#define SAY_TO_PLAYER(...)\
		printf(__VA_ARGS__);\
		printf("\n");\
		snprintf(line_to_say, 100, "say -vReed " __VA_ARGS__);\
		system(line_to_say); // systemf()

#define HANDLE_ERROR															\
{																				\
	if(error_code != AKI_ALL_GOOD)												\
	{																			\
		b_tree_dump(btr, error_code, __func__);									\
																				\
		return error_code;														\
	}																			\
}

enum Aki_err_ID
{
	AKI_ALL_GOOD 		     = 0,
	AKI_UNABLE_TO_OPEN_FILE  = 1 << 0,
	UNABLE_TO_ALLOCATE       = 1 << 1,
	BTR_NULL_PTR             = 1 << 2,
	UNEXPECTED_WRITTEN_ELEMS = 1 << 3,
	B_TREE_NULL_PTR          = 1 << 4,
	B_TREE_NODE_NULL_PTR     = 1 << 5,
	INVALID_VALUE            = 1 << 6,
	INVALID_NODE_INDEX       = 1 << 7,
	PARENT_NODE_IS_FREE      = 1 << 8,
	INVALID_NODE_PATH_TURN   = 1 << 9,
};

enum Menu_option
{
	QUIT           = 'q',
	START          = 's',
	SHOW_DATA_BASE = 'b',
	COMPARE        = 'c',
	DESCRIBE       = 'd',
};

typedef char *b_tree_elem_t;
typedef enum Aki_err_ID error_t;

const size_t               NODE_LABEL_STR_SIZE      = 100;
const int                  FREE_ELEM_MARKER         = -1;
const size_t               REALLOC_COEFF            = 2;
const int                  INDEX_POISON  			= -666;
const b_tree_elem_t        DATA_POISON              = NULL;
const size_t               DEFAULT_STARTER_CAPACITY = 10;
struct B_tree_node *const  FREE_NODE   			    = NULL;
const size_t               PTR_POISON               = NULL;
const size_t               MAX_NEW_NODE_DATA_STRLEN = 100;
const size_t               SPACE_FOR_NEW_NODES      = 3;
const size_t               AKINATOR_LINE_MAX_LEN    = 100;
const bool                 RIGHT_CHILD              = true;
const bool                 LEFT_CHILD               = false;
const size_t               FIRST                    = 0;
const size_t               SECOND                   = 1;

struct B_tree_node
{
	b_tree_elem_t data;
	struct B_tree_node *left;
	struct B_tree_node *right;
};

struct B_tree
{
	struct B_tree_node *node;
	struct B_tree_node *root;
	size_t capacity;
	struct B_tree_node *current_free;
};

union Second_arg
{
	FILE               *file_ptr;
	struct B_tree      *btr;
	int                 int_val;
	size_t              size_t_val;
	struct              Stack stk;
	struct B_tree_node *node;
};

struct Universal_ret
{
	error_t error_code;
	union Second_arg second_arg;
};

struct Lexemes_w_carriage
{
	char * *buf;
	size_t  carriage;
};

struct Search_for_node_result
{
	struct B_tree_node *found_node;
	struct Stack node_path_stk;
};

struct Leaf_w_path
{
	char name[MAX_NEW_NODE_DATA_STRLEN];
	struct Stack node_path;
};

struct Current_tree_position
{
	size_t node_path_ID;
	struct B_tree_node *current_node;
};


void                 op_del(struct B_tree *btr);
void                 log(const char *fmt, ...);

error_t 		     b_tree_dump(const struct B_tree *btr, error_t error_code,
								const char *func_name);
error_t              b_tree_verifier(struct B_tree *btr);
error_t              destroy_subtree(struct B_tree *btr, int parent_index, bool is_left_child);
error_t              add_child(struct B_tree_node *parent,
							  struct B_tree_node *child, bool is_right_child);
error_t              set_root(struct B_tree *btr, int root_ID);
error_t              play_akinator(const char *data_base_file_name);

struct B_tree_node 	*search_for_node(struct B_tree_node *node, struct Leaf_w_path *cmp_leaf);

struct Universal_ret create_node(struct B_tree *btr, const b_tree_elem_t value);
struct Universal_ret construct_b_tree(const char *data_base_file_name);
struct Universal_ret b_tree_ctor(size_t starter_capacity);
struct Universal_ret generate_code_for_graphic_dump(struct B_tree *btr);
struct Universal_ret create_data_base(struct B_tree *btr, const char *file_name);


#endif
