#include "woody.h"

void parse_content(t_file *file)
{
	file->size = lseek(file->fd, 0, SEEK_END);
	if (file->size <= 0)
		safe_exit(file, NULL, NULL,"Failed to get file size\n");

	file->ptr = mmap(NULL, file->size, PROT_READ | PROT_WRITE, MAP_PRIVATE, file->fd, 0);
	if (file->ptr == MAP_FAILED)
		safe_exit(file, NULL, NULL, "Failed to map file to memory\n");

	file->end = file->ptr + file->size;
}

void	parse_header(t_file *file)
{
	unsigned char	*ident;
	Elf64_Ehdr		*header;

	header = file->ptr;
	ident = file->ptr;
	file->arch = ident[EI_CLASS];
	file->endian = ident[EI_DATA];
	
	if (file->arch != ELFCLASS64)
		safe_exit(file, NULL, NULL, "File architecture not suported. x86_64 only\n");

	if (ft_memcmp(ident, ELFMAG, 4) 
		|| ident[EI_VERSION] == EV_NONE
		|| ident[EI_CLASS] == ELFCLASSNONE
		|| ident[EI_DATA] == ELFDATANONE 
		|| !(get_uint16(header->e_machine, file->endian) == EM_386 || get_uint16(header->e_machine, file->endian) == EM_X86_64)
		|| get_uint32(header->e_version, file->endian) == EV_NONE
		|| get_uint16(header->e_phnum, file->endian) == 0)
		safe_exit(file, NULL, NULL, "File format not supported\n");
	if (header->e_type != ET_EXEC && header->e_type != ET_DYN)
		safe_exit(file, NULL, NULL, "File type not supported\n");
}

void			parse_file(char *filename, t_file *file)
{
	file->fd = open(filename, O_RDONLY);
	if (file->fd == -1)
		safe_exit(file, NULL, NULL, "Cannot open file\n");

	parse_content(file);
	parse_header(file);
}
