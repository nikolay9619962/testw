#include "woody.h"

static void	get_key(t_file *file)
{
	int		fd;

	if ((fd = open("/dev/urandom", O_RDONLY)) == -1)
		return (safe_exit(file, NULL, NULL, "Failed to open /dev/urandom\n"));
	
	if (read(fd, &file->key, KEY_SIZE) != KEY_SIZE)
	{
		close(fd);
		safe_exit(file, NULL, NULL, "Failed to read random key\n");
	}
	close(fd);
	
	ft_putstr("key_value: ");
	printf("%016lX", file->key);  // Изменено на заглавные буквы и формат как в примере
	fflush(stdout);
	ft_putstr("\n");
}

void		encrypt_code(t_file *file)
{
	void		*text;
	void		*key;
	uint32_t	text_size;
	uint32_t	key_size;

	if (!(file->text = get_ph_segment(file, is_text_segment)))
		return (safe_exit(file, NULL, NULL, "program headers extends past the end of the file."));
	((Elf64_Phdr *)file->text)->p_flags |= PF_W;
	text = file->ptr + get_uint64(((Elf64_Phdr *)file->text)->p_offset,
		file->endian);
	text_size = get_uint64(((Elf64_Phdr *)file->text)->p_filesz, file->endian);
	get_key(file);
	key = &file->key;
	key_size = KEY_SIZE;
	if (text < file->ptr || text > file->end || text + text_size > file->end)
		return (safe_exit(file, NULL, NULL, "text segment extends past the end of the file."));
	encrypt(key, key_size, text, text_size);
}