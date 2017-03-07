/*

File:	 ParseNode.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/07/21 12:37:05 zdu"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.zydebug;

class ParseNode {
  /** Create a new parse node.
   * @param label:	Label for node.
   * @param type:	Type of node.
   */
  public ParseNode(String label, int type) {
    this.label= label; this.type= type;
  }
  /** Create a new parse node which may have more kids.
   * @param label:	Label for this node.
   */
  public ParseNode(String label) {
    this.label= label; this.type= 0;
  }
  /** Create a new parse node with an empty label, which is not closed. */
  public ParseNode() {
    this.label= ""; this.type= 0;
  }
  /** @return The label associated with this node. */
  public String getLabel() {
    return label;
  }

  /** @return true iff this node is closed (i.e. it may not have more kids). */
  public int getType() {
    return type;
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
   * @param type:	new value of type.
   * @return		old value of type.
   */
  public int setType(int type) {
    int ret= this.type;
    this.type= type;
    return ret;
  }
  /** Convert to string. 
   * @return		Label of node.
   */
  public String toString() {
    return label;
  }

  public static final int ERROR= 0;
  public static final int TERM= 1;
  public static final int NON_TERM= 2;

  private int type;		//one of the above constants.
  private String label;
}
