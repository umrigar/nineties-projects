m4_dnl This file should be processed by GNU-m4 to produce a Makefile.in using:
m4_dnl gm4 --prefix-builtins --include=MAKE_LIB_DIR Makefile.m4 >Makefile.in
m4_dnl where MAKE_LIB_DIR is the name of a directory which contains a library
m4_dnl of Makefile fragments.  The m4 preprocessing eases the maintenance of
m4_dnl many Makefile.in's which are almost identical.
m4_changequote({{, }})m4_dnl
#Makefile.in produced automatically using GNU-m4.

#Name of program.
PROGRAM=		c2tex
VERSION=		0.0

m4_dnl Initial configuration section of Makefile.in.
m4_include({{init.mf}})

ZLEX_LIBSRC_DIR=	/usr/local/share/zlex

PRG_LIBSRC_DIR=		/$(PROGRAM)

LIBS=			$(ZLIBS) $(XLIBS)

m4_dnl Suffix rules.
m4_include({{suffix.mf}})

#Main target to be built in this directory.
TARGET=		c2tex

#Extra -D option passed to the C-compiler.
XFLAGS=	-DZYACC_SEARCH_PATH=\"$$''ZYACC_SKEL_DIR:.:$$''HOME:$(libsrcdir)\" \
	-DVERSION=\"$(VERSION)\"

#FILE DESCRIPTIONS.
#Human-generated C files.
C_SRC_FILES= 

#Computer-generated C files.
C_NON_SRC_FILES= \
  c2tex.c scanlib.c

#All C files.
CFILES= \
  $(C_SRC_FILES) $(C_NON_SRC_FILES)

#Human-generated header files.
H_SRC_FILES= 


#Computer-generated header files.
H_NON_SRC_FILES= 

#All header files.
HFILES= \
  $(H_SRC_FILES) $(H_NON_SRC_FILES)

#All human-generated files to be distributed.
SRC_FILES= \
  $(C_SRC_FILES) $(H_SRC_FILES) c2tex.l  \
  Makefile.m4 README

#All computer-generated files to be distributed.
NON_SRC_FILES= \
  $(C_NON_SRC_FILES) $(H_NON_SRC_FILES) \
  Makefile.in DEPEND 

#All files to be distributed.
DIST_FILES= \
  $(SRC_FILES) $(NON_SRC_FILES)

#Object files.
OFILES= \
  c2tex.o scanlib.o


all:		$(TARGET) $(SKEL)

install:	$(TARGET)
		$(topdir)/install-chk $(SIGNATURE) $(bindir)/$(TARGET) \
		&& $(INSTALL_PROGRAM) $(TARGET) $(bindir)/$(TARGET)

uninstall:
		$(topdir)/install-chk $(SIGNATURE) $(bindir)/$(TARGET) \
		&& $(RM) $(bindir)/$(TARGET)

check:		$(TARGET)
		@echo No checks for c2tex

$(TARGET):	$(OFILES) $(LIBZ_BIN)/libz.a
		$(CC) $(LDFLAGS) $(OFILES) $(LIBS) -o $@ 

#We compile c2tex.c without $(CFLAGS) specified; this should turn off
#optimization to ensure that no library function calls get optimized out.
scanlib.c:	c2tex.c
		$(CC) -c $(CPPFLAGS) $(CXXFLAGS) $(DEFFLAGS) $? -o fullscan.o
		$(ZLEX_LIBSRC_DIR)/mklibsrc fullscan.o $@
		$(RM) fullscan.o


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

c2tex.c:	c2tex.l

m4_include({{DEPEND}})

