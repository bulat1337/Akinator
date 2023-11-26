#include <stdio.h>

#include "akinator.h"

int main()
{
	error_t error_code = ALL_GOOD;
	struct B_tree *btr = b_tree_ctor(10).new_btr;

	struct B_tree_node *angem = create_node(btr, "angem").created_node;
	struct B_tree_node *aiden = create_node(btr, "aiden").created_node;
	struct B_tree_node *phd = create_node(btr, "phd").created_node;
	struct B_tree_node *chubarov = create_node(btr, "chubarov").created_node;
	struct B_tree_node *stukopin = create_node(btr, "stukopin").created_node;

	add_child(angem, aiden, true);
	add_child(angem, phd, false);
	add_child(phd, chubarov, true);
	add_child(phd, stukopin, false);

	set_root(btr, 0);

	b_tree_dump(btr, ALL_GOOD, __func__);

	create_data_base(btr, "data_base.txt");

	FILE *data_base = fopen("data_base.txt", "r");

	struct B_tree *btr_2 = construct_b_tree(data_base).btr;

	create_data_base(btr_2, "data_base_2.txt");

	b_tree_dump(btr_2, ALL_GOOD, __func__);

	play_akinator("data_base.txt");

	return 0;
}
