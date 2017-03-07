/*

File:	 memtest.c
Purpose: Test program for memory debugging routines.

Last Update Time-stamp: <97/07/30 19:05:26 zdu>

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


#define MEM_DEBUG 1
#include "memalloc.h"

int main() {
#define TEST_SIZE 10
  VOIDP mPtrs[TEST_SIZE];
  Index i;
  atexit(memAtExit);
  for (i= 0; i < TEST_SIZE; i++) {
    mPtrs[i]= MALLOC((SIZE_T)1024);
  }
  /* cause a few fence errors */
  for (i= 0; i < TEST_SIZE; i++) {
    if (i % 2) {
      *((UChar *)(mPtrs[i]) - i - 1)= i;
    }
    else {
      *((UChar *)(mPtrs[i]) + 1024 + i)= i;
    }
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
  memAtExit();
  return 0;
}



