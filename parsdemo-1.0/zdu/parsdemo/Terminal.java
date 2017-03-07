/*

File:	 Terminal.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:40:53 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

class Terminal extends GramSym {
  public Terminal(String name, int num, int tok) {
    super(TERMINAL, num);
    this.tok= tok; this.name= name;
  }

  int getTok() {
    return tok;
  }

  public String toString() {
    return name;
  }

  private int tok;
  private String name;
}
