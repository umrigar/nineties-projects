m4_dnl This file should be processed by GNU-m4 to produce a Makefile.in using:
m4_dnl gm4 --prefix-builtins --include=MAKE_LIB_DIR Makefile.m4 >Makefile.in
m4_dnl where MAKE_LIB_DIR is the name of a directory which contains a library
m4_dnl of Makefile fragments.  The m4 preprocessing eases the maintenance of
m4_dnl many Makefile.in's which are almost identical.
m4_changequote({{, }})m4_dnl
#Makefile.in produced automatically using GNU-m4.

#Name of program.
PROGRAM=		zyacc
VERSION=		1.03

m4_dnl Initial configuration section of Makefile.in.
m4_include({{init.mf}})

#Main target to be built in this directory.
TARGET=		zdu/zydebug/FullFrame.class


#FILE DESCRIPTIONS.
#Human-generated java files.
J_SRC_FILES= \
  zdu/zydebug/Act.java \
  zdu/zydebug/AppletFrame.java \
  zdu/zydebug/Attention.java \
  zdu/zydebug/BoundedObj.java \
  zdu/zydebug/FileCanvas.java \
  zdu/zydebug/FullFrame.java \
  zdu/zydebug/GridLocator.java \
  zdu/zydebug/GridLocatorXEnumeration.java \
  zdu/zydebug/List.java \
  zdu/zydebug/ListCanvas.java \
  zdu/zydebug/Locator.java \
  zdu/zydebug/OffsetForest.java \
  zdu/zydebug/OffsetTree.java \
  zdu/zydebug/ParseNode.java \
  zdu/zydebug/Parser.java \
  zdu/zydebug/Scanner.java \
  zdu/zydebug/ScrollPanel.java \
  zdu/zydebug/ScrollableCanvas.java \
  zdu/zydebug/Selectable.java \
  zdu/zydebug/TextCanvas.java \
  zdu/zydebug/TextLine.java \
  zdu/zydebug/TextMark.java \
  zdu/zydebug/Token.java \
  zdu/zydebug/Trace.java \
  zdu/zydebug/TraceCanvas.java \
  zdu/zydebug/Tree.java \
  zdu/zydebug/TreeCanvas.java \
  zdu/zydebug/XEnumeration.java \
  zdu/zydebug/ZYDebug.java 


#Computer-generated java files.
J_NON_SRC_FILES= 

#All Java files.
JFILES= \
  $(J_SRC_FILES) $(J_NON_SRC_FILES)


#All human-generated files to be distributed.
SRC_FILES= \
  $(J_SRC_FILES) \
  Makefile.m4 README

CLASS_FILES= \
  zdu/zydebug/Act.class \
  zdu/zydebug/AppletFrame.class \
  zdu/zydebug/Attention.class \
  zdu/zydebug/BoundedObj.class \
  zdu/zydebug/FileCanvas.class \
  zdu/zydebug/FullFrame.class \
  zdu/zydebug/GridLocator.class \
  zdu/zydebug/GridLocatorXEnumeration.class \
  zdu/zydebug/List.class \
  zdu/zydebug/ListCanvas.class \
  zdu/zydebug/Locator.class \
  zdu/zydebug/OffsetForest.class \
  zdu/zydebug/OffsetTree.class \
  zdu/zydebug/ParseNode.class \
  zdu/zydebug/Parser.class \
  zdu/zydebug/Scanner.class \
  zdu/zydebug/ScrollPanel.class \
  zdu/zydebug/ScrollableCanvas.class \
  zdu/zydebug/Selectable.class \
  zdu/zydebug/TextCanvas.class \
  zdu/zydebug/TextLine.class \
  zdu/zydebug/TextMark.class \
  zdu/zydebug/Token.class \
  zdu/zydebug/Trace.class \
  zdu/zydebug/TraceCanvas.class \
  zdu/zydebug/Tree.class \
  zdu/zydebug/TreeCanvas.class \
  zdu/zydebug/XEnumeration.class \
  zdu/zydebug/ZYDebug.class 

#All computer-generated files to be distributed.
NON_SRC_FILES= \
  $(J_NON_SRC_FILES) $(CLASS_FILES) \
  Makefile.in

#All files to be distributed.
DIST_FILES= \
  $(SRC_FILES) $(NON_SRC_FILES)


all:		$(TARGET)

$(TARGET):	$(JFILES)
		javac -depend zdu/zydebug/FullFrame.java

install:	$(TARGET) 
		@for f in $(CLASS_FILES) ; \
		do \
		  $(INSTALL_DATA) $(srcdir)/$$f $(classdir)/$$f; \
		done

uninstall:
		for f in CLASS_FILES ; \
		do \
		  $(RM) $(classdir)/$$f; \
		done

check:		$(TARGET)
		@echo No checks for java zydebug.


Makefile.in:	Makefile.m4 FORCE
		$(MAKE_MF_IN)

m4_include({{clean.mf}})

m4_include({{dist.mf}})


