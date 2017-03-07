/*

File:	 SimpCompLL1Gram.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:40:41 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.Grammar;
import zdu.parsdemo.NonTerm;
import zdu.parsdemo.Terminal;

class SimpCompLL1Gram extends Grammar {

  SimpCompLL1Gram() {

    setStartSym(program);

    beginRule(program);
      addRHS(stmts);
    endRule();

    beginRule(stmts);
      addRHS(assgnStmt); addRHS(stmtsRest);
    endRule();

    beginRule(stmtsRest);
      addRHS(SEMI); addRHS(stmts);
    endRule();

    beginRule(stmtsRest);
    endRule();

    beginRule(assgnStmt);
      addRHS(ID); addRHS(ASSGN); addRHS(expr);
    endRule();

    beginRule(expr);
      addRHS(term); addRHS(exprRest);
    endRule();

    beginRule(exprRest);
      addRHS(ADD); addRHS(term); addRHS(exprRest);
    endRule();

    beginRule(exprRest);
      addRHS(SUB); addRHS(term); addRHS(exprRest);
    endRule();

    beginRule(exprRest);
    endRule();

    beginRule(term);
      addRHS(factor); addRHS(termRest);
    endRule();

    beginRule(termRest);
      addRHS(MULT); addRHS(factor); addRHS(termRest);
    endRule();

    beginRule(termRest);
      addRHS(DIVIDE); addRHS(factor); addRHS(termRest);
    endRule();

    beginRule(termRest);
      addRHS(DIV); addRHS(factor); addRHS(termRest);
    endRule();

    beginRule(termRest);
      addRHS(MOD); addRHS(factor); addRHS(termRest);
    endRule();

    beginRule(termRest);
    endRule();

    beginRule(factor);
      addRHS(LPAREN); addRHS(expr); addRHS(RPAREN);
    endRule();

    beginRule(factor);
      addRHS(ID);
    endRule();

    beginRule(factor);
      addRHS(NUM);
    endRule();


  }


  static public void main(String args[]) {
    SimpCompLL1Gram g= new SimpCompLL1Gram();
    System.out.println(g.toString());
  }

  final NonTerm assgnStmt= nonTerm("assgnStmt");
  final NonTerm expr= nonTerm("expr");
  final NonTerm exprRest= nonTerm("exprRest");
  final NonTerm factor= nonTerm("factor");
  final NonTerm program= nonTerm("program");
  final NonTerm stmts= nonTerm("stmts");
  final NonTerm stmtsRest= nonTerm("stmtsRest");
  final NonTerm term= nonTerm("term");
  final NonTerm termRest= nonTerm("termRest");

  final Terminal ID= terminal("ID", SimpCompScanner.ID_TOK);
  final Terminal NUM= terminal("NUM", SimpCompScanner.NUM_TOK);
  final Terminal SEMI= terminal("';'");
  final Terminal ASSGN= terminal("':='", SimpCompScanner.ASSGN_TOK);
  final Terminal ADD= terminal("'+'");
  final Terminal SUB= terminal("'-'");
  final Terminal MULT= terminal("'*'");
  final Terminal DIVIDE= terminal("'/'");
  final Terminal DIV= terminal("DIV", SimpCompScanner.DIV_TOK);
  final Terminal MOD= terminal("MOD", SimpCompScanner.MOD_TOK);
  final Terminal LPAREN= terminal("'('");
  final Terminal RPAREN= terminal("')'");

}
