#include <stdio.h>

#include "akinator.h"

int main()
{
	error_t error_code = AKI_ALL_GOOD;
	struct B_tree *btr = b_tree_ctor(10).second_arg.btr;
//
// 	struct B_tree_node *angem = create_node(btr, "angem").created_node;
// 	struct B_tree_node *aiden = create_node(btr, "aiden").created_node;
// 	struct B_tree_node *phd = create_node(btr, "phd").created_node;
// 	struct B_tree_node *chubarov = create_node(btr, "chubarov").created_node;
// 	struct B_tree_node *stukopin = create_node(btr, "stukopin").created_node;
//
// 	add_child(angem, aiden, false);
// 	add_child(angem, phd, true);
// 	add_child(phd, chubarov, false);
// 	add_child(phd, stukopin, true);
//
//
// 	set_root(btr, 0);
//
// 	create_data_base(btr, "data_base.txt");
//
// 	b_tree_dump(btr, AKI_ALL_GOOD, __func__);

	play_akinator("data_base.txt");


	return 0;
}
