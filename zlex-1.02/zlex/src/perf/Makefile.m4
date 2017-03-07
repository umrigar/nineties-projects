m4_dnl This file should be processed by GNU-m4 to produce a Makefile.in using:
m4_dnl gm4 --prefix-builtins --include=MAKE_LIB_DIR Makefile.m4 >Makefile.in
m4_dnl where MAKE_LIB_DIR is the name of a directory which contains a library
m4_dnl of Makefile fragments.  The m4 preprocessing eases the maintenance of
m4_dnl many Makefile.in's which are almost identical.
m4_changequote({{, }})m4_dnl
#Makefile.in produced automatically using GNU-m4.
#We assume that any make variables defined on the make command-line are
#passed into a recursive make.

#Name of program.
PROGRAM=		zlex
VERSION=		1.02

m4_dnl Initial configuration section of Makefile.in.
m4_include({{init.mf}})

#SUFFIX RULES.
.SUFFIXES:

.SUFFIXES:	.l

.l:
	$(ZLEX_SEARCH_PATH) $(LEX) $(LFLAGS) $<
	$(CC) $(CFLAGS) lex.yy.c $(LIBS) -o $*

all:	crec1.in  crec2.in cscan1.in cscan2.in reject.in
	./doperf

crec1.in: ../dfa.c
	$(CAT) ../dfa.c ../dfa.c ../dfa.c ../dfa.c ../dfa.c ../dfa.c >t1.$$ ; \
	$(CAT) t1.$$ t1.$$ t1.$$ t1.$$ t1.$$ t1.$$ t1.$$ t1.$$ \
	       t1.$$ t1.$$ >$@ ; \
	$(RM) t1.$$

cscan1.in: crec1.in
	$(LN) crec1.in $@

crec2.in: crec1.in
	$(LN) crec1.in $@

cscan2.in: crec1.in
	$(LN) crec1.in $@

reject.in:
	echo "abcd" >t1.$$ ; \
        for n in 1 2 3 4 5 6 7 8 ; \
	do \
	  $(CAT) t1.$$ t1.$$ t1.$$ t1.$$ t1.$$ >t2.$$ ; \
	  $(MV) t2.$$ t1.$$ ; \
	done ; \
	$(MV) t1.$$ $@
	
SRC_FILES= \
  crec1.doc crec1.l \
  crec2.doc crec2.l \
  cscan1.doc cscan1.l \
  cscan2.doc cscan2.l \
  reject.doc reject.l \
  cscan.re cscan.l \
  Makefile.m4 doperf README

NON_SRC_FILES= \
  Makefile.in linux.log solaris.log sunos.log ultrix.log

DIST_FILES= \
  $(SRC_FILES) $(NON_SRC_FILES)

clean:	FORCE
	$(RM) *.in t.* t?.* core *.c $(TESTS)
	
distclean: clean FORCE
	$(RM) Makefile

maintainer-clean: distclean FORCE
	$(RM) $(NON_SRC_FILES)

FORCE:

m4_include({{dist.mf}})

Makefile.in:	Makefile.m4 FORCE
		$(MAKE_MF_IN)
