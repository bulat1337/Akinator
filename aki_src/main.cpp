#include <stdio.h>

#include "akinator.h"

int main()
{
	error_t error_code = ALL_GOOD;
	struct B_tree *btr = op_new(10).new_btr;

	b_tree_insert(btr, 45);
	b_tree_insert(btr, 50);
	b_tree_insert(btr, 55);
	b_tree_insert(btr, 11);
	b_tree_insert(btr, 13);
	b_tree_insert(btr, 70);
	b_tree_insert(btr, 65);
	b_tree_insert(btr, 54);



	FILE *data_base = fopen("data_base.txt", "w");
	print_node(btr->node, btr->root, data_base);
	fclose(data_base);

	data_base = fopen("data_base.txt", "r");
	struct B_tree *btr_2 = construct_b_tree(data_base).btr;
	generate_code_for_graphic_dump(btr_2);
	b_tree_dump(btr_2, ALL_GOOD, __func__);

	return 0;
}
