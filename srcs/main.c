#include "woody.h"

static t_payload	get_payload()
{
	int			size;
	t_payload	payload;

	size = CODE_SIZE;
	payload = (t_payload){malloc(size), size, 0x1e, 0x23, 0x2a, 0x78, 0x5c};
	ft_memcpy(payload.code, DECRYPTION_CODE, size);
	return (payload);
}

void		safe_exit(t_file *file, t_payload *payload, t_woody *woody,
				char* msg)
{
	if (msg != NULL)
		fprintf(stderr, "%s\n", msg);
	else if (errno != 0)
		perror("");

	if (file->ptr)
		munmap(file->ptr, file->size);
	if (file->fd)
		close(file->fd);
	if (payload)
		ft_strdel(&payload->code);
	if (woody)
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
	return (0);
}
