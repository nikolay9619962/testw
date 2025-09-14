
#ifndef WOODY_H
# define WOODY_H

# include <errno.h>
# include <fcntl.h>
# include <sys/mman.h>
# include <sys/syscall.h>
# include <stdio.h>
# include <elf.h>
# include <stdlib.h>
# include <unistd.h>

typedef uint16_t	t_arch;
typedef uint16_t	t_endian;

# define LENDIAN ELFDATA2LSB
# define BENDIAN ELFDATA2MSB

# define DECRYPTION_CODE "\x50\x56\x57\x52\x51\x41\x50\x41\x51\xeb\x55\x5e\xb8\x01\x00\x00\x00\xbf\x01\x00\x00\x00\xba\x0e\x00\x00\x00\x0f\x05\xb9\x2b\x00\x00\x00\xbe\x04\x00\x00\x00\x48\x8d\x15\x09\x00\x00\x00\x4d\x31\xc0\x48\x31\xc0\xeb\x3d\x5f\x42\x8a\x04\x07\x00\xe0\x30\x02\x48\xff\xc2\x49\xff\xc0\x49\x39\xf0\x75\x06\x80\xc4\x2a\x4d\x31\xc0\xe2\xe5\x41\x59\x41\x58\x59\x5a\x5f\x5e\x58\xe9\x26\x00\x00\x00\xe8\xa6\xff\xff\xff\x2e\x2e\x2e\x2e\x57\x4f\x4f\x44\x59\x2e\x2e\x2e\x2e\x0a\xe8\xbe\xff\xff\xff"
# define CODE_SIZE sizeof(DECRYPTION_CODE) -1 + KEY_SIZE

# define KEY_SIZE 32

typedef enum { false, true } bool;

typedef struct		s_file
{
	void		*ptr;
	void		*end;
	void		*text;
	void		*note;
	char		key[KEY_SIZE];
	off_t		size;
	t_arch		arch;
	t_endian	endian;
	int			fd;
}					t_file;

typedef struct		s_payload
{
	char		*code;
	size_t		size;
	uint16_t	i_tsize;
	uint16_t	i_ksize;
	uint16_t	i_text;
	uint16_t	i_key;
	uint16_t	i_jmp;
}					t_payload;

typedef struct 		s_woody
{
	void		*ptr;
	size_t		size;
	Elf64_Phdr	*data;
	Elf64_Phdr	*last;
}					t_woody;

extern void			encrypt(void *data, uint32_t data_len, void *text,
						uint32_t text_len);
uint16_t			get_uint16(uint16_t byte, t_endian endian);
int32_t				get_int32(int32_t byte, t_endian endian);
uint32_t			get_uint32(uint32_t byte, t_endian endian);
uint64_t			get_uint64(uint64_t byte, t_endian endian);
Elf64_Phdr			*get_ph_segment(t_file *file, bool (*predicate)(Elf64_Phdr *));
Elf64_Phdr			*get_last_load_segment(t_file *file);
bool					is_text_segment(Elf64_Phdr *phdr);
bool					is_data_segment(Elf64_Phdr *phdr);
void				inject(t_file *file, t_payload *payload);
void				encrypt_code(t_file *file);
void					parse_file(char *filename, t_file *file);
void				safe_exit(t_file *file, t_payload *payload,
						t_woody *woody, char* msg);

size_t			ft_strlen(const char *s);
void			ft_strdel(char **as);
void			ft_putstr(const char *s);

void			*ft_memset(void *b, int c, size_t len);
void			*ft_memcpy(void *dst, const void *src, size_t n);
int				ft_memcmp(const void *s1, const void *s2, size_t n);

#endif
