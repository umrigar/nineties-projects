/*

File:	 Rule.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:40:22 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.GramSym;
import zdu.parsdemo.NonTerm;
import zdu.parsdemo.Rule;

class Rule extends GramSym {

  Rule(NonTerm lhs, int lo, int hi, int ruleN, Grammar grammar) {
    super(RULE, ruleN);
    this.lhs= lhs; this.lo= lo; this.hi= hi; this.grammar= grammar;
  }

  NonTerm getLHS() {
    return lhs;
  }

  int getItemLo() {
    return lo;
  }

  int getItemHi() {
    return hi;
  }

  Rule getAlt() {
    return alt;
  }

  Grammar getGrammar() {
    return grammar;
  }

  int rhsLen() {
    return hi - lo;
  }

  GramSym rhsSym(int i) {
    if (i >= rhsLen()) throw new InternalError("bad rhs index " + i);
    return grammar.getItem(lo + i);
  }

  void addAlt(Rule alt) {
    this.alt= alt;
  }

  public String toString() {
    return "" + getNum();
  }

  public String getText() {
    StringBuffer b= new StringBuffer();
    b.append(getLHS().toString() + ": /* " + getNum() + " */\n");
    int lo= getItemLo();
    int hi= getItemHi();
    if (lo == hi) {
      b.append("  /* EMPTY */");
    }
    else {
      for (int i= getItemLo(); i < hi; i++) {
	b.append("  " + grammar.getItem(i).toString());
	if (i != hi - 1) b.append("\n");
      }
    }
    return b.toString();
  }

  private NonTerm lhs= null;	//lhs non-terminal.
  private int lo= -1;		//index of first rhs sym.
  private int hi= -1;		//index 1 beyond last rhs sym; points to this.
  private Rule alt= null;	//next rule for lhs.
  private Grammar grammar= null;//Rule belongs to this grammar.

}
