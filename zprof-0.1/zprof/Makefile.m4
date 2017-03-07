m4_dnl This file should be processed by GNU-m4 to produce a Makefile.in using:
m4_dnl gm4 --prefix-builtins --include=MAKE_LIB_DIR Makefile.m4 >Makefile.in
m4_dnl where MAKE_LIB_DIR is the name of a directory which contains a library
m4_dnl of Makefile fragments.  The m4 preprocessing eases the maintenance of
m4_dnl many Makefile.in's which are almost identical.
m4_changequote({{, }})m4_dnl
#Makefile.in produced automatically using GNU-m4.

#Name of program.
PROGRAM=		zprof
VERSION=		0.1

m4_dnl Initial configuration section of Makefile.in.
m4_include({{init.mf}})

ZYACCFLAGS=		-d -v

ZLIBS=			-L../libz -lz

ZLEX_LIBSRC_DIR=	/usr/local/share/zlex

m4_dnl Suffix rules.
m4_include({{suffix.mf}})

#Extra -D option passed to the C-compiler.
XFLAGS=	-DVERSION=\"$(VERSION)\"

PRGLIB=		lib$(PROGRAM).a

PRGMAN=		man1/$(PROGRAM).1

PRGHTML=	$(PROGRAM).html

ZCOUNTS=	zcounts

#Main target to be built in this directory.
TARGET=		$(PROGRAM) $(PRGLIB) $(ZCOUNTS)

#Path to Perl on *MY* machine, as distributed in $(ZCOUNTS).src file.
MY_PERL_PATH=	/usr/bin/perl


#FILE DESCRIPTIONS.
#Human-generated C files.
C_SRC_FILES= \
  out.c profout.c sig.c

#Computer-generated C files.
C_NON_SRC_FILES= \
  scanlib.c \
  parse.c scan.c

#All C files.
CFILES= \
  $(C_SRC_FILES) $(C_NON_SRC_FILES)

#Human-generated header files.
H_SRC_FILES= \
  options.h out.h scan.h

#Computer-generated header files.
H_NON_SRC_FILES= \
  parse.h

#All header files.
HFILES= \
  $(H_SRC_FILES) $(H_NON_SRC_FILES)

#All human-generated files to be distributed.
SRC_FILES= \
  $(C_SRC_FILES) $(H_SRC_FILES) scan.l parse.y \
  $(PROGRAM).1 \
  typedef.c primes.c \
  $(ZCOUNTS).src Makefile.m4 NOTES.txt NOTES.html

#All computer-generated files to be distributed.
NON_SRC_FILES= \
  $(C_NON_SRC_FILES) $(H_NON_SRC_FILES) \
  $(PROGRAM).html \
  primes_BB.c primes.c.bb \
  typedef.log \
  Makefile.in DEPEND

#All files to be distributed.
DIST_FILES= \
  $(SRC_FILES) $(NON_SRC_FILES)

#Object files.
OFILES= \
  out.o parse.o scan.o scanlib.o

LIBOBJS= \
  profout.o sig.o

all:		$(TARGET)

#set the interpreter exec path for perl.
#don't use 'which' as there are egregious differences on various machines.
$(ZCOUNTS):	$(ZCOUNTS).src
		@echo making $@
		@for f in `echo $$PATH | sed -e 's/:/ /g'` ; \
		do \
		  if test -f $$f/perl ; then PERL_PATH=$$f/perl; break; fi; \
		done ; \
		if test -z "$$PERL_PATH" ; \
		then \
		  echo "could not find perl"; exit 1; \
		else \
		  sed -e "s@$(MY_PERL_PATH)@$$PERL_PATH@" <$? >$@ ; \
		fi
		chmod 755 $@

install:	$(TARGET)
		$(topdir)/install-chk $(SIGNATURE) $(bindir)/$(PROGRAM) \
		&& $(INSTALL_PROGRAM) $(PROGRAM) $(bindir)/$(PROGRAM)
		$(topdir)/install-chk $(SIGNATURE) $(bindir)/$(ZCOUNTS) \
		&& $(INSTALL_PROGRAM) $(ZCOUNTS) $(bindir)/$(ZCOUNTS)
		$(topdir)/install-chk $(SIGNATURE) $(libdir)/$(PRGLIB) \
		&& $(INSTALL_DATA) $(PRGLIB) $(libdir)/$(PRGLIB)
		$(topdir)/install-chk $(SIGNATURE) $(mandir)/$(PRGMAN) \
		&& $(INSTALL_DATA) $(PROGRAM).1 $(mandir)/$(PRGMAN)
		$(topdir)/install-chk $(SIGNATURE) $(htmldir)/$(PRGHTML) \
		&& $(INSTALL_DATA) $(PRGHTML) $(htmldir)/$(PRGHTML)

