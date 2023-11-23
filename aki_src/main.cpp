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


	FILE *data_base = create_data_base(btr).data_base;

	data_base = fopen("data_base.txt", "r");
	struct B_tree *btr_2 = construct_b_tree(data_base).btr;
	b_tree_dump(btr_2, ALL_GOOD, __func__);

	generate_code_for_graphic_dump(btr_2);
	op_del(btr_2);
	op_del(btr);

	return 0;
}
