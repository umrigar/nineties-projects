#! /usr/bin/perl

#This perl script generates a Makefile.in from this file.  It 
#facilitates generating different distributions depending on which 
#client is using this library.
#
#If run with no arguments, then the entire library is included in the
#distribution.
#
#If run with a single argument, then the argument must be of the form:
#HDRS=hfiles where hfiles is a space separated list of library header
#files required from this library by the client.

#All the local declarations should be replaceable with statically
#scoped my declarations.  But it seems nicer to stay with Perl 4.0.

###################### GLOBAL PERL DEFINITIONS #######################

#The name of this file.
$file= __FILE__;

#Directory where included files should be looked for.
$SEARCHPATH= ".:../mf";

#Headers specified via HDRS command-line argument.
@HDRS= ();

#All modules.
@modules= ();

#Modules selected by HDRS command-line argument.
%selectedModules= ();

#Associative array of .c source files indexed by module-name.
%cSrc= ();

#Associative array of extra source files indexed by module-name.
%xSrc= ();

#Associative array of .h source files indexed by module-name.
%hSrc= ();

#Associative array of .c non-source (automatically generated) files 
#indexed by module-name.
%cNonSrc= ();

#Associative array of extra non-source (automatically generated) files
#indexed by module-name.
%xNonSrc= ();

#Associative array of .h non-source (automatically generated) files 
#indexed by module-name.
%hNonSrc= ();


##################### PERL COMMAND LINE PROCESSING #####################

if ($#ARGV == 0) { #have a single argument
  if ($ARGV[0] =~ "HDRS=(.*)") {
    @HDRS= split(/\s+/, $1);
  }
  else {
    &usage();
  }
}
elsif ($#ARGV > 0) {
  &usage();
}


########################### PERL DEFINITIONS ###########################

#Include a file in the output.
sub doInclude {
  local($name)= @_;
  foreach $d (split(':', $SEARCHPATH)) {
    if (-f "$d/$name" && -r "$d/$name") {
      open(MF, "$d/$name") || die "could not open $d/$name: $!\n";
      print while (<MF>);
      close(MF);
      return;
    }
  }
  die "$name not found.\n";
}

#Print usage message and exit.
sub usage {
  print "usage: Makefile.p [HDRS=hfiles]\n";
  exit(1);
}

#Define the name of a module by adding it to @modules.
sub MODULE {
  local($module)= @_;
  push(@modules, $module);
}

#Form the closure of all the selected modules --- i.e. include within 
#the selected modules all modules which are required.
sub selectedModulesClosure {
  local(%hdrMod);
  foreach $type ("hSrc", "hNonSrc") { #Form inverted header index.
    local(%assocArray);
    %assocArray= eval "%".$type;
    foreach $mod (keys %assocArray) {
      local(@hdrs)= split(/[ \t\\\n]+/, $assocArray{$mod});
      foreach $h (@hdrs) {
        $hdrMod{$h}= $mod;
      }
    }
  }
  local(@selectedModules)= keys %selectedModules;
  while (@selectedModules) { #while queue not empty, do main closure loop.
    local($module)= (shift @selectedModules);
    local($cfiles);
    foreach $type ("cSrc", "cNonSrc", "xSrc", "xNonSrc") {
      local(%assocArray);
      %assocArray= eval "%".$type;
      $cfiles.= " $assocArray{$module}";
    }
    if ($cfiles =~ /\w/) { #there is at least 1 c-file.
      $cfiles =~ s/\\\n/ /g; #without this, DEPEND gives a strange error.
      open(DEPEND, "gcc -MM $cfiles | ");
      while (<DEPEND>) {
        foreach $f (split(/[ \t\\\n]+/)) {
          local($m)= ($hdrMod{$f});
          if ($m && !$selectedModules{$m}) {
	    push(@selectedModules, $m); $selectedModules{$m}= 1;
	  }
        }
      }
      close(DEPEND);
    }
  }
}

#Define @selectModules to be the modules selected by the @HDRS.
sub selectModules {
  if (@HDRS > 0) {
  MODULES: 
      foreach $module (@modules) {
      $modHdrs= $hSrc{$module} . " " . $hNonSrc{$module};
      foreach $hdr (@HDRS) {
        if (index($modHdrs, $hdr) >= 0) {
          $selectedModules{$module}= 1;
	  next MODULES;
	}
      }
    }
    &selectedModulesClosure();
  }
}

#files($isSelected, @assocArrays): returns a space or escaped newline
#separated string of the files contained in assoc-arrays @assocArrays.
#If $isSelected is 1, then only the files corresponding to @selectedModules
#are included in the definition; otherwise all files in
#@assocArrays are included in the definition.
#We go out of our way to ensure there is no \ at the end of the definition
#as some make's get confused.
sub files {
  local($isSelected, @assocArrays)= @_;
  local(@mods)= 
    ($isSelected && %selectedModules) ? keys %selectedModules : @modules;
  local($defStr);
  foreach $assocArrayName (@assocArrays) {
    local(%arr)= eval "%$assocArrayName";
    foreach $mod (@mods) {
      $defStr.= $arr{$mod};
    }
  }
  #Remove last escaped newline as some makes (Ultrix for eg) get confused.
  $*= 1; $defStr=~ s/\\\n$//; $*= 0;
  return $defStr;
}


######################### MODULE DEFINITIONS ###########################

&MODULE("AREA");

$cSrc{"AREA"}= <<'END';
  arcutx.c ardel.c ardelx.c arfreeze.c argrow.c argrowx.c \
  armem.c arreset.c \
END

$hSrc{"AREA"}= <<'END';
  area.h \
END

#-----------------------------------------------------------------------
&MODULE("AUTOBUF");

$hSrc{"AUTOBUF"}= <<'END';
  autobuf.h \
END

#-----------------------------------------------------------------------
&MODULE("BSET");

$cSrc{"BSET"}= <<'END';
  bsadd.c bsassign.c bscomp.c bscopy.c bsdel.c bsdiff.c bsempty.c \
  bsfprint.c bshshsum.c bsinbset.c bsinit.c bsintrsc.c bsmake.c \
  bsnew.c bsnext.c bsprint.c bsrange.c bsrm.c bssize.c bssubset.c \
  bsunary.c bsunion.c \
END

$hSrc{"BSET"}= <<'END';
  bset.h bsetp.h \
END

$testSrc{"BSET"}= <<'END';
  bstest.c \
END

#-----------------------------------------------------------------------
&MODULE("ERROR");

$cSrc{"ERROR"}= <<'END';
  error.c errorz.c libzerr.c \
END

$hSrc{"ERROR"}= <<'END';
  error.h errorz.h \
END

$testSrc{"ERROR"}= <<'END';
  ertest.c \
END

#-----------------------------------------------------------------------
&MODULE("FREELIST");

$cSrc{"FREELIST"}= <<'END';
  freelist.c \
END


$hSrc{"FREELIST"}= <<'END';
  freelist.h \
END

	
#-----------------------------------------------------------------------
&MODULE("HASHTAB");

$cSrc{"HASHTAB"}= <<'END';
  htbasic.c htcursor.c htdelete.c htscope.c htstats.c \
END


$hSrc{"HASHTAB"}= <<'END';
  hashtab.h hashtabp.h \
END

$testSrc{"HASHTAB"}= <<'END';
  httest.c \
END

#-----------------------------------------------------------------------
&MODULE("IDS");

$cSrc{"IDS"}= <<'END';
  idsfail.c idsget.c idsinit.c idsintrn.c idslen.c idsstrng.c  \
END


$hSrc{"IDS"}= <<'END';
  ids.h idsp.h \
END

#-----------------------------------------------------------------------
&MODULE("ISET");

$cSrc{"ISET"}= <<'END';
  isadd.c isassign.c ischown.c iscomp.c iscopy.c \
  isdel.c isdiff.c isdebug.c isempty.c \
  isfprint.c ishshsum.c isiniset.c isinit.c isintrsc.c ismake.c \
  isnew.c isnext.c isprint.c isrange.c isrm.c issize.c issubset.c \
  isunary.c isunion.c  \
END


$hSrc{"ISET"}= <<'END';
  iset.h isetp.h \
END

$testSrc{"ISET"}= <<'END';
  istest.c \
END

#-----------------------------------------------------------------------
&MODULE("MEMALLOC");

$cSrc{"MEMALLOC"}= <<'END';
  memcheck.c memdebug.c \
END


$hSrc{"MEMALLOC"}= <<'END';
  memalloc.h \
END

$testSrc{"MEMALLOC"}= <<'END';
  memtest.c \
END


#-----------------------------------------------------------------------
&MODULE("PARSEOPT");

$cSrc{"PARSEOPT"}= <<'END';
  po01fn.c po2info.c pochklng.c pochrfn.c pochrfn0.c pochrfn1.c poerr.c \
  pohelp.c pohlpfmt.c pointfn0.c pointfn1.c \
  polngfn0.c polngfn1.c pooptdef.c \
  poparse.c poshift.c poshrfn0.c poshrfn1.c postrarg.c postrfn.c postvfn.c \
END

if (0) {
$cNonSrc{"PARSEOPT"}= <<'END';
  potstopt.c \
END
}

$hSrc{"PARSEOPT"}= <<'END';
  optp.h parseopt.h \
END

$testSrc{"PARSEOPT"}= <<'END';
  potest.c \
END

$xSrc{"PARSEOPT"}= <<'END';
  potstchk.c potstinf.c potstop0.c potstmk.c  \
END

$xNonSrc{"PARSEOPT"}= <<'END';
  potstopt.c \
END

#-----------------------------------------------------------------------
&MODULE("PORT");

$hSrc{"PORT"}= <<'END';
  ccport.h osport.h \
END


#-----------------------------------------------------------------------
&MODULE("STRSPACE");

$cSrc{"STRSPACE"}= <<'END';
  ssadd.c sscat.c ssdel.c ssinit.c ssmore.c sspop.c  \
END


$hSrc{"STRSPACE"}= <<'END';
  sspacep.h strspace.h  \
END


#-----------------------------------------------------------------------
&MODULE("TEXTID");

$cSrc{"TEXTID"}= <<'END';
  tiadd.c tidel.c tigetid.c tiidlen.c tiidtext.c tiinit.c \
  titxtlen.c \
END


$hSrc{"TEXTID"}= <<'END';
  textid.h textidp.h \
END

$testSrc{"TEXTID"}= <<'END';
  titest.c \
END


#-----------------------------------------------------------------------
&MODULE("XASSERT");

$hSrc{"XASSERT"}= <<'END';
  xassert.h \
END


#-----------------------------------------------------------------------
&MODULE("ZUTIL");

$cSrc{"ZUTIL"}= <<'END';
  utcesc.c utflfind.c utlnglen.c utopen.c utlg2.c utread.c \
  utstrlo.c utstrlon.c utstrnst.c utstrup.c utstrupn.c utsuffix.c \
END

$hSrc{"ZUTIL"}= <<'END';
  zutil.h \
END

$testSrc{"ZUTIL"}= <<'END';
  utfltest.c \
END

#-----------------------------------------------------------------------
#Compute which modules are selected.  This must follow *ALL* module defs.
&selectModules();

####################### MAKEFILE GENERATION ############################

print <<"END";
# *** DO NOT EDIT. ***
# Automatically generated by perl from $file using the command:
#	$0 @ARGV
END

#Initial configuration section of Makefile.in.
&doInclude('init.mf');

#Suffix rules
&doInclude('suffix.mf');

print <<'END';
########################################################################
#This section describes *ALL* files in the library.  All these files need
#not actually be distributed for a particular program.
END

print <<'END';
#Main target to be built in this directory.
TARGET=		libz.a

END

$t= &files(0, 'cSrc', 'testSrc');
print <<"END";
#All C source files in library.
ALL_C_SRC_FILES= \\
$t

END

$t= &files(0, 'hSrc');
print <<"END";
#All header files in library.
ALL_H_SRC_FILES= \\
$t

END

$t= &files(0, 'testSrc');
$t =~ s/\.c/\.in/g;
$t1= &files(0, 'xSrc');
print <<"END";
#All source files in library.
ALL_SRC_FILES= \\
  \$(ALL_C_SRC_FILES) \$(ALL_H_SRC_FILES) \\
$t \\
$t1 \\
  Makefile.p README

END

$t= &files(0, 'testSrc');
$t =~ s/\.c/\.log/g;
$t1= &files(0, 'xNonSrc');
print <<"END";
#All non-source files in library.
ALL_NON_SRC_FILES= \\
  \$(ALL_C_NON_SRC_FILES) \$(ALL_H_NON_SRC_FILES) \\
$t \\
$t1 \\
  Makefile.in DEPEND

END

print "ALL_DEBUG_ISET_OFILES= \\\n";
$t= $cSrc{"ISET"};
$t =~ s/(\w+)\.c/d\1\.o/g ;
$*= 1; $t=~ s/\\\n$//; $*= 0;
print "$t\n\n";

$t= &files(0, 'cSrc', 'cNonSrc');
$t=~ s/\.c/\.o/g;
print <<"END";
#All object files in library.
ALL_OFILES= \\
  \$(ALL_DEBUG_ISET_OFILES) \\
$t

END

print <<'END';
########################################################################
#This section describes the files actually distributed for a particular
#program.
END

$t= &files(1, 'cSrc', 'testSrc');
print <<"END";
#C source files distributed in library.
DIST_C_SRC_FILES= \\
$t

END

$t= &files(1, 'hSrc');
print <<"END";
#Header files distributed in library.
DIST_H_SRC_FILES= \\
$t

END

$t= &files(1, 'testSrc');
$t =~ s/\.c/\.in/g;
$t1= &files(0, 'xSrc');
print <<"END";
#Source files being distributed in library.
DIST_SRC_FILES= \\
  \$(DIST_C_SRC_FILES) \$(DIST_H_SRC_FILES) \\
$t \\
$t1 \\
  Makefile.p README 

END

