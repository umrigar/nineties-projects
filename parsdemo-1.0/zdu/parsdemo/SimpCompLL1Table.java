/*

File:	 SimpCompLL1Table.java
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
import zdu.parsdemo.SparseTable2;

import java.util.Enumeration;


class SimpCompLL1Table extends LL1Table {

  SimpCompLL1Table(SimpCompLL1Gram g) {
    super(g);

    put(g.program, g.ID, g.rule(1));

    put(g.stmts, g.ID, g.rule(2));

    put(g.stmtsRest, g.SEMI, g.rule(3));
    put(g.stmtsRest, g.EOF, g.rule(4));

    put(g.assgnStmt, g.ID, g.rule(5));

    put(g.expr, g.LPAREN, g.rule(6));
    put(g.expr, g.ID, g.rule(6));
    put(g.expr, g.NUM, g.rule(6));

    put(g.exprRest, g.SEMI, g.rule(9));
    put(g.exprRest, g.ADD, g.rule(7));
    put(g.exprRest, g.SUB, g.rule(8));
    put(g.exprRest, g.RPAREN, g.rule(9));
    put(g.exprRest, g.EOF, g.rule(9));

    put(g.term, g.LPAREN, g.rule(10));
    put(g.term, g.ID, g.rule(10));
    put(g.term, g.NUM, g.rule(10));

    put(g.termRest, g.SEMI, g.rule(15));
    put(g.termRest, g.ADD, g.rule(15));
    put(g.termRest, g.SUB, g.rule(15));
    put(g.termRest, g.MULT, g.rule(11));
    put(g.termRest, g.DIVIDE, g.rule(12));
    put(g.termRest, g.DIV, g.rule(13));
    put(g.termRest, g.MOD, g.rule(14));
    put(g.termRest, g.RPAREN, g.rule(15));
    put(g.termRest, g.EOF, g.rule(15));

    put(g.factor, g.LPAREN, g.rule(16));
    put(g.factor, g.ID, g.rule(17));
    put(g.factor, g.NUM, g.rule(18));
  }

  static public void main(String args[]) {
    SimpCompLL1Table tab= 
      new SimpCompLL1Table(new SimpCompLL1Gram());
    System.out.println(tab.toString());
  }

};
