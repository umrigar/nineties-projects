/*

File:	 SRState.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:40:28 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

class SRState {

  SRState(int num) {
    this.num= num;
  }

  public String toString() {
    return "" + num;
  }

  public int hashCode() {
    return num;
  }

  public boolean equals(Object s) {
    return (s instanceof SRState) && num == ((SRState)s).num;
  }

  private int num;
}
