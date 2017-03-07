/*

File:	 SimpCompSRTable.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:40:47 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.GramSym;
import zdu.parsdemo.Grammar;
import zdu.parsdemo.NonTerm;
import zdu.parsdemo.Rule;
import zdu.parsdemo.SRAct;
import zdu.parsdemo.SimpCompSRGram;
import zdu.parsdemo.SparseTable2;
import zdu.parsdemo.Terminal;


class SimpCompSRTable extends SRTable {

  SimpCompSRTable(SimpCompSRGram g) {
    
    super(N_STATES, g);

    SRState[] states= new SRState[N_STATES];

    for (int i= 0; i < N_STATES; i++) {
      states[i]= (SRState) getRow(i);
    }

    //shift/reduce/goto acts follow.
    put(states[0], g.ID, new SRAct(SRAct.SHIFT, states[1]));
    put(states[0], g.program, new SRAct(SRAct.GOTO, states[2]));
    put(states[0], g.stmts, new SRAct(SRAct.GOTO, states[3]));
    put(states[0], g.assgnStmt, new SRAct(SRAct.GOTO, states[4]));


    put(states[1], g.ASSGN, new SRAct(SRAct.SHIFT, states[5]));

    put(states[2], g.EOF, new SRAct(SRAct.SHIFT, states[6]));

    defaults.put(states[3], new SRAct(SRAct.REDUCE, g.rule(1)));

    put(states[4], g.SEMI, new SRAct(SRAct.SHIFT, states[7]));
    defaults.put(states[4], new SRAct(SRAct.REDUCE, g.rule(2)));

    put(states[5], g.ID, new SRAct(SRAct.SHIFT, states[9]));
    put(states[5], g.NUM, new SRAct(SRAct.SHIFT, states[10]));
    put(states[5], g.LPAREN, new SRAct(SRAct.SHIFT, states[8]));
    put(states[5], g.expr, new SRAct(SRAct.GOTO, states[11]));
    put(states[5], g.term, new SRAct(SRAct.GOTO, states[12]));
    put(states[5], g.factor, new SRAct(SRAct.GOTO, states[13]));

    defaults.put(states[6], new SRAct(SRAct.REDUCE, g.rule(0)));
    
    put(states[7], g.ID, new SRAct(SRAct.SHIFT, states[1]));
    put(states[7], g.stmts, new SRAct(SRAct.GOTO, states[14]));
    put(states[7], g.assgnStmt, new SRAct(SRAct.GOTO, states[4]));

    put(states[8], g.ID, new SRAct(SRAct.SHIFT, states[9]));
    put(states[8], g.NUM, new SRAct(SRAct.SHIFT, states[10]));
    put(states[8], g.LPAREN, new SRAct(SRAct.SHIFT, states[8]));
    put(states[8], g.expr, new SRAct(SRAct.GOTO, states[15]));
    put(states[8], g.term, new SRAct(SRAct.GOTO, states[12]));
    put(states[8], g.factor, new SRAct(SRAct.GOTO, states[13]));

    defaults.put(states[9], new SRAct(SRAct.REDUCE, g.rule(14)));
    
    defaults.put(states[10], new SRAct(SRAct.REDUCE, g.rule(15)));
    
    put(states[11], g.ADD, new SRAct(SRAct.SHIFT, states[16]));
    put(states[11], g.SUB, new SRAct(SRAct.SHIFT, states[17]));
    defaults.put(states[11], new SRAct(SRAct.REDUCE, g.rule(4)));
    
    put(states[12], g.MOD, new SRAct(SRAct.SHIFT, states[21]));
    put(states[12], g.DIV, new SRAct(SRAct.SHIFT, states[20]));
    put(states[12], g.MULT, new SRAct(SRAct.SHIFT, states[18]));
    put(states[12], g.DIVIDE, new SRAct(SRAct.SHIFT, states[19]));
    defaults.put(states[12], new SRAct(SRAct.REDUCE, g.rule(7)));

    defaults.put(states[13], new SRAct(SRAct.REDUCE, g.rule(12)));

    defaults.put(states[14], new SRAct(SRAct.REDUCE, g.rule(3)));

    put(states[15], g.ADD, new SRAct(SRAct.SHIFT, states[16]));
    put(states[15], g.SUB, new SRAct(SRAct.SHIFT, states[17]));
    put(states[15], g.RPAREN, new SRAct(SRAct.SHIFT, states[22]));

    put(states[16], g.ID, new SRAct(SRAct.SHIFT, states[9]));
    put(states[16], g.NUM, new SRAct(SRAct.SHIFT, states[10]));
    put(states[16], g.LPAREN, new SRAct(SRAct.SHIFT, states[8]));
    put(states[16], g.term, new SRAct(SRAct.GOTO, states[23]));
    put(states[16], g.factor, new SRAct(SRAct.GOTO, states[13]));

    put(states[17], g.ID, new SRAct(SRAct.SHIFT, states[9]));
    put(states[17], g.NUM, new SRAct(SRAct.SHIFT, states[10]));
    put(states[17], g.LPAREN, new SRAct(SRAct.SHIFT, states[8]));
    put(states[17], g.term, new SRAct(SRAct.GOTO, states[24]));
    put(states[17], g.factor, new SRAct(SRAct.GOTO, states[13]));

    put(states[18], g.ID, new SRAct(SRAct.SHIFT, states[9]));
    put(states[18], g.NUM, new SRAct(SRAct.SHIFT, states[10]));
    put(states[18], g.LPAREN, new SRAct(SRAct.SHIFT, states[8]));
    put(states[18], g.factor, new SRAct(SRAct.GOTO, states[25]));

    put(states[19], g.ID, new SRAct(SRAct.SHIFT, states[9]));
    put(states[19], g.NUM, new SRAct(SRAct.SHIFT, states[10]));
    put(states[19], g.LPAREN, new SRAct(SRAct.SHIFT, states[8]));
    put(states[19], g.factor, new SRAct(SRAct.GOTO, states[26]));

    put(states[20], g.ID, new SRAct(SRAct.SHIFT, states[9]));
    put(states[20], g.NUM, new SRAct(SRAct.SHIFT, states[10]));
    put(states[20], g.LPAREN, new SRAct(SRAct.SHIFT, states[8]));
    put(states[20], g.factor, new SRAct(SRAct.GOTO, states[27]));

    put(states[21], g.ID, new SRAct(SRAct.SHIFT, states[9]));
    put(states[21], g.NUM, new SRAct(SRAct.SHIFT, states[10]));
    put(states[21], g.LPAREN, new SRAct(SRAct.SHIFT, states[8]));
    put(states[21], g.factor, new SRAct(SRAct.GOTO, states[28]));
 
    defaults.put(states[22], new SRAct(SRAct.REDUCE, g.rule(13)));

    put(states[23], g.MOD, new SRAct(SRAct.SHIFT, states[21]));
    put(states[23], g.DIV, new SRAct(SRAct.SHIFT, states[20]));
    put(states[23], g.MULT, new SRAct(SRAct.SHIFT, states[18]));
    put(states[23], g.DIVIDE, new SRAct(SRAct.SHIFT, states[19]));
    defaults.put(states[23], new SRAct(SRAct.REDUCE, g.rule(5)));

    put(states[24], g.MOD, new SRAct(SRAct.SHIFT, states[21]));
    put(states[24], g.DIV, new SRAct(SRAct.SHIFT, states[20]));
    put(states[24], g.MULT, new SRAct(SRAct.SHIFT, states[18]));
    put(states[24], g.DIVIDE, new SRAct(SRAct.SHIFT, states[19]));
    defaults.put(states[24], new SRAct(SRAct.REDUCE, g.rule(6)));

    defaults.put(states[25], new SRAct(SRAct.REDUCE, g.rule(8)));

    defaults.put(states[26], new SRAct(SRAct.REDUCE, g.rule(9)));

    defaults.put(states[27], new SRAct(SRAct.REDUCE, g.rule(10)));

    defaults.put(states[28], new SRAct(SRAct.REDUCE, g.rule(11)));

  }


  static public void main(String args[]) {
    SimpCompSRTable tab= 
      new SimpCompSRTable(new SimpCompSRGram());

    System.out.print(tab.toString());

  }
  
  static final int N_STATES= 29;
}
