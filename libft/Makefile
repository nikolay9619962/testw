# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: alagroy- <alagroy-@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/09/17 17:52:36 by alagroy-          #+#    #+#              #
#    Updated: 2021/04/27 10:45:18 by alagroy-         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libft.a
 
CC = gcc

SRCS = ft_strdel.c ft_strlen.c ft_putstr.c  \
	   ft_memset.c \
	   ft_memcpy.c ft_memcmp.c

OBJ = $(patsubst %.c, $(OBJ_PATH)%.o, $(SRCS))

vpath %.c srcs/2dstr/
vpath %.c srcs/str/
vpath %.c srcs/mem/
vpath %.c srcs/print/
vpath %.c srcs/lst/
vpath %.c srcs/gnl/

CFLAGS = -Wall -Werror -Wextra #-g -fsanitize=address

INCLUDES = ./includes/
HEADER += $(INCLUDES)libft.h 


OBJ_PATH = ./.obj/

all: $(OBJ_PATH) $(NAME)

$(NAME): $(OBJ)
	ar rcs $@ $^
	printf "\n\033[0;32m[libft] Linking [OK]\n"

$(OBJ_PATH):
	mkdir $@

$(OBJ): $(OBJ_PATH)%.o: %.c $(HEADER) Makefile
	$(CC) $(CFLAGS) -I$(INCLUDES) -c $< -o $@
	printf "\033[0;32m[libft] Compilation [$<]                 \r\033[0m"

clean:
	$(RM) -R $(OBJ_PATH)
	printf "\033[0;31m[libft] Clean [OK]\n"

fclean: clean
	$(RM) $(NAME)
	printf "\033[0;31m[libft] Fclean [OK]\n"

re: fclean all

.PHONY: fclean clean all re
.SILENT:
