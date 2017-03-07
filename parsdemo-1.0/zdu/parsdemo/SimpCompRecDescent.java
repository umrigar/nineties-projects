/*

File:	 SimpCompRecDescent.jpp
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:42:09 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

/*
Straight-forward recursive-descent parser whose derivation from a
translation scheme is sketched below:

Parser for following translation scheme (terminals enclosed within single
quotes or all upper-cased; semantic actions within braces; If a symbol
S has a synthesized attribute s, then the symbol is shown as S(s).

program
  : stmts
  ;
stmts
  : assgnStmt
  | assgnStmt ';' stmts
  ;  
assgnStmt
  : ID ':='  expr
  ;
expr
  : expr '+' term
  | expr '-' term
  | term
  ;
term
  : term '*' factor
  | term '/' factor
  | term 'div' factor
  | term 'mod' factor
  | factor
  ;
factor
  : '(' expr ')'
  | ID
  | NUM
  ;
*/



/*
After removing left-recursion, to allow recursive-descent parsing:

program
  : stmts
  ;
stmts
  : assgnStmt stmtsRest
  ;
stmtsRest
  : ';' stmts
  | EMPTY
  ;
assgnStmt
  : ID ':=' expr
  ;
expr
  : term exprRest
  ;
exprRest
  : '+' term exprRest
  | '-' term exprRest
  | EMPTY
  ;
term
  : factor termRest
  ;
termRest
  : '*' factor termRest 
  | '/' factor termRest 
  | 'div' factor termRest 
  | 'mod' factor termRest 
  | EMPTY
  ;
factor
  : '(' expr ')'
  | ID
  | NUM
  ;
*/














import zdu.parsdemo.NonTerm;
import zdu.parsdemo.ParseException;
import zdu.parsdemo.ParseResetException;
import zdu.parsdemo.RecParser;
import zdu.parsdemo.Scanner;
import zdu.parsdemo.SimpCompLL1Gram;
import zdu.parsdemo.Terminal;

import java.util.Hashtable;

class SimpCompRecDescent extends RecParser 
{
  
  SimpCompRecDescent(SimpCompLL1Gram grammar, Scanner scanner,
		     ParseDisplay parseDisplay) {
    super(grammar, scanner, parseDisplay);

    prgSyms.put("assgnStmt", grammar.assgnStmt);
    prgSyms.put("expr", grammar.expr);
    prgSyms.put("exprRest", grammar.exprRest);
    prgSyms.put("factor", grammar.factor);
    prgSyms.put("program", grammar.program);
    prgSyms.put("stmts", grammar.stmts);
    prgSyms.put("stmtsRest", grammar.stmtsRest);
    prgSyms.put("term", grammar.term);
    prgSyms.put("termRest", grammar.termRest);

    prgSyms.put("'+'", grammar.ADD);
    prgSyms.put("ASSGN", grammar.ASSGN);
    prgSyms.put("DIV", grammar.DIV);
    prgSyms.put("'/'", grammar.DIVIDE);
    prgSyms.put("ID", grammar.ID);
    prgSyms.put("'('", grammar.LPAREN);
    prgSyms.put("MOD", grammar.MOD);
    prgSyms.put("'*'", grammar.MULT);
    prgSyms.put("NUM", grammar.NUM);
    prgSyms.put("')'", grammar.RPAREN);
    prgSyms.put("';'", grammar.SEMI);
    prgSyms.put("'-'", grammar.SUB);


  }

  SimpCompRecDescent(SimpCompLL1Gram grammar, Scanner scanner) {
    this(grammar, scanner, null);
  }

  protected final void parse() throws ParseException, ParseResetException
  {
    int ruleN=  0  ;
    do {	call((NonTerm) prgSyms.get("program"), ruleN, 175);	 program ();	} while (false) ;
    do {	accept(ruleN, 176); return;	} while (false) ;
  }

  private void program() throws ParseException, ParseResetException
  {
    int ruleN=  1  ;
    do {	call((NonTerm) prgSyms.get("stmts"), ruleN, 182);	 stmts ();	} while (false) ;
  }

  private void stmts() throws ParseException, ParseResetException
  {
    int ruleN=  2  ;
    do {	call((NonTerm) prgSyms.get("assgnStmt"), ruleN, 188);	 assgnStmt ();	} while (false) ;
    do {	call((NonTerm) prgSyms.get("stmtsRest"), ruleN, 189);	 stmtsRest ();	} while (false) ;
    do {	ret(ruleN, 190); return;	} while (false) ; /* stmts() */
  }

  private void stmtsRest() throws ParseException, ParseResetException
  {
    if (tok.getTokNum() == ';') {
      int ruleN=  3  ;
      do {	match((Terminal) prgSyms.get("';'"), ruleN, 197);	} while (false) ;
      do {	call((NonTerm) prgSyms.get("stmts"), ruleN, 198);	 stmts ();	} while (false) ;
      do {	ret(ruleN, 199); return;	} while (false) ; /* stmtsRest() */
    }
    else {
      int ruleN=  4  ;
      do {	ret(ruleN, 203); return;	} while (false) ; /* stmtsRest() */
    }
  }

  private void assgnStmt() throws ParseException, ParseResetException
  {
    int ruleN=  5  ;
    do {	match((Terminal) prgSyms.get("ID"), ruleN, 210);	} while (false) ;
    do {	match((Terminal) prgSyms.get("ASSGN"), ruleN, 211);	} while (false) ;
    do {	call((NonTerm) prgSyms.get("expr"), ruleN, 212);	 expr ();	} while (false) ;
    do {	ret(ruleN, 213); return;	} while (false) ; /* assgnStmt() */
  }

