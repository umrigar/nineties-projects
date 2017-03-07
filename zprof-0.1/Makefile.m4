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
PROGRAM=		zprof
VERSION=		0.1

m4_dnl Initial configuration section of Makefile.in.
m4_include({{init.mf}})

m4_dnl Suffix rules.
m4_include({{suffix.mf}})

SRC_FILES= \
  Makefile.m4 \
  acconfig.h aclocal.m4 configure.in id914 \
  install-sh mkinstalldirs install-chk mk-mf_in \
  COPYING GPL README INSTALL index.html CHANGES \
  mf/*.mf

NON_SRC_FILES= \
  Makefile.in config.h.in configure 
DIST_FILES= \
  $(SRC_FILES) $(NON_SRC_FILES)

#Directory which contains program sources.
PRG_DIR=		zprof

#Directories to be built in normal build.
BUILD_DIRS=		$(LIBZ_SRC) $(PRG_DIR)

#Directories which contain normal installation checks.
CHECK_DIRS=		$(PRG_DIR)

#Directories which need to be cleaned from top-level.
CLEAN_DIRS=		$(PRG_DIR)

#Directories which contain dependency information.
DEPEND_DIRS=		$(PRG_DIR) $(LIBZ_SRC)

#Directories which contain distribution.
DIST_DIRS=		$(PRG_DIR) $(LIBZ_SRC)

#Directories which contain installation targets.
INSTALL_DIRS=		$(PRG_DIR)

#Directories with Makefile.m4 files which need to be converted to Makefile.in.
MAKE_DIRS=		$(PRG_DIR) $(LIBZ_SRC)

DIST_NAME=		$(PROGRAM)-$(VERSION)

#PUBLIC TARGETS.

all:		
		@for d in $(BUILD_DIRS) ; \
		do \
		  echo Making in directory $$d ; \
		  ( cd $$d ; $(REC_MAKE) $@ ) ; \
		done

install:	installdirs
		@for d in $(INSTALL_DIRS) ; \
		do \
		  echo Installing files from directory $$d ; \
		  ( cd $$d ;  $(REC_MAKE) $@  ) ; \
		done
	
uninstall:
		@for d in $(INSTALL_DIRS) ; \
		do \
		  echo Uninstalling files from directory $$d ; \
		  ( cd $$d ; $(REC_MAKE) $@ ) ; \
		done

check:
		@for d in $(CHECK_DIRS) ; \
		do \
		  echo Checking directory $$d ; \
		  ( cd $$d ; $(REC_MAKE) $@ ) ; \
		done



clean:		FORCE
		$(RM) *.o core *~ t t.? t[0-9]* tmp tmp.* tmp[0-9]* *.tmp
		for d in $(CLEAN_DIRS) ; \
		do \
		  ( cd $$d ; $(REC_MAKE) $@ ) ; \
		done

distclean:	clean FORCE
		$(RM) config.h config.log \
		      config.cache config.status Makefile makefile stamp-h
		@for d in $(CLEAN_DIRS) ; \
		do \
		  ( cd $$d ; $(REC_MAKE) $@ ) ; \
		done

maintainer-clean: distclean FORCE
		$(RM) $(NON_SRC_FILES)
		@for d in $(CLEAN_DIRS) ; \
		do \
		  ( cd $$d ; $(REC_MAKE) $@ ) ; \
		done

FORCE:

installdirs:	mkinstalldirs
		$(srcdir)/mkinstalldirs $(bindir) $(libdir) \
		      $(mandir)/man1 $(htmldir)

Makefile.in:	Makefile.m4 FORCE
		$(MAKE_MF_IN)
		HDRS=`( cd $(PRG_DIR) ; $(REC_MAKE) libzhdr ) \
                | sed -n 's/LIBZ:[	]*//p'` ; \
		for d in $(MAKE_DIRS) ; \
		do \
		  ( cd $$d ; $(REC_MAKE) HDRS="$$HDRS" $@ ) ; \
		done

dist:
		$(REC_MAKE)
		$(RM) -r $(DIST_NAME) 
		mkdir $(DIST_NAME) $(DIST_NAME)/mf
		@echo Making distribution for top-level files.
		@for f in $(DIST_FILES) ; \
		do \
		  cp $$f $(DIST_NAME)/$$f ; \
		done
		@HDRS=`( cd $(PRG_DIR) ; $(REC_MAKE) libzhdr ) \
                | sed -n 's/LIBZ:[	]*//p'` ; \
		for d in $(DIST_DIRS) ; \
		do \
		  $(srcdir)/mkinstalldirs $(DIST_NAME)/$$d ; \
		  echo Making distribution for subdirectory $$d. ; \
		  (cd $$d ; \
                   $(MAKE) HDRS="$$HDRS" DIST_DIR=$(DIST_NAME)/$$d dist ) ; \
		done
		@echo Making distribution tar file.
		@tar chf $(DIST_NAME).tar $(DIST_NAME)
		@$(RM) -r $(DIST_NAME)
		@$(GZIP) $(DIST_NAME).tar
		@echo Distribution in $(DIST_NAME).tar.gz

DEPEND:
		for d in $(DEPEND_DIRS) ; \
		do \
		  ( cd $$d ; $(REC_MAKE) $@ ) ; \
		done


#CONFIGURATION TARGETS
CONFIGURE:	$(srcdir)/configure $(srcdir)/config.h.in

${srcdir}/configure: configure.in aclocal.m4
		cd ${srcdir} && autoconf
     
#autoheader might not change config.h.in, so touch a stamp file.
${srcdir}/config.h.in: stamp-h.in
stamp-h.in:	configure.in aclocal.m4 acconfig.h
		cd ${srcdir} && autoheader
		echo timestamp > stamp-h.in
     
config.h:	stamp-h
stamp-h:	config.h.in config.status
		./config.status

#This rule acts weird --- omit for the time being.     
#Makefile:	Makefile.in config.status
#		./config.status
     
config.status: configure
		./config.status --recheck

m4_include({{package.mf}})
