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

ZLEX_PATH=		../
ZLEX_SEARCH_PATH=	$(srcdir)/..

XLIBS=			-L$(builddir)/libzlex -lzlex

m4_dnl Suffix rules.
m4_include({{suffix.mf}})

#All human-generated files to be distributed.
SRC_FILES= \
  16bit.in1 16bit.l \
  8bit.in 8bit.l \
  backup.in backup.l \
  cecho.in cecho.l \
  chrcount.in chrcount.l \
  context1.in context1.l \
  finito.in finito.l \
  flush.in flush.l \
  huge.l \
  include.in include.in1 include.l \
  interact.in interact.l \
  intratok.in intratok.l \
  lessmore.in lessmore.l \
  linecol.in linecol.l \
  memscan.in memscan.l \
  reject1.in reject1.l \
  rejintra.in rejintra.l \
  rejunput.in rejunput.l \
  sentinel.in sentinel.l \
  ssstk.in ssstk.l \
  startalt.in startalt.l \
  unput.in unput.l \
  useract.in useract.l \
  wrap.in wrap.l \
  yylineno.in yylineno.l \
  mk16bit.c \
  dotests Makefile.m4 README

#All computer-generated files to be distributed.
NON_SRC_FILES= \
  16bit.log \
  8bit.log \
  backup.log \
  cecho.log \
  chrcount.log \
  context1.log \
  finito.log \
  flush.log \
  include.log \
  interact.log \
  intratok.log \
  lessmore.log \
  linecol.log \
  memscan.log \
  reject1.log \
  rejintra.log \
  rejunput.log \
  sentinel.log \
  ssstk.log \
  startalt.log \
  unput.log \
  useract.log \
  wrap.log \
  yylineno.log \
  Makefile.in 

#All files to be distributed.
DIST_FILES= \
  $(SRC_FILES) $(NON_SRC_FILES)

check:		huge.in huge.log 16bit.in
		./dotests

#Does only a subset of the tests.
QUICK:		huge.in huge.log 16bit.in
		QUICK_TEST=1 ./dotests

#Does only code scanners
CODE:		huge.in huge.log 16bit.in
		CODE_TEST=1 ./dotests

#Make 16bit.in from Ascii file to guard against endian switcheroos.
16bit.in:	mk16bit
		./mk16bit 16bit

#No dependency on ../dfa.c below as we don't really care.
huge.in:	
		@SRC=../dfa.c ; \
		echo Making a huge input file using $$SRC ; \
		cat $$SRC $$SRC $$SRC $$SRC $$SRC $$SRC $$SRC >tmp1.$$$$ ; \
		cat tmp1.$$$$ tmp1.$$$$ tmp1.$$$$ tmp1.$$$$ >tmp2.$$$$ ; \
		cat tmp2.$$$$ tmp2.$$$$ tmp2.$$$$ >$@ ; \
		$(RM) tmp1.$$$$ tmp2.$$$$ 

huge.log:	huge.in
		@$(LN) huge.in $@

clean:		FORCE
		$(RM) *.o core *~ *.bak t.* t[0-9]* 

distclean:	clean FORCE
		$(RM) $(TESTS) *.tst *.c huge.in huge.log Makefile *.ERR

maintainer-clean: distclean FORCE

FORCE:

m4_include({{dist.mf}})

Makefile.in:	Makefile.m4 FORCE
		$(MAKE_MF_IN)
