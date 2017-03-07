ZYACC=		zyacc
ZYACCFLAGS=		
CC=		gcc
CFLAGS=		-g -Wall
LIBS=		-lm
RM=		rm -f


.SUFFIXES:

.SUFFIXES:	.o .c .y

.y.c:
		$(ZYACC) $(ZYACCFLAGS) $< -o $*.c

.y:
		$(ZYACC) $(ZYACCFLAGS) $< -o $*.c
		$(CC) $(CPPFLAGS) $(CXXFLAGS) $(DFLAGS) \
		      $(CFLAGS) $*.c $(LIBS) $(LDFLAGS) -o $*
		$(RM) $*.c

.c:
		$(CC) $(CPPFLAGS) $(CXXFLAGS) $(DFLAGS) \
		      $(CFLAGS) $< $(LIBS) $(LDFLAGS) -o $*

.c.o:
		$(CC) -c $(CPPFLAGS) $(CXXFLAGS) $(DFLAGS) \
		      $(CFLAGS) $< 


all:		rpcalc calc mfcalc nmcalc polycalc lazycalc

#Dependencies
rpcalc:		rpcalc.y

calc:		calc.y

mfcalc:		mfcalc.y

nmcalc:		nmcalc.y

polycalc:	polycalc.y

lazycalc:	lazycalc.y

