# 90s-projects

**Unmaintained** archive of projects worked on in the 1990s. 

Though most of the code will need non-trivial rework to run on modern
systems, the documentation is quite extensive and may prove useful.
For example, the
zprof tutorial
documents the development of a [C basic-block profiler](https://rawgit.com/zerksis/90s-projects/master/zprof-0.1/zprof/NOTES.pdf), the
[zlex manual](https://rawgit.com/zerksis/90s-projects/master/zlex-1.02/zlex/doc/zlex.html) gives a
[regular expression for C comments](https://rawgit.com/zerksis/90s-projects/master/zlex-1.02/zlex/doc/zlex.html#SEC47),
the
[zyacc manual](https://rawgit.com/zerksis/90s-projects/master/zyacc-1.03/zyacc/doc/zyacc_toc.html)
gives multiple enhancements useful for bottom-up parser generators,
etc.

The only additions made when putting up this repository were to
automatically generate PDF's from the existing Postscript or HTML
documentation and to add this README.  All files other than this file
are unchanged from the 1990's.  Hence external links will not work.
You also get to see vintage Netscape Navigator style gray backgrounds!

The [dist](./dist) directory contains the original distribution
archives.

## Dimensional Analysis with C++

[Source code](./dim-1.02) for the SIGPLAN paper
[Fully Static Dimensional Analysis with C++](https://rawgit.com/zerksis/90s-projects/master/dim-1.02/sigplan.pdf).
Based on early C++ templates (C++ 2.0).  Original
[README](https://rawgit.com/zerksis/90s-projects/master/dim-1.02/README).

## Partial Evaluator

An [implementation](./ch4_pl) in Prolog of the partial evaluator
described in the book
[Partial Evaluation and Automatic Program Generation](https://www.itu.dk/~sestoft/pebook/pebook.html).
Original [README](https://rawgit.com/zerksis/90s-projects/master/ch4_pl/README).

## Animation of Parsing Algorithms

A Java 1.02 applet which demonstrates
[recursive-descent parsing](https://rawgit.com/zerksis/90s-projects/master/parsdemo-1.0/recframe.html),
[LL(1) parsing](https://rawgit.com/zerksis/90s-projects/master/parsdemo-1.0/ll1frame.html)
and
[shift-reduce parsing](https://rawgit.com/zerksis/90s-projects/master/parsdemo-1.0/srframe.html)
on a simple expression grammar.  Shows the currently constructed parse
tree/forest, the current parse table/program and a trace of the parse.
This will not work with modern browsers since they will not run Java
applets (quite a contrast with the hype of the late 90s).  Original
[README](https://rawgit.com/zerksis/90s-projects/master/parsdemo-1.0/README).


## Unobstrusive Markup Text

Though this [project](./umt) was never released publicly, it is the
only project I still use.  It is similar to the markups in many wikis
or markdown.  I have used [AsciiDoc](http://asciidoc.org/) in the
past, but have returned to this cruder program as it scratched my
itch!  I hope to redo this program in the near future.

## zlex Scanner Generator

A lex-compatible scanner generator.
[Manual](https://rawgit.com/zerksis/90s-projects/master/zlex-1.02/zlex/doc/zlex.html), [PDF](https://rawgit.com/zerksis/90s-projects/master/zlex-1.02/zlex/doc/zlex.pdf),  original
[README](https://rawgit.com/zerksis/90s-projects/master/zlex-1.02/README).

## zyacc Parser Generator

A yacc-compatible parser generator with support for inherited attributes using
[LR-attributed grammars](https://en.wikipedia.org/wiki/LR-attributed_grammar).
Provided the following enhancements:

* Supports [inherited attributes](https://rawgit.com/zerksis/90s-projects/master/zyacc-1.03/zyacc/doc/zyacc_4.html#SEC55) which can be
uniquely evaluated in a left-to-right parse.

* Supports [semantic tests](https://rawgit.com/zerksis/90s-projects/master/zyacc-1.03/zyacc/doc/zyacc_4.html#SEC59)
which allow the outcome of runtime semantic tests to affect parsing
decisions.

* Permits remote interactive
[debugging](https://rawgit.com/zerksis/90s-projects/master/zyacc-1.03/zyacc/doc/zyacc_9.html#SEC104) of generated
parsers either by using a textual interface or by using a
(zydebug)[java-based GUI].  The debugger allows the setting of
breakpoints on any grammar symbol and selective display of the current
parser state.

* Allows
  [named attribute variables](https://rawgit.com/zerksis/90s-projects/master/zyacc-1.03/zyacc/doc/zyacc_4.html#SEC52)
  which make maintaining grammars easier.

* Can generate its parser description files in HTML which can then be viewed
using any Web browser ([Zyacc Options](https://rawgit.com/zerksis/90s-projects/master/zyacc-1.03/zyacc/doc/zyacc_10.html#SEC122)).

* Provides a [`%look` directive](https://rawgit.com/zerksis/90s-projects/master/zyacc-1.03/zyacc/doc/zyacc_4.html#SEC74) to
check (at parser construction time) whether a reduction requires lookahead.

* Allows
[multiple start nonterminals](https://rawgit.com/zerksis/90s-projects/master/zyacc-1.03/zyacc/doc/zyacc_4.html#SEC70)
and allows a call to the parsing function to be made for a particular
start nonterminal.

* It is possible to avoid having types which are only used in describing
nonterminal semantics written into the generated `.h` file
([Data Types of Semantic Values](https://rawgit.com/zerksis/90s-projects/master/zyacc-1.03/zyacc/doc/zyacc_4.html#SEC47)).

* Allows
  [multiple-character quoted literal tokens](https://rawgit.com/zerksis/90s-projects/master/zyacc-1.03/zyacc/doc/zyacc_4.html#SEC66).

* Allows [command-line options](https://rawgit.com/zerksis/90s-projects/master/zyacc-1.03/zyacc/doc/zyacc_4.html#SEC73)
  to be specified from within the parser file.


[Manual](https://rawgit.com/zerksis/90s-projects/master/zyacc-1.03/zyacc/doc/zyacc_toc.html), [PDF](https://rawgit.com/zerksis/90s-projects/master/zyacc-1.03/zyacc/doc/zyacc.pdf),  original [README](https://rawgit.com/zerksis/90s-projects/master/zyacc-1.03/zyacc/doc/zyacc_toc.html).

## zprof Profiler

A source transformation tool to produce basic-block profiles of ANSI-C
programs.  Given a simple primes-computation program
[primes.c](./zprof-0.1/zprof/primes.c), zprof can
[annotate](./zprof-0.1/zprof/primes.c.bb) each basic block with its
execution count.

Used as a
[tutorial](https://rawgit.com/zerksis/90s-projects/master/zprof-0.1/zprof/NOTES.html)
for zlex and zyacc
([PDF](https://rawgit.com/zerksis/90s-projects/master/zprof-0.1/zprof/NOTES.pdf)).
[man page](https://rawgit.com/zerksis/90s-projects/master/zprof-0.1/zprof/zprof.html),
original
[README](https://rawgit.com/zerksis/90s-projects/master/zprof-0.1/README).
