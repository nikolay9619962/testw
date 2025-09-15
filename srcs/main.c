#include "woody.h"

// Внешние символы из ассемблерного кода
extern void decrypt(void);
extern uint64_t decrypt_size;

static t_payload	get_payload()
{
	t_payload	payload;
	
	size_t size = (size_t)decrypt_size + KEY_SIZE;
	payload = (t_payload){
		.code = malloc(size),
		.size = size,
		.i_tsize = 0x1e,
		.i_ksize = 0x23,
		.i_text = 0x2a,
		.i_key = 0x78,
		.i_jmp = 0x5c
	};
	
	ft_memcpy(payload.code, decrypt, size);
	return (payload);
}

void		safe_exit(t_file *file, t_payload *payload, t_woody *woody,
				char* msg)
{
	if (msg != NULL)
		fprintf(stderr, "%s\n", msg);
	else if (errno != 0)
		perror("");

	if (file && file->ptr)
		munmap(file->ptr, file->size);
	if (file && file->fd)
		close(file->fd);
	if (payload && payload->code)
		ft_strdel(&payload->code);
	if (woody && woody->ptr)
		free(woody->ptr);
	exit(EXIT_FAILURE);
}

int					main(int argc, char **argv)
{
	t_file		file;
	t_payload	payload;

	if (argc != 2)
		safe_exit(NULL, NULL, NULL, "usage: ./woody_woodpacker <filename>\n");

	errno = 0;
	ft_memset(&file, 0, sizeof(t_file));
	parse_file(argv[1], &file);

	encrypt_code(&file);
	payload = get_payload();
	inject(&file, &payload);
	ft_strdel(&payload.code);
	
	// Освобождаем ресурсы
	if (file.ptr)
		munmap(file.ptr, file.size);
	if (file.fd)
		close(file.fd);
		
	return (0);
}