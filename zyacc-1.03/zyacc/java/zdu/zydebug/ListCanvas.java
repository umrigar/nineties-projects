/*

File:	 ListCanvas.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/07/27 12:12:30 zdu"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.zydebug;

import zdu.zydebug.Attention;
import zdu.zydebug.TextCanvas;

import java.awt.*;
import java.util.BitSet;

//Quick & dirty class equivalent to List, to allow colored selections and
//mouse hints.
class ListCanvas extends TextCanvas {
  
  ListCanvas(int width, int height, String mouseHint) {
    super(width, height, mouseHint);
  }

  ListCanvas(int width, int height) {
    this(width, height, null);
  }

  ListCanvas() {
    this(300, 180);
  }

  void addItem(String text) {
    addLine(text);	
  }

  void setAttention(Attention attentionObj) {
    this.attentionObj= attentionObj;
  }

  boolean isSelected(int itemN) {
    return selected.get(itemN + 1);
  }

  int lastClickItemN() {
    return lastClickLineN - 1;
  }

  String getItemText(int itemN) {
    return getText(itemN + 1);
  }

  private int lineNum(int y) {
    FontMetrics m= getFontMetrics(getFont());
    int fontHeight= m.getHeight();
    int lineLo= (vScroll.getValue() + y)/fontHeight;
    return lineLo + 1; // + 1 to convert to 1-origin.
  }

  public boolean mouseExit(Event event, int x, int y) {
    if (mouseLineN != 0) {
      unmarkLine(mouseLineN); 
      mouseLineN= mouseDownLineN= 0; isMouseDown= false; repaint();
    }
    return super.mouseExit(event, x, y);    
  }

  public boolean mouseDown(Event evt, int x, int y) {
    int lineN= lineNum(y);
    isMouseDown= true;
    mouseLineN= mouseDownLineN= lineN; 
    repaint();
    return super.mouseDown(evt, x, y);
  }

  public boolean mouseDrag(Event evt, int x, int y) {
    int lineN= lineNum(y);
    if (lineN != mouseLineN) {
      mouseLineN= lineN; isMouseDown= true;
      repaint();
    }
    return super.mouseDrag(evt, x, y);
  }

  public boolean mouseMove(Event evt, int x, int y) {
    int lineN= lineNum(y);
    if (lineN != mouseLineN) {
      mouseLineN= lineN; isMouseDown= false;
      repaint();
    }
    return super.mouseMove(evt, x, y);
  }

  public boolean mouseUp(Event  evt, int  x, int  y) {
    int lineN= lineNum(y);
    if (lineN == mouseDownLineN) {
      boolean isSelected= selected.get(lineN);
      if (!isSelected) {
	markLine(lineN, SELECT_COLOR); selected.set(lineN);
      }
      else {
	unmarkLine(lineN); selected.clear(lineN);
      }
    }
    mouseLineN= lineN; isMouseDown= false; mouseDownLineN= 0;
    repaint();
    lastClickLineN= lineN;
    if (attentionObj != null) attentionObj.attention();
    return super.mouseUp(evt, x, y);
  }

  public void paint(Graphics g) {
    if (mouseLineN > 0) {
      Dimension s= size();
      FontMetrics m= getFontMetrics(getFont());
      int h= m.getHeight();
      int y= (mouseLineN - 1)*h - vScroll.getValue();
      g.setColor((isMouseDown && mouseDownLineN == mouseLineN)
		 ? MOUSE_DOWN_COLOR 
		 : MOUSE_UP_COLOR);
      g.fillRect(0, y, s.width, h);      
    }
    super.paint(g);
  }

  private static final Color MOUSE_UP_COLOR= Color.lightGray;
  private static final Color MOUSE_DOWN_COLOR= Color.gray;
  private static final Color SELECT_COLOR= Color.red;

  private Attention attentionObj= null;
  private BitSet selected= new BitSet();
  private int mouseLineN= 0;
  private int mouseDownLineN= 0;
  private int lastClickLineN= 0;
  private boolean isMouseDown= false;
}
