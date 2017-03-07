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
PROGRAM=		zlex
VERSION=		1.02

m4_dnl Initial configuration section of Makefile.in.
m4_include({{init.mf}})

#SUFFIX RULES
.SUFFIXES:

.SUFFIXES:	.ps .html .info .texi

.texi.html:
		$(TEXI2HTML) -invisible '.' $<

.texi.ps:
		$(TEXI2DVI) $<
		$(DVI2PS) $*.dvi -o $@
		$(RM) $*.dvi

.texi.info:
		$(MAKEINFO) $<

#Human-generated files.
SRC_FILES= \
  $(PROGRAM).texi debug.l wc.in wc.l wcRevLns.in wcRevLns.l \
  Makefile.m4 xoptnode.sed README

#Computer-generated files.
NON_SRC_FILES= \
  $(PROGRAM).info \
  $(PROGRAM).info-1 $(PROGRAM).info-2 $(PROGRAM).info-3 $(PROGRAM).info-4 \
  $(PROGRAM).html $(PROGRAM).ps \
  Makefile.in optnodes.sed options.texi

DIST_FILES= \
  $(SRC_FILES) $(NON_SRC_FILES)


all:		$(PROGRAM).info $(PROGRAM).html $(PROGRAM).ps

install:
		-if test -f $(PROGRAM).html ; \
		then d=. ; \
		else d=$(srcdir) ; \
		fi ; \
		$(topdir)/install-chk $(SIGNATURE) $(docdir)/$(PROGRAM).html \
		&& $(INSTALL_DATA) $$d/$(PROGRAM).html \
			$(docdir)/$(PROGRAM).html 
		-if test -f $(PROGRAM).info ; \
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
		  if $(SHELL) -c 'install-info --version' \
		    >/dev/null 2>&1; \
		  then \
		    install-info --infodir=$(infodir) $$d/$(PROGRAM).info; \
		  else true ; \
		  fi ; \
		fi

uninstall:
		$(topdir)/install-chk $(SIGNATURE) $(docdir)/$(PROGRAM).html \
		&& $(RM) $(docdir)/$(PROGRAM).html 
		if $(topdir)/install-chk $(SIGNATURE) \
			$(infodir)/$(PROGRAM).info* ; \
		then \
		  $(RM) $(infodir)/$(PROGRAM).info* ; \
		  if test -f $(infodir)/dir \
		     && grep -i "$(PROGRAM)" $(infodir)/dir >/dev/null 2>&1 ; \
		  then \
		    cat $(infodir)/dir | sed -e "/$(PROGRAM)/d" >t.$$$$ ; \
		    mv t.$$$$ $(infodir)/dir ; \
		  else true ; \
		  fi ; \
		fi

optnodes.sed:	$(PROGRAM).texi
		$(SED) -n -f xoptnode.sed <$(PROGRAM).texi >$@

options.texi:	optnodes.sed ../src/opttab.texi
		$(SED) -f optnodes.sed <../src/opttab.texi >$@

#CLEANING TARGETS
clean:		FORCE
		$(RM) *.log *.dvi *~ *.bak wc wcRevLns debug 
		$(RM) *.tmp *.c t.* t?.*
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

$(PROGRAM).info:	$(PROGRAM).texi options.texi

$(PROGRAM).html:	$(PROGRAM).texi options.texi

$(PROGRAM).ps:		$(PROGRAM).texi options.texi

