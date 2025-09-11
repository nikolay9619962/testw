NAME = woody_woodpacker
CC = gcc
CFLAGS = -Wall -Wextra -Werror -fno-stack-protector -z execstack -g
ASM = nasm
ASMFLAGS = -f elf64 -g

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
ASM_SRC = $(wildcard asm/*.s)
ASM_OBJ = $(ASM_SRC:.s=.o)
HEADERS = src/woody.h

all: $(NAME)

$(NAME): $(OBJ) $(ASM_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -no-pie

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -f $(OBJ) $(ASM_OBJ)

fclean: clean
	rm -f $(NAME) woody

re: fclean all

.PHONY: all clean fclean re