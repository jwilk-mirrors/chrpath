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
#include <stdlib.h>
#include "protos.h"

#ifdef HAVE_GETOPT_LONG
#  define GETOPT_LONG getopt_long
#else /* not HAVE_GETOPT_LONG */
#  define GETOPT_LONG(argc,argv,optstr,lopts,lidx) getopt(argc,argv,optstr)
#endif /* not HAVE_GETOPT_LONG */

static void
usage(char *progname)
{
  printf("Usage: %s [-v|-d|-r <path>] <program>\n\n", progname);
  printf("   -v|--version                Display program version number\n");
  printf("   -d|--delete                 Delete current rpath setting\n");
  printf("   -r <path>|--replace <path>  Replace current rpath setting\n");
  printf("                               with the path given\n");
#ifndef HAVE_GETOPT_LONG
  printf("\n *** The long options are not available on this platform\n");
#endif /* not HAVE_GETOPT_LONG */
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
    opt = GETOPT_LONG(argc, argv, "dr:v", long_options, &option_index);
    switch (opt)
      {
      case 'd':
        remove = 1;
        break;
      case 'r':
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
    killrpath(argv[optind]);
  else
    /* list by default, replace if path is set */
    chrpath(argv[optind], newpath);

  return 0;
}
