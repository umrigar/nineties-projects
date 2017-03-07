/*

File:	 ParseDisplay.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:37:58 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.ExtFrame;
import zdu.parsdemo.OffsetForest;
import zdu.parsdemo.Selectable;
import zdu.parsdemo.TraceCanvas;
import zdu.parsdemo.TreeCanvas;

import java.util.Stack;

class ParseDisplay {

  ParseDisplay(TreeCanvas treeCanvas, ScrollableCanvas prgTableCanvas,
	       TraceCanvas traceCanvas, ExtFrame extFrame) {
    this.treeCanvas= treeCanvas;
    this.prgTableCanvas= prgTableCanvas;
    this.traceCanvas= traceCanvas;
    this.extFrame= extFrame;
  }



  void update(OffsetForest forest, Object xSelect, Object ySelect,
	      OffsetTree treeSelect, 
	      String stk, String input, String extFrameTag) {
    treeCanvas.setForest(forest);
    ((Selectable)treeCanvas).select(treeSelect, null);
    ((Selectable)prgTableCanvas).select(xSelect, ySelect);
    traceCanvas.addTrace(stk, input);
    if (extFrame != null) extFrame.show(extFrameTag);
    treeCanvas.repaint();
    prgTableCanvas.repaint();
    traceCanvas.repaint();
  }

  void reset() {
    traceCanvas.reset();
    treeCanvas.reset();
    prgTableCanvas.reset();
  }

  private TreeCanvas treeCanvas;
  private ScrollableCanvas prgTableCanvas;
  private TraceCanvas traceCanvas;
  private ExtFrame extFrame;

}
