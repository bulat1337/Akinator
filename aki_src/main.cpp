#include <stdio.h>

#include "akinator.h"

int main()
{
	error_t error_code = ALL_GOOD;
	struct B_tree *btr = op_new(10).new_btr;

	b_tree_insert(btr, 7);
	b_tree_insert(btr, 3);
	b_tree_insert(btr, 2);
	b_tree_insert(btr, 5);
	b_tree_insert(btr, 6);
	b_tree_insert(btr, 1);
	b_tree_insert(btr, 4);
	b_tree_insert(btr, 9);
	b_tree_insert(btr, 8);
	b_tree_insert(btr, 12);

	FILE *data_base = create_data_base(btr).data_base;

	data_base = fopen("data_base.txt", "r");
	struct B_tree *btr_2 = construct_b_tree(data_base).btr;
	b_tree_dump(btr_2, ALL_GOOD, __func__);

	destroy_subtree(btr, 1, 0);
	generate_code_for_graphic_dump(btr);
	op_del(btr_2);
	op_del(btr);

	return 0;
}
