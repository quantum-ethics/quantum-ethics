###########################################################################
#  This is the Unix makefile for FeynDiagram.
#  You should edit the options below.  Then, to build the libarary
#  and manpage, do "make".  The manpage will contain information about
#  where things will be put during installation.  If you want to add
#  special information relevant to your system, you should edit the manpage
#  'feyndiagram.3' at this point.  If all goes well, do "make install" to 
#  move everything to the right place, set permissions, etc.
#  Tutorial-style documentation for using FeynDiagram is contained in a
#  compressed PostScript file in the Docs directory.
###########################################################################
# SCCS ID for makefile: $Id: makefile_distrib,v 2.6 2000/04/18 19:49:29 billd Exp $

# CC - the name of your C++ compiler (default CC)
CC=CC

# LIBDIR - where you keep local C++ libraries (default /usr/local/lib)
LIBDIR=/usr/local/lib

# LIBNAME - name of the FeynDiagram library, without leading 'lib' or
#           trailing '.a' (default FD)
LIBNAME=FD

# FDHOME - directory where FeynDiagram's prolog, documentation, examples, etc.
#          are to be kept (default /usr/local/lib/FeynDiagram)
FDHOME=/usr/local/lib/FeynDiagram

# INCLUDEDIR - directory for local include files (default /usr/local/include)
INCLUDEDIR=/usr/local/include

# MANDIR - directory for level 3 local manpages in nroff 
#          format (default /usr/local/man/man3)
MANDIR=/usr/local/man/man3

# RANLIB - set to 'ranlib' if your system uses ranlib, and set to empty string
#          if your system doesn't use ranlib to convert archive to libraries
#          (default ranlib)
RANLIB=ranlib

# OWNER - who should own the files when they are installed (default root)
OWNER=root

# GROUP - group id for files after installation (default bin)
GROUP=bin

# PERM - permissions of files after installation (default 644)
PERM=644

# DIRPERM - permissions of directories created in installation (default 755)
DIRPERM=755

# Uncomment the line below (ie. remove the '#' at the beginning) if your
# compiler doesn't have the include file "osfcn.h"
NOOSFCN=-DNOOSFCN

###########  You should not need to modify anything beyond this point #########

CFLAGS=-I.. -DPROLOGDIR=\"$(FDHOME)/Prolog/\" $(NOOSFCN)
SHELL=/bin/sh

all: lib$(LIBNAME).a feyndiagram.3

lib$(LIBNAME).a: default.o fd.o text2PS.o
	ar rc lib$(LIBNAME).a default.o fd.o text2PS.o
	-if test -n "$(RANLIB)"; then \
	$(RANLIB) lib$(LIBNAME).a ; fi

install: install_lib install_fdhome install_include install_manpage

install_lib: lib$(LIBNAME).a
	cp lib$(LIBNAME).a $(LIBDIR)/lib$(LIBNAME).a
	-if test -n "$(RANLIB)"; then \
	$(RANLIB) $(LIBDIR)/lib$(LIBNAME).a ; fi
	chmod $(PERM) $(LIBDIR)/lib$(LIBNAME).a
	chgrp $(GROUP) $(LIBDIR)/lib$(LIBNAME).a
	chown $(OWNER) $(LIBDIR)/lib$(LIBNAME).a

install_fdhome:
	-if test ! -d $(FDHOME); then \
	mkdir $(FDHOME); fi
	-if test ! -d $(FDHOME)/Docs; then \
	mkdir $(FDHOME)/Docs; fi
	cp Docs/tutorial.ps.Z $(FDHOME)/Docs
	cp Docs/technical_notes $(FDHOME)/Docs
	-if test ! -d $(FDHOME)/Examples; then \
	mkdir $(FDHOME)/Examples; fi
	cp Examples/*.C $(FDHOME)/Examples
	-if test ! -d $(FDHOME)/Prolog; then \
	mkdir $(FDHOME)/Prolog; fi
	cp Prolog/*.ps $(FDHOME)/Prolog
	find $(FDHOME) -type f -print | xargs chmod $(PERM)
	find $(FDHOME) -type f -print | xargs chgrp $(GROUP)
	find $(FDHOME) -type f -print | xargs chown $(OWNER)
	find $(FDHOME) -type d -print | xargs chmod $(DIRPERM)

install_include:
	-if test ! -d $(INCLUDEDIR)/FeynDiagram; then \
	mkdir $(INCLUDEDIR)/FeynDiagram; fi
	cp default.h fd.h text2PS.h $(INCLUDEDIR)/FeynDiagram
	find $(INCLUDEDIR)/FeynDiagram -type f -print | xargs chmod $(PERM)
	find $(INCLUDEDIR)/FeynDiagram -type f -print | xargs chgrp $(GROUP)
	find $(INCLUDEDIR)/FeynDiagram -type f -print | xargs chown $(OWNER)
	find $(INCLUDEDIR)/FeynDiagram -type d -print | xargs chmod $(DIRPERM)

install_manpage: feyndiagram.3
	cp feyndiagram.3 $(MANDIR)
	chmod $(PERM) $(MANDIR)/feyndiagram.3
	chgrp $(GROUP) $(MANDIR)/feyndiagram.3
	chown $(OWNER) $(MANDIR)/feyndiagram.3

default.o: default.C default.h
	$(CC) $(CFLAGS) -c default.C

fd.o: fd.C fd.h default.h text2PS.h makefile
	$(CC) $(CFLAGS) -c fd.C

text2PS.o: text2PS.C text2PS.h
	$(CC) $(CFLAGS) -c text2PS.C

feyndiagram.3: makefile
	./mkmanpage $(CC) $(LIBNAME) $(INCLUDEDIR) $(FDHOME)

clean:
	rm -f *.o lib$(LIBNAME).a feyndiagram.3

