/*

File:	 NonTerm.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:34:19 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.GramSym;
import zdu.parsdemo.Rule;

class NonTerm extends GramSym {
  public NonTerm(String name, int num) {
    super(NONTERM, num);
    this.name= name;
  }

  public String toString() {
    return name;
  }

  Rule getFirstRule() {
    return firstRule;
  }

  void addRule(Rule rule) {
    if (lastRule == null) {
      firstRule= lastRule= rule;
    }
    else {
      lastRule.addAlt(rule);
      lastRule= rule;
    }
  }

  private String name= null;
  private Rule firstRule= null;
  private Rule lastRule= null;
}
