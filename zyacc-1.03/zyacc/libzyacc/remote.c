/*

File:	 remote.c
Purpose: Remote debugging initialization for zyacc.

Last Update Time-stamp: "97/07/30 07:45:52 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzyacp.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if HAVE_SYS_SOCKET_H		/* compile in sockets interface */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*

Largely pulled out of "Unix Network Programming", by Richard Stevens.

I am not entirely comfortable with the fact that I am using stdio
routines on sockets.  This is done using _IOLBF line-buffering.  I
have not seen anything which says this is dangerous.  If it doesn't
work, I'd replace the fprintf() calls in debug.c with calls to a
stdarg/varargs wrapper which did its work using sprintf() followed
by a write.

I am not checking the return value on setvbuf(), because at least
under Ultrix, setvbuf() seems to consistently return -1, even if I use
it on regular fopen'd FILE's or stdin.  The code appears to work ok
even with the -1 return value.  I could have ifdef'd out the check on
the return value, but if Ultrix appears to get it wrong, other OS's
may too.

*/

enum { 
  MAX_BACKLOG= 5,	/* max. # of queued connections */
  MIN_PORT= 5001,	/* assigned port # must be >= this */
  MAX_PORT= 65535,	/* assigned port # must be <= this */
  DEFAULT_PORT= 6001	/* if no port # specified, start with this */
};

enum {
  LINE_LEN= 120,
  NLINES= 5
};

/* Open file fName for writing: if it exists and is not a /dev file,
 * then it opens it with truncation only if sig occurs within the
 * first 5 lines (each line containing upto 120 chars) of it.
 */
static FILE *
safeOpen(fName, sig)
  YY_CONST char *fName;
  YY_CONST char *sig;
{
  FILE *f;
  if (strncmp("/dev", fName, 4) != 0) {
    f= fopen(fName, "r");
    if (f) {
      char line[LINE_LEN];
      YY_CONST char *p;
      YYUInt i;
      for (i= 0; i < NLINES && (p= fgets(line, LINE_LEN, f)); i++) {
	if (strstr(p, sig)) break;
      }
      fclose(f);
      if (p == NULL || i == NLINES) {
	fprintf(stderr, 
"\
File `%s' already exists and appears to have been generated\n\
by another program\n\
",
		fName);
	exit(1);
      }
    }
  }
  f= fopen(fName, "w");
  if (!f) {
    fprintf(stderr, "could not open file %s\n", fName); exit(1);
  }
  return f;
}

static YYBoolean 
isNumber(p)
  YY_CONST char *p;
{
  for (; *p; p++) { if (!isdigit(*p)) return FALSE; }
  return TRUE;
}

/* I'd like to have this library as self-contained as possible.  Hence
 * I prefer to have the templates for the HTML files generated for
 * the applet stored within the library, rather than as an external 
 * resource which could get mislaid.  Hence I am including those
 * templates as strings in this file.  I was originally including
 * a huge string for each template, but understandably some cc's
 * were'nt too happy.  I now include each template as an array of
 * strings --- 1 entry for each line.  At runtime I whack all the
 * lines into a single string in a dynamically allocated buffer
 * which is used as the fmt string to printf().
 */

static YY_CONST char *framesFmt[]= {
#include "frames.str"
};

#define FRAMES_NLINES		(sizeof(framesFmt)/sizeof(framesFmt[0]))

static YY_CONST char *appletFmt[]= {
#include "applet.str"
};

#define APPLET_NLINES		(sizeof(appletFmt)/sizeof(appletFmt[0]))

static YYUInt
htmlLen(array, nEntries) 
  YY_CONST char *array[];
  YYUInt nEntries;
{
  YYUInt s= 0;
  YYUInt i;
  for (i= 0; i < nEntries; i++) s+= strlen(array[i]);
  return s;
}

static YY_CONST char *
htmlCopy(buf, array, nEntries)
  char *buf;
  YY_CONST char *array[];
  YYUInt nEntries;
{
  YYUInt i;
  char *p= buf;
  for (i= 0; i < nEntries; i++) {
    strcpy(p, array[i]); p+= strlen(array[i]);
  }
  *p= '\0';
  return buf;
}

#define SIG	"zydebug"

