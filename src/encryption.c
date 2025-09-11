#include "woody.h"
#include <errno.h>

unsigned long generate_key(void) {
    unsigned long key;
    int fd;
    
    fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1) {
        srand(time(NULL));
        key = ((unsigned long)rand() << 32) | rand();
    } else {
        read(fd, &key, sizeof(key));
        close(fd);
    }
    return key;
}

void xor_encrypt(void *data, size_t size, unsigned long key) {
    unsigned long *ptr = (unsigned long *)data;
    size_t blocks = size / sizeof(unsigned long);
    unsigned char *byte_ptr;
    unsigned char *key_bytes = (unsigned char *)&key;
    size_t remaining;
    
    // Encrypt in 8-byte blocks
    for (size_t i = 0; i < blocks; i++) {
        ptr[i] ^= key;
    }
    
    // Encrypt remaining bytes
    remaining = size % sizeof(unsigned long);
    if (remaining > 0) {
        byte_ptr = (unsigned char *)(ptr + blocks);
        for (size_t i = 0; i < remaining; i++) {
            byte_ptr[i] ^= key_bytes[i % sizeof(key)];
        }
    }
}


int create_woody_file(elf_file_t *elf, unsigned long key, const char *output_filename) {
    int fd_out;
    Elf64_Phdr *text_phdr;
    woody_data_t woody_data;

    printf("Creating woody file...\n");

    if (find_text_segment(elf, &text_phdr) != 0) {
        fprintf(stderr, "Could not find executable segment\n");
        return -1;
    }

    // Encrypt the text segment
    void *text_start = elf->map + text_phdr->p_offset;
    size_t text_size = text_phdr->p_filesz;

    printf("Encrypting text segment:\n");
    printf("  Size: %zu bytes\n", text_size);
    printf("  Key: 0x%lx\n", key);

    xor_encrypt(text_start, text_size, key);
    printf("Text segment encrypted successfully\n");

    // Prepare woody data
    woody_data.key = key;
    woody_data.original_entry = elf->ehdr->e_entry;
    woody_data.text_size = text_size;
    woody_data.text_vaddr = text_phdr->p_vaddr;
    
    printf("Woody data prepared\n");

    // Calculate unpacker size
    size_t unpacker_size = (size_t)((char*)&unpacker_end - (char*)&unpacker_start);
    size_t woody_data_size = sizeof(woody_data);
    size_t total_unpacker_size = unpacker_size + woody_data_size;

    printf("Unpacker size: %zu bytes\n", unpacker_size);
    printf("Total additional size: %zu bytes\n", total_unpacker_size);

    // Create output file
    fd_out = open(output_filename, O_CREAT | O_WRONLY | O_TRUNC, 0755);
    if (fd_out == -1) {
        perror("open output");
        return -1;
    }

    // 1. Create modified ELF header
    Elf64_Ehdr modified_ehdr = *elf->ehdr;
    
    // Create new program header for unpacker
    Elf64_Phdr new_phdr = {0};
    new_phdr.p_type = PT_LOAD;
    new_phdr.p_flags = PF_R | PF_X;  // Read + Execute
    new_phdr.p_offset = elf->size;   // After original file
    new_phdr.p_vaddr = UNPACKER_VADDR;
    new_phdr.p_paddr = UNPACKER_VADDR;
    new_phdr.p_filesz = total_unpacker_size;
    new_phdr.p_memsz = total_unpacker_size;
    new_phdr.p_align = 0x1000;

    // Update ELF header
    modified_ehdr.e_entry = UNPACKER_VADDR;  // Entry point to unpacker
    modified_ehdr.e_phnum++;                 // Add one program header

    printf("New entry point: 0x%lx\n", modified_ehdr.e_entry);
    printf("New program header count: %d\n", modified_ehdr.e_phnum);

    // 2. Write modified ELF header
    if (write(fd_out, &modified_ehdr, sizeof(Elf64_Ehdr)) != sizeof(Elf64_Ehdr)) {
        perror("write ELF header");
        close(fd_out);
        return -1;
    }

    // 3. Write original program headers + new program header
    size_t phdr_size = elf->ehdr->e_phnum * elf->ehdr->e_phentsize;
    if (write(fd_out, elf->phdr, phdr_size) != (ssize_t)phdr_size) {
        perror("write original program headers");
        close(fd_out);
        return -1;
    }

    // Write new program header
    if (write(fd_out, &new_phdr, sizeof(new_phdr)) != sizeof(new_phdr)) {
        perror("write new program header");
        close(fd_out);
        return -1;
    }

    // 4. Write the rest of original ELF (sections, etc.)
    // Calculate the size of headers we've written so far
    //size_t headers_written = sizeof(Elf64_Ehdr) + phdr_size + sizeof(new_phdr);
    size_t headers_original = sizeof(Elf64_Ehdr) + (elf->ehdr->e_phnum * elf->ehdr->e_phentsize);
    size_t remaining_elf_size = elf->size - headers_original;
    void *remaining_elf_data = elf->map + headers_original;
    
    printf("Writing remaining ELF (%zu bytes)...\n", remaining_elf_size);
    if (write(fd_out, remaining_elf_data, remaining_elf_size) != (ssize_t)remaining_elf_size) {
        perror("write remaining ELF");
        close(fd_out);
        return -1;
    }

    // 5. Write unpacker at the end
    printf("Writing unpacker (%zu bytes)...\n", unpacker_size);
    if (write(fd_out, &unpacker_start, unpacker_size) != (ssize_t)unpacker_size) {
        perror("write unpacker");
        close(fd_out);
        return -1;
    }

    // 6. Write woody data after unpacker
    printf("Writing woody data (%zu bytes)...\n", woody_data_size);
    if (write(fd_out, &woody_data, woody_data_size) != (ssize_t)woody_data_size) {
        perror("write woody data");
        close(fd_out);
        return -1;
    }

    // Get final file size
    off_t final_size = lseek(fd_out, 0, SEEK_CUR);
    printf("Final file size: %ld bytes\n", final_size);
    
    close(fd_out);
    
    if (chmod(output_filename, 0755) == -1) {
        perror("chmod");
        return -1;
    }
    
    printf("Woody file created successfully\n");
    return 0;
}


