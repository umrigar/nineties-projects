m4_dnl This file should be processed by GNU-m4 to produce a Makefile.in using:
m4_dnl gm4 --prefix-builtins --include=MAKE_LIB_DIR Makefile.m4 >Makefile.in
m4_dnl where MAKE_LIB_DIR is the name of a directory which contains a library
m4_dnl of Makefile fragments.  The m4 preprocessing eases the maintenance of
m4_dnl many Makefile.in's which are almost identical.
m4_changequote({{, }})m4_dnl
#Makefile.in produced automatically using GNU-m4.
#We assume that any make variables defined on the make command-line are
#passed into a recursive make.

PROGRAM=	zyacc
VERSION=	1.03

m4_dnl Initial configuration section of Makefile.in.
m4_include({{init.mf}})

m4_dnl Suffix rules.
m4_include({{suffix.mf}})

SRC_FILES= \
  Makefile.m4 \
  acconfig.h aclocal.m4 configure.in id914 \
  zyacc/refs.bib \
  install-sh mkinstalldirs install-chk mk-mf_in \
  COPYING GPL README INSTALL INSTALL.zyacc CHANGES TODO \
  mf/*.mf 


NON_SRC_FILES= \
  Makefile.in config.h.in configure zyacc/c2tex/c2tex.c
DIST_FILES= \
  $(SRC_FILES) $(NON_SRC_FILES)

PRG_DIR=		zyacc/src

#Directories to be built in normal build.
BUILD_DIRS=		libz zyacc/libzyacc zyacc/src

#Directory containing documentation.  Not normally built.
DOC_DIR=		zyacc/doc

#Directories which contain normal installation checks.
CHECK_DIRS=		zyacc/src

#Directories which need to be cleaned from top-level.
CLEAN_DIRS=		zyacc/doc zyacc/libzyacc zyacc/src 

#Directories which contain dependency information.
DEPEND_DIRS=		 libz zyacc/zpl zyacc/libzyacc zyacc/src zyacc/c2tex

#Directories which contain distribution.
DIST_DIRS=		zyacc/doc libz zyacc/zpl zyacc/src zyacc/src/tests \
			zyacc/libzyacc zyacc/c2tex zyacc/java

#Directories which contain installation targets.
INSTALL_DIRS=		zyacc/libzyacc zyacc/src zyacc/java zyacc/doc 

#Directories with Makefile.m4 files which need to be converted to Makefile.in.
MAKE_DIRS=		zyacc/doc libz zyacc/zpl zyacc/libzyacc zyacc/src \
			zyacc/src/tests zyacc/c2tex zyacc/java

DIST_NAME=		$(PROGRAM)-$(VERSION)
DIST_MIN_DOC_NAME=	$(PROGRAM)-$(VERSION)-min-doc

#PUBLIC TARGETS.

all:		
		@for d in $(BUILD_DIRS) ; \
		do \
		  echo Making in directory $$d ; \
		  ( cd $$d ; $(REC_MAKE) $@ ) ; \
		done

doc:
		cd $(DOC_DIR) ; $(REC_MAKE)

install:	installdirs
		@for d in $(INSTALL_DIRS) ; \
		do \
		  echo installing files from directory $$d ; \
		  ( cd $$d ; $(REC_MAKE) $@ ) ; \
		done
	
uninstall:
		@for d in $(INSTALL_DIRS) ; \
		do \
		  echo uninstalling files from directory $$d ; \
		  ( cd $$d ; $(REC_MAKE) $@ ) ; \
		done

check:
		@for d in $(CHECK_DIRS) ; \
		do \
		  echo Checking directory $$d ; \
		  ( cd $$d ; $(REC_MAKE) $@ ) ; \
		done



clean:		FORCE
		$(RM) *.o core *~ t t.? t[0-9]* tmp tmp.* tmp[0-9]* *.tmp \
			*.output
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
		$(srcdir)/mkinstalldirs $(bindir) $(datadir) $(libdir) \
		           $(libsrcdir) $(infodir) $(htmldir) \
			   $(classdir)/zdu/zydebug

#We should really check HDRS in c2tex directory too, but for the moment,
#things work, cause zyacc requires all the modules c2tex requires.
Makefile.in:	Makefile.m4 FORCE
		$(MAKE_MF_IN)
		HDRS=`( cd $(PRG_DIR) ; $(REC_MAKE) libzhdr ) \
                | sed -n 's/LIBZ:[	]*//p'` ; \
		for d in $(MAKE_DIRS) ; \
		do \
		  ( cd $$d ; $(REC_MAKE) HDRS="$$HDRS" $@ ) ; \
		done