uninstall:
		$(topdir)/install-chk $(SIGNATURE) $(bindir)/$(TARGET) \
		&& $(RM) $(bindir)/$(TARGET)
		$(topdir)/install-chk $(SIGNATURE) $(libsrcdir)/$(PRGLIB) \
		&& $(RM) $(libsrcdir)/$(PRGLIB)

check:		$(TARGET) typedefchk
		@echo -n "Checking primes ... " ; \
		$(RM) zprof.out ; \
		ln primes.c t$$$$.c ; \
		./$(PROGRAM) t$$$$.c ; \
		$(CC) $(CFLAGS) t$$$$_BB.c -L. -l$(PROGRAM) -o t$$$$_BB ; \
		./t$$$$_BB >/dev/null ; \
		$(srcdir)/$(ZCOUNTS) ; \
		sed "s/t$$$$.c/primes.c/g" t$$$$_BB.c > t1$$$$_BB.c ; \
		$(MV) t1$$$$_BB.c t$$$$_BB.c ; \
		if $(CMP) t$$$$.c.bb primes.c.bb >/dev/null 2>&1 && \
                   $(CMP) t$$$$_BB.c primes_BB.c >/dev/null 2>&1 ; \
		then \
		   echo OK; $(RM) t$$$$* ; \
		else \
		   echo FAILED ; \
		   $(MV) t$$$$.c.bb primes.c.bb.ERR ; \
		   $(MV) t$$$$_BB.c primes_BB.c.ERR ; \
		   $(RM) t$$$$* ; \
 	   echo "diff primes_BB.c.ERR, primes.c.bb.ERR with non-ERR files" ; \
		fi
		@echo -n "Checking typedefs ... " ; \
		ln typedef.c t$$$$.c ; \
		./$(PROGRAM) t$$$$.c ; \
		./typedefchk t$$$$.c >t$$$$.log ; \
		if cmp t$$$$.log typedef.log ; \
		then \
		  echo OK; $(RM) t$$$$* ; \
		else \
		  echo FAILED ; \
		  $(MV) t$$$$.log typedef.ERR ; \
		  $(RM) t$$$$* ; \
		  echo diff typedef.log with typedef.ERR ; \
		fi

$(PROGRAM):	$(OFILES) $(LIBZ_BIN)/libz.a
		$(CC) $(LDFLAGS) $(OFILES) $(ZLIBS) $(XLIBS)  -o $@ 

$(PRGLIB):	$(LIBOBJS)
		$(AR) r $@ $?
		$(RANLIB) $@

zprofDebug:	$(OFILES) $(LIBZ_BIN)/libz.a
		zyacc -H parse.y -o parse.c
		$(CC) -c $(CPPFLAGS) $(CXXFLAGS) $(DEFFLAGS) \
		      $(CFLAGS) -DYY_ZYACC_DEBUG parse.c -o parseDebug.o
		$(CC) $(LDFLAGS) \
	          `echo $(OFILES) | sed -e 's/parse\.o/parseDebug\.o/'` \
		   -L$(libdir) -lzyacc $(ZLIBS) $(XLIBS) -o $@ 
		

$(PROGRAM).html: $(PROGRAM).1
		nroff -man $? | man2html -topm 6 -botm 6 >$@

typedefchk:	typedefparse.o
		$(CC) $(LDFLAGS) \
		  `echo "$(OFILES)" | sed s/parse.o/$?/` \
		  $(ZLIBS) $(XLIBS)  -o $@

typedefparse.o:	parse.c
		$(CC) -c $(CPPFLAGS) $(CXXFLAGS) $(DEFFLAGS) \
		      $(CFLAGS) -DTEST_TYPEDEF $? -o $@

#We compile scan.c without $(CFLAGS) specified; this should turn off
#optimization to ensure that no library function calls get optimized out.
scanlib.c:	scan.c
		$(CC) -c $(CPPFLAGS) $(CXXFLAGS) $(DEFFLAGS) $? -o fullscan.o
		$(ZLEX_LIBSRC_DIR)/mklibsrc fullscan.o $@
		$(RM) fullscan.o
		touch $@

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


scan.c:		scan.l parse.h
parse.c:	parse.y
parse.h:	parse.y

.FORCE:


m4_include({{DEPEND}})

