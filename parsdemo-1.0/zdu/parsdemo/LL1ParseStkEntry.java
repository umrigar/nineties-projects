/*

File:	 LL1ParseStkEntry.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:33:08 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.GramSym;
import zdu.parsdemo.ParseStkEntry;

class LL1ParseStkEntry extends ParseStkEntry {

  LL1ParseStkEntry(GramSym gramSym, int ruleN, OffsetTree tree) {
    super(tree);
    this.gramSym= gramSym; this.ruleN= ruleN;
  }

  int getRuleN() {
    return ruleN;
  }

  GramSym getGramSym() {
    return gramSym;
  }

  GramSym gramSym;
  int ruleN;

};
