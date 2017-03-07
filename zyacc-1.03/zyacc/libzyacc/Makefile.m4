m4_dnl This file should be processed by GNU-m4 to produce a Makefile.in using:
m4_dnl gm4 --prefix-builtins --include=MAKE_LIB_DIR Makefile.m4 >Makefile.in
m4_dnl where MAKE_LIB_DIR is the name of a directory which contains a library
m4_dnl of Makefile fragments.  The m4 preprocessing eases the maintenance of
m4_dnl many Makefile.in's which are almost identical.
m4_changequote({{, }})m4_dnl
#Makefile.in produced automatically using GNU-m4.

m4_dnl Initial configuration section of Makefile.in.
m4_include({{init.mf}})

m4_dnl Suffix rules.
m4_include({{suffix.mf}})

#Main target to be built in this directory.
TARGET=			libzyacc.a

#Human-generated C files.
C_SRC_FILES= \
  debug.c main.c remote.c

#Computer-generated C files.
C_NON_SRC_FILES=

CFILES= \
  $(C_SRC_FILES) $(C_NON_SRC_FILES)

#Human-generated header files.
H_SRC_FILES= \
  libzyacc.h libzyacp.h

#Computer-generated header files.
H_NON_SRC_FILES= 
  
#All header files.
HFILES= \
  $(H_SRC_FILES) $(H_NON_SRC_FILES)

OFILES= \
  debug.o main.o remote.o
 
#All human-generated files to be distributed.
SRC_FILES= \
  $(C_SRC_FILES) $(H_SRC_FILES) Makefile.m4 \
  frames.html applet.html \
  README

#All computer-generated files to be distributed.
NON_SRC_FILES= \
  $(C_NON_SRC_FILES) $(H_NON_SRC_FILES) Makefile.in DEPEND \
  frames.str applet.str

DIST_FILES= \
  $(SRC_FILES) $(NON_SRC_FILES)

#PUBLIC TARGETS.
all:		$(TARGET)

install:	$(TARGET)
		@if $(topdir)/install-chk $(SIGNATURE) $(libdir)/$(TARGET) ; \
		then \
		  $(INSTALL_DATA) $(TARGET) $(libdir)/$(TARGET) ; \
		fi
uninstall:
		if $(topdir)/install-chk $(SIGNATURE) $(libdir)/$(TARGET) ; \
		then \
		  $(RM) $(libdir)/$(TARGET) ; \
		fi

$(TARGET):	$(OFILES)
		$(AR) r $@ $?
		$(RANLIB) $@

frames.str:	frames.html
		echo "/* Automatically generated from $? */" >$@
		sed -e 's/"/\\"/g;s/^/"/;s/$$/\\n",/' >>$@ <$?
		echo '""' >>$@

applet.str:	applet.html
		echo "/* Automatically generated from $? */" >$@
		sed -e 's/"/\\"/g;s/^/"/;s/$$/\\n",/' >>$@ <$?
		echo '""' >>$@

m4_include({{clean.mf}})

Makefile.in:	Makefile.m4 DEPEND FORCE
		$(MAKE_MF_IN)

#Dependency generation.
m4_include({{depend.mf}})

m4_include({{dist.mf}})

#Manually inserted dependencies for 16-bit library.

m4_include({{DEPEND}})
