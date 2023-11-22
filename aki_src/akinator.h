#ifndef AKINATOR_H
#define AKINATOR_H

#include <stdio.h>
#include <stdlib.h>

#define WRITE_IN_LOG_FILE(...) fprintf(log_file, __VA_ARGS__);

typedef double b_tree_elem_t;

enum Err_ID
{
	ALL_GOOD 		        = 0,
	UNABLE_TO_OPEN_FILE     = 1 << 0,
	UNABLE_TO_ALLOCATE      = 1 << 1,
	BTR_NULL_PTR            = 1 << 2,
};

typedef enum Err_ID error_t;

struct node_charachteristics
{
	char *name;
	char *color;
	char *label;
};

struct B_tree_node
{
	double data;
	int left;
	int right;
};

struct B_tree
{
	struct B_tree_node *node;
	int root;
	size_t capacity;
	int current_free;
};

struct Generate_code_for_graphic_dump_result
{
	error_t error_code;
	FILE *graphic_dump_code_file_ptr;
};


struct Op_new_result
{
	error_t error_code;
	struct B_tree *new_btr;
};

struct B_tree_insert_result
{
	error_t error_code;
	int inserted_elem_ID;
};

const size_t NODE_LABEL_STR_SIZE = 100;
const int FREE_ELEM_MARKER = -1;
const size_t REALLOC_COEFF = 2;

struct Op_new_result op_new(size_t starter_capacity);
struct Generate_code_for_graphic_dump_result generate_code_for_graphic_dump(struct B_tree *btr);
struct B_tree_insert_result b_tree_insert(struct B_tree *btr, b_tree_elem_t value);
error_t b_tree_dump(const struct B_tree *btr, error_t error_code, const char *func_name);

#endif
