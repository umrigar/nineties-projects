/*

File:	 LL1Table.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:34:15 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.SparseTable2;

import java.util.Enumeration;
import java.util.Hashtable;

abstract class LL1Table extends SparseTable2 {

  LL1Table(Grammar g) {
    for (Enumeration e= g.nonTermsEnum(); e.hasMoreElements(); ) {
      addRow(e.nextElement());
    }

    for (Enumeration e= g.terminalsEnum(); e.hasMoreElements(); ) {
      Terminal terminal= (Terminal) e.nextElement();
      if (terminal.getTok() >= 0) addCol(terminal);
    }

  }

}
