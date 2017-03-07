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
TARGET=			libzlex.a

#Human-generated C files.
C_SRC_FILES= \
  bytesbuf.c creatbuf.c debug.c delbuf.c fatal.c flushbuf.c get.c \
  less.c main.c makeadj.c makebuf.c membuf.c more.c move.c movetext.c \
  replace.c restart.c \
  savetext.c sentinel.c shiftbuf.c sig.c ssstk.c strngbuf.c swtchbuf.c \
  terminat.c unput.c wrap.c

#Computer-generated C files.
C_NON_SRC_FILES=

CFILES= \
  $(C_SRC_FILES) $(C_NON_SRC_FILES)

#Human-generated header files.
H_SRC_FILES= \
  libzlex0.h libzlex.h libzlexs.h

#Computer-generated header files.
H_NON_SRC_FILES= \
  libzlexp.h
  
#All header files.
HFILES= \
  $(H_SRC_FILES) $(H_NON_SRC_FILES)

#C files to recompile for 16-bit library.
CFILES_16= \
  bytesbuf.c creatbuf.c debug.c delbuf.c \
  flushbuf.c get.c \
  less.c makeadj.c makebuf.c \
  membuf.c more.c move.c movetext.c \
  replace.c restart.c  \
  savetext.c sentinel.c shiftbuf.c \
  ssstk.c strngbuf.c swtchbuf.c \
  terminat.c unput.c

#Object files to be built.
OFILES_16= \
  bytesbuf16.o creatbuf16.o debug16.o delbuf16.o \
  flushbuf16.o get16.o \
  less16.o makeadj16.o makebuf16.o \
  membuf16.o more16.o move16.o movetext16.o \
  replace16.o restart16.o  \
  savetext16.o sentinel16.o shiftbuf16.o \
  ssstk16.o strngbuf16.o swtchbuf16.o \
  terminat16.o unput16.o

OFILES= \
  bytesbuf.o creatbuf.o debug.o delbuf.o fatal.o flushbuf.o get.o \
  less.o main.o makeadj.o makebuf.o membuf.o more.o move.o movetext.o \
  replace.o restart.o  \
  savetext.o sentinel.o shiftbuf.o sig.o ssstk.o strngbuf.o swtchbuf.o \
  terminat.o unput.o wrap.o \
  $(OFILES_16)
 
#All human-generated files to be distributed.
SRC_FILES= \
  $(C_SRC_FILES) $(H_SRC_FILES) mklibsrc.m4 libdep Makefile.m4 \
  NOTES.txt README

#All computer-generated files to be distributed.
NON_SRC_FILES= \
  $(C_NON_SRC_FILES) $(H_NON_SRC_FILES) Makefile.in DEPEND mklibsrc

DIST_FILES= \
  $(SRC_FILES) $(NON_SRC_FILES)

#Source files to be installed in library source directory.
LIBINSTALL= \
  $(CFILES) mklibsrc mklibsrc.m4 libdep libzlexp.h

#PUBLIC TARGETS.
all:		$(TARGET)

install:	$(TARGET) $(LIBINSTALL)
		if $(topdir)/install-chk $(SIGNATURE) $(libdir)/$(TARGET) ; \
		then \
		  $(INSTALL_DATA) $(TARGET) $(libdir)/$(TARGET) ; \
		fi
		if $(topdir)/install-chk $(SIGNATURE) \
			$(includedir)/libzlex.h ; \
		then \
		  $(INSTALL_DATA) $(srcdir)/libzlex.h \
			$(includedir)/libzlex.h ; \
		fi
		if $(topdir)/install-chk $(SIGNATURE) \
			$(libsrcdir)/libzlexp.h ; \
		then \
		  for f in $(LIBINSTALL) ; \
		  do \
		    $(INSTALL_DATA) $(srcdir)/$$f $(libsrcdir)/$$f ; \
		  done ; \
		fi

uninstall:
		if $(topdir)/install-chk $(SIGNATURE) $(libdir)/$(TARGET) ; \
		then \
		  $(RM) $(libdir)/$(TARGET) ; \
		fi
		if $(topdir)/install-chk $(SIGNATURE) \
			$(includedir)/libzlex.h ; \
		then \
		  $(RM) $(includedir)/libzlex.h ; \
		fi
		if $(topdir)/install-chk $(SIGNATURE) \
			$(libsrcdir)/libzlexp.h ; \
		then \
		  $(RM) -r $(libsrcdir) ; \
		fi

$(OFILES_16):	
		cFile16=`basename $@ '.o'`.c; \
		srcCfile=`echo $$cFile16 | $(SED) 's/16//'`; \
		$(LN) $$srcCfile $$cFile16 ; \
		$(CC) -c $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) \
		    -DYY_16_BIT $(DFLAGS) $$cFile16; \
		$(RM) $$cFile16

libzlexp.h:	libzlex.h libzlexs.h libzlex0.h
		echo '/******* DO NOT EDIT *********/' >$@
	        echo '/*' automatically produced by catenating \
		     libzlex.h libzlexs.h and libzlex0.h '*/' >>$@
	        echo >>$@
		cat libzlex.h libzlexs.h libzlex0.h >>$@

$(TARGET):	$(OFILES)
		$(AR) r $@ $?
		$(RANLIB) $@

#We don't depend on $(TARGET) because the intention is that mklibsrc does
#not need to be remade after distribution unless the sources have changed.
mklibsrc:	$(SRCFILES) mklibsrc.m4
		m4 -DDEPEND_CMD="./libdep $(TARGET) yy" \
		  $(srcdir)/mklibsrc.m4 >$@

m4_include({{clean.mf}})

Makefile.in:	Makefile.m4 DEPEND FORCE
		$(MAKE_MF_IN)

#Dependency generation.
m4_include({{depend.mf}})

m4_include({{dist.mf}})

#Manually inserted dependencies for 16-bit library.

$(OFILES_16):	libzlexp.h

bytesbuf16.o:	bytesbuf.c

creatbuf16.o:	creatbuf.c

debug16.o:	debug.c

delbuf16.o:	delbuf.c

flushbuf16.o:	flushbuf.c

get16.o:	get.c

less16.o:	less.c

makeadj16.o:	makeadj.c

makebuf16.o:	makebuf.c

membuf16.o:	membuf.c

more16.o:	more.c

move16.o:	move.c

movetext16.o:	movetext.c

replace16.o:	replace.c

restart16.o:	restart.c

savetext16.o:	savetext.c

sentinel16.o:	sentinel.c

shiftbuf16.o:	shiftbuf.c

ssstk16.o:	ssstk.c

strngbuf16.o:	strngbuf.c

swtchbuf16.o:	swtchbuf.c

terminat16.o:	terminat.c

unput16.o:	unput.c

m4_include({{DEPEND}})
