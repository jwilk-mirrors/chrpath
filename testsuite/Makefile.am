CHRPATH = ../chrpath

noinst_PROGRAMS = prog

prog_SOURCES = \
	prog.c

EXTRA_DIST = runtest.sh

prog: prog.c
	$(CC) -Wl,-rpath,/usr/local/lib -o $@ $<

check: $(CHRPATH)
	$(srcdir)/runtest.sh
