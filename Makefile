NAME = woody_woodpacker

CC = gcc
CFLAGS = -Wall -Werror -Wextra -g #-fsanitize=address
CFLAGS += $(addprefix -I , $(INCLUDES))
NASM = nasm -f elf64

INCLUDES_DIR = ./includes/
SRCS_DIR = ./srcs/
INCLUDES = $(INCLUDES_DIR)
OBJS_DIR = ./.objs/

SRC_FILES = main.c check_file.c error.c endian.c encryption.c segment.c injection.c str_funcs.c mem_funcs.c
ASM_FILES = encrypt.s
OBJ_FILES = $(SRC_FILES:.c=.o)
OBJ_FILES += $(ASM_FILES:.s=.o)
OBJS = $(addprefix $(OBJS_DIR), $(OBJ_FILES))
HEADERS = $(INCLUDES_DIR)woody.h

all: $(OBJS_DIR) $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	printf "\n\033[0;32m[$(NAME)] Linking [OK]\n\033[0;0m"

$(OBJS_DIR)%.o: $(SRCS_DIR)%.c $(HEADERS) Makefile
	$(CC) $(CFLAGS) -o $@ -c $<
	printf "\033[0;34m[$(NAME)] Compilation [$<]\n"

$(OBJS_DIR)%.o: $(SRCS_DIR)%.s $(HEADERS) Makefile
	$(NASM) -o $@ $<
	printf "\033[0;34m[$(NAME)] Compilation ASM [$<]\n"

$(OBJS_DIR):
	mkdir -p $@

clean:
	$(RM) -Rf $(OBJS_DIR)
	printf "\033[0;31m[$(NAME)] Clean [OK]\n"

fclean: clean
	$(RM) $(NAME)
	printf "\033[0;31m[$(NAME)] Fclean [OK]\n"

re: fclean all

.PHONY: clean re fclean all
.SILENT: