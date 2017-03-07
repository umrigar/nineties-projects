divert(-1)
#File:    mklibsrc.m4
#Purpose: M4 script to extract library sources.
#
#Copyright (C) 1995 Zerksis D. Umrigar
#
#See the file LICENSE for copying and distribution conditions.
#THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

ifdef(`DEPEND_CMD', ,
      `errprint(`ERROR: Run with -DDEPEND_CMD="shell command for dependencies".
') m4exit')
changequote({, })
divert(0)dnl
#! /bin/sh
#*** DO NOT EDIT. ***
#This file was automatically produced by m4 using a command like:
{#}m4 -DDEPEND_CMD="DEPEND_CMD" <mklibsrc.m4 >THIS_FILE

#nm command used to analyze object file.
NM="nm -u"

#Library header file.
libhdr="libzlexp.h"

case $# in
  1) oFile=$1;;
  2) oFile=$1;
     if test -d $2 
     then
       libSrcDir=$2
     else
       destFile=$2
     fi;;
  3) oFile=$1; libSrcDir=$2; destFile=$3;;
  *) echo Usage: $0 OBJECT_FILE [LIBRARY_SOURCE_DIRECTORY] [DEST_FILE]; 
     exit 1;;
esac

if test ! -f $oFile 
then
  echo Object file $oFile not found; exit 1;
fi

if test -z "$libSrcDir"
then
  libSrcDir=`echo $0 | sed -e "s%/[^/]*$%%"`
fi

if test ! -d $libSrcDir
then
  echo "Library source directory $libSrcDir invalid."; exit 1
fi

if test -z "$destFile"
then
  destFile=libsrc.c
fi

test -f $destFile || echo '/*' Library sources. '*/' >$destFile

fns=`$NM $oFile \
     | sed -n 's/.*\(yy[a-zA-Z0-9_]*\).*/\1/p' \
     | tr '\012' ' '`

for fn in $fns
do
  deps=`grep $fn <<EOF
syscmd(DEPEND_CMD)
EOF
`
  if test "$deps"
  then
    deps=`echo $deps | cut -f2 -d:`
    deps="$libhdr $deps"
    for f in $deps
    do
      echo -n "$f... "
      if test ! -f $libSrcDir/$f
      then
        echo "Could not find library source file $f."; exit 1;
      fi
      hdr="===File: $f==="
      if grep "$hdr" $destFile >/dev/null 2>/dev/null 
      then
	echo "already present."
      else
        echo '/* ' $hdr ' */' >>$destFile ; 
        sed -e "/$libhdr/d" $libSrcDir/$f >>$destFile
        echo "added."
      fi
    done
  fi
done
exit 0
