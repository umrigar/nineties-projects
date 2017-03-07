/*

File:	 ScrollPanel.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/07/19 19:40:57 zdu"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.zydebug;

import java.awt.*;

/** A panel containing a canvas, a south and east scrollbars.
 * This class is necessary solely to handle scroll events. 
 */
class ScrollPanel extends Panel {

  ScrollPanel(Canvas canvas, Scrollbar hScroll, Scrollbar vScroll) {
    this.canvas= canvas; this.hScroll= hScroll; this.vScroll= vScroll;
    setLayout(new BorderLayout());
    add("South", hScroll); add("East", vScroll);
    add("Center", canvas); 
  }

  public boolean handleEvent(Event event) {
    if (event.target == hScroll || event.target == vScroll) {
      canvas.repaint();
    }
    return super.handleEvent(event);
  }

  private Canvas canvas;
  private Scrollbar hScroll, vScroll;
}

