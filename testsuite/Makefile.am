CHRPATH = ../chrpath

RPATH = $(LDRPATH)/usr/local/lib

noinst_PROGRAMS = prog

prog_SOURCES = \
	prog.c

EXTRA_DIST = runtest.sh

prog: prog.c
	$(CC) $(RPATH) -o $@ $<

check: $(CHRPATH)
	$(srcdir)/runtest.sh
