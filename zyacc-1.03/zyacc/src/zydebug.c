/*

File:	 zydebug.c
Purpose: Remote debugging client for zyacc.

Last Update Time-stamp: "97/07/30 08:52:59 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include <config.h>

/*

Trivial client for accessing zyacc debugger via socket interface.

Merely echoes lines from user to remote, and echoes remote output
to stdout until it sees a prompt.

*/


#if HAVE_SYS_SOCKET_H

#include "port.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/* look for this prompt for input */
#define PS1		"\nzydebug> "
#define PS2		"\n% "

enum { 
  MIN_PORT= 5001,	/* port # must be >= this */
  MAX_PORT= 65535,	/* port # must be <= this */
  MAX_LINE= 120		/* max. length of user input line */
};

#ifndef INADDR_NONE
#define INADDR_NONE	0xFFFFFFFF
#endif

/* Insert signature string into executable to permit distinguishing it
 * from other executables of the same name.
 */
char uniqueSignature[]= "Zerksis";

/* A little function to ensure that above signature doesn't get optimized
 * away by optimizing linkers.
 */
int 
ensureSig()
{
  int n= sizeof(uniqueSignature);
  int i;
  for (i= 0; i < n; i++) uniqueSignature[i]= (char)(2*i);
  return uniqueSignature[2];
}


static Boolean 
isNumber(p)
  CONST char *p;
{
  for (; *p; p++) { if (!isdigit(*p)) return FALSE; }
  return TRUE;
}

/* Display output from in on stdout, until a line starting with one of the
 * prompts is found.
 */
static VOID
displayOutput(in)
  FILE *in;
{
  Int c;
  Index i1= 1;	/* go past initial '\n', in case response starts with prompt */
  Index i2= 1;	/* go past initial '\n', in case response starts with prompt */
  CONST char *CONST ps1= PS1;
  Count n1= strlen(ps1);
  CONST char *CONST ps2= PS2;
  Count n2= strlen(ps2);
  while ((c= getc(in)) != EOF) {
    putchar(c);
    i1= (ps1[i1] == c) ? i1 + 1 : 0;
    i2= (ps2[i2] == c) ? i2 + 1 : 0;
    if (i1 == n1 || i2 == n2) break;
  }
  VOID_RET();
}

int 
main(argc, argv)
  int argc;
  CONST char *argv[];
{
  int port= -1;
  CONST char *host= "localhost";
  struct sockaddr_in serverAddr;
  int socketFD;
  FILE *in, *out;
  char line[MAX_LINE];

  ensureSig();
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family= AF_INET;

  switch (argc) {
    case 2:
      if (isNumber(argv[1])) {
	port= atoi(argv[1]);	
      }
      else {
	fprintf(stderr, "bad port # %s; must be a number", argv[1]);
      }
      break;
    case 3:
      host= argv[1];
      if (isNumber(argv[2])) {
	port= atoi(argv[2]);	
      }
      else {
	fprintf(stderr, "bad port # %s; must be a number", argv[2]);
      }
      break;
    default:
      fprintf(stderr, "usage: zydebug [HOST_NAME|HOST_ADDR] PORT\n");
      exit(1);
      break;
  } /* switch (argc) */

  { /* set up host address */
    ULong inAddr= inet_addr(host);
    if (inAddr != INADDR_NONE) {
      memcpy(&serverAddr.sin_addr, &inAddr, sizeof(inAddr));
    }
    else {
      CONST struct hostent *hp= gethostbyname(host);
      if (hp == NULL) {
	fprintf(stderr, "could not find host %s\n", host); exit(1);
      }
      memcpy(&serverAddr.sin_addr, hp->h_addr, hp->h_length);
    }
  }

  if (port < MIN_PORT || port > MAX_PORT) {
    fprintf(stderr, "port # %d out of range; must be in [%d, %d].\n",
	    port, MIN_PORT, MAX_PORT);
    exit(1);
  }
  else {
    serverAddr.sin_port= htons(port);
  }

  if ((socketFD= socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("could not open socket"); exit(1);
  }

  if (connect(socketFD, (struct sockaddr *)&serverAddr, sizeof(serverAddr))
      < 0) {
    perror("can't connect to server"); exit(1);
  }

  if ((in= fdopen(socketFD, "r")) == NULL) {
    perror("could not open socket input"); exit(1);
  }

#if 0 /* Ultrix seems to always return -1 with errno == 0! */
  if (setvbuf(in, NULL, _IOLBF, 0) != 0) {
    perror("could not line buffer socket input"); exit(1);
  }
#endif
  setvbuf(in, NULL, _IOLBF, 0);

  if ((out= fdopen(socketFD, "w")) == NULL) {
    perror("could not open socket output"); exit(1);
  }

#if 0 /* Ultrix seems to always return -1 with errno == 0! */
  if (setvbuf(out, NULL, _IOLBF, 0) != 0) {
    perror("could not line buffer socket output"); exit(1);
  }
#endif
  setvbuf(out, NULL, _IOLBF, 0);

  while (!feof(stdin) && !feof(in)) {
    displayOutput(in);
    if (feof(in)) break;
    do {
      if (fgets(line, MAX_LINE, stdin) == NULL) {
        if (feof(stdin)) break; else { perror("stdin read error"); exit(1); }
      }
      if (line[strlen(line) - 1] != '\n') {
	fprintf(stderr, "line too long ... discarded.%s", PS1);
      }
      else break;
    } while(1);
    if (feof(stdin)) break;
    if (fprintf(out, "%s", line) < 0) {
      perror("socket write error"); exit(1);
    }
    fflush(out); /* should not be necessary, but needed on Ultrix. */
  }
  fclose(in); fclose(out);
  return 0;


}

#else /* !HAVE_SYS_SOCKET_H */

#include <stdio.h>

int 
main()
{
  fprintf(stderr, "remote debugging not compiled\n");
  exit(1);
  return 1;
}

#endif /* if HAVE_SYS_SOCKET_H */
