/*

File:	 GramSym.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:33:08 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

abstract class GramSym {

  protected GramSym(int type, int num) {
    this.type= type; this.num= num;
  }

  int getType() {
    return type;
  }

  int getNum() {
    return num;
  }

  public boolean equals(Object sym) {
    return (sym instanceof GramSym) && 
           type == ((GramSym)sym).type && num == ((GramSym)sym).num;
  }

  public int hashCode() {
    return num << type;
  }
  
  protected int type;		//type of grammar symbol.
  protected int num;		//its number.

  final static int TERMINAL= 0;
  final static int NONTERM= 1;
  final static int RULE= 2;
}
