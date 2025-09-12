#include "woody.h"

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

void	ft_strdel(char **as)
{
	if (as && *as)
	{
		free(*as);
		*as = NULL;
	}
}

void	ft_putstr(const char *s)
{
	if (s)
		write(1, s, ft_strlen(s));
}
