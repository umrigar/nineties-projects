# 90s-projects

**Unmaintained** archive of projects worked on in the 1990s. 

(Good old Netscape Navigator gray background HTML).

## Dimensional Analysis with C++

[Source code](./dim) for the SIGPLAN paper
[Fully Static Dimensional Analysis with C++](<./dim/sigplan/sigplan.html>).
Based on early C++ templates (C++ 2.0).  Original
[README](./dim/README).

## Partial Evaluator

An [implementation](./ch4_pl) in Prolog of the partial evaluator
described in the book
[Partial Evaluation and Automatic Program Generation](https://www.itu.dk/~sestoft/pebook/pebook.html).
Original [README](./ch4_pl/README).

## Animation of Parsing Algorithms

A Java 1.02 applet which demonstrates
[recursive-descent parsing](./parsdemo/recframe.html),
[LL(1) parsing](./parsdemo/ll1frame.html) and
[shift-reduce parsing](./parsdemo/srframe.html) on a simple expression
grammar.  Shows the currently constructed parse tree/forest, the
current parse table/program and a trace of the parse.  Original
[README](./parsdemo/README).


## Unobstrusive Markup Text

Though this [project](./umt) was never released publicly, it is the
only project I still use.  It is similar to the markups in many wikis
or markdown.  I have used [AsciiDoc](http://asciidoc.org/) in the
past, but have returned to this cruder program as it scratched my
itch!  I hope to redo this program in the near future.

## zlex Scanner Generator

A lex-compatible scanner generator.
[Manual](./zlex/zlex/doc/zlex.html),  original
[README](./zlex/README).

## zyacc Parser Generator

A yacc-compatible parser generator with support for inherited attributes using
[LR-attributed grammars](https://en.wikipedia.org/wiki/LR-attributed_grammar).
Provided the following enhancements:

* Supports [inherited attributes](./zyacc/zyacc/doc/zyacc_4.html#SEC55) which can be
uniquely evaluated in a left-to-right parse.

* Supports [semantic tests](./zyacc/zyacc/doc/zyacc_4.html#SEC59)
which allow the outcome of runtime semantic tests to affect parsing
decisions.

* Permits remote interactive
[debugging](./zyacc/zyacc/doc/zyacc_9.html#SEC104) of generated
parsers either by using a textual interface or by using a
(zydebug)[java-based GUI].  The debugger allows the setting of
breakpoints on any grammar symbol and selective display of the current
parser state.

* Allows
  [named attribute variables](./zyacc/zyacc/doc/zyacc_4.html#SEC52)
  which make maintaining grammars easier.

* Can generate its parser description files in HTML which can then be viewed
using any Web browser ([Zyacc Options](./zyacc/zyacc/doc/zyacc_10.html#SEC122)).

* Provides a [`%look` directive](./zyacc/zyacc/doc/zyacc_4.html#SEC74) to
check (at parser construction time) whether a reduction requires lookahead.

* Allows
[multiple start nonterminals](./zyacc/zyacc/doc/zyacc_4.html#SEC70)
and allows a call to the parsing function to be made for a particular
start nonterminal.

* It is possible to avoid having types which are only used in describing
nonterminal semantics written into the generated `.h` file
([Data Types of Semantic Values](./zyacc/zyacc/doc/zyacc_4.html#SEC47)).

* Allows
  [multiple-character quoted literal tokens](./zyacc/zyacc/doc/zyacc_4.html#SEC66).

* Allows [command-line options](./zyacc/zyacc/doc/zyacc_4.html#SEC73)
  to be specified from within the parser file.


[Manual](./zyacc/zyacc/doc/zyacc_toc.html),  original [README](./zyacc/zyacc/doc/zyacc_toc.html).

## zprof Profiler

A source transformation tool to produce basic-block profiles of ANSI-C
programs.  Given a simple primes-computation program
[primes.c](./zprof/zprof/primes.c), zprof can
[annotate](./zprof/zprof/primes.c.bb) each basic block with its
execution count.

Used as a [tutorial](zprof/zprof/NOTES.html) for zlex and
zyacc.  [man page](./zprof/zprof/zprof.html), original
[README](./zprof/README).
