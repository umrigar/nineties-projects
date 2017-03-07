PROGRAM=		dim
VERSION=		1.02

#Human-generated files
SRC_FILES= \
  Dim.hm4 \
  dimOut.cc \
  slide.cc slideBug.cc ratPow.cc period.cc \
  sigplan.tex dim.bib \
  Makefile README CHANGES index.html

#Computer-generated files to be distributed.
NON_SRC_FILES= \
  Dim.h \
  sigplan.ps sigplan/* 

CC=		gcc

LIBS=		dimOut.o -lg++ -lm

.SUFFIXES:	.o .cc

.cc.o:
		$(CC) -c $(CPPFLAGS) $(CXXFLAGS) $(DFLAGS) \
		      $(CFLAGS) $<

.cc:
		$(CC) $(CPPFLAGS) $(CXXFLAGS) $(DFLAGS) \
		      $(CFLAGS) $< $(LIBS) $(XLIBS) $(LDFLAGS) -o $*

.o:
		$(CC) $(CPPFLAGS) $(CXXFLAGS) $(DFLAGS) \
		      $(CFLAGS) $< $(LIBS) $(XLIBS) $(LDFLAGS) -o $*


all:		slide slideBug


dist:		$(NON_SRC_FILES)
		rm -rf $(PROGRAM)-$(VERSION)
		mkdir $(PROGRAM)-$(VERSION) $(PROGRAM)-$(VERSION)/sigplan
		for f in $(SRC_FILES) $(NON_SRC_FILES) ; \
		do \
		  cp -f $$f $(PROGRAM)-$(VERSION)/$$f ; \
		done
		tar -cvf $(PROGRAM)-$(VERSION).tar $(PROGRAM)-$(VERSION)
		rm -rf $(PROGRAM)-$(VERSION)
		gzip -f $(PROGRAM)-$(VERSION).tar

Dim.h:		Dim.hm4
		m4 Dim.hm4 >$@

sigplan.ps:	sigplan.tex
		latex $?
		dvips sigplan.dvi -o $@

sigplan.html:	sigplan.tex
		latex2html -top_navigation -bottom_navigation \
		  -up_url .. -up_title "Dimensional Analysis" $?
		echo "just a marker file: html in ./sigplan/$@" >$@

update-html:
		for f in sigplan/*.html; \
		do \
		  echo $$f ; \
		  sed -e "s@localhost/latex2html/icons@localhost/~zdu/icons@g" $$f >t.$$$$ ; \
		  mv t.$$$$ $$f ; \
		done

