#ifndef PROTOS_H
#define PROTOS_H

#include <elf.h>

int killrpath(const char *filename);
int chrpath(const char *filename, const char *newpath);

int elf_open(const char *filename, int flags, Elf32_Ehdr *ehdr);
void elf_close(int fd);

#endif // PROTOS_H
