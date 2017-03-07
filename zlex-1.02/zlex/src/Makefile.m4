m4_dnl This file should be processed by GNU-m4 to produce a Makefile.in using:
m4_dnl gm4 --prefix-builtins --include=MAKE_LIB_DIR Makefile.m4 >Makefile.in
m4_dnl where MAKE_LIB_DIR is the name of a directory which contains a library
m4_dnl of Makefile fragments.  The m4 preprocessing eases the maintenance of
m4_dnl many Makefile.in's which are almost identical.
m4_changequote({{, }})m4_dnl
#Makefile.in produced automatically using GNU-m4.

#Name of program.
PROGRAM=		zlex
VERSION=		1.02

m4_dnl Initial configuration section of Makefile.in.
m4_include({{init.mf}})

ZLEX_PATH=		./
ZLEX_SEARCH_PATH=	.

LIBZLEX_SRC=		$(srcdir)/../libzlex
LIBZLEX_BIN=		../libzlex

XLIBS=			-L$(LIBZLEX_BIN) -lzlex

ZYACCFLAGS=		-d -v

m4_dnl Suffix rules.
m4_include({{suffix.mf}})

#Extra -D option passed to the C-compiler.
XFLAGS=	-DZLEX_SEARCH_PATH='"$$ZLEX_SEARCH_PATH:.:$$HOME:$(libsrcdir)"' \
	-DVERSION='"$(VERSION)"' 

#Main target to be built in this directory.
TARGET=		zlex
SKEL=		zlexskl.c

#FILE DESCRIPTIONS.
#Human-generated C files.
C_SRC_FILES= \
  chrclass.c codescan.c comptab.c datetime.c dfa.c \
  main.c mkoptinf.c mkopttab.c optcheck.c opttab0.c options.c \
  outdfa.c outvec.c patterns.c

#Computer-generated C files.
C_NON_SRC_FILES= \
  opttab.c parse.c scan.c

#All C files.
CFILES= \
  $(C_SRC_FILES) $(C_NON_SRC_FILES)

#Human-generated header files.
H_SRC_FILES= \
  chrclass.h codescan.h comptab.h datetime.h dfa.h files.h \
  main.h options.h outdfa.h outprms.h outvec.h patterns.h port.h \
  scan.h sklnames.h zparse.h

#Computer-generated header files.
H_NON_SRC_FILES= \
  outfns.h parse.h

#All header files.
HFILES= \
  $(H_SRC_FILES) $(H_NON_SRC_FILES)

#All human-generated files to be distributed.
SRC_FILES= \
  $(C_SRC_FILES) $(H_SRC_FILES) scan.l parse.y \
  Makefile.m4 $(SKEL)m4 NOTES.txt README SYSDEPS.txt

#All computer-generated files to be distributed.
NON_SRC_FILES= \
  $(C_NON_SRC_FILES) $(H_NON_SRC_FILES) \
  Makefile.in DEPEND $(SKEL) opttab.texi 

#All files to be distributed.
DIST_FILES= \
  $(SRC_FILES) $(NON_SRC_FILES)

#Object files.
OFILES= \
  chrclass.o codescan.o comptab.o datetime.o dfa.o \
  main.o options.o outdfa.o outvec.o parse.o patterns.o scan.o

all:		$(TARGET) $(SKEL)

install:	$(TARGET) $(SKEL)
		$(topdir)/install-chk $(SIGNATURE) $(bindir)/$(TARGET) \
		&& $(INSTALL_PROGRAM) $(TARGET) $(bindir)/$(TARGET)
		$(topdir)/install-chk $(SIGNATURE) $(libsrcdir)/$(SKEL) \
		&& $(INSTALL_DATA) $(srcdir)/$(SKEL) $(libsrcdir)/$(SKEL)

uninstall:
		$(topdir)/install-chk $(SIGNATURE) $(bindir)/$(TARGET) \
		&& $(RM) $(bindir)/$(TARGET)
		$(topdir)/install-chk $(SIGNATURE) $(libsrcdir)/$(SKEL) \
		&& $(RM) $(libsrcdir)/$(SKEL)

check:		$(TARGET) scan.l
		@echo -n "Using zlex to build zlex scanner ... " ; \
		$(ZLEX_PATH)$(TARGET) $(ZLEXFLAGS) scan.l -o t$$$$.c ; \
		$(SED) -e "s/t$$$$.c/scan.c/g" t$$$$.c >t.c ; \
		$(RM) t$$$$.* ; \
		if $(CMP) scan.c t.c >/dev/null 2>&1 ; \
		then \
		  echo OK ; $(RM) t.c ; \
		else \
		  $(MV) t.c scan.ERR ; \
		  echo FAILED ; \
		  echo "Suggest diff'ing error log in scan.ERR with " \
		       "distributed scan.c" ; \
		fi

$(TARGET):	$(OFILES) $(SKEL) $(LIBZ_BIN)/libz.a $(LIBZLEX_BIN)/libzlex.a
		$(CC) $(LDFLAGS) $(OFILES) $(ZLIBS) $(XLIBS)  -o $@ 

$(SKEL) outfns.h: $(SKEL)m4 \
		  $(LIBZLEX_SRC)/libzlex.h $(LIBZLEX_SRC)/libzlexs.h
		$(GM4) -I$(srcdir) $(srcdir)/$(SKEL)m4 >$(srcdir)/$(SKEL)

mkopttab:	mkopttab.o
		$(CC) $(LDFLAGS) mkopttab.o $(ZLIBS) $(XLIBS)  -o $@ 

opttab.c:	mkopttab
		./mkopttab >$@

mkoptinf:	mkoptinf.o
		$(CC) $(LDFLAGS) mkoptinf.o $(ZLIBS) $(XLIBS)  -o $@ 

opttab.texi:	mkoptinf
		./mkoptinf >$@

m4_include({{clean.mf}})

Makefile.in:	Makefile.m4 DEPEND FORCE
		$(MAKE_MF_IN)

#Dependency generation.
m4_include({{depend.mf}})

dist:		$(SKEL) opttab.texi

m4_include({{dist.mf}})

#All the object files depend on config.h.  Not put into automatic
#dependencies as we don't know the directory for config.h till build-time.
$(OFILES):	$(builddir)/config.h


scan.c:		scan.l $(SKEL)
parse.c:	parse.y
parse.h:	parse.y

#Force recompilation of datetime.c for each make.
datetime.o:	.FORCE

.FORCE:


m4_include({{DEPEND}})

