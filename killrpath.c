/*
Taken from another list:

_Changing_ is a little tricky, but the attached program strips rpaths
from executables (I find it essential for debugging the binutils).
It's endian-dependent, if you want this for x86 you can just change
the occurrences of 'MSB' to 'LSB' and compile (I should really fix
that).

--
Geoffrey Keating <geoffk@ozemail.com.au>
*/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <elf.h>
#if defined(HAVE_LINK_H)
#  include <link.h>
#endif /* HAVE_LINK_H */
#include <stdlib.h>
#include "protos.h"
#include <string.h>

/* Reads an ELF file, nukes all the RPATH entries. */

int
killrpath(const char *filename)
{
   int fd;
   Elf_Ehdr ehdr;
   int i;
   Elf_Phdr phdr;
   Elf_Dyn *dyns;
   int dynpos;

   fd = elf_open(filename, O_RDWR, &ehdr);

   if (fd == -1)
   {
     perror ("elf_open");
     return 1;
   }

   if (0 != elf_find_dynamic_section(fd, &ehdr, &phdr))
   {
     perror("found no dynamic section");
     return 1;
   }

   dyns = malloc(phdr.p_memsz);
   if (dyns == NULL)
     {
       perror ("allocating memory for dynamic section");
       return 1;
     }
   memset(dyns, 0, phdr.p_memsz);
   if (lseek(fd, phdr.p_offset, SEEK_SET) == -1
       || read(fd, dyns, phdr.p_filesz) != (int)phdr.p_filesz)
     {
       perror ("reading dynamic section");
       return 1;
     }

   dynpos = 0;
   for (i = 0; dyns[i].d_tag != DT_NULL; i++)
     {
       dyns[dynpos] = dyns[i];
       if ( ! elf_dynpath_tag(dyns[i].d_tag) )
        dynpos++;
     }
   for (; dynpos < i; dynpos++)
     dyns[dynpos].d_tag = DT_NULL;

   if (lseek(fd, phdr.p_offset, SEEK_SET) == -1
       || write(fd, dyns, phdr.p_filesz) != (int)phdr.p_filesz)
     {
       perror ("writing dynamic section");
       return 1;
     }

   elf_close(fd);

   return 0;
}
