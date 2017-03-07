/*

File:	 SRTable.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:40:29 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.SparseTable2;
import zdu.parsdemo.SRState;

import java.util.Enumeration;
import java.util.Hashtable;

abstract class SRTable extends SparseTable2 {

  SRTable(int nStates, Grammar g) {
    for (int i= 0; i < nStates; i++) {
      addRow(new SRState(i));
    }

    for (Enumeration e= g.terminalsEnum(); e.hasMoreElements(); ) {
      Terminal terminal= (Terminal) e.nextElement();
      if (terminal.getTok() >= 0) addCol(terminal);
    }

    for (Enumeration e= g.nonTermsEnum(); e.hasMoreElements(); ) {
      NonTerm nonTerm= (NonTerm) e.nextElement();
      addCol(nonTerm);
    }
  }

  public Object get(Object state, Object sym) {
    Object act= super.get(state, sym);
    return (act == null && ((GramSym)sym).getType() == GramSym.TERMINAL)
           ? defaults.get(state) 
           : act;
  }

  protected Hashtable defaults= new Hashtable();

}
