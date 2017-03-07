/*

File:	 TraceCanvas.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/07/27 20:15:36 zdu"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.zydebug;

import zdu.zydebug.ScrollableCanvas;
import zdu.zydebug.Trace;

import java.applet.*;
import java.awt.*;
import java.io.*;
import java.util.Vector;

/** This canvas displays successive trace lines.  Each trace line consists 
 * of a stack dump followed by a dump of the remaining input.  The canvas
 * tries to center the boundary between the stack and input in the canvas. 
 * BUG: breaks if tabs are displayed, since the FontMetrics class seems
 * to return a wrong width for tabs.
 */
class TraceCanvas extends ScrollableCanvas  {

  TraceCanvas(int width, int height) {
    super(width, height, "");	//non-null hint message to enable hints.
    FontMetrics m= getFontMetrics(getFont());
    stkWidth= inputWidth= actWidth= 0;
    reset();
  }

  TraceCanvas() {
    this(300, 180);
  }

  public void setFont(Font f) {
    super.setFont(f); updateFlags|= (SCROLL_UPDATE|PARAM_UPDATE);
  }

  public void reset() {
    lines= new Vector();
    stkWidth= inputWidth= 0;
    updateFlags|= (SCROLL_UPDATE|PARAM_UPDATE);
  }

  //Recompute max. stkWidth and max. inputWidth by iterating over all lines.
  private void updateParams(FontMetrics m) {
    int nLines= lines.size();
    stkWidth= inputWidth= actWidth= 0;
    for (int i= 0; i < nLines; i++) {
      Trace trace= (Trace)(lines.elementAt(i));
      int sWidth= m.stringWidth(trace.stk);
      int iWidth= m.stringWidth(trace.input);
      int aWidth= m.stringWidth(trace.act);
      if (sWidth > stkWidth) stkWidth= sWidth;
      if (iWidth > inputWidth) inputWidth= iWidth;
      if (aWidth > actWidth) actWidth= aWidth;
    }
  }

  //Update scrollbars to display last trace at bottom, centered between 
  //stk and input traces.
  protected void updateScrollbars(FontMetrics m) {
    Dimension s= size();
    int nLines= lines.size();
    setScrollbar(hScroll, hScroll.getValue(), s.width, 0, 
		 stkWidth + inputWidth + actWidth + SLOP);
    hScroll.setLineIncrement(m.stringWidth("M")); 
    hScroll.setPageIncrement(
      s.width > SLOP ? s.width - SLOP : s.width);
    int x= (stkWidth > s.width/2) ? stkWidth - s.width/2 : 0;
    hScroll.setValue(x);
    setScrollbar(vScroll, vScroll.getValue(), s.height, 0,
		 (nLines + 1)*m.getHeight());
    vScroll.setLineIncrement(m.getHeight());
    vScroll.setPageIncrement(
      s.height > m.getHeight() ? s.height - m.getHeight() : s.height);
    int y= (lines.size() + 1)*m.getHeight() - s.height;
    vScroll.setValue(y < 0 ? 0 : y);
  }

  //Add a trace line to the canvas, incrementally updating the stkWidth
  //and inputWidth parameters, and set a flag to update scrollbars.
  void addTrace(String stk, String input, String act) {
    String input1= new String(input + " ");
    lines.addElement(new Trace(stk, input1, act));
    int nLines= lines.size();
    FontMetrics m= getFontMetrics(getFont());
    int sWidth= m.stringWidth(stk);
    int iWidth= m.stringWidth(input1);    
    int aWidth= m.stringWidth(act);    
    if (sWidth > stkWidth) stkWidth= sWidth;
    if (iWidth > inputWidth) inputWidth= iWidth;
    if (aWidth > actWidth) actWidth= aWidth;
    int maxH= (lines.size() + 1)*m.getHeight();
    updateFlags|= SCROLL_UPDATE;
  }

  //Return # of trace-lines.
  int nLines() {
    return lines.size();
  } 

  public void reshape(int x, int y, int w, int h) {
    updateFlags|= SCROLL_UPDATE;
    super.reshape(x, y, w, h);
  }

