/*
 * Author: Petter Reinholdtsen <pere@td.org.uit.no>
 * date:   2001-01-20
 *
 * Alter ELF rpath information (insert, modify, remove).
 *
 * Based on source from Peeter Joot <peeterj@ca.ibm.com> and Geoffrey
 * Keating <geoffk@ozemail.com.au>.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <stdio.h>
#include <getopt.h>
#include "protos.h"

static void
usage(char *progname)
{
  printf("Usage: %s [-d|-r <path>] <program>\n\n", progname);
  printf("   -d|--delete                 Delete current rpath setting\n");
  printf("   -r <path>|--replace <path>  Replace current rpath setting\n");
  printf("                               with the path given\n");
  printf("   -v|--version                Display program version number\n");
}

static struct option long_options[] =
{
  {"delete",  1, 0, 'd'},
  {"replace", 1, 0, 'r'},
  {"version", 0, 0, 'v'}
};

int
main(int argc, char * const argv[])
{
  int remove = 0;       /* remove or not */
  char *newpath = NULL; /* insert this path */
  int opt;
  int option_index;

  if (argc < 2)
    {
      usage(argv[0]);
      return 1;
    }

  do {
    opt = getopt_long(argc, argv, "dr:v",
                      long_options, &option_index);
    switch (opt)
      {
      case 'k':
        remove = 1;
        break;
      case 'R':
        newpath = optarg;
        break;
      case 'v':
        printf("%s version %s\n", PACKAGE, VERSION);
        exit(0);
        break;
      case -1:
        break;
      default:
        printf("Invalid argument '%c'\n", opt);
        usage(argv[0]);
        exit(0);
      }
  } while (-1 != opt);

  if (remove)
    killrpath(argv[1]);
  else
    chrpath(argv[1], newpath); /* list by default, replace if path is set */

  return 0;
}