  private void expr() throws ParseException, ParseResetException
  {
    int ruleN=  6  ;
    do {	call((NonTerm) prgSyms.get("term"), ruleN, 219);	 term ();	} while (false) ;
    do {	call((NonTerm) prgSyms.get("exprRest"), ruleN, 220);	 exprRest ();	} while (false) ;
    do {	ret(ruleN, 221); return;	} while (false) ; /* expr() */
  }

  private void exprRest() throws ParseException, ParseResetException
  {
    if (tok.getTokNum() == '+') {
      int ruleN=  7  ;
      do {	match((Terminal) prgSyms.get("'+'"), ruleN, 228);	} while (false) ;
      do {	call((NonTerm) prgSyms.get("term"), ruleN, 229);	 term ();	} while (false) ;
      do {	call((NonTerm) prgSyms.get("exprRest"), ruleN, 230);	 exprRest ();	} while (false) ;
      do {	ret(ruleN, 231); return;	} while (false) ; /* exprRest() */
    }
    else if (tok.getTokNum() == '-') {
      int ruleN=  8  ;
      do {	match((Terminal) prgSyms.get("'-'"), ruleN, 235);	} while (false) ;
      do {	call((NonTerm) prgSyms.get("term"), ruleN, 236);	 term ();	} while (false) ;
      do {	call((NonTerm) prgSyms.get("exprRest"), ruleN, 237);	 exprRest ();	} while (false) ;
      do {	ret(ruleN, 238); return;	} while (false) ; /* exprRest() */
    }
    else {
      int ruleN=  9  ; 
      do {	ret(ruleN, 242); return;	} while (false) ; /* exprRest() */
    }
  }

  private void term() throws ParseException, ParseResetException
  {
    int ruleN=  10  ;
    do {	call((NonTerm) prgSyms.get("factor"), ruleN, 249);	 factor ();	} while (false) ;
    do {	call((NonTerm) prgSyms.get("termRest"), ruleN, 250);	 termRest ();	} while (false) ;
    do {	ret(ruleN, 251); return;	} while (false) ; /* term() */
  }

  private void termRest() throws ParseException, ParseResetException
  {
    if (tok.getTokNum() == '*') {
      int ruleN=  11  ;
      do {	match((Terminal) prgSyms.get("'*'"), ruleN, 258);	} while (false) ;
      do {	call((NonTerm) prgSyms.get("factor"), ruleN, 259);	 factor ();	} while (false) ;
      do {	call((NonTerm) prgSyms.get("termRest"), ruleN, 260);	 termRest ();	} while (false) ;
      do {	ret(ruleN, 261); return;	} while (false) ; /* termRest() */
    }
    else if (tok.getTokNum() == '/') {
      int ruleN=  12  ;
      do {	match((Terminal) prgSyms.get("'/'"), ruleN, 265);	} while (false) ;
      do {	call((NonTerm) prgSyms.get("factor"), ruleN, 266);	 factor ();	} while (false) ;
      do {	call((NonTerm) prgSyms.get("termRest"), ruleN, 267);	 termRest ();	} while (false) ;
      do {	ret(ruleN, 268); return;	} while (false) ; /* termRest() */
    }
    else if (tok.getTokNum() == SimpCompScanner.DIV_TOK) {
      int ruleN=  13  ;
      do {	match((Terminal) prgSyms.get("DIV"), ruleN, 272);	} while (false) ;
      do {	call((NonTerm) prgSyms.get("factor"), ruleN, 273);	 factor ();	} while (false) ;
      do {	call((NonTerm) prgSyms.get("termRest"), ruleN, 274);	 termRest ();	} while (false) ;
      do {	ret(ruleN, 275); return;	} while (false) ; /* termRest() */
    }
    else if (tok.getTokNum() == SimpCompScanner.MOD_TOK) {
      int ruleN=  14  ;
      do {	match((Terminal) prgSyms.get("MOD"), ruleN, 279);	} while (false) ;
      do {	call((NonTerm) prgSyms.get("factor"), ruleN, 280);	 factor ();	} while (false) ;
      do {	call((NonTerm) prgSyms.get("termRest"), ruleN, 281);	 termRest ();	} while (false) ;
      do {	ret(ruleN, 282); return;	} while (false) ; /* termRest() */
    }
    else {
      int ruleN=  15  ;
      do {	ret(ruleN, 286); return;	} while (false) ; /* termRest() */
    }
  }

  private void factor() throws ParseException, ParseResetException
  {
    if (tok.getTokNum() == '(') {
      int ruleN=  16  ;
      do {	match((Terminal) prgSyms.get("'('"), ruleN, 294);	} while (false) ;
      do {	call((NonTerm) prgSyms.get("expr"), ruleN, 295);	 expr ();	} while (false) ;
      do {	match((Terminal) prgSyms.get("')'"), ruleN, 296);	} while (false) ;
      do {	ret(ruleN, 297); return;	} while (false) ; /* factor() */
    }
    else if (tok.getTokNum() == SimpCompScanner.ID_TOK) {
      int ruleN=  17  ;
      do {	match((Terminal) prgSyms.get("ID"), ruleN, 301);	} while (false) ;
      do {	ret(ruleN, 302); return;	} while (false) ; /* factor() */
    }
    else {
      int ruleN=  18  ;
      String v= new String(scanner.lexemeText());
      do {	match((Terminal) prgSyms.get("NUM"), ruleN, 307);	} while (false) ;
      do {	ret(ruleN, 308); return;	} while (false) ; /* factor() */
    }
  }

  private Hashtable prgSyms= new Hashtable();

}