  //Paint the canvas in the position dictated by the scrollbar values
  //(after updating them if requested via updateFlags).
  public void paint(Graphics g) {
    FontMetrics m= g.getFontMetrics(g.getFont());
    if ((updateFlags & PARAM_UPDATE) != 0) {
      updateParams(m); updateFlags&= ~PARAM_UPDATE;
    }
    if ((updateFlags & SCROLL_UPDATE) != 0) {
      updateScrollbars(m); updateFlags&= ~SCROLL_UPDATE;
    }
    int fontHeight= m.getHeight();
    Dimension s= size();
    int nLines= s.height/fontHeight;		//# of visible lines.
    int lineLo= vScroll.getValue() / fontHeight;
    int lineHi= 
      (lineLo + nLines < lines.size()) ? lineLo + nLines : lines.size();
    g.translate(-hScroll.getValue(), -vScroll.getValue());
    Color saveColor= g.getColor();
    for (int i= lineLo; i < lineHi; i++) {
      int y= i * fontHeight + m.getAscent();
      Trace trace= (Trace)lines.elementAt(i);
      String stk= trace.stk;
      String input= trace.input;
      String act= trace.act;
      g.setColor(STK_COLOR);
      g.drawString(stk, stkWidth - m.stringWidth(stk), y);
      g.setColor(INPUT_COLOR);
      g.drawString(input, stkWidth, y);
      g.setColor(ACT_COLOR);
      g.drawString(act, stkWidth + inputWidth, y);
    }
    g.setColor(saveColor);
  }

  Image getHintImage() {
    FontMetrics m= getFontMetrics(getFont());
    Dimension s= size();
    if (hintBuf == null || hintBufDim.width != s.width || 
        hintBufDim.height != m.getHeight()) {
      hintBufDim= new Dimension(s.width, m.getHeight());
      hintBuf= createImage(hintBufDim.width, hintBufDim.height);
      Graphics g= hintBuf.getGraphics();
      g.setColor(HINT_BG_COLOR);
      g.fillRect(0, 0, s.width, m.getHeight());
      g.setColor(STK_COLOR);
      int y= m.getAscent();
      g.drawString(STK_HINT, SLOP, y);
      g.setColor(INPUT_COLOR);
      int wStk= m.stringWidth(STK_HINT);
      g.drawString(INPUT_HINT, 2*SLOP + wStk, y);
      g.setColor(ACT_COLOR);
      g.drawString(ACT_HINT, 3*SLOP + wStk + m.stringWidth(INPUT_HINT), y);
    }
    return hintBuf;
  }

  private int updateFlags;		//nonzero if update needed.
  private Vector lines;			//trace lines.
  private int stkWidth;			//max. stk width (in pixels).
  private int inputWidth;            	//max. input width (in pixels).
  private int actWidth;            	//max. act width (in pixels).
  private Image hintBuf;		//buffer for hint message.
  private Dimension hintBufDim;		//dimensions of allocated hint buf.
					    

  //update flags.					    
  static final private int SCROLL_UPDATE= 0x1;
  static final private int PARAM_UPDATE= 0x2;

  //fixed colors used for display.
  static final private Color STK_COLOR= Color.gray;
  static final private Color INPUT_COLOR= Color.red;
  static final private Color ACT_COLOR= Color.blue;

  //String used for hints.
  static final private String STK_HINT= "Stack";
  static final private String INPUT_HINT= "Lookahead";
  static final private String ACT_HINT= "Action";
  static final private String ALT_INPUT_HINT= "Input";
  private static final Color HINT_BG_COLOR= Color.yellow;

  /** Test program which displays the contents of this file, with the
   * trace consisting of the first 30 characters of each line, the
   * input consisting of the rest of the line.
   */
  static public void main(String[] args) {
    InputStream i= System.in;
    String fName=  "zdu/zydebug/TraceCanvas.java";
    try {
      i= new FileInputStream(fName);
    }
    catch (FileNotFoundException e) {
      System.err.println("TraceCanvas.main() " + e + " " + fName);
      System.exit(1);
    }
    catch (SecurityException e) {
      throw new InternalError("FileCanvas.main() " + e);
    }
    DataInputStream d= new DataInputStream(i);
    TraceCanvas traceCanvas= new TraceCanvas(200, 200);
    try {
      String line;
      while ((line= d.readLine()) != null) {
	int TRACE_LEN= 30;
	line= line.replace('\t', ' ');
	if (line.length() <= TRACE_LEN) {
	  traceCanvas.addTrace(line, "", "ACTION");
	}
	else {
	  traceCanvas.addTrace(line.substring(0, TRACE_LEN), 
			       line.substring(TRACE_LEN), "ACTION");
	}
      }
    }
    catch (IOException e) {
      System.err.println("IO Exception");
      System.exit(1);
    }

    Frame frame= new Frame("TraceCanvas Test");
    frame.setLayout(new FlowLayout());
    frame.add(traceCanvas.getComponent());
    traceCanvas.setFont(new Font("Helvetica", 0, 18));
    frame.resize(400, 400);
    frame.show();

  }
}



