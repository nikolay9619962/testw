#include "woody.h"

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
