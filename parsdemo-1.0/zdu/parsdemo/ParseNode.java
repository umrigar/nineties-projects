/*

File:	 ParseNode.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:38:49 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.GramSym;

class ParseNode {
  /** Create a new parse node.
   * @param label:	Label for node.
   * @param isClosed:	True if this node will not have any more kids.
   */
  public ParseNode(String label, boolean closed) {
    this.label= label; this.closed= closed;
  }
  /** Create a new parse node which may have more kids.
   * @param label:	Label for this node.
   */
  public ParseNode(String label) {
    this.label= label; this.closed= false;
  }
  /** Create a new parse node with an empty label, which is not closed. */
  public ParseNode() {
    this.label= ""; this.closed= false;
  }
  /** @return The label associated with this node. */
  public String getLabel() {
    return label;
  }

  /** @return true iff this node is closed (i.e. it may not have more kids). */
  public boolean isClosed() {
    return closed;
  }
  /** Set node label.
   * @param label:	New label.
   * @return		Old label.
   */
  public String setLabel(String label) {
    String ret= this.label;
    this.label= label;
    return ret;
  }
  /** Set whether or not the node is closed (cannot have more kids).
   * @param closed:	true iff node is to be closed.
   * @return		true iff node was previously closed.
   */
  public boolean setClosed(boolean closed) {
    boolean ret= this.closed;
    this.closed= closed;
    return ret;
  }
  /** Convert to string. 
   * @return		Label of node.
   */
  public String toString() {
    return label;
  }
  private String label;
  private boolean closed;
}
