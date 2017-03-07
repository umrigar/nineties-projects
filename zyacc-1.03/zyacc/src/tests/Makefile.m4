m4_dnl This file should be processed by GNU-m4 to produce a Makefile.in using:
m4_dnl gm4 --prefix-builtins --include=MAKE_LIB_DIR Makefile.m4 >Makefile.in
m4_dnl where MAKE_LIB_DIR is the name of a directory which contains a library
m4_dnl of Makefile fragments.  The m4 preprocessing eases the maintenance of
m4_dnl many Makefile.in's which are almost identical.
m4_changequote({{, }})m4_dnl
#Makefile.in produced automatically using GNU-m4.

m4_dnl Initial configuration section of Makefile.in.
m4_include({{init.mf}})

ZYACC_PATH=		..
ZYACC_SEARCH_PATH=	..

m4_dnl Suffix rules.
m4_include({{suffix.mf}})

ZLIBS=			-L$(builddir)/zyacc/libzyacc -lzyacc

#FILE DESCRIPTIONS.
#All human-generated files to be distributed.
SRC_FILES= \
  arithexp.y attrfrnt.y decuse.y lalr.y loc.y look.y multi.y \
  assocprc.y binnum.y inattrib.y semtest.y conflicts.y debug.y zprof.y \
  arithexp.in assocprc.in binnum.in decuse.in \
  inattrib.in loc.in multi.in semtest.in debug.in debug.in1 \
  Makefile.m4 README

#All computer-generated files to be distributed.
NON_SRC_FILES= \
  arithexp.log attrfrnt.log decuse.log lalr.log loc.log look.log multi.log \
  assocprc.log binnum.log inattrib.log semtest.log conflicts.log debug.log \
  zprof.log Makefile.in 

#All files to be distributed.
DIST_FILES= \
  $(SRC_FILES) $(NON_SRC_FILES)


#Run parser generator, compile and then execute these tests.
EXEC_TESTS= \
	 arithexp \
	 assocprc \
	 binnum \
	 decuse \
	 inattrib \
	 loc \
	 multi \
	 semtest 

#Only run parser generator and look at output file.
OUT_TESTS= \
	 conflicts \
	 lalr 

#Only run parser generator and look at diagnostic errors.
ERR_TESTS= \
	 attrfrnt \
	 look

DEBUG_TESTS= \
	 debug 

C_TESTS= \
	zprof

check:		EXEC_TESTS OUT_TESTS ERR_TESTS DEBUG_TESTS C_TESTS
		@echo Tests completed.

EXEC_TESTS:
		@for t in $(EXEC_TESTS) ; \
		do \
		  $(MAKE) $$t TARGET=$$t && \
		  ./$$t <$$t.in >tmp.$$$$ && \
		  $(MAKE) FILE_CMP TEST=$$t \
		    GOOD_LOG=$$t.log TEST_LOG=tmp.$$$$ ; \
		done

ERR_TESTS:	
		@for t in $(ERR_TESTS) ; \
		do \
		  $(RM) $$t.c ; \
		  $(MAKE) $$t.c TARGET=$$t.c 2>tmp.$$$$ || true; \
		  $(MAKE) FILE_CMP TEST=$$t \
		    GOOD_LOG=$$t.log TEST_LOG=tmp.$$$$ ; \
		done

OUT_TESTS:
		@for t in $(OUT_TESTS) ; \
		do \
		  $(RM) $$t.c ; \
		  $(MAKE) $$t.c TARGET=$$t.c && \
		  tail +2 $$t.output >$$t.$$$$ && \
		  $(MAKE) FILE_CMP TEST=$$t \
		    GOOD_LOG=$$t.log TEST_LOG=$$t.$$$$ ; \
		done

DEBUG_TESTS:
		@for t in $(DEBUG_TESTS) ; \
		do \
		  $(MAKE) CFLAGS="-g -DYYDEBUG" $$t TARGET=$$t && \
		  ./$$t $$t.in1 <$$t.in >tmp.$$$$ && \
		  $(MAKE) FILE_CMP TEST=$$t \
		    GOOD_LOG=$$t.log TEST_LOG=tmp.$$$$ ; \
		done

C_TESTS:
		@for t in $(C_TESTS) ; \
		do \
		  $(RM) $$t.c ; \
		  $(MAKE) $$t.c TARGET=$$t.c && \
		  tail +5 $$t.c >$$t.$$$$ && \
		  $(MAKE) FILE_CMP TEST=$$t \
		    GOOD_LOG=$$t.log TEST_LOG=$$t.$$$$ ; \
		done


#Ensure that every target depends on zyacc executable & library being tested.
#@@@ If your make has problems, try commenting out the following 2 lines.
$(EXEC_TESTS) $(OUT_TESTS) $(ERR_TESTS) $(DEBUG_TESTS):	\
  $(ZYACC_PATH)/$(ZYACC) $(builddir)/zyacc/libzyacc/libzyacc.a

#The following rule acts as a subroutine invoked via a recursive make to
#compare $(GOOD_LOG) with $(TEST_LOG) for test $(TEST).
FILE_CMP:	
		@echo -n "Testing $(TEST) ... " ; \
		$(SED) -e '/make/d' $(TEST_LOG) >tmp.$$$$ ; \
		$(MV) tmp.$$$$ $(TEST_LOG) ; \
		if cmp $(GOOD_LOG) $(TEST_LOG) >/dev/null 2>/dev/null ; \
		then \
		  echo OK. ; \
		  $(RM) $(TEST_LOG) ; \
		else \
		  mv $(TEST_LOG) $(TEST).ERR ; \
		  echo FAILED. ; \
		  echo Diff test log in $(TEST).ERR \
		       with good log in $(GOOD_LOG) ; \
		fi ;

install:	
		@echo "No files to be installed from this directory."

uninstall:
		@echo "No files to be uninstalled from this directory."

m4_include({{clean.mf}})

Makefile.in:	Makefile.m4 FORCE
		$(MAKE_MF_IN)

m4_include({{dist.mf}})



