#ifndef WOODY_H
#define WOODY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>
#include <time.h>

#define PAGE_SIZE 4096
#define UNPACKER_VADDR 0x410000  // Виртуальный адрес для распаковщика

typedef struct {
    void *map;
    size_t size;
    Elf64_Ehdr *ehdr;
    Elf64_Phdr *phdr;
    Elf64_Shdr *shdr;
    char *strtab;
} elf_file_t;

typedef struct {
    unsigned long key;
    Elf64_Addr original_entry;
    Elf64_Addr text_size;
    Elf64_Addr text_vaddr;
} woody_data_t;

// Function declarations
int parse_elf(const char *filename, elf_file_t *elf);
void munmap_elf(elf_file_t *elf);
unsigned long generate_key(void);
void xor_encrypt(void *data, size_t size, unsigned long key);
int find_text_segment(elf_file_t *elf, Elf64_Phdr **text_phdr);
int create_woody_file(elf_file_t *elf, unsigned long key, 
                     const char *output_filename);
int verify_woody_file(const char *filename);
int is_elf64(Elf64_Ehdr *ehdr);

// External assembly functions
extern void unpacker_start(void);
extern void unpacker_end(void);

#endif