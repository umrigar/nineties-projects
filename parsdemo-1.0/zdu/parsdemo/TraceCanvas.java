/*

File:	 TraceCanvas.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:41:00 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.ScrollableCanvas;
import zdu.parsdemo.Trace;

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
    stkWidth= inputWidth= 0;
    for (int i= 0; i < nLines; i++) {
      Trace trace= (Trace)(lines.elementAt(i));
      int sWidth= m.stringWidth(trace.stk);
      int iWidth= m.stringWidth(trace.input);
      if (sWidth > stkWidth) stkWidth= sWidth;
      if (iWidth > inputWidth) inputWidth= iWidth;
    }
  }

  //Update scrollbars to display last trace at bottom, centered between 
  //stk and input traces.
  private void updateScrollbars(FontMetrics m) {
    Dimension s= size();
    int nLines= lines.size();
    hScroll.setValues(hScroll.getValue(), s.width, 0, 
		      stkWidth + inputWidth - s.width + SLOP);
    hScroll.setLineIncrement(m.stringWidth("M"));
    hScroll.setPageIncrement(s.width - SLOP);
    int x= (stkWidth > s.width/2) ? stkWidth - s.width/2 : 0;
    hScroll.setValue(x);
    vScroll.setValues(vScroll.getValue(), s.height, 0,
		      (nLines + 1)*m.getHeight() - s.height);
    vScroll.setLineIncrement(m.getHeight());
    vScroll.setPageIncrement(s.height - m.getHeight());
    int y= (lines.size() + 1)*m.getHeight() - s.height;
    vScroll.setValue(y < 0 ? 0 : y);
  }

  //Add a trace line to the canvas, incrementally updating the stkWidth
  //and inputWidth parameters, and set a flag to update scrollbars.
  void addTrace(String stk, String input) {
    lines.addElement(new Trace(stk, input));
    int nLines= lines.size();
    FontMetrics m= getFontMetrics(getFont());
    int sWidth= m.stringWidth(stk);
    int iWidth= m.stringWidth(input);
    if (sWidth > stkWidth) stkWidth= sWidth;
    if (iWidth > inputWidth) inputWidth= iWidth;
    int maxH= (lines.size() + 1)*m.getHeight();
    updateFlags|= SCROLL_UPDATE;
  }

  //Return # of trace-lines.
  int nLines() {
    return lines.size();
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
    g.translate(-hScroll.getValue(), -lineLo*fontHeight);
    Color saveColor= g.getColor();
    for (int i= lineLo; i < lineHi; i++) {
      int y= i * fontHeight + m.getAscent();
      Trace trace= (Trace)lines.elementAt(i);
      String stk= trace.stk;
      String input= trace.input;
      g.setColor(STK_COLOR);
      g.drawString(stk, stkWidth - m.stringWidth(stk), y);
      g.setColor(INPUT_COLOR);
      g.drawString(input, stkWidth, y);
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
      int x= s.width - m.stringWidth(INPUT_HINT) - SLOP;
      if (SLOP + m.stringWidth(STK_HINT) > x) {
	g.drawString(ALT_INPUT_HINT, 
		     s.width - m.stringWidth(ALT_INPUT_HINT) - SLOP, y);
      }	
      else {
	g.drawString(INPUT_HINT, x, y);
      }
    }
    return hintBuf;
  }

  private int updateFlags;		//nonzero if update needed.
  private Vector lines;			//trace lines.
  private int stkWidth;			//max. stk width (in pixels).
  private int inputWidth;            	//max. input width (in pixels).
  private Image hintBuf;		//buffer for hint message.
  private Dimension hintBufDim;		//dimensions of allocated hint buf.
					    

  //update flags.					    
  static final private int SCROLL_UPDATE= 0x1;
  static final private int PARAM_UPDATE= 0x2;

  //fixed colors used for display.
  static final private Color STK_COLOR= Color.green;
  static final private Color INPUT_COLOR= Color.black;

  //String used for hints.
  static final private String STK_HINT= "Stack";
  static final private String INPUT_HINT= "Rest Input";
  static final private String ALT_INPUT_HINT= "Input";
  private static final Color HINT_BG_COLOR= Color.yellow;

  /** Test program which displays the contents of this file, with the
   * trace consisting of the first 30 characters of each line, the
   * input consisting of the rest of the line.
   */
  static public void main(String[] args) {
    InputStream i= System.in;
    String fName=  "zdu/parsdemo/TraceCanvas.java";
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
	  traceCanvas.addTrace(line, "");
	}
	else {
	  traceCanvas.addTrace(line.substring(0, TRACE_LEN), 
			       line.substring(TRACE_LEN));
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



