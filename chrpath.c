/*
<URL:http://gcc.gnu.org/ml/gcc/1999-04n/msg01105.html>

Re: changing embedded RPATH in existing executables.

  To: geoffk@ozemail.com.au
  Subject: Re: changing embedded RPATH in existing executables.
  From: <peeter_joot@VNET.IBM.COM> (peeter joot)
  Date: Fri, 30 Apr 1999 16:14:44 -0400 (EDT)
  Cc: peeterj@ca.ibm.com, egcs@cygnus.com, libc-hacker@cygnus.com, linux-gcc@vger.rutgers.edu
  Reply-To: <peeter_joot@VNET.IBM.COM>

> _Changing_ is a little tricky, but the attached program strips rpaths
> from executables (I find it essential for debugging the binutils).
> It's endian-dependent, if you want this for x86 you can just change
> the occurrences of 'MSB' to 'LSB' and compile (I should really fix
> that).

Hi Geoff,

With your program as a guide (and some peeks into libbfd, elf.h, a bit
of the glibc dynamic loader code, objdump, and a hex-editor) I was able to
figure out enough to find and change the rpath string.  That was fun!

This program assumes (unlike your original program) that there is only
one DT_RPATH tag in the dynamic section as even with multiple '-Wl,-rpath,'
commands in the link this seems to occur (they all get concatonated into
a : separated path).

Thanks for your help.  If you want to use this on non-x86 you have to change
the occurances of LSB back to MSB:)

Peeter
--
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
#include <string.h>
#include <sys/stat.h>
#include "protos.h"

/**
 * Reads an ELF file, and reads or alters the RPATH setting.
 *
 * TODO:
 *  modify to add RPATH setting if none exists.
 */


int
chrpath(const char *filename, const char *newpath, int convert)
{
  int fd;
  Elf_Ehdr ehdr;
  int i;
  Elf_Phdr phdr;
  Elf_Shdr shdr;
  Elf_Dyn *dyns;
  int rpathoff;
  char * strtab;
  char * rpath;
  unsigned int rpathlen;
  int oflags;
  int rpath_dyns_index;

  if (NULL == newpath && 0 == convert)
     oflags = O_RDONLY;
  else
     oflags = O_RDWR;

  fd = elf_open(filename, oflags, &ehdr);
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

  dyns = malloc(phdr.p_filesz);
  if (dyns == NULL)
    {
      perror ("allocating memory for dynamic section");
      return 1;
    }
  memset(dyns, 0, phdr.p_filesz);
  if (lseek(fd, phdr.p_offset, SEEK_SET) == -1
      || read(fd, dyns, phdr.p_filesz) != (int)phdr.p_filesz)
    {
      perror ("reading dynamic section");
      free(dyns);
      return 1;
    }

  rpathoff = -1;
  for ( rpath_dyns_index = 0; dyns[rpath_dyns_index].d_tag != DT_NULL;
        ++rpath_dyns_index )
    {
      if ( elf_dynpath_tag(dyns[rpath_dyns_index].d_tag) )
      {
         rpathoff = dyns[rpath_dyns_index].d_un.d_ptr;
         break;
      }
    }
  if (rpathoff == -1)
    {
      printf("%s: no rpath or runpath tag found.\n", filename);
      free(dyns);
      return 2;
    }

  if (lseek(fd, ehdr.e_shoff, SEEK_SET) == -1)
  {
    perror ("positioning for sections");
    free(dyns);
    return 1;
  }

  for (i = 0; i < ehdr.e_shnum; i++)
  {
    if (read(fd, &shdr, sizeof(shdr)) != sizeof(shdr))
    {
      perror ("reading section header");
      free(dyns);
      return 1;
    }
    if (shdr.sh_type == SHT_STRTAB)
      break;
  }
  if (i == ehdr.e_shnum)
    {
      fprintf (stderr, "No string table found.\n");
      free(dyns);
      return 2;
    }
  strtab = (char *)malloc(shdr.sh_size);
  if (strtab == NULL)
    {
      perror ("allocating memory for string table");
      free(dyns);
      return 1;
    }
  memset(strtab, 0, shdr.sh_size);

  if (lseek(fd, shdr.sh_offset, SEEK_SET) == -1)
  {
    perror ("positioning for string table");
    free(strtab);
    free(dyns);
    return 1;
  }
  if (read(fd, strtab, shdr.sh_size) != (int)shdr.sh_size)
  {
    perror ("reading string table");
    free(strtab);
    free(dyns);
    return 1;
  }

  if ((int)shdr.sh_size < rpathoff)
  {
    fprintf(stderr, "%s string offset not contained in string table",
            elf_tagname(dyns[rpath_dyns_index].d_tag));
    free(strtab);
    free(dyns);
    return 5;
  }
  rpath = strtab+rpathoff;

#if defined(DT_RUNPATH)
  if (convert && dyns[rpath_dyns_index].d_tag == DT_RPATH)
  {
    dyns[rpath_dyns_index].d_tag = DT_RUNPATH;
    if (lseek(fd, phdr.p_offset, SEEK_SET) == -1
        || write(fd, dyns, phdr.p_filesz) != (int)phdr.p_filesz)
    {
      perror ("converting RPATH to RUNPATH");
      return 1;
    }
    printf("%s: RPATH converted to RUNPATH\n", filename);
  }
#endif /* DT_RUNPATH */

  printf("%s: %s=%s\n", filename, elf_tagname(dyns[rpath_dyns_index].d_tag),
         rpath);

  if (NULL == newpath)
  {
    free(dyns);
    free(strtab);
    return 0;
  }

  rpathlen = strlen(rpath);

  /*
   * Calculate the maximum rpath length (will be equal to rpathlen unless
   * we have previously truncated it).
   */
  for ( i = rpathoff + rpathlen ; (i < (int)shdr.sh_size
                                   && strtab[i] == '\0') ; i++ )
    ;
  i--;

  if (i > (int)(rpathoff + rpathlen))
     rpathlen = i - rpathoff;

  if (strlen(newpath) > rpathlen)
  {
    fprintf(stderr, "new rpath '%s' too large; maximum length %i\n",
            newpath, rpathlen);
    free(dyns);
    free(strtab);
    return 7;
  }

  memset(rpath, 0, rpathlen);
  strcpy(rpath, newpath);

  if (lseek(fd, shdr.sh_offset+rpathoff, SEEK_SET) == -1)
  {
    perror ("positioning for RPATH");
    free(dyns);
    free(strtab);
    return 1;
  }
  if (write(fd, rpath, rpathlen) != (int)rpathlen)
  {
    perror ("writing RPATH");
    free(dyns);
    free(strtab);
    return 1;
  }
  printf("%s: new %s: %s\n", filename,
         elf_tagname(dyns[rpath_dyns_index].d_tag), rpath);

  elf_close(fd);

  free(dyns);
  dyns = NULL;

  free(strtab);

  return 0;
}
