#ifndef AKINATOR_H
#define AKINATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WRITE_IN_LOG_FILE(...) fprintf(log_file, __VA_ARGS__);

typedef char *b_tree_elem_t;

enum Err_ID
{
	ALL_GOOD 		         = 0,
	UNABLE_TO_OPEN_FILE      = 1 << 0,
	UNABLE_TO_ALLOCATE       = 1 << 1,
	BTR_NULL_PTR             = 1 << 2,
	UNEXPECTED_WRITTEN_ELEMS = 1 << 3,
	B_TREE_NULL_PTR          = 1 << 4,
	B_TREE_NODE_NULL_PTR     = 1 << 5,
	INVALID_VALUE            = 1 << 6,
	INVALID_INDEX            = 1 << 7,
	PARENT_NODE_IS_FREE      = 1 << 8,
};

typedef enum Err_ID error_t;

struct B_tree;

struct Generate_code_for_graphic_dump_result
{
	error_t error_code;
	FILE *graphic_dump_code_file_ptr;
};

struct B_tree_ctor_result
{
	error_t error_code;
	struct B_tree *new_btr;
};

struct B_tree_insert_result
{
	error_t error_code;
	int inserted_elem_ID;
};

struct Construct_b_tree_result
{
	error_t error_code;
	struct B_tree *btr;
};

struct Lexemes_w_carriage
{
	char * *buf;
	size_t carriage;
};

struct Create_data_base_result
{
	enum Err_ID error_code;
	FILE *data_base;
};

struct Create_node_result
{
	error_t error_code;
	struct B_tree_node *created_node;
};


const size_t NODE_LABEL_STR_SIZE      = 100;
const int FREE_ELEM_MARKER            = -1;
const size_t REALLOC_COEFF            = 2;
const int INDEX_POISON  			  = -666;
const b_tree_elem_t DATA_POISON       = NULL;
const size_t DEFAULT_STARTER_CAPACITY = 10;
const size_t FREE_NODE    			  = NULL;
const size_t PTR_POISON               = NULL;
const size_t MAX_NEW_NODE_DATA_STRLEN = 100;

struct  B_tree_ctor_result b_tree_ctor(size_t starter_capacity);
// struct  Generate_code_for_graphic_dump_result generate_code_for_graphic_dump(struct B_tree *btr);
struct  B_tree_insert_result b_tree_insert(struct B_tree *btr, b_tree_elem_t value);
error_t b_tree_dump(const struct B_tree *btr, error_t error_code, const char *func_name);
struct  Construct_b_tree_result construct_b_tree(FILE *data_base);
void    op_del(struct B_tree *btr);
struct Create_data_base_result create_data_base(struct B_tree *btr, const char *file_name);
error_t b_tree_verifier(struct B_tree *btr);
error_t destroy_subtree(struct B_tree *btr, int parent_index, bool is_left_child);
struct Create_node_result create_node(struct B_tree *btr, const b_tree_elem_t value);
error_t add_child(struct B_tree_node *parent, struct B_tree_node *child, bool is_left_child);
error_t set_root(struct B_tree *btr, int root_ID);

error_t play_akinator(const char *data_base_file_name);


#endif
