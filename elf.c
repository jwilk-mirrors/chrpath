
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <elf.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include "protos.h"

#ifdef WORDS_BIGENDIAN
#define ELFDATA2 ELFDATA2MSB
#else
#define ELFDATA2 ELFDATA2LSB
#endif

int
elf_open(const char *filename, int flags, Elf32_Ehdr *ehdr)
{
   int fd;

   fd = open(filename, flags);
   if (fd == -1)
   {
     perror ("open");
     return -1;
   }

   if (read(fd, ehdr, sizeof(*ehdr)) != sizeof(*ehdr))
   {
     perror ("reading header");
     close(fd);
     return -1;
   }

   if (0 != memcmp(ehdr->e_ident, ELFMAG, SELFMAG) ||
       ehdr->e_ident[EI_CLASS] != ELFCLASS32 ||
       ehdr->e_ident[EI_DATA] != ELFDATA2 ||
       ehdr->e_ident[EI_VERSION] != EV_CURRENT)
   {
     fprintf(stderr,
#ifdef WORDS_BIGENDIAN
             "`%s' probably isn't a 32-bit MSB-first ELF file.\n",
#else /* not WORD_BIGENDIAN */
             "`%s' probably isn't a 32-bit LSB-first ELF file.\n",
#endif /* not WORD_BIGENDIAN */
             filename);
     close(fd);
     errno = ENOEXEC; /* Hm, is this the best errno code to use? */
     return -1;
   }

   if (ehdr->e_phentsize != sizeof(Elf32_Phdr))
   {
     fprintf(stderr, "section size was read as %d, not %d!\n",
            ehdr->e_phentsize, sizeof(Elf32_Phdr));
     close(fd);
     return -1;
   }
   return fd;
}

int
elf_find_dynamic_section(int fd, Elf32_Ehdr *ehdr, Elf32_Phdr *phdr)
{
  int i;
  if (lseek(fd, ehdr->e_phoff, SEEK_SET) == -1)
  {
    perror ("positioning for sections");
    return 1;
  }

  for (i = 0; i < ehdr->e_phnum; i++)
  {
    if (read(fd, phdr, sizeof(*phdr)) != sizeof(*phdr))
    {
      perror ("reading section header");
      return 1;
    }
    if (phdr->p_type == PT_DYNAMIC)
      break;
  }
  if (i == ehdr->e_phnum)
    {
      fprintf (stderr, "No dynamic section found.\n");
      return 2;
    }

  if (0 == phdr->p_memsz)
    {
      fprintf (stderr, "Length of dynamic section is zero.\n");
      return 3;
    }

  return 0;
}

void
elf_close(int fd)
{
  close(fd);
}
