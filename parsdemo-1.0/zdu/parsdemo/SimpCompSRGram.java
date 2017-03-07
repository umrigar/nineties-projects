/*

File:	 SimpCompSRGram.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:40:47 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.Grammar;
import zdu.parsdemo.NonTerm;
import zdu.parsdemo.Terminal;

class SimpCompSRGram extends Grammar {

  SimpCompSRGram() {
    setStartSym(program);

    beginRule(program);
      addRHS(stmts);
    endRule();

    beginRule(stmts);
      addRHS(assgnStmt);
    endRule();

    beginRule(stmts);
      addRHS(assgnStmt); addRHS(SEMI); addRHS(stmts);
    endRule();


    beginRule(assgnStmt);
      addRHS(ID); addRHS(ASSGN); addRHS(expr);
    endRule();

    beginRule(expr);
      addRHS(expr); addRHS(ADD); addRHS(term); 
    endRule();

    beginRule(expr);
      addRHS(expr); addRHS(SUB); addRHS(term); 
    endRule();

    beginRule(expr);
      addRHS(term); 
    endRule();

    beginRule(term);
      addRHS(term); addRHS(MULT); addRHS(factor); 
    endRule();

    beginRule(term);
      addRHS(term); addRHS(DIVIDE); addRHS(factor); 
    endRule();

    beginRule(term);
      addRHS(term); addRHS(DIV); addRHS(factor); 
    endRule();

    beginRule(term);
      addRHS(term); addRHS(MOD); addRHS(factor); 
    endRule();

    beginRule(term);
      addRHS(factor); 
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
    SimpCompSRGram g= new SimpCompSRGram();
    System.out.println(g.toString());
  }

  final NonTerm assgnStmt= nonTerm("assgnStmt");
  final NonTerm expr= nonTerm("expr");
  final NonTerm factor= nonTerm("factor");
  final NonTerm program= nonTerm("program");
  final NonTerm stmts= nonTerm("stmts");
  final NonTerm term= nonTerm("term");

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
