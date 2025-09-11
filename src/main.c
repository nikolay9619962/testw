#include "woody.h"
#include <errno.h>

int main(int argc, char **argv) {
    elf_file_t elf = {0};
    unsigned long key;
    
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <binary>\n", argv[0]);
        return 1;
    }
    
    printf("Parsing ELF file: %s\n", argv[1]);
    
    // Parse ELF file
    if (parse_elf(argv[1], &elf) != 0) {
        fprintf(stderr, "Failed to parse ELF file\n");
        return 1;
    }
    
    // Check if it's 64-bit ELF
    if (!is_elf64(elf.ehdr)) {
        fprintf(stderr, "File architecture not supported. x86_64 only\n");
        munmap_elf(&elf);
        return 1;
    }
    
    printf("ELF parsed successfully:\n");
    printf("  Entry point: 0x%lx\n", elf.ehdr->e_entry);
    printf("  Program headers: %d\n", elf.ehdr->e_phnum);
    printf("  Section headers: %d\n", elf.ehdr->e_shnum);
    
    // Generate encryption key
    key = generate_key();
    printf("Generated key: %lX\n", key);

    // Create woody file
    if (create_woody_file(&elf, key, "woody") != 0) {
        fprintf(stderr, "Failed to create woody file\n");
        munmap_elf(&elf);
        return 1;
    }

    // Verify woody file
    if (verify_woody_file("woody") != 0) {
        fprintf(stderr, "Woody file verification failed!\n");
        munmap_elf(&elf);
        return 1;
    }

    munmap_elf(&elf);
    printf("Woody file created and verified successfully!\n");
    printf("Key: %lX\n", key);
    return 0;
}