int verify_woody_file(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open verify");
        return -1;
    }
    
    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("fstat verify");
        close(fd);
        return -1;
    }
    
    printf("Woody file size: %ld bytes\n", st.st_size);
    
    // Read ELF header
    Elf64_Ehdr ehdr;
    if (read(fd, &ehdr, sizeof(ehdr)) != sizeof(ehdr)) {
        perror("read ELF header");
        close(fd);
        return -1;
    }
    
    printf("New entry point: 0x%lx\n", ehdr.e_entry);
    printf("Number of program headers: %d\n", ehdr.e_phnum);
    
    // Read program headers to verify unpacker header was added
    lseek(fd, ehdr.e_phoff, SEEK_SET);
    for (int i = 0; i < ehdr.e_phnum; i++) {
        Elf64_Phdr phdr;
        if (read(fd, &phdr, sizeof(phdr)) != sizeof(phdr)) {
            perror("read program header");
            close(fd);
            return -1;
        }
        
        if (phdr.p_type == PT_LOAD && phdr.p_vaddr == UNPACKER_VADDR) {
            printf("Found unpacker program header:\n");
            printf("  File offset: 0x%lx\n", phdr.p_offset);
            printf("  Virtual address: 0x%lx\n", phdr.p_vaddr);
            printf("  File size: %lu bytes\n", phdr.p_filesz);
            printf("  Memory size: %lu bytes\n", phdr.p_memsz);
            printf("  Flags: %s%s%s\n", 
                   (phdr.p_flags & PF_R) ? "R" : "",
                   (phdr.p_flags & PF_W) ? "W" : "",
                   (phdr.p_flags & PF_X) ? "X" : "");
        }
    }
    
    close(fd);
    return 0;
}