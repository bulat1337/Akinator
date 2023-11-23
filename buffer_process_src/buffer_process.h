#ifndef BUFFER_PROCESS
#define BUFFER_PROCESS

struct Buf_w_carriage_n_len
{
    char *buf;
    size_t carriage;
    size_t length;
};

struct Buffer_w_info
{
	char *buf;
	size_t length;
};

size_t get_file_length(FILE *fp);
size_t count_buf_lexemes(struct Buf_w_carriage_n_len buf_w_info);
void lexemes_ptr_arranger(char * *lexemes, struct Buf_w_carriage_n_len buf_w_info);

#endif
