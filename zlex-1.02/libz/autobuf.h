#ifndef _AUTOBUF_H
#define _AUTOBUF_H

#include "memalloc.h"

#define DCL_AUTO_BUF(type, n)						\
  struct {								\
    type *buf;								\
    type *dynamicBuf;							\
    Size size;								\
    type autoBuf[n];							\
  }

#define INIT_AUTO_BUF(name)						\
  do { 									\
    (name).dynamicBuf= NULL; (name).buf= (name).autoBuf; 		\
    (name).size= sizeof((name).autoBuf);				\
  } while (0)

#define CHECK_AUTO_BUF(name, neededSize)				\
  do {									\
    if ((neededSize) > (name).size) {					\
      (name).size= (neededSize);					\
      (name).buf= (name).dynamicBuf= 					\
	 REALLOC((name).dynamicBuf,	 				\
		 (neededSize) * sizeof(*(name).dynamicBuf));		\
    }									\
  } while (0)

#define AUTO_BUF(name)	((name).buf)

#define FREE_AUTO_BUF(name)						\
  do { if ((name).dynamicBuf) FREE((name.dynamicBuf)); } while (0)

#endif /* ifndef _AUTOBUF_H */
