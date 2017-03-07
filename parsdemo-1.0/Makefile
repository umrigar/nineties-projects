CC=		gcc
JAVAC=		javac
JFLAGS=		-g
EXPAND=		expand

JFLAGS=		-depend

NAME=		parsdemo
PACKAGE=	zdu/$(NAME)/
VERSION=	1.0


J_SRC_FILES= \
  $(PACKAGE)AppletFrame.java \
  $(PACKAGE)ExtFrame.java \
  $(PACKAGE)FileCanvas.java \
  $(PACKAGE)FullFrame.java \
  $(PACKAGE)GramSym.java \
  $(PACKAGE)Grammar.java \
  $(PACKAGE)LL1ParseStkEntry.java \
  $(PACKAGE)LL1Parser.java \
  $(PACKAGE)LL1Table.java \
  $(PACKAGE)NonTerm.java \
  $(PACKAGE)OffsetForest.java \
  $(PACKAGE)OffsetTree.java \
  $(PACKAGE)ParseDisplay.java \
  $(PACKAGE)ParseException.java \
  $(PACKAGE)ParseNode.java \
  $(PACKAGE)ParseResetException.java \
  $(PACKAGE)ParseStk.java \
  $(PACKAGE)ParseStkEntry.java \
  $(PACKAGE)ParsDemo.java \
  $(PACKAGE)RecParser.java \
  $(PACKAGE)RowCol.java \
  $(PACKAGE)Rule.java \
  $(PACKAGE)SRAct.java \
  $(PACKAGE)SRParseStkEntry.java \
  $(PACKAGE)SRParser.java \
  $(PACKAGE)SRState.java \
  $(PACKAGE)SRTable.java \
  $(PACKAGE)Scanner.java \
  $(PACKAGE)ScrollPanel.java \
  $(PACKAGE)ScrollableCanvas.java \
  $(PACKAGE)Selectable.java \
  $(PACKAGE)SimpCompLL1Gram.java \
  $(PACKAGE)SimpCompLL1Table.java \
  $(PACKAGE)SimpCompSRGram.java \
  $(PACKAGE)SimpCompSRTable.java \
  $(PACKAGE)SimpCompScanner.java \
  $(PACKAGE)SparseTable2.java \
  $(PACKAGE)StepParser.java \
  $(PACKAGE)Table2.java \
  $(PACKAGE)Table2Canvas.java \
  $(PACKAGE)Terminal.java \
  $(PACKAGE)TextCanvas.java \
  $(PACKAGE)TextLine.java \
  $(PACKAGE)TextMark.java \
  $(PACKAGE)Token.java \
  $(PACKAGE)Trace.java \
  $(PACKAGE)TraceCanvas.java \
  $(PACKAGE)Tree.java \
  $(PACKAGE)TreeCanvas.java \

#Top-level java files: compiling these files will compile all.
J_TOP_FILES= \
  $(PACKAGE)FullFrame.java \

HTML_SRC_FILES= \
  HereApplet.html \
  LL1Parser.html \
  RecParser.html \
  SRParser.html \
  controls.html \
  compat.html \
  gramintro.html \
  help.html \
  index.html \
  ll1frame.html \
  ll1intro.html \
  ll1nav.html \
  ll1output.html \
  recframe.html \
  recintro.html \
  recnav.html \
  recoutput.html \
  simpcompLL1Gram.html \
  srframe.html \
  srintro.html \
  srnav.html \
  sroutput.html \
  template.html


MISC_SRC_FILES= \
  simpcomp.y $(PACKAGE)SimpCompRecDescent.jpp \
  README Makefile COPYING $(PACKAGE)DOC.html \
  parstree.fig langanal.fig recapplet.fig ll1applet.fig srapplet.fig \
  scrollbar_bug.txt

#All human generated files which should be distributed.
SRC_FILES= \
  $(J_SRC_FILES) \
  $(HTML_SRC_FILES) \
  $(MISC_SRC_FILES)


#Automatically generated files which should be distributed.
J_NON_SRC_FILES= \
  $(PACKAGE)SimpCompRecDescent.java 

HTML_NON_SRC_FILES= \
  simpcomp.html \
  simpcomp_1.html \
  simpcomp_2.html \
  simpcomp_3.html \
  simpcomp_4.html \
  simpcomp_5.html \
  simpcomp_6.html \
  simpcomp_7.html \

