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
TARGET=		libz.a

#FILE DESCRIPTIONS
#Human-generated C files.
C_SRC_FILES= \
  arcutx.c ardel.c ardelx.c arfreeze.c argrow.c argrowx.c armem.c arreset.c \
  bsadd.c bsassign.c bscomp.c bscopy.c bsdel.c bsdiff.c bsempty.c \
  bsfprint.c bshshsum.c bsinbset.c bsinit.c bsintrsc.c bsmake.c \
  bsnew.c bsnext.c bsprint.c bsrange.c bsrm.c bssize.c bssubset.c \
  bstest.c bsunary.c bsunion.c \
  error.c errorz.c \
  freelist.c \
  idsfail.c idsget.c idsinit.c idsintrn.c idslen.c idsstrng.c \
  isadd.c isassign.c ischown.c iscomp.c iscopy.c \
  isdel.c isdiff.c isdebug.c isempty.c \
  isfprint.c ishshsum.c isiniset.c isinit.c isintrsc.c ismake.c \
  isnew.c isnext.c isprint.c isrange.c isrm.c issize.c issubset.c \
  istest.c isunary.c isunion.c \
  libzerr.c \
  memcheck.c memdebug.c memtest.c \
  po01fn.c po2info.c pochklng.c pochrfn.c pochrfn0.c pochrfn1.c poerr.c \
  pohelp.c pohlpfmt.c pointfn0.c pointfn1.c \
  polngfn0.c polngfn1.c pooptdef.c \
  poparse.c poshift.c poshrfn0.c poshrfn1.c postrarg.c postrfn.c postvfn.c \
  potest.c potstchk.c potstinf.c potstop0.c potstmk.c \
  ssadd.c sscat.c ssdel.c ssinit.c ssmore.c sspop.c \
  tiadd.c tidel.c tigetid.c tiidlen.c tiidtext.c tiinit.c \
  tipop.c titest.c titxtlen.c \
  utcesc.c utflfind.c utlnglen.c utopen.c utlg2.c utread.c \
  utstrlo.c utstrlon.c utstrnst.c utstrup.c utstrupn.c utsuffix.c \
  utfltest.c \
  vfprintf.c

#Computer-generated C files.
C_NON_SRC_FILES= \
  potstopt.c
 
#All C files.
CFILES= \
  $(C_SRC_FILES) $(C_NON_SRC_FILES)

#Human-generated header files.
H_SRC_FILES= \
  area.h autobuf.h bset.h bsetp.h ccport.h \
  error.h errorz.h freelist.h \
  memalloc.h ids.h idsp.h iset.h isetp.h osport.h optp.h \
  parseopt.h sspacep.h strspace.h textid.h textidp.h \
  zutil.h xassert.h

#Computer-generated header files.
H_NON_SRC_FILES= 
 
#All header files.
HFILES= \
  $(H_SRC_FILES) $(H_NON_SRC_FILES)

#Special debug OFILES, built using special DFLAGS.
DEBUG_ISET_OFILES= \
    disadd.o disassign.o discomp.o discopy.o disdel.o disdiff.o \
    disempty.o disfprint.o dishshsum.o \
    disiniset.o disintrsc.o dismake.o \
    disnew.o disnext.o disprint.o disrange.o disrm.o dissize.o dissubset.o \
    disunary.o disunion.o 

#Object files to be build.
OFILES= \
    arcutx.o ardel.o ardelx.o arfreeze.o argrow.o argrowx.o armem.o arreset.o \
    bsadd.o bsassign.o bscomp.o bscopy.o bsdel.o bsdiff.o bsempty.o \
    bsfprint.o bshshsum.o bsinbset.o bsinit.o bsintrsc.o bsmake.o \
    bsnew.o bsnext.o bsprint.o bsrange.o bsrm.o bssize.o bssubset.o \
    bsunary.o bsunion.o \
    error.o \
    errorz.o \
    freelist.o \
    idsfail.o idsget.o idsinit.o idsintrn.o idslen.o idsstrng.o \
    isadd.o isassign.o ischown.o iscomp.o iscopy.o isdebug.o isdel.o isdiff.o \
    isempty.o isfprint.o ishshsum.o isiniset.o isinit.o isintrsc.o ismake.o \
    isnew.o isnext.o isprint.o isrange.o isrm.o issize.o issubset.o \
    isunary.o isunion.o \
    libzerr.o \
    memcheck.o memdebug.o \
    po01fn.o po2info.o pochklng.o pochrfn.o pochrfn0.o pochrfn1.o poerr.o \
    pohelp.o pohlpfmt.o pointfn0.o pointfn1.o \
    polngfn0.o polngfn1.o pooptdef.o \
    poparse.o poshift.o poshrfn0.o poshrfn1.o postrarg.o postrfn.o \
    postvfn.o \
    ssadd.o sscat.o ssdel.o ssinit.o ssmore.o sspop.o \
    tiadd.o tidel.o tigetid.o tiidlen.o tiidtext.o tiinit.o \
    tipop.o titxtlen.o \
    utcesc.o utflfind.o utlnglen.o utopen.o utlg2.o utread.o \
    utstrlo.o utstrlon.o utstrnst.o utstrup.o utstrupn.o utsuffix.o \
    vfprintf.o \
    $(DEBUG_ISET_OFILES)
