section .text
global unpacker_start
global unpacker_end

%define WOODY_MAGIC "....WOODY...."
%define WOODY_MAGIC_LEN 14
%define SYS_WRITE 1
%define SYS_MPROTECT 10
%define STDOUT 1
%define PROT_READ 1
%define PROT_WRITE 2
%define PROT_EXEC 4

unpacker_start:
    ; Save all registers
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; Print Woody message
    mov rax, SYS_WRITE
    mov rdi, STDOUT
    lea rsi, [rel woody_msg]
    mov rdx, WOODY_MAGIC_LEN
    syscall

    ; Get our base address (unpacker start)
    call get_base_addr
    mov r12, rax
    
    ; Load woody data (located right after unpacker)
    mov r13, [r12 + unpacker_size]      ; key
    mov r14, [r12 + unpacker_size + 8]  ; original_entry
    mov r15, [r12 + unpacker_size + 16] ; text_size
    mov rbx, [r12 + unpacker_size + 24] ; text_vaddr

    ; Calculate text segment start address
    mov rsi, rbx        ; text segment virtual address

    ; Make text segment writable
    mov rax, SYS_MPROTECT
    mov rdi, rsi
    and rdi, ~0xFFF     ; Align to page boundary
    mov rdx, rsi
    add rdx, r15        ; text segment end
    sub rdx, rdi        ; size to protect
    add rdx, 0x1000     ; add one page for safety
    mov rsi, rdx
    mov rdx, PROT_READ | PROT_WRITE | PROT_EXEC
    syscall

    ; Decrypt text segment
    mov rdi, rsi        ; text segment end
    mov rsi, rbx        ; text segment start

.decrypt_loop:
    cmp rsi, rdi
    jge .decrypt_done
    
    mov rax, [rsi]
    xor rax, r13
    mov [rsi], rax
    
    add rsi, 8
    jmp .decrypt_loop

.decrypt_done:
    ; Restore text segment protection
    mov rax, SYS_MPROTECT
    mov rdi, rbx
    and rdi, ~0xFFF
    mov rsi, r15
    add rsi, 0x1000     ; size to protect
    mov rdx, PROT_READ | PROT_EXEC
    syscall

    ; Restore all registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    ; Jump to original entry point
    jmp r14

get_base_addr:
    mov rax, [rsp]
    ret

woody_msg: db WOODY_MAGIC, 10

unpacker_size: equ $ - unpacker_start
unpacker_end: