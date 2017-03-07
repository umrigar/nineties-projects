/*

File:	 SRParseStkEntry.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:40:26 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.SRState;
import zdu.parsdemo.ParseStkEntry;

class SRParseStkEntry extends ParseStkEntry {

  SRParseStkEntry(SRState state, OffsetTree tree) {
    super(tree);
    this.state= state;
  }

  SRState getState() {
    return state;
  }

  public String toString() {
    return state + "/" + super.toString();
  }

  SRState state;

};
