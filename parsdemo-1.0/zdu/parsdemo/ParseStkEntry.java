/*

File:	 ParseStkEntry.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:40:10 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.OffsetTree;
import zdu.parsdemo.ParseNode;

class ParseStkEntry {

  ParseStkEntry(OffsetTree tree) {
    this.tree= tree;
  }

  OffsetTree getOffsetTree() {
    return tree;
  }
  
  public String toString() {
    return ((ParseNode)tree.getInfo()).toString();
  }

  OffsetTree tree;

};
