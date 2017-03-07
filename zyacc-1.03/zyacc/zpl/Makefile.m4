m4_dnl This file should be processed by GNU-m4 to produce a Makefile.in using:
m4_dnl gm4 --prefix-builtins --include=MAKE_LIB_DIR Makefile.m4 >Makefile.in
m4_dnl where MAKE_LIB_DIR is the name of a directory which contains a library
m4_dnl of Makefile fragments.  The m4 preprocessing eases the maintenance of
m4_dnl many Makefile.in's which are almost identical.
m4_changequote({{, }})m4_dnl
#Makefile.in produced automatically using GNU-m4.

m4_dnl Initial configuration section of Makefile.in.
m4_include({{init.mf}})

ZYACC_PATH=		../src
ZYACC_SEARCH_PATH=	../src
ZLEX_LIBSRC_DIR=	/usr/local/share/zlex

LIBS=			$(ZLIBS) $(XLIBS)

m4_dnl Suffix rules.
m4_include({{suffix.mf}})

ZLEXFLAGS=	--whitespace

#Main target to be built in this directory.
TARGET=		zpl

#FILE DESCRIPTIONS.
#Human-generated C files.
C_SRC_FILES= \
  plops.c

#Computer-generated C files.
C_NON_SRC_FILES= \
  scan.c scanlib.c

#All C files.
CFILES= \
  $(C_SRC_FILES) $(C_NON_SRC_FILES)

#Human-generated header files.
H_SRC_FILES= \
  plops.h scan.h

#Computer-generated header files.
H_NON_SRC_FILES= \
  parse.h

#All header files.
HFILES= \
  $(H_SRC_FILES) $(H_NON_SRC_FILES)

#All human-generated files to be distributed.
SRC_FILES= \
  $(C_SRC_FILES) $(H_SRC_FILES) scan.l parse.y \
  Makefile.m4 NOTES.txt README test.in

#All computer-generated files to be distributed.
NON_SRC_FILES= \
  $(C_NON_SRC_FILES) $(H_NON_SRC_FILES) \
  Makefile.in DEPEND test.log

#All files to be distributed.
DIST_FILES= \
  $(SRC_FILES) $(NON_SRC_FILES)

#Object files.
OFILES= \
  parse.o scan.o plops.o scanlib.o


all:		$(TARGET)

install:
		echo "No files to be installed in this directory."
uninstall:
		echo "No files to be installed in this directory."

check:		$(TARGET)
		@echo -n "Testing $(TARGET) ... "; \
		./$(TARGET) <test.in >tmp.$$$$; \
		if cmp tmp.$$$$ test.log  >/dev/null 2>/dev/null ; \
		then \
	          echo OK; $(RM) tmp.$$$$; \
		else \
		  echo FAILURE; \
		  mv tmp.$$$$ test.ERR; \
		  echo Compare test.ERR with desired output in test.log.; \
		fi

$(TARGET):	$(OFILES) $(LIBZ_BIN)/libz.a
		$(CC) $(LDFLAGS) $(OFILES) $(LIBS) -o $@ 

m4_include({{clean.mf}})

Makefile.in:	Makefile.m4 DEPEND FORCE
		$(MAKE_MF_IN)

#We compile scan.c without $(CFLAGS) specified; this should turn off
#optimization to ensure that no library function calls get optimized out.
scanlib.c:	scan.c
		$(CC) -c $(CPPFLAGS) $(CXXFLAGS) $(DEFFLAGS) $? -o fullscan.o
		$(ZLEX_LIBSRC_DIR)/mklibsrc fullscan.o $@
		$(RM) fullscan.o
		touch $@

#Dependency generation.
m4_include({{depend.mf}})

m4_include({{dist.mf}})

#All the object files depend on config.h.  Not put into automatic
#dependencies as we don't know the directory for config.h till build-time.
$(OFILES):	$(builddir)/config.h

scan.c:		scan.l
parse.c:	parse.y
parse.h:	parse.y

m4_include({{DEPEND}})

