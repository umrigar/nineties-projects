/*

File:	 utflfind.c
Purpose: Find a file given a list of paths and environment variables.

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "autobuf.h"
#include "osport.h"
#include "zutil.h"

/* 

Open file fileName in a given mode. The file is searched for using the
search directories specified by dirs which is a list of directories
separated by ':'s (';' for MSDOG).  If an entry in dirs starts with a
'$' and the second character is not '$', then it is interpreted to be
an environment variable whose value specifies a list of directories to
be searched recursively.  If a directory name starts with a '$'
character, then that '$' (only the starting '$') must be quoted by
repeating it.  If the file can be opened in the requested mode, then
its FILE descriptor is returned; otherwise NULL is returned.  If the
file is found and succDir is non-NULL, then *succDir is returned as a
pointer within dirList to the directory in which it was found.  If the
file is found and succDirLenP is non-NULL, then *succDirLenP is
returned as the length of the success directory name (the length does
not include any terminating DIRECTORY_SEPARATOR).

If dirList is empty, then the current directory is searched.  An empty
component in dirList also specifies a search of the current directory:
an empty component is specified when dirList has either two
adjacent PATH_SEPARATORs or starts or ends with a PATH_SEPARATOR.

If the system getenv() routine is non-reentrant, then the current
implementation does not support a environment variable dirList
component which contains another environment variable.

*/

#define PATH AUTO_BUF(path)
FILE *
findFileDir(fName, mode, dirList, succDirP, succDirLenP)
  ConstString fName;		/* The name of the file we're looking for. */
  ConstString mode;		/* The mode in which it should be opened. */
  ConstString dirList;		/* List of directories to search. */
  ConstString *succDirP;	/* If non-NULL, store success dir ptr. here. */
  UInt *succDirLenP;		/* Length of success dir (does not include
				 * a terminating DIR_SEPARATOR).
				 */
{
  CONST Size fNameLen= strlen(fName);
  DCL_AUTO_BUF(char, 512) path;
  ConstString currentDir= dirList;
  ConstString dirListEnd= dirList + strlen(dirList);
  FILE *fP= 0;
  ConstString lastTryDir;	/* Directory last searched. */
  UInt lastTryDirLen;
  Size xDirLen;
  INIT_AUTO_BUF(path);
  while (fP == 0 && currentDir < dirListEnd) {
    ConstString p= strchr(currentDir, PATH_SEPARATOR);
    CONST Size dirLen= (p) ? p - currentDir : strlen(currentDir);
    CONST Boolean addDirSeparator= 
      (dirLen > 0 && currentDir[dirLen - 1] != DIR_SEPARATOR);
    CONST Size pathLen= dirLen + addDirSeparator + fNameLen + 1;
    xDirLen= dirLen;
    CHECK_AUTO_BUF(path, pathLen);
    strncpy(PATH, currentDir, dirLen);
    if (dirLen > 1 && PATH[0] == '$' && PATH[1] != '$') { /* Environment var. */
      ConstString env;
      PATH[dirLen]= '\0'; 
      env= (ConstString)getenv(&PATH[1]); 
      if (env) {
#if 0	/* If getenv() isn't reentrant, but won't support succDirP. */
	CONST Size envLen= strlen(env);
	CHECK_AUTO_BUF(path, envLen);
	strcpy(PATH, env); 
	fP= findFileDir(fName, mode, PATH, &lastTryDir);
#endif
	fP= findFileDir(fName, mode, env, &lastTryDir, &lastTryDirLen);
      } /* if (env) */
    } /* if environment var. */
    else { /* Not an environment var. */
      CONST Boolean isDollarDir= (dirLen > 0) && (PATH[0] == '$');
      lastTryDir= currentDir + isDollarDir;
      lastTryDirLen= addDirSeparator ? dirLen : dirLen - 1;
      if (addDirSeparator) PATH[dirLen]= DIR_SEPARATOR;
      strcpy(&PATH[dirLen + addDirSeparator], fName);
      fP= fopen(&PATH[isDollarDir], mode);
    }
    currentDir+= dirLen + (currentDir[dirLen] == PATH_SEPARATOR);
  } /* while (fP == 0 && currentDir < dirListEnd) */
  assert(fP || currentDir == dirListEnd);
  if (!fP && currentDir != dirList 
      && *(currentDir - 1) == PATH_SEPARATOR) { /* Search current dir. */
    xDirLen= 0; fP= fopen(fName, mode);
  }
  if (fP) {
    if (xDirLen == 0) { lastTryDir= "."; lastTryDirLen= 1; }
    if (succDirP) *succDirP= lastTryDir;
    if (succDirLenP) *succDirLenP= lastTryDirLen;
  }
  FREE_AUTO_BUF(path);
  return fP;
}
#undef PATH

/* Wrapper around findFileDir without any succDir argument. */
FILE *
findFile(fileName, mode, dirList)
  ConstString fileName;
  ConstString mode;
  ConstString dirList;
{ 
  return findFileDir(fileName, mode, dirList, NULL, NULL);
}

