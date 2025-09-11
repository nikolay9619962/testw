#include "woody.h"
#include <errno.h>

int parse_elf(const char *filename, elf_file_t *elf) {
    int fd;
    struct stat st;

    printf("Opening file: %s\n", filename);
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    if (fstat(fd, &st) == -1) {
        perror("fstat");
        close(fd);
        return -1;
    }

    elf->size = st.st_size;
    printf("File size: %zu bytes\n", elf->size);

    // Выравниваем размер до границы страницы
    size_t mapped_size = (elf->size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    printf("Mapped size: %zu bytes\n", mapped_size);

    elf->map = mmap(NULL, mapped_size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE, fd, 0);
    if (elf->map == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return -1;
    }

    elf->ehdr = (Elf64_Ehdr *)elf->map;
    elf->phdr = (Elf64_Phdr *)(elf->map + elf->ehdr->e_phoff);
    elf->shdr = (Elf64_Shdr *)(elf->map + elf->ehdr->e_shoff);

    printf("ELF header at: %p\n", elf->ehdr);
    printf("Program headers at: %p\n", elf->phdr);
    printf("Section headers at: %p\n", elf->shdr);

    if (elf->ehdr->e_shstrndx != SHN_UNDEF) {
        elf->strtab = (char *)elf->map + elf->shdr[elf->ehdr->e_shstrndx].sh_offset;
        printf("String table at: %p\n", elf->strtab);
    }

    close(fd);
    return 0;
}
void munmap_elf(elf_file_t *elf) {
    if (elf->map != NULL && elf->map != MAP_FAILED) {
        munmap(elf->map, elf->size);
    }
}

int is_elf64(Elf64_Ehdr *ehdr) {
    return (ehdr->e_ident[EI_CLASS] == ELFCLASS64);
}

int find_text_segment(elf_file_t *elf, Elf64_Phdr **text_phdr) {
    printf("Looking for text segment...\n");
    for (int i = 0; i < elf->ehdr->e_phnum; i++) {
        printf("PHDR %d: type=%d, flags=%d, offset=0x%lx, vaddr=0x%lx, filesz=%lu\n",
               i, elf->phdr[i].p_type, elf->phdr[i].p_flags,
               elf->phdr[i].p_offset, elf->phdr[i].p_vaddr,
               elf->phdr[i].p_filesz);
        
        if (elf->phdr[i].p_type == PT_LOAD &&
            (elf->phdr[i].p_flags & PF_X)) {
            *text_phdr = &elf->phdr[i];
            printf("Found text segment at index %d\n", i);
            printf("  Virtual address: 0x%lx\n", elf->phdr[i].p_vaddr);
            printf("  File offset: 0x%lx\n", elf->phdr[i].p_offset);
            printf("  File size: %lu bytes\n", elf->phdr[i].p_filesz);
            return 0;
        }
    }
    printf("No text segment found!\n");
    return -1;
}