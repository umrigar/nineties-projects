/*

File:	 TextCanvas.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/07/27 11:11:32 zdu"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.zydebug;

import zdu.zydebug.ScrollableCanvas;
import zdu.zydebug.Selectable;
import zdu.zydebug.TextMark;
import zdu.zydebug.TextLine;

import java.applet.*;
import java.awt.*;
import java.io.*;
import java.util.Vector;


class TextCanvas extends ScrollableCanvas implements Selectable {

  TextCanvas(int width, int height, String mouseHint) {
    super(width, height, mouseHint);
    reset(true);
  }

  TextCanvas(int width, int height) {
    this(width, height, null);
  }

  TextCanvas() {
    this(300, 180);
  }

  public void reset() {
    reset(false);
  }

  void reset(boolean isDestructive) {
    if (isDestructive) lines= new Vector();
    unmarkLine(selectedLineN);
    selectedLineN= 0; topLineN= 1;
    prgUpdate= true;
  }

  public void setFont(Font f) {
    super.setFont(f); prgUpdate= true;
  }

  //This routine is called before painting.
  private void updateScrollbars() {
    FontMetrics fontMetrics= getFontMetrics(getFont());
    int nLines= lines.size();
    int maxWidth= 0;
    for (int i= 0; i < nLines; i++) {
      int w= fontMetrics.stringWidth(((TextLine)(lines.elementAt(i))).text);
      if (w > maxWidth) maxWidth= w;
    }
    Dimension visSize= size();
    setScrollbar(hScroll, 0, visSize.width, 0, maxWidth + SLOP);
    hScroll.setLineIncrement(fontMetrics.stringWidth("m")); 
    hScroll.setPageIncrement(
      visSize.width > SLOP ? visSize.width - SLOP : visSize.width);
    int h= fontMetrics.getHeight();
    setScrollbar(vScroll, (topLineN - 1)*h, visSize.height, 0, 
		 (nLines + 1)*h);
    vScroll.setLineIncrement(fontMetrics.getHeight());
    vScroll.setPageIncrement(
       visSize.height > SLOP ? visSize.height - SLOP : visSize.height);
    prgUpdate= false;
  }

  void addLine(String text) {
    lines.addElement(new TextLine(text));
    prgUpdate= true;
  }

  int nLines() {
    return lines.size();
  } 

  void markLine(int lineN, Color c) {
    if (0 < lineN && lineN <= lines.size()) {
      markLine(lineN, 0, 
	       ((TextLine)(lines.elementAt(lineN - 1))).text.length(), c);
    }
  }

  void markLine(int lineN, int lo, int hi, Color c) {
    if (0 < lineN && lineN <= lines.size()) {
      ((TextLine)lines.elementAt(lineN - 1)).addTextMark(lo, hi, c);
    }
  }

  void unmarkLine(int lineN) {
    if (0 < lineN && lineN <= lines.size()) {
      ((TextLine)(lines.elementAt(lineN - 1))).marks= null;
    }
  }

  public void select(int x, int y) {
    selectLine(y);
  }

  public void select(Object xSpec, Object ySpec) {
    selectLine(ySpec == null ? -1 : (((Integer)ySpec).intValue()));
  }

  String getText(int lineN) {
    return ((TextLine)(lines.elementAt(lineN - 1))).text;
  }

  void selectLine(int lineN) {
    FontMetrics fontMetrics= getFontMetrics(getFont());
    unmarkLine(selectedLineN); 
    if (0 < lineN && lineN <= lines.size()) {
      markLine(lineN, Color.red);
      int nLines2= ((size().height)/(fontMetrics.getHeight())) / 2;
      topLineN= (lineN - nLines2 < 1) ? 1 : lineN - nLines2;
    }
    selectedLineN= lineN;
    prgUpdate= true; repaint();
  }

  public void reshape(int x, int y, int w, int h) {
    prgUpdate= true;
    super.reshape(x, y, w, h);
  }

  public void paint(Graphics g) {
    if (prgUpdate) {
      updateScrollbars(); 
    }
    FontMetrics m= g.getFontMetrics(g.getFont());
    int fontHeight= m.getHeight();
    Dimension vis= size();
    int nLines= vis.height/fontHeight;	//# of visible lines.
    int lineLo= vScroll.getValue() / fontHeight;
    int lineHi= 
      (lineLo + nLines < lines.size()) ? lineLo + nLines : lines.size();
    g.translate(-hScroll.getValue(), -vScroll.getValue());
    for (int i= lineLo; i < lineHi; i++) {
      int y= i * fontHeight + m.getAscent();
      TextLine line= (TextLine)lines.elementAt(i);
      String text= line.text;
      //Saving the color from the graphics context, and then restoring it
      //doesn't appear to work under Netscape on Linux!
      //Color saveColor= g.getColor();
      for (TextMark marks= line.marks; marks != null; marks= marks.succ) {
	g.setColor(marks.color);
	g.fillRect(marks.lo > 0 
		   ? m.stringWidth(text.substring(0, marks.lo - 1)) 
		   : 0,
		   y - m.getAscent(),
		   m.stringWidth(text.substring(marks.lo, marks.hi)),
		   m.getAscent() + m.getDescent()
	);
      }
      //g.setColor(saveColor);
      g.setColor(Color.black);
      g.drawString(text, 0, y);
    }
  }

  private Vector lines;
  private int selectedLineN;	//Currently selected line # (1-origin).
  private int topLineN= 1;      //currently displayed top line # (1-origin).
                                //valid only if prgUpdate.
  private boolean prgUpdate;

}



