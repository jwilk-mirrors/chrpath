#ifndef PROTOS_H
#define PROTOS_H

#include <elf.h>
#include "config.h"

#ifdef WORDS_BIGENDIAN
#define ELFDATA2 ELFDATA2MSB
#else
#define ELFDATA2 ELFDATA2LSB
#endif
#if SIZEOF_VOID_P == 8
#define Elf_Ehdr Elf64_Ehdr
#define ELFCLASS ELFCLASS64
#define Elf_Phdr Elf64_Phdr
#define Elf_Shdr Elf64_Shdr
#define Elf_Dyn  Elf64_Dyn
#elif SIZEOF_VOID_P == 4
#define Elf_Ehdr Elf32_Ehdr
#define ELFCLASS ELFCLASS32
#define Elf_Phdr Elf32_Phdr
#define Elf_Shdr Elf32_Shdr
#define Elf_Dyn  Elf32_Dyn
#else
#error "Unknown word size (SIZEOF_VOID_P)!"
#endif

int killrpath(const char *filename);
int chrpath(const char *filename, const char *newpath, int convert);

int elf_open(const char *filename, int flags, Elf_Ehdr *ehdr);
void elf_close(int fd);
int elf_find_dynamic_section(int fd, Elf_Ehdr *ehdr, Elf_Phdr *phdr);
const char *elf_tagname(int tag);
int elf_dynpath_tag(int tag);

#endif /* PROTOS_H */
