#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "buffer_process.h"

//
void lexemes_ptr_arranger(char * *lexemes, struct Buf_w_carriage_n_len buf_w_info)
{
	lexemes[0] = buf_w_info.buf;
	size_t lexem_ID = 1;
    for (size_t buf_ID = 0; buf_ID < buf_w_info.length; buf_ID++)
    {
        if(buf_w_info.buf[buf_ID] == ' ')
        {
            buf_w_info.buf[buf_ID] = '\0';
			if(buf_ID + 1 < buf_w_info.length)
			{
            	lexemes[lexem_ID] = &(buf_w_info.buf[buf_ID + 1]);
            	lexem_ID++;
			}
        }
    }
}
//

//
size_t count_buf_lexemes(struct Buf_w_carriage_n_len buf_w_info)
{
	size_t amount_of_lexemes = 0;
	for (size_t buf_ID = 0; buf_ID < buf_w_info.length; buf_ID++)
    {
        if(buf_w_info.buf[buf_ID] == ' ')
        {
            amount_of_lexemes++;
        }
    }

	return amount_of_lexemes;
}
//

//
size_t get_file_length(FILE *fp)
{
    size_t length = 0;
	rewind(fp);

    fseek(fp, 0L, SEEK_END);
    length = (size_t)ftell(fp);
    rewind(fp);

    return length;
}
//
