#ifndef PROTOS_H
#define PROTOS_H

#include <elf.h>

int killrpath(const char *filename);
int chrpath(const char *filename, const char *newpath, int convert);

int elf_open(const char *filename, int flags, Elf32_Ehdr *ehdr);
void elf_close(int fd);
int elf_find_dynamic_section(int fd, Elf32_Ehdr *ehdr, Elf32_Phdr *phdr);

#endif /* PROTOS_H */
