/*

File:	 utsuffix.c
Purpose: Return a new string with a different suffix.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "memalloc.h"
#include "zutil.h"

/* Change the suffix of a string from one of several to a specified
 * value.  Specifically find a suffix of str which matches a suffix in
 * the NULL-terminated array suffixVec (if several match different
 * suffixes the suffix which occurs earliest in suffixVec is chosen).
 * If none of the strings in suffixVec match a suffix of str, then the
 * empty suffix is chosen.  Remove the chosen suffix from str and add
 * newSuffix, returning the new name in dynamically allocated memory
 * which can subsequently be FREE'd.  This routine can be used to
 * change file extensions.
 */
ConstString 
chStrSuffix(str, suffixVec, newSuffix)
  ConstString str;
  ConstString suffixVec[];
  ConstString newSuffix;
{
  CONST Size newSuffixLen= strlen(newSuffix);
  CONST Size strLen= strlen(str);
  ConstString strEnd= str + strLen;
  Size oldSuffixLen= 0;
  Char *newStr;
  ConstString *p;
  for (p= suffixVec; *p != NULL; p++) {
    CONST Size suffixLen= strlen(*p);
    if (strcmp(strEnd - suffixLen, *p) == 0) {
      oldSuffixLen= suffixLen; break;
    }
  }
  newStr= (Char*)MALLOC(strLen - oldSuffixLen + newSuffixLen + 1);
  strncpy(newStr, str, strLen - oldSuffixLen);
  strcpy(newStr + strLen - oldSuffixLen, newSuffix);
  return newStr;
}
