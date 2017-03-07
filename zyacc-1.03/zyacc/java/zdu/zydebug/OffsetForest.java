/*

File:	 OffsetForest.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/07/19 19:59:11 zdu"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.zydebug;

import zdu.zydebug.OffsetTree;

/* Implement an OffsetForest as nothing but an OffsetTree with a hidden
 * invisible root.  
 *
 * IMPORTANT NOTE: If the OffsetTree's in the forest are shared in other
 * data-structures, it is safe to change their tree structure only
 * AFTER they have been removed from the forest.
 */

class OffsetForest {

  OffsetForest() {
    root= new OffsetTree("");
  }

  int size() {
    return root.nKids();
  }

  void push(OffsetTree t) {
    root.addKid(t);
  }

  void addElement(OffsetTree t) {
    root.addKid(t);
  }

  OffsetTree pop() {    
    return (OffsetTree) root.rmKid(root.nKids() - 1);
  }

  OffsetTree elementAt(int i) {
    return (OffsetTree) root.kid(i);
  }

  OffsetTree peek() {
    return (OffsetTree) root.kid(root.nKids() - 1);
  }

  int leftWidth() {
    return root.leftWidth();
  }

  int rightWidth() {
    return root.rightWidth();
  }

  int height() {
    return root.height() - 1;
  }

  //this implementation requires the gc to collect one more node, but is
  //trivial compared to removing all kids.
  void removeAllElements() {
    root= new OffsetTree("");
  }


  private OffsetTree root;

    
}
