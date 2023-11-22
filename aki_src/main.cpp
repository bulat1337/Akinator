#include <stdio.h>

#include "akinator.h"

int main()
{
	error_t error_code = ALL_GOOD;
	struct B_tree *btr = op_new(10).new_btr;

	for(double ID = 0; ID <= 200; ID++)
	{
		b_tree_insert(btr, ID);
		b_tree_insert(btr, ID);
	}





	b_tree_dump(btr, error_code, __func__);
	generate_code_for_graphic_dump(btr);


	return 0;
}
