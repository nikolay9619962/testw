#include "woody.h"

bool			is_text_segment(Elf64_Phdr *phdr)
{
	return (phdr->p_type == PT_LOAD 
		&& phdr->p_flags & PF_X
		&& phdr->p_flags & PF_R);
}

bool			is_data_segment(Elf64_Phdr *phdr)
{
	return (phdr->p_type == PT_LOAD 
		&& phdr->p_filesz != phdr->p_memsz);
}

Elf64_Phdr	*get_ph_segment(t_file *file, bool (*predicate)(Elf64_Phdr *))
{
	uint16_t	i;
	Elf64_Phdr	*ph_offset;
	uint16_t	ph_count;

	ph_count = ((Elf64_Ehdr *)file->ptr)->e_phnum;
	ph_offset = file->ptr + ((Elf64_Ehdr *)file->ptr)->e_phoff;
	if ((void *)ph_offset < file->ptr)
		return (NULL);

	i = 0;
	while (i < ph_count && (void *)(ph_offset + i + 1) < file->end)
	{
		if (predicate(ph_offset + i))
			return (ph_offset + i);
		i++;
	}

	return (NULL);
}

Elf64_Phdr	*get_last_load_segment(t_file *file)
{
	Elf64_Phdr	*ph_offset;
	uint16_t	ph_count;

	ph_count = ((Elf64_Ehdr *)file->ptr)->e_phnum;
	ph_offset = file->ptr + ((Elf64_Ehdr *)file->ptr)->e_phoff;
	while (ph_count > 0 && (void *)(ph_offset + ph_count - 1) < file->end)
	{
		if (ph_offset[ph_count - 1].p_type == PT_LOAD)
			return (ph_offset + ph_count - 1);
		ph_count--;
	}
	
	return (NULL);
}
