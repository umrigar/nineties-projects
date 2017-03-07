m4_dnl This file should be processed by GNU-m4 to produce a Makefile.in using:
m4_dnl gm4 --prefix-builtins --include=MAKE_LIB_DIR Makefile.m4 >Makefile.in
m4_dnl where MAKE_LIB_DIR is the name of a directory which contains a library
m4_dnl of Makefile fragments.  The m4 preprocessing eases the maintenance of
m4_dnl many Makefile.in's which are almost identical.
m4_changequote({{, }})m4_dnl
#Makefile.in produced automatically using GNU-m4.
#We assume that any make variables defined on the make command-line are
#passed into a recursive make.

m4_dnl Initial configuration section of Makefile.in.
m4_include({{init.mf}})

#SUFFIX RULES
.SUFFIXES:

.SUFFIXES:	.ps .html .info .texi .tm4

.tm4.texi:
		$(GM4) $< >$@

.texi.ps:
		$(TEXI2DVI) $<
		$(DVI2PS) $*.dvi -o $@
		$(RM) $*.dvi

.texi.info:
		$(MAKEINFO) $<

#Examples
EXAMPLEDIR=	examples/

#c2tex
C2TEX=		c2tex
C2TEX_DIR=	../c2tex

INCLUDES= \
  $(EXAMPLEDIR)rpcalc.y \
  $(EXAMPLEDIR)calc.y \
  $(EXAMPLEDIR)mfcalc.y \
  $(EXAMPLEDIR)nmcalc.y \
  $(EXAMPLEDIR)polycalc.y \
  $(EXAMPLEDIR)lazycalc.y \
  $(EXAMPLEDIR)semtest.y \
  evalpoly.c \
  union.c

#Human-generated files.
SRC_FILES= \
  $(PROGRAM).tm4 \
  $(INCLUDES) \
  macros.m4 Makefile.m4 xoptnode.sed $(EXAMPLEDIR)Makefile README

HTML_FILES= \
  $(PROGRAM)_toc.html $(PROGRAM)_1.html $(PROGRAM)_2.html $(PROGRAM)_3.html \
  $(PROGRAM)_4.html $(PROGRAM)_5.html $(PROGRAM)_6.html $(PROGRAM)_7.html \
  $(PROGRAM)_8.html $(PROGRAM)_9.html $(PROGRAM)_10.html $(PROGRAM)_11.html \
  $(PROGRAM)_12.html $(PROGRAM)_13.html $(PROGRAM)_14.html 


#Computer-generated files.
NON_SRC_FILES= \
  $(PROGRAM).texi \
  $(PROGRAM).info \
  $(PROGRAM).info-1 $(PROGRAM).info-2 $(PROGRAM).info-3 $(PROGRAM).info-4 \
  $(PROGRAM).info-5 $(PROGRAM).info-6 \
  $(HTML_FILES) \
  $(PROGRAM).ps \
  Makefile.in

DIST_FILES= \
  $(SRC_FILES) $(NON_SRC_FILES)


all:		$(PROGRAM).info $(PROGRAM)_toc.html $(PROGRAM).ps

$(HTML_FILES):	$(PROGRAM).texi
		$(TEXI2HTML) -split_chapter $? 

install:
		@if test -f ./$(PROGRAM)_toc.html || \
		    test -f $(srcdir)/$(PROGRAM)_toc.html ; \
		then \
		  echo "installing documentation" ; \
		else \
		  echo "*** no doc made; Must make doc first"; exit 1 ; \
		fi
		@-if test -f $(PROGRAM)_toc.html ; \
		then d=. ; \
		else d=$(srcdir) ; \
		fi ; \
		for h in $(HTML_FILES) ; \
		do \
		  $(topdir)/install-chk $(SIGNATURE) $(htmldir)/$$h \
		    && $(INSTALL_DATA) $$d/$$h $(htmldir)/$$h ; \
		done 
		@-if test -f $(PROGRAM).info ; \
		then d=. ; \
		else d=$(srcdir) ; \
		fi ; \
		if $(topdir)/install-chk $(SIGNATURE) \
			$(infodir)/$(PROGRAM).info* ; \
		then \
		  for i in $$d/$(PROGRAM).info* ; \
		  do \
		    $(INSTALL_DATA) $$i $(infodir)/`basename $$i` ; \
		  done ; \
		fi

dist-min-doc:	$(NON_SRC_FILES)
		@for f in $(SRC_FILES) Makefile.in ; \
		do \
		  $(LN) $(srcdir)/$$f $(topdir)/$(DIST_DIR)/$$f ; \
		done


uninstall:
		$(topdir)/install-chk $(SIGNATURE) \
		      $(htmldir)/$(PROGRAM)_toc.html \
		&& $(RM) $(htmldir)/$(PROGRAM)*.html 
		if $(topdir)/install-chk $(SIGNATURE) \
			$(infodir)/$(PROGRAM).info* ; \
		then \
		  $(RM) $(infodir)/$(PROGRAM).info* ; \
		fi

optnodes.sed:	$(PROGRAM).texi
		$(SED) -n -f xoptnode.sed <$(PROGRAM).texi >$@

options.texi:	optnodes.sed ../src/opttab.texi
		$(SED) -f optnodes.sed <../src/opttab.texi >$@

$(PROGRAM).texi: $(PROGRAM).tm4 c2tex

c2tex:
		cd $(C2TEX_DIR); $(MAKE)

#CLEANING TARGETS
clean:		FORCE
		$(RM) *.log *.dvi *~ *.bak wc wcRevLns debug 
		$(RM) *.tmp t.* t?.*
		$(RM) *.aux *.cp *.cps *.fn *.fns *.ky *.pg *.tp *.vr

distclean:	clean FORCE
		$(RM) Makefile

maintainer-clean: distclean FORCE
		$(RM) $(PROGRAM).info* $(PROGRAM).html $(PROGRAM).ps \
		Makefile.in

FORCE:

#Assume Makefile.in target only when building within srcdir.
Makefile.in:	Makefile.m4 FORCE
		$(MAKE_MF_IN)

m4_include({{dist.mf}})

$(PROGRAM).info:	$(PROGRAM).tm4 $(INCLUDES)

$(PROGRAM).html:	$(PROGRAM).tm4 $(INCLUDES)

$(PROGRAM).ps:		$(PROGRAM).tm4 $(INCLUDES)

