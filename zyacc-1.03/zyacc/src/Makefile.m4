m4_dnl This file should be processed by GNU-m4 to produce a Makefile.in using:
m4_dnl gm4 --prefix-builtins --include=MAKE_LIB_DIR Makefile.m4 >Makefile.in
m4_dnl where MAKE_LIB_DIR is the name of a directory which contains a library
m4_dnl of Makefile fragments.  The m4 preprocessing eases the maintenance of
m4_dnl many Makefile.in's which are almost identical.
m4_changequote({{, }})m4_dnl
#Makefile.in produced automatically using GNU-m4.

#Name of program.
PROGRAM=		zyacc
VERSION=		1.03

m4_dnl Initial configuration section of Makefile.in.
m4_include({{init.mf}})

ZYACC_PATH=		.
ZYACC_SEARCH_PATH=	.
ZLEX_LIBSRC_DIR=	/usr/local/share/zlex

PRG_LIBSRC_DIR=		/$(PROGRAM)

LIBS=			$(ZLIBS) $(XLIBS)

m4_dnl Suffix rules.
m4_include({{suffix.mf}})

#Main target to be built in this directory.
TARGET=		zyacc
SKEL=		zyaccskl.c
DEBUG=		zydebug

#Extra -D option passed to the C-compiler.
XFLAGS=	-DZYACC_SEARCH_PATH=\"$$''ZYACC_SEARCH_PATH:.:$$''HOME:$(libsrcdir)\" \
	-DVERSION=\"$(VERSION)\"

#FILE DESCRIPTIONS.
#Human-generated C files.
C_SRC_FILES= \
  attribs.c buildrec.c comptab.c gcolor.c gram.c lalr.c \
  main.c mkoptinf.c mkopttab.c \
  opttab0.c options.c outvec.c xset.c zydebug.c

#Computer-generated C files.
C_NON_SRC_FILES= \
  opttab.c parse.c scan.c scanlib.c

#All C files.
CFILES= \
  $(C_SRC_FILES) $(C_NON_SRC_FILES)

#Human-generated header files.
H_SRC_FILES= \
  attribs.h buildrec.h comptab.h gcolor.h gram.h lalr.h main.h \
  options.h outvec.h \
  port.h scan.h xset.h zparse.h

#Computer-generated header files.
H_NON_SRC_FILES= \
  outfns.h parse.h

#All header files.
HFILES= \
  $(H_SRC_FILES) $(H_NON_SRC_FILES)

#All human-generated files to be distributed.
SRC_FILES= \
  $(C_SRC_FILES) $(H_SRC_FILES) scan.l parse.y \
  Makefile.m4 $(SKEL)m4 README TODO

#All computer-generated files to be distributed.
NON_SRC_FILES= \
  $(C_NON_SRC_FILES) $(H_NON_SRC_FILES) \
  opttab.texi Makefile.in DEPEND $(SKEL)

#All files to be distributed.
DIST_FILES= \
  $(SRC_FILES) $(NON_SRC_FILES)

#Object files for zyacc.
OFILES= \
  attribs.o buildrec.o comptab.o gcolor.o gram.o lalr.o \
  main.o options.o outvec.o \
  parse.o scan.o scanlib.o xset.o


all:		$(TARGET) $(SKEL) $(DEBUG)

install:	$(TARGET) $(SKEL) $(DEBUG)
		@$(topdir)/install-chk $(SIGNATURE) $(bindir)/$(TARGET) \
		&& $(INSTALL_PROGRAM) $(TARGET) $(bindir)/$(TARGET)
		@$(topdir)/install-chk $(SIGNATURE) $(libsrcdir)/$(SKEL) \
		&& $(INSTALL_DATA) $(srcdir)/$(SKEL) $(libsrcdir)/$(SKEL); \
		if grep '#define[ \t]*HAVE_SYS_SOCKET_H[ \t]*1' \
			$(builddir)/config.h >/dev/null ; \
		then \
		 $(topdir)/install-chk $(SIGNATURE) $(bindir)/$(DEBUG) \
		 && $(INSTALL_PROGRAM) $(srcdir)/$(DEBUG) $(bindir)/$(DEBUG); \
		fi
uninstall:
		$(topdir)/install-chk $(SIGNATURE) $(bindir)/$(TARGET) \
		&& $(RM) $(bindir)/$(TARGET)
		$(topdir)/install-chk $(SIGNATURE) $(bindir)/$(DEBUG) \
		&& $(RM) $(bindir)/$(DEBUG)
		$(topdir)/install-chk $(SIGNATURE) $(libsrcdir)/$(SKEL) \
		&& $(RM) $(libsrcdir)/$(SKEL)

check:		$(TARGET) parse.y 
		@echo -n "Using zyacc to build zyacc parser ... " ; \
		$(ZYACC_PATH)/$(TARGET) $(ZYACCFLAGS) parse.y -o t$$$$.c ; \
		$(SED) -e "s/t$$$$.c/parse.c/g" t$$$$.c >t.c ; \
		$(RM) t$$$$.* ; \
		if $(CMP) parse.c t.c >/dev/null 2>&1 ; \
		then \
		  echo OK ; $(RM) t.c ; \
		else \
		  $(MV) t.c parse.ERR ; \
		  echo FAILED ; \
		  echo "Suggest diff'ing error log in parse.ERR with " \
		       "distributed parse.c" ; \
		fi

$(TARGET):	$(OFILES) $(SKEL) $(LIBZ_BIN)/libz.a
		$(CC) -c $(CPPFLAGS) $(CXXFLAGS) $(DEFFLAGS) $(CFLAGS) \
	-DBUILD_CMD='"$(CC) $(CPPFLAGS) $(CXXFLAGS) $(DEFFLAGS) $(CFLAGS)"' \
		  buildrec.c
		$(CC) $(LDFLAGS) $(OFILES) $(LIBS) -o $@ 

$(SKEL) outfns.h: $(SKEL)m4 $(srcdir)/../libzyacc/libzyacc.h
		$(GM4) -I$(srcdir) $(srcdir)/$(SKEL)m4 >$(srcdir)/$(SKEL)

$(DEBUG):	$(DEBUG).o
		$(CC) $(LDFLAGS) $(DEBUG).o $(LIBS) -o $@ 


#We compile scan.c without $(CFLAGS) specified; this should turn off
#optimization to ensure that no library function calls get optimized out.
scanlib.c:	scan.c
		$(CC) -c $(CPPFLAGS) $(CXXFLAGS) $(DEFFLAGS) $? -o fullscan.o
		$(ZLEX_LIBSRC_DIR)/mklibsrc fullscan.o $@
		$(RM) fullscan.o
		touch $@

mkopttab:	mkopttab.o
		$(CC) $(LDFLAGS) mkopttab.o $(ZLIBS) $(XLIBS)  -o $@ 

opttab.c:	
		$(REC_MAKE) mkopttab
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

m4_include({{dist.mf}})

m4_include({{libzhdr.mf}})

#All the object files depend on config.h.  Not put into automatic
#dependencies as we don't know the directory for config.h till build-time.
$(OFILES):	$(builddir)/config.h

scan.c:		scan.l
parse.c:	parse.y  $(SKEL)
parse.h:	parse.y  $(SKEL)

m4_include({{DEPEND}})

