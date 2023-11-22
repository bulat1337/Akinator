#include "akinator_helpers.h"

extern FILE *log_file;

error_t mark_b_tree_nodes_as_free(struct B_tree_node *node, size_t size, size_t start_ID)
{
	error_t error_code = ALL_GOOD;

	for(size_t ID = start_ID; ID < size - 1; ID++)
	{
		node[ID].left = FREE_ELEM_MARKER;
		node[ID].right = (int) (ID + 1);
	}

	node[start_ID].right = FREE_ELEM_MARKER;
	node[size - 1].left = FREE_ELEM_MARKER;
	node[size - 1].right = FREE_ELEM_MARKER;
	WRITE_IN_LOG_FILE("\nnode[%lu].right = %d\n\n", size - 1, node[size - 1].right);

	return error_code;
}

error_t create_node(int node_ID, struct node_charachteristics *nd_description,
					FILE *graphic_dump_code_file_ptr)
{
	fprintf(graphic_dump_code_file_ptr, "\t%s%d [shape = Mrecord, "
		"fillcolor = \"%s\", label = \"%s\" ];\n",
			nd_description->name, node_ID, nd_description->color, nd_description->label);

	return ALL_GOOD;
}

void print_regular_nodes(int ID, struct B_tree_node *node,
						struct node_charachteristics *nd_description,
						FILE *graphic_dump_code_file_ptr)
{
	if(ID == FREE_ELEM_MARKER)
	{
		WRITE_IN_LOG_FILE( "ID = %d so im returning\n", ID);
		return;
	}

	snprintf(nd_description->label, NODE_LABEL_STR_SIZE,
					"{ID: %d | val: %.2lf |{L: %d | R: %d}}",
					ID, node[ID].data,
					node[ID].left, node[ID].right);

	create_node(ID, nd_description, graphic_dump_code_file_ptr);

	WRITE_IN_LOG_FILE( "ID = %d\n", ID);
	print_regular_nodes(node[ID].left , node, nd_description, graphic_dump_code_file_ptr);
	print_regular_nodes(node[ID].right, node, nd_description, graphic_dump_code_file_ptr);
}

void connect_nodes(int ID, struct B_tree_node *node, FILE *graphic_dump_code_file_ptr)
{
	if(ID == FREE_ELEM_MARKER)
	{
		return;
	}

	if(node[ID].left != FREE_ELEM_MARKER)
	{
		WRITE_IN_LOG_FILE( "node->left[%d] != FREE_ELEM_MARKER so im connecting\n", node->left);
		fprintf(graphic_dump_code_file_ptr, "node_%d -> node_%d\n", ID, node[ID].left);
	}

	if(node[ID].right != FREE_ELEM_MARKER)
	{
		WRITE_IN_LOG_FILE( "node->right[%d] != FREE_ELEM_MARKER so im connecting\n", node->right);
		fprintf(graphic_dump_code_file_ptr, "node_%d -> node_%d\n", ID, node[ID].right);
	}

	connect_nodes(node[ID].left, node, graphic_dump_code_file_ptr);
	connect_nodes(node[ID].right, node, graphic_dump_code_file_ptr);
}

error_t b_tree_realloc(struct B_tree *btr)
{
	error_t error_code = ALL_GOOD;

	size_t data_size = sizeof(struct B_tree_node) * btr->capacity;

	btr->node = (struct B_tree_node *)realloc(btr->node, data_size);
	if(btr->node == NULL)
	{
		WRITE_IN_LOG_FILE("Unable to allocate btr->node\n");
		return UNABLE_TO_ALLOCATE;
	}

	size_t new_allocated_areas_start_ID = (btr->capacity / REALLOC_COEFF) - 1;
	WRITE_IN_LOG_FILE("new_allocated_areas_start_ID == %lu\n", new_allocated_areas_start_ID);
	btr->current_free = (int)new_allocated_areas_start_ID + 1;

	mark_b_tree_nodes_as_free(btr->node, btr->capacity, new_allocated_areas_start_ID);

	b_tree_dump(btr, error_code, __func__);

	return error_code;
}
