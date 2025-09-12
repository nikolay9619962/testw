/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alagroy- <alagroy-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/09/17 18:28:53 by alagroy-          #+#    #+#             */
/*   Updated: 2019/09/23 17:28:54 by alagroy-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VECTOR_H
# define VECTOR_H
# include "libft.h"

typedef int			(*t_func)(int c);

typedef struct		s_vector
{
	char	*value;
	size_t	len;
	size_t	size;
	int		scale;
}					t_vector;

ssize_t				vct_chr(t_vector *vct, char c);
t_vector			*vct_create(int len);
int					vct_delpart(t_vector *vct, unsigned int i,
					unsigned int len);
void				vct_destroy(t_vector **vct);
void				vct_dprint(t_vector *vct, int fd);
char				vct_gnc(t_vector *vct);
int					vct_nappend(t_vector *vct, char c, int n);
void				vct_print(t_vector *vct);
void				vct_printendl(t_vector *vct);
ssize_t				vct_rchr(t_vector *vct, char c);
int					vct_realloc(t_vector *vct);
void				vct_rev(t_vector *vct);
int					vct_strcpy(t_vector *vct, char *str);
t_vector			*vct_strdup(char *str);
int					vct_strncpy(t_vector *vct, char *str, unsigned int len);
ssize_t				vct_strstr(t_vector *vct, char *s);
t_vector			*vct_sub(t_vector *vct, int i, int len);
int					vct_vctcpy(t_vector *vct, t_vector *cpy);
t_vector			*vct_vctdup(t_vector *vct);
int					vct_vctncpy(t_vector *vct, t_vector *cpy, unsigned int n);

#endif