dist:		doc
		$(REC_MAKE)
		$(RM) -r $(DIST_NAME) 
		mkdir $(DIST_NAME)
		@for d in $(DIST_DIRS) ; \
		do \
		  $(srcdir)/mkinstalldirs $(DIST_NAME)/$$d ; \
		done 
		@$(srcdir)/mkinstalldirs $(DIST_NAME)/mf \
		  $(DIST_NAME)/zyacc/c2tex \
		  $(DIST_NAME)/zyacc/doc/examples \
		  $(DIST_NAME)/zyacc/java/zdu/zydebug
		@echo Making distribution for top-level files.
		@for f in $(DIST_FILES) ; \
		do \
		  cp $$f $(DIST_NAME)/$$f ; \
		done
		@HDRS=`( cd $(PRG_DIR) ; $(REC_MAKE) libzhdr ) \
                | sed -n 's/LIBZ:[	]*//p'` ; \
		for d in $(DIST_DIRS) ; \
		do \
		  echo Making distribution for subdirectory $$d. ; \
		  (cd $$d ; \
		   $(REC_MAKE) HDRS="$$HDRS" \
			DIST_DIR=$(DIST_NAME)/$$d dist ) ; \
		done
		@echo Making distribution tar file.
		@tar cf $(DIST_NAME).tar $(DIST_NAME)
		@$(RM) -r $(DIST_NAME)
		@$(GZIP) $(DIST_NAME).tar
		@echo Distribution in $(DIST_NAME).tar.gz

dist-min-doc:
		$(REC_MAKE)
		$(RM) -r $(DIST_MIN_DOC_NAME) 
		mkdir $(DIST_MIN_DOC_NAME)
		@for d in $(DIST_DIRS) ; \
		do \
		  $(srcdir)/mkinstalldirs $(DIST_MIN_DOC_NAME)/$$d ; \
		done 
		@$(srcdir)/mkinstalldirs $(DIST_MIN_DOC_NAME)/mf \
		  $(DIST_MIN_DOC_NAME)/zyacc/c2tex \
		  $(DIST_MIN_DOC_NAME)/zyacc/doc/examples \
		  $(DIST_MIN_DOC_NAME)/zyacc/java/zdu/zydebug
		@echo Making distribution for top-level files.
		@for f in $(DIST_FILES) ; \
		do \
		  cp $$f $(DIST_MIN_DOC_NAME)/$$f ; \
		done
		@HDRS=`( cd $(PRG_DIR) ; $(REC_MAKE) libzhdr ) \
                | sed -n 's/LIBZ:[	]*//p'` ; \
		for d in $(DIST_DIRS) ; \
		do \
		  echo Making distribution for subdirectory $$d. ; \
		  if test $$d = "zyacc/doc" ; \
		  then target="dist-min-doc" ; \
		  else target="dist" ; \
		  fi; \
		  (cd $$d ; \
		   $(REC_MAKE) HDRS="$$HDRS" \
			DIST_DIR=$(DIST_MIN_DOC_NAME)/$$d $$target ) ; \
		done
		@echo Making distribution tar file.
		@tar cf $(DIST_MIN_DOC_NAME).tar $(DIST_MIN_DOC_NAME)
		@$(RM) -r $(DIST_MIN_DOC_NAME)
		@$(GZIP) $(DIST_MIN_DOC_NAME).tar
		@echo Distribution in $(DIST_MIN_DOC_NAME).tar.gz

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
