/*

File:	 memtest.c
Purpose: Test program for memory debugging routines.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


#define DEBUG_MEM 1
#include "memalloc.h"

int main() {
#define TEST_SIZE 10
  VOIDP mPtrs[TEST_SIZE];
  Index i;
  for (i= 0; i < TEST_SIZE; i++) {
    mPtrs[i]= MALLOC((SIZE_T)1024);
  }
  for (i= 0; i < TEST_SIZE; i++) {
    mPtrs[i]= REALLOC(mPtrs[i], (SIZE_T)2048);
  }
  for (i= 0; i < TEST_SIZE/2; i++) {
    FREE(mPtrs[i]);
  }
  memAllocations(__FILE__, __LINE__);

  /* Cause memory deallocation error. */
  FREE(mPtrs[0]);
  return 0;
}



