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

int					main(int ac, char **av)
{
	t_file		file;
	t_payload	payload;

	ft_memset(&file, 0, sizeof(t_file));
	errno = 0;
	if (ac != 2 || check_file(av[1], &file) == EXIT_FAILURE)
		woody_error(&file, NULL, NULL, !errno ? ERROR_ARGS : ERROR_ERRNO);
	encrypt_code(&file);
	payload = get_payload();
	inject(&file, &payload);
	ft_strdel(&payload.code);
	return (0);
}