$t= &files(1, 'testSrc');
$t =~ s/\.c/\.log/g;
$t1= &files(1, 'xNonSrc');
print <<"END";
#Non-source files distributed in library.
DIST_NON_SRC_FILES= \\
  \$(DIST_C_NON_SRC_FILES) \$(DIST_H_NON_SRC_FILES) \\
$t \\
$t1 \\
  Makefile.in DEPEND

END

if ($selectedModules{"ISET"}) {
  print "DEBUG_ISET_OFILES= \$(ALL_DEBUG_ISET_OFILES)\n\n";
}

$t= &files(1, 'cSrc', 'cNonSrc');
$t=~ s/\.c/\.o/g;
print <<"END";
#Object files to be compiled into distributed library.
DIST_OFILES= \\
  \$(DEBUG_ISET_OFILES) \\
$t

END

$t= &files(1, 'testSrc');
$t =~ s/\.c//g;
print <<"END";
#Tests to be run in distributed library.
DIST_TESTS= \\
$t

END

print <<'END';
#All files to be distributed.
DIST_FILES= \
  $(DIST_SRC_FILES) $(DIST_NON_SRC_FILES)

#PUBLIC TARGETS
all:		$(TARGET)

install:	$(TARGET)
		echo Nothing to be installed for libz.

uninstall:
		echo Nothing to be uninstalled for libz.

$(TARGET):	$(DIST_OFILES)
		$(AR) r $@ $?
		$(RANLIB) $@

#Use this target to build the entire library when all source files are present.
all_libz:	$(ALL_OFILES)
		$(AR) r $@ $?
		$(RANLIB) $@

check:		$(DIST_TESTS) 
		@-for t in $(DIST_TESTS) ; \
		do \
		  echo -n Checking $$t ... ; \
		  FIND_FILE_PATH=/usr/include \
                  MEM_DEBUG_LOG=memdebug.log \
                    ./$$t $$t.in <$$t.in >tmp1.$$$$ 2>tmp2.$$$$ ; \
		  cat tmp1.$$$$ tmp2.$$$$ >tmp.$$$$ ; \
		  $(RM) tmp1.$$$$ tmp2.$$$$ ; \
		  if test $$t = memtest ; \
		  then \
                    cat memdebug.log tmp.$$$$ >tmp1.$$$$ ; \
		    sed -e "s/\`[0-9a-fA-FxX]*'/\`PTR'/" \
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

#All the object files depend on config.h.  Not put into automatic
#dependencies as we don't know the directory for config.h till build-time.
$(DIST_OFILES):	$(builddir)/config.h

$(ALL_OFILES):  $(builddir)/config.h

END

$t= &files(1, 'testSrc');
print <<'END' if ($t);
$(DIST_TESTS):	$(TARGET)

END


if ($selectedModules{"ISET"}) {
  local(@t);
  @t= split(/[ \t\\\n]+/, $cSrc{"ISET"});
  print "#Manual dependencies for DEBUG_ISET_OFILES.\n";
  print "\$(DEBUG_ISET_OFILES):\tiset.h isetp.h\n";
  foreach $t (@t) {
    if ($t =~ /\w/) {
      local($ofile);
      $ofile.= $t;
      $ofile =~ s/(\w+)\.c/d\1\.o/;
      print "$ofile:\t\t$t\n";
    }
  }
  print <<'END';
$(DEBUG_ISET_OFILES):
			@debugCFile=`basename $@ '.o'`.c; \
			srcCFile=`echo $@ | sed 's/^.//;s/o$$/c/'`; \
			echo making $@ from $$srcCFile ; \
			$(LN) $$srcCFile $$debugCFile; \
			$(CC) -c $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) \
			    -DDEBUG_ISET $(DFLAGS) $$debugCFile; \
			$(RM) $$debugCFile

END
}

print <<'END';
#Make dependencies to insert into Makefile.in.
DEPEND:		$(DIST_C_SRC_FILES) $(DIST_C_NON_SRC_FILES) $(DIST_H_SRC_FILES)
		echo "#Automatically generated dependencies." >$(srcdir)/DEPEND
		$(DEPEND_CMD) $(DIST_C_SRC_FILES) $(DIST_C_NON_SRC_FILES) \
		  >>$(srcdir)/DEPEND

END

print <<'END';
#Make makefile.in from this file.
Makefile.in:	Makefile.p DEPEND FORCE
		$(srcdir)/Makefile.p HDRS="$(HDRS)" >$@

END

print <<'END';
#Make distribution.
dist:		$(DIST_NON_SRC_FILES)
		@for f in $(DIST_FILES) ; \
		do \
		  $(LN) $(srcdir)/$$f $(topdir)/$(DIST_DIR)/$$f ; \
		done

END

#Cleaning rules
&doInclude('clean.mf');

#Dependencies.
&doInclude('DEPEND');