CLASS_FILES= \
  $(PACKAGE)AppletFrame.class \
  $(PACKAGE)ExtFrame.class \
  $(PACKAGE)FileCanvas.class \
  $(PACKAGE)FullFrame.class \
  $(PACKAGE)GramSym.class \
  $(PACKAGE)Grammar.class \
  $(PACKAGE)LL1ParseStkEntry.class \
  $(PACKAGE)LL1Parser.class \
  $(PACKAGE)LL1Table.class \
  $(PACKAGE)NonTerm.class \
  $(PACKAGE)OffsetForest.class \
  $(PACKAGE)OffsetTree.class \
  $(PACKAGE)ParseDisplay.class \
  $(PACKAGE)ParseException.class \
  $(PACKAGE)ParseNode.class \
  $(PACKAGE)ParseResetException.class \
  $(PACKAGE)ParseStk.class \
  $(PACKAGE)ParseStkEntry.class \
  $(PACKAGE)ParsDemo.class \
  $(PACKAGE)RecParser.class \
  $(PACKAGE)RowCol.class \
  $(PACKAGE)Rule.class \
  $(PACKAGE)SRAct.class \
  $(PACKAGE)SRParseStkEntry.class \
  $(PACKAGE)SRParser.class \
  $(PACKAGE)SRState.class \
  $(PACKAGE)SRTable.class \
  $(PACKAGE)Scanner.class \
  $(PACKAGE)ScrollPanel.class \
  $(PACKAGE)ScrollableCanvas.class \
  $(PACKAGE)Selectable.class \
  $(PACKAGE)SimpCompLL1Gram.class \
  $(PACKAGE)SimpCompLL1Table.class \
  $(PACKAGE)SimpCompRecDescent.class \
  $(PACKAGE)SimpCompSRGram.class \
  $(PACKAGE)SimpCompSRTable.class \
  $(PACKAGE)SimpCompScanner.class \
  $(PACKAGE)SparseTable2.class \
  $(PACKAGE)StepParser.class \
  $(PACKAGE)Table2.class \
  $(PACKAGE)Table2Canvas.class \
  $(PACKAGE)Terminal.class \
  $(PACKAGE)TextCanvas.class \
  $(PACKAGE)TextLine.class \
  $(PACKAGE)TextMark.class \
  $(PACKAGE)Token.class \
  $(PACKAGE)Trace.class \
  $(PACKAGE)TraceCanvas.class \
  $(PACKAGE)Tree.class \
  $(PACKAGE)TreeCanvas.class \

MISC_NON_SRC_FILES= \
  $(PACKAGE)SimpCompRecDescent.jpx \
  icons/* \
  parstree.gif langanal.gif recapplet.gif ll1applet.gif srapplet.gif

#All automatically generated files which should be distributed.
NON_SRC_FILES= \
  $(J_NON_SRC_FILES) \
  $(HTML_NON_SRC_FILES) \
  $(CLASS_FILES) \
  $(MISC_NON_SRC_FILES)

.SUFFIXES:	.jpp .jpx .java .class

.jpp.java:
		$(CC) -x c -E -C -P $< >$*.java
.java.class:
		$(JAVAC) $(JFLAGS) $<

.jpp.jpx:
		$(EXPAND) $< >$*.jpx

all:		$(J_SRC_FILES) $(J_NON_SRC_FILES) $(MISC_NON_SRC_FILES)
		$(JAVAC) $(JFLAGS) $(J_TOP_FILES)

clean:
		rm  $(PACKAGE)*~ $(PACKAGE)*.class *~

real-clean:	clean
		rm $(NON_SRC_FILES)

simpcomp.html:	simpcomp.y
		zyacc -H $< -o $*.c
		rm -f $*.c

dist:		$(SRC_FILES) $(NON_SRC_FILES)
		-rm -rf $(NAME)-$(VERSION)
		mkdir $(NAME)-$(VERSION)
		mkdir $(NAME)-$(VERSION)/icons	
		mkdir $(NAME)-$(VERSION)/zdu
		mkdir $(NAME)-$(VERSION)/zdu/parsdemo
		for f in $(SRC_FILES) $(NON_SRC_FILES) ; \
		do \
		  ln $$f $(NAME)-$(VERSION)/$$f ; \
		done
		tar -cvf $(NAME)-$(VERSION).tar $(NAME)-$(VERSION)
		gzip -f $(NAME)-$(VERSION).tar 

$(PACKAGE)SimpCompRecDescent.java:	$(PACKAGE)SimpCompRecDescent.jpp
$(PACKAGE)SimpCompRecDescent.jpx:	$(PACKAGE)SimpCompRecDescent.jpp