#All human-generated files to be distributed.
SRC_FILES= \
  $(C_SRC_FILES) $(H_SRC_FILES) \
  Makefile.m4 README \
  bstest.in ertest.in istest.in memtest.in \
  potest.in potstinf.in titest.in utfltest.in 

#All computer-generated files to be distributed.
NON_SRC_FILES= \
  $(C_NON_SRC_FILES) $(H_NON_SRC_FILES) \
  Makefile.in DEPEND \
  bstest.log ertest.log istest.log memdebug.log memtest.log \
  potest.log potstinf.log titest.log utfltest.log 

DIST_FILES= \
  $(SRC_FILES) $(NON_SRC_FILES)

TESTS= bstest ertest istest memtest potest potstinf titest utfltest

#PUBLIC TARGETS
all:		$(TARGET)

install:	$(TARGET)
		echo Nothing to be installed for libz.

uninstall:
		echo Nothing to be uninstalled for libz.

$(TARGET):	$(OFILES)
		$(AR) r $@ $?
		$(RANLIB) $@

$(DEBUG_ISET_OFILES):
		debugCFile=`basename $@ '.o'`.c; \
		srcCFile=`echo $@ | sed 's/^.//;s/o$$/c/'`; \
		$(LN) $$srcCFile $$debugCFile; \
		$(CC) -c $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) \
		    -DDEBUG_ISET $(DFLAGS) $$debugCFile; \
		$(RM) $$debugCFile


check:		$(TESTS) 
		@-for t in $(TESTS) ; \
		do \
		  echo -n Checking $$t ... ; \
		  FIND_FILE_PATH=/usr/include \
		  $$t $$t.in <$$t.in >tmp1.$$$$ 2>tmp2.$$$$ ; \
		  cat tmp1.$$$$ tmp2.$$$$ >tmp.$$$$ ; \
		  $(RM) tmp1.$$$$ tmp2.$$$$ ; \
		  if test $$t = memtest ; \
		  then \
		    cat memDebug.log tmp.$$$$ >tmp1.$$$$ ; \
		    sed -e "s/\`[0-9a-fA-F]*\'/\`PTR\'/" \
			tmp1.$$$$ >tmp.$$$$ ; \
		    $(RM) tmp1.$$$$ ; \
		  fi ; \
		  if cmp $$t.log tmp.$$$$ >/dev/null 2>/dev/null ; \
		  then \
		    echo OK; $(RM) tmp.$$$$ ; \
		  else \
		    echo FAILED. ; \
		    if test -f $$t.msg ; \
		    then \
		      cat $$t.msg ; \
		    fi ; \
		    mv tmp.$$$$ $$t.ERR ; \
		    echo Suggest diff\'ing erroneous log in $$t.ERR with \
			 expected log in $$t.log. ; \
		  fi ; \
		done

potstinf:	potstinf.o
		$(CC) $(LDFLAGS) potstinf.o $(ZLIBS) $(XLIBS)  -o $@ 

potstmk:	potstmk.o
		$(CC) $(LDFLAGS) potstmk.o $(ZLIBS) $(XLIBS)  -o $@ 

potstopt.c:	potstmk
		./potstmk >$@

m4_include({{clean.mf}})

Makefile.in:	Makefile.m4 DEPEND FORCE
		$(MAKE_MF_IN)

$(TESTS):	$(TARGET)

#All the object files depend on config.h.  Not put into automatic
#dependencies as we don't know the directory for config.h till build-time.
$(OFILES):	$(builddir)/config.h

#Manual dependencies for DEBUG_ISET_OFILES.
$(DEBUG_ISET_OFILES):		isetp.h iset.h
disadd.o:	isadd.c 
disassign.o:	isassign.c 
discomp.o:	iscomp.c 
discopy.o:	iscopy.c 
disdel.o:	isdel.c 
disdiff.o:	isdiff.c 
disempty.o:	isempty.c 
disfprint.o:	isfprint.c 
dishshsum.o:	ishshsum.c 
disiniset.o:	isiniset.c 
disinit.o:	isinit.c 
disintrsc.o:	isintrsc.c 
dismake.o:	ismake.c 
disnew.o:	isnew.c 
disnext.o:	isnext.c 
disprint.o:	isprint.c 
disrange.o:	isrange.c 
disrm.o:	isrm.c 
dissize.o:	issize.c 
dissubset.o:	issubset.c 
disunary.o:	isunary.c 
disunion.o:	isunion.c 


#Dependency generation.
m4_include({{depend.mf}})

m4_include({{dist.mf}})

m4_include({{DEPEND}})
