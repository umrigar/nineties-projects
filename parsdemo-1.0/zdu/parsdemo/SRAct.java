/*

File:	 SRAct.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:40:24 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

class SRAct {

  SRAct(int type, Rule rule) {
    this.type= type; this.stateOrRule= rule;
  }

  SRAct(int type, SRState state) {
    this.type= type; this.stateOrRule= state;
  }

  int getType() {
    return type;
  }

  SRState getState() {
    return (SRState)stateOrRule;
  }

  Rule getRule() {
    return (Rule)stateOrRule;
  }

  public String toString() {
    if (type == REDUCE) {
      return "r" + ((Rule)stateOrRule);
    }
    else {
      return ((type == SHIFT) ? "s" : "g") + (SRState)(stateOrRule);
    }
  }

  private int type;
  private Object stateOrRule;

  static final int SHIFT= 0;
  static final int REDUCE= SHIFT + 1;
  static final int GOTO= REDUCE + 1;

};
