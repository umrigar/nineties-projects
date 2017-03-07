/*

File:	 BoundedObj.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/07/26 20:32:39 zdu"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.zydebug;

import java.awt.Rectangle;

class BoundedObj {
  BoundedObj(Rectangle bounds, Object obj) {
    this.bounds= bounds;
    this.obj= obj;
  }

  boolean isInside(int x, int y) {
    return bounds.inside(x, y);
  }

  Object getObject() {
    return obj;
  }

  Rectangle getBounds() {
    return bounds;
  }

  public String toString() {
    return super.toString() + "[" + bounds + "," + obj + "]";
  }

  private Rectangle bounds;
  private Object obj;
}
