  /* -*-c-*- Test for interactions between reject and unput. */
/* Input: abc; Output abcababacababa;

Rejected text is represented as X/Y where X matches some pattern, whereas Y
will be rescanned.

  Input		Output	Rejected Text	Unscanned Input
  abc\n		abc	ab/c		cab\n
  cab\n		ab	a/b		bacab\n
  bacab\n	a	-		bacab\n
  bacab\n	b	-		acab\n
  acab\n	a	-		cab\n
  cab\n		c	-		ab\n
  ab\n		ab	a/b		ba\n
  ba\n		a	-		ba\n
  ba\n		b	-		a\n
  a\n		a	-		\n
  \n		\n	-		-

*/

%%
abc	unput('b'); unput('a'); ECHO; REJECT; 
ab	unput('a'); ECHO; REJECT;
a	ECHO;