YYBoolean 
yyRemoteDebug(debugDataP)
  YYDebugData *YY_CONST debugDataP;
{
  YY_CONST char *appletEnv= (YY_CONST char*)getenv(APPLET_ENV_VAR);
  YY_CONST char *portEnv= (YY_CONST char*)getenv(PORT_ENV_VAR);
  YY_CONST YYBoolean isApplet= (appletEnv != NULL);
  YY_CONST YYBoolean isRemote= isApplet || (portEnv != NULL);
  if (!isRemote) {
    return FALSE;
  }
  else {
    int specifiedPortNumber= 
      (portEnv != NULL && isNumber(portEnv)) ? atoi(portEnv) : -1;
    int port= 
      (MIN_PORT <= specifiedPortNumber && specifiedPortNumber <= MAX_PORT) 
      ? specifiedPortNumber 
      : DEFAULT_PORT;
    int socketFD, newSocketFD, clientLen;
    struct sockaddr_in serverAddr, clientAddr;
    FILE *in, *out;

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family= AF_INET;
    serverAddr.sin_port= htons(port);
    serverAddr.sin_addr.s_addr= htonl(INADDR_ANY);
  
    if ((socketFD= socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("could not open socket"); exit(1);
    }
  
    do {
      if (bind(socketFD, (struct sockaddr *) &serverAddr, 
	       sizeof(serverAddr)) >= 0) break;
#ifdef EADDRINUSE    
      if (errno == EADDRINUSE) {
	++port; serverAddr.sin_port= htons(port); continue;
      }
#endif
      perror("bind failed"); exit(1);
    } while (port <= MAX_PORT);
    if (port > MAX_PORT) {
      fprintf(stderr, "could not find a free port"); exit(1);
    }

    if (listen(socketFD, MAX_BACKLOG) < 0) {
      perror("listen failed"); exit(1);
    }

    if (isApplet) {
      enum { NAME_BUF_LEN= 128 };
      YY_CONST char *codebaseEnv= (YY_CONST char*)getenv(CODEBASE_ENV_VAR);
      YY_CONST char *codebase= (codebaseEnv) ? codebaseEnv : ".";
      YY_CONST char *htmlbaseEnv= (YY_CONST char*)getenv(HTMLBASE_ENV_VAR);
      YY_CONST char *htmlbase= (htmlbaseEnv) ? htmlbaseEnv : ".";
      YY_CONST char *srcName= debugDataP->srcName;
      YY_CONST char *appletLastDot= strrchr(appletEnv, '.');
      YY_CONST int appletBaseLen= 
	(appletLastDot) ? appletLastDot - appletEnv : strlen(appletEnv);
      YY_CONST char *appletLastSlash= strrchr(appletEnv, '/');
      YY_CONST char *appletRoot= 
	(appletLastSlash) ? appletLastSlash + 1 :appletEnv;
      YY_CONST YYUInt appletRootLen= appletBaseLen - (appletRoot - appletEnv);
      YY_CONST char *srcLastDot= strrchr(srcName, '.');
      YY_CONST YYUInt srcBaseLen= 
	(srcLastDot) ? srcLastDot - srcName : strlen(srcName);
      YY_CONST YYUInt appletHTMLLen= htmlLen(appletFmt, APPLET_NLINES);
      YY_CONST YYUInt framesHTMLLen= htmlLen(framesFmt, FRAMES_NLINES);
      YY_CONST YYUInt htmlBufLen= 
	(appletHTMLLen > framesHTMLLen ? appletHTMLLen : framesHTMLLen) + 1;
      char *htmlBuf= malloc(htmlBufLen);
      char nameBuf[NAME_BUF_LEN];
      FILE *f;

      if (!htmlBuf) {
	fprintf(stderr, "out of memory for HTML buffer\n"); exit(1);
      }
	
      if (appletBaseLen + 9 >= NAME_BUF_LEN) {
	fprintf(stderr, "applet file name specified by %s too long\n",
		APPLET_ENV_VAR);
	exit(1);
      }

      sprintf(nameBuf, "%.*s.html", appletBaseLen, appletEnv);
      f= safeOpen(nameBuf, SIG);
      htmlCopy(htmlBuf, framesFmt, FRAMES_NLINES);
      fprintf(f, htmlBuf, srcName, htmlbase, srcBaseLen, srcName, 
	      appletRootLen, appletRoot, srcName, appletRootLen, appletRoot);
      fclose(f);

      sprintf(nameBuf, "%.*sZ.html", appletBaseLen, appletEnv);
      f= safeOpen(nameBuf, SIG);
      htmlCopy(htmlBuf, appletFmt, APPLET_NLINES);
      fprintf(f, htmlBuf, srcName, srcName, codebase, 
	      port, srcName, htmlbase);
      fclose(f);
      
      free(htmlBuf);
    }

    fprintf(stderr, "zydebug port: %d\n", port);

    clientLen= sizeof(clientAddr);
    newSocketFD= accept(socketFD, (struct sockaddr *)&clientAddr, 
			&clientLen);
    if (newSocketFD < 0) {
      perror("could not accept connection"); exit(1);
    }

    if ((in= fdopen(newSocketFD, "r")) == NULL) {
      perror("could not open socket input"); exit(1);
    }

#if 0 /* Ultrix seems to always return -1 with errno == 0! */
    if (setvbuf(in, NULL, _IOLBF, 0) != 0) {
      perror("could not line buffer socket input"); exit(1);
    }
#endif
    setvbuf(in, NULL, _IOLBF, 0);

    if ((out= fdopen(newSocketFD, "w")) == NULL) {
      perror("could not open socket output"); exit(1);
    }

#if 0 /* Ultrix seems to always return -1 with errno == 0! */
    if (setvbuf(out, NULL, _IOLBF, 0) != 0) {
      perror("could not line buffer socket output"); exit(1);
    }
#endif
    setvbuf(out, NULL, _IOLBF, 0);

    debugDataP->in= in; debugDataP->out= out; debugDataP->err= out;
    debugDataP->flags|= FLAG(REMOTE_F);

  } /* else remote debugging */
  return TRUE;
}

#else /* !HAVE_SYS_SOCKET_H */

YYBoolean 
yyRemoteDebug(debugDataP)
  YYDebugData *YY_CONST debugDataP;
{
  YY_CONST char *appletEnv= (YY_CONST char*)getenv(APPLET_ENV_VAR);
  YY_CONST char *portEnv= (YY_CONST char*)getenv(PORT_ENV_VAR);
  if (appletEnv != NULL || portEnv != NULL) {
    fprintf(stderr, 
   "sorry.  remote debugging has not been compiled into the zyacc library.\n");
  }
  return FALSE;
}


#endif /* !HAVE_SYS_SOCKET_H */
