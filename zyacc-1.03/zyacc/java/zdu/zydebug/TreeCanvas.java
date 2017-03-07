/*

File:	 TreeCanvas.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/07/28 01:13:24 zdu"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.zydebug;

import zdu.zydebug.OffsetForest;
import zdu.zydebug.ScrollableCanvas;

import java.applet.*;
import java.awt.*;
import java.io.*;
import java.util.Vector;

class TreeCanvas extends ScrollableCanvas implements Selectable {

  TreeCanvas(int width, int height, String mouseHint) {
    super(width, height, mouseHint == null ? null : "");
    hintMsg= mouseHint; forest= null;
    locator= new GridLocator(0, 0, width, height, colSep, rowSep);
  }

  TreeCanvas(int width, int height) {
    this(width, height, null);
  }

  public void setFont(Font f) {
    super.setFont(f); 
    FontMetrics fontMetrics= getFontMetrics(f);
    hScroll.setLineIncrement(fontMetrics.stringWidth("M"));
    vScroll.setLineIncrement(fontMetrics.getHeight());
    repaint();
  }

  public void setRowSep(int sep) {
    Dimension s= size();
    rowSep= sep;
    locator= new GridLocator(0, 0, s.width, s.height, colSep, rowSep);
  }

  public void setColSep(int sep) {
    Dimension s= size();
    colSep= sep < MIN_NODE_WIDTH + SLOP ? MIN_NODE_WIDTH + SLOP : sep;
    locator= new GridLocator(0, 0, s.width, s.height, colSep, rowSep);
  }

  public void setShadows(boolean doShadow) {
    this.doShadow= doShadow;
  }

  public void resize(int x, int y, int w, int h) {
    Rectangle rect= locator.getBounds();
    if (w > rect.width || h > rect.height) {
      locator= new GridLocator(0, 0, w, h, colSep, rowSep);
    }
  }

  public void reset() {
    rowLo= colLo= 0;
    if (forest != null) forest.removeAllElements();
  }

  public void select(Object xSpec, Object ySpec) {
    selectedNode= (OffsetTree)xSpec;
    didSelect= true; 
  }

  public void paint(Graphics g) {
    Dimension s= size();
    if (forest != null && forest.size() > 0) {
      locator.locatorReset();
      int n= forest.size();
      int wLeft= forest.leftWidth();
      int wRight= forest.rightWidth();
      int h= forest.height();
      int x= hScroll.getValue();
      int y= vScroll.getValue();
      if (didSelect) {
	x= wLeft;
	y= 0;
	for (OffsetTree t= selectedNode; t != null; 
	     t= (OffsetTree)t.parent()) {
	  y++; x+= t.getOffset();
	}
	x= x > 1 ? (x - 1)*colSep : 0; 
	y= y > 2 ? (y - 2)*rowSep : 0;
	didSelect= false;
      }
      updateScrollbars(x, y, 
		       (wLeft + wRight + 1) * colSep, (h + 1) * rowSep);
      g.translate(-hScroll.getValue(), -vScroll.getValue());
      int minX= hScroll.getValue();
      int minY= vScroll.getValue();
      int maxX= minX + s.width;
      int maxY= minY + s.height;
      for (int i= 0; i < n; i++) {
	OffsetTree tree= forest.elementAt(i);
	drawTree(g, tree, (wLeft + tree.getOffset())*colSep + colSep/2, 
		 10, minX, minY, maxX, maxY);
      }
    }
    else {
      updateScrollbars(0, 0, s.width, s.height);
    }
  }

  public boolean mouseExit(Event event, int x, int y) {
    if (mouseNode != null) {
      mouseNode= null; isMouseDown= false;
      repaint();
    }
    return super.mouseExit(event, x, y);    
  }

  public boolean mouseDown(Event evt, int x, int y) {
    BoundedObj boundedObj= locator.locate(x, y);
    isMouseDown= true;
    if (boundedObj != null) {
      OffsetTree t= (OffsetTree)(boundedObj.getObject());
      mouseDownNode= mouseNode= t;
      repaint();
    }
    else {
      mouseDownNode= mouseNode= null;
    }
    return super.mouseDown(evt, x, y);
  }

  public boolean mouseDrag(Event evt, int x, int y) {
    BoundedObj boundedObj= locator.locate(x, y);
    isMouseDown= true;
    if (boundedObj == null) {
      if (mouseNode != null) {
	isMouseDown= false; mouseNode= null;
	repaint();
      }
    }
    else {
      OffsetTree t= (OffsetTree)(boundedObj.getObject());
      if (t != mouseNode) {
	mouseNode= t; repaint();
      }
    }
    return super.mouseDrag(evt, x, y);
  }

  public boolean mouseMove(Event evt, int x, int y) {
    BoundedObj boundedObj= locator.locate(x, y);
    if (boundedObj == null) {
      if (mouseNode != null) {
	mouseNode= null;
	repaint();
      }
    }
    else {
      OffsetTree t= (OffsetTree)(boundedObj.getObject());
      if (t != mouseNode) {
	mouseNode= t; repaint();
      }
    }
    return super.mouseMove(evt, x, y);
  }

  public boolean mouseUp(Event  evt, int  x, int  y) {
    BoundedObj boundedObj= locator.locate(x, y);
    if (boundedObj == null) {
      if (mouseNode != null) {
	mouseNode= null; 
	repaint();
      }
    }
    else {
      OffsetTree t= (OffsetTree)(boundedObj.getObject());
      mouseNode= t;
      if (t == mouseDownNode) {
	mouseNode.setExpand(!mouseNode.getExpand());
      }
      repaint();
    }
    isMouseDown= false;  mouseDownNode= null;   
    return super.mouseUp(evt, x, y);
  }

  void setForest(OffsetForest forest) {
    this.forest= forest;
  }

  protected void updateScrollbars(int x, int y, int width, int height) {
    Dimension s= size();
    int xMax= s.width > width ? s.width : width;
    int w1= width - s.width + SLOP;
    int x1= (x <= width) ? x : width;
    setScrollbar(hScroll, x1, s.width, 0, xMax + SLOP);
    int yMax= s.height > height ? s.height : height;
    int y1= (y <= height) ? y : height;
    setScrollbar(vScroll, y1, s.height, 0, yMax + SLOP);
    FontMetrics fontMetrics= getFontMetrics(getFont());
    hScroll.setLineIncrement(fontMetrics.stringWidth("M"));
    hScroll.setPageIncrement(colSep); 
    vScroll.setLineIncrement(fontMetrics.getHeight());
    vScroll.setPageIncrement(rowSep); 
  }

  Image getHintImage() {
    FontMetrics m= getFontMetrics(getFont());
    Dimension s= size();
    if (hintMsg != null &&
	(hintBuf == null || hintBufDim.width != s.width || 
	 hintBufDim.height != m.getHeight() + 2*HINT_Y_INSET)) {
      int h= m.getHeight();
      hintBufDim= new Dimension(s.width, h + 2*HINT_Y_INSET);
      hintBuf= createImage(hintBufDim.width, hintBufDim.height);
      Graphics g= hintBuf.getGraphics();
      int xMsg= SLOP;
      int xTerm= xMsg + m.stringWidth(hintMsg) + HINT_SPACE;
      int xNonTerm= xTerm + m.stringWidth(TERM_HINT) + HINT_SPACE;
      int xError= xNonTerm + m.stringWidth(NON_TERM_HINT) + HINT_SPACE;
      g.setColor(HINT_BG_COLOR);
      g.fillRect(0, 0, s.width, h + 2*HINT_Y_INSET);
      g.setColor(TERM_COLOR); 
      g.fillRect(xTerm, HINT_Y_INSET, m.stringWidth(TERM_HINT), h);
      g.setColor(NON_TERM_COLOR);
      g.fillRect(xNonTerm, HINT_Y_INSET, m.stringWidth(NON_TERM_HINT), h);
      g.setColor(ERROR_COLOR);
      g.fillRect(xError, HINT_Y_INSET, m.stringWidth(ERROR_HINT), h);
      g.setColor(HINT_FG_COLOR);
      int a= m.getAscent();
      g.drawString(hintMsg, xMsg, HINT_Y_INSET + a);
      g.drawString(TERM_HINT, xTerm, HINT_Y_INSET + a); 
      g.drawString(NON_TERM_HINT, xNonTerm, HINT_Y_INSET + a);
      g.drawString(ERROR_HINT, xError, HINT_Y_INSET + a);
    }
    return hintBuf;
  }

  private void drawNode(Graphics g, OffsetTree t, int x, int y) {
    ParseNode n= (ParseNode)t.getInfo();
    FontMetrics m= g.getFontMetrics(g.getFont());
    int h= m.getHeight();
    Color c= nodeColor(n);
    String lab= n.getLabel();
    boolean isSelected= (t == selectedNode);
    int w= m.stringWidth(lab);
    int a= m.getAscent();
    int w1= (w < MIN_NODE_WIDTH) ? MIN_NODE_WIDTH : w;
    int xR= x - (int)((w1/2)*EXTRA_WIDTH);
    int yR= y;
    int wR= (int)(w1*EXTRA_WIDTH);
    int hR= (int)(h*EXTRA_HEIGHT);
    if (doShadow) {
      g.setColor(RECT_SHADOW_COLOR);
      g.fillRect(xR + (xR - SOURCE_X)*NODE_Z/(SOURCE_Z - NODE_Z), 
		 yR + (yR - SOURCE_Y)*NODE_Z/(SOURCE_Z - NODE_Z), 
		 wR, hR);
    }
    if (isSelected) {
      g.setColor(SELECT_COLOR);
      g.fillRect(xR - SELECT_WIDTH, yR - SELECT_WIDTH,
		 wR + 2*SELECT_WIDTH, hR + 2*SELECT_WIDTH);
    }
    if (!t.getExpand()) {
      g.setColor(UNEXPAND_COLOR);
      g.fillRect(xR - SELECT_WIDTH, yR - SELECT_WIDTH,
		 wR + 2*SELECT_WIDTH, hR + 2*SELECT_WIDTH);
    }
    g.setColor(c);
    g.fillRect(xR, yR, wR, hR);
    if (t.nKids() > 0) {
      int xR1= xR - hScroll.getValue();
      int yR1= yR - vScroll.getValue();
      locator.locatorRegister(new Rectangle(xR1, yR1, wR, hR), t);
    }
    g.setColor(TEXT_COLOR);
    g.drawString(lab, x - w/2, y + (int)(a * EXTRA_HEIGHT));
  }

  private void drawTree(Graphics g, OffsetTree t, int x, int y, 
		       int minX, int minY, int maxX, int maxY) {
    if (y > maxY) return;
    FontMetrics m= g.getFontMetrics(g.getFont());
    int h= m.getHeight();
    if (x + colSep/2 >= minX && y + rowSep >= minY && x - colSep/2 <= maxX) {
      drawNode(g, t, x, y);
    }
    if (t.getExpand()) {
      for (int i= 0; i < t.nKids(); i++) {
	OffsetTree kid= (OffsetTree)t.kid(i);
	ParseNode kidNode= (ParseNode)kid.getInfo();
	Color cKid= edgeColor(kidNode);
	int eX0= x;
	int eY0= y + (int)(h*EXTRA_HEIGHT);
	int eX1= x + kid.getOffset()*colSep;
	int eY1= y + rowSep;
	if (doShadow) {
	  int eX0Offset= (eX0 - SOURCE_X)*EDGE_Z/(SOURCE_Z - EDGE_Z);
	  int eY0Offset= (eY0 - SOURCE_Y)*EDGE_Z/(SOURCE_Z - EDGE_Z);
	  int eX1Offset= (eX1 - SOURCE_X)*EDGE_Z/(SOURCE_Z - EDGE_Z);
	  int eY1Offset= (eY1 - SOURCE_Y)*EDGE_Z/(SOURCE_Z - EDGE_Z);
	  g.setColor(LINE_SHADOW_COLOR);
	  g.drawLine(eX0 + eX0Offset, eY0 + eY0Offset, 
		     eX1 + eX1Offset, eY1 + eY1Offset);
	}
	g.setColor(cKid);
	g.drawLine(eX0, eY0, eX1, eY1);
	g.drawLine(eX0 + 1, eY0, eX1 + 1, eY1);
	drawTree(g, kid, x + kid.getOffset()*colSep, y + rowSep, 
		 minX, minY, maxX, maxY);
      }
    } //if (t.getExpand())
  }

  private Color nodeColor(ParseNode n) {
    int nodeType= n.getType();
    ParseNode mNode=  
      (mouseNode == null) ? null : (ParseNode)mouseNode.getInfo();
    ParseNode lastMNode= 
      (mouseDownNode == null) ? null : (ParseNode)mouseDownNode.getInfo();
    return (mNode == n) 
           ? ((isMouseDown && n == lastMNode)
	      ? MOUSE_DOWN_COLOR 
	      : MOUSE_IN_COLOR) 
           : (nodeType == ParseNode.ERROR) ? ERROR_COLOR
           : (nodeType == ParseNode.TERM) ? TERM_COLOR
           : NON_TERM_COLOR;;
  }

  private Color edgeColor(ParseNode n) {
    int nodeType= n.getType();
    return   (nodeType == ParseNode.ERROR) ? ERROR_COLOR
           : (nodeType == ParseNode.TERM) ? TERM_COLOR
           : NON_TERM_COLOR;;
  }

  private Dimension minSize= new Dimension(200, 80);
  private Dimension prefSize= new Dimension(400, 1600);
  private GridLocator locator;
  private Rectangle mouseRect;
  private OffsetTree mouseNode;
  private OffsetTree mouseDownNode;
  private int rowLo= 0;	
  private int colLo= 0;
  private OffsetForest forest;
  private OffsetTree selectedNode;
  private String hintMsg;		//hint msg. to output on top of canvas.
  private Image hintBuf;		//buffer for hint message.
  private Dimension hintBufDim;		//dimensions of allocated hint buf.
  private boolean didSelect;
  private boolean isMouseDown;
  private boolean doShadow= true;
  private static final String TERM_HINT= "terminal";
  private static final String NON_TERM_HINT= "non-terminal";
  private static final String ERROR_HINT= "error";
  private static final Color HINT_BG_COLOR= Color.yellow;
  private static final Color HINT_FG_COLOR= Color.black;
  private static final int HINT_SPACE= 20;
  private static final int HINT_Y_INSET= 3;
  private static final int FONTSIZE= 14;
  private static final int MIN_NODE_WIDTH= 20;
  private static final int TREE_SEP= 0;
  private static final double EXTRA_WIDTH= 1.2;
  private static final double EXTRA_HEIGHT= 1.2;
  private static final Color TERM_COLOR= Color.red;
  private static final Color NON_TERM_COLOR= Color.green;
  private static final Color ERROR_COLOR= Color.pink;
  private static final Color MOUSE_IN_COLOR= Color.lightGray;
  private static final Color MOUSE_DOWN_COLOR= Color.gray;
  private static final Color SELECT_COLOR= Color.yellow;
  private static final Color UNEXPAND_COLOR= Color.black;
  private static final int SELECT_WIDTH= 3;
  private static final Color TEXT_COLOR= Color.black;

  //Right-hand axes with Z increasing towards viewer.  Canvas is at Z= 0.  
  //Ensure light source is real far away, relative to object
  //distances so that all parts of the object have similar sized shadows.
  //Coordinates of light source.
  private static final int SOURCE_X= -10000;
  private static final int SOURCE_Y= -10000; //must be < 0 to overlay shadows.
  private static final int SOURCE_Z= 10000;
  private static final int NODE_Z= 8;        //Nodes at plane z= NODE_Z
  private static final int EDGE_Z= 7;        //Edges at plane z= EDGE_Z

  private static final int RECT_GRAY= 220;
  private static final int LINE_GRAY= 200;
  private static final Color RECT_SHADOW_COLOR= 
    new Color(RECT_GRAY, RECT_GRAY, RECT_GRAY);
  private static final Color LINE_SHADOW_COLOR= 
    new Color(LINE_GRAY, LINE_GRAY, LINE_GRAY);

  private int rowSep= 50;
  private int colSep= 100;

  /* Simple test program, basically tests addKid() and constructors. */
  static public void main(String args[]) {
    OffsetForest forest= new OffsetForest();

    OffsetTree t000= new OffsetTree(new ParseNode("A", 1));
    OffsetTree t001= new OffsetTree(new ParseNode("B", 1));
    OffsetTree t002= new OffsetTree(new ParseNode("C", 1));
    OffsetTree t00= new OffsetTree(new ParseNode("D", 1), t000, t001, t002);
    t00.addKid(new OffsetTree(new ParseNode("E", 1)));
    t00.addKid(new OffsetTree(new ParseNode("F", 1), 
			      new OffsetTree(new ParseNode("G", 1))), 1);
    OffsetTree OffsetTrees[]= { t00, 
		    new OffsetTree(new ParseNode("H", 1), 
				   new OffsetTree(new ParseNode("I", 1)), 
				   new OffsetTree(new ParseNode("J", 1))), 
		    new OffsetTree(new ParseNode("K", 1)), 
		    new OffsetTree(new ParseNode("L", 1))
                  };
    OffsetTree t0= new OffsetTree(new ParseNode("M", 1), OffsetTrees);
    forest.addElement(t0);

    OffsetTree prg= new OffsetTree(new ParseNode("program", 1));
    OffsetTree stmts= new OffsetTree(new ParseNode("stmts", 1));
    prg.addKid(stmts);
    OffsetTree assgnStmt= new OffsetTree(new ParseNode("assgn", 1));
    stmts.addKid(assgnStmt);
    assgnStmt.addKid(new OffsetTree(new ParseNode("a", 1)));
    assgnStmt.addKid(new OffsetTree(new ParseNode(":=", 1)));
    OffsetTree expr= new OffsetTree(new ParseNode("expr", 1));
    assgnStmt.addKid(expr);
    OffsetTree term= new OffsetTree(new ParseNode("term", 1));
    expr.addKid(term);
    OffsetTree factor= new OffsetTree(new ParseNode("factor", 1));
    term.addKid(factor);
    factor.addKid(new OffsetTree(new ParseNode("1", 1)));
    OffsetTree stmtsRest= new OffsetTree(new ParseNode("stmtsRest", 1));
    stmts.addKid(stmtsRest);
    stmtsRest.addKid(new OffsetTree(new ParseNode(";", 1)));
    forest.addElement(prg);

    OffsetTree tStmts= new OffsetTree(new ParseNode("stmts", 0));
    OffsetTree tAssgn= new OffsetTree(new ParseNode("assgn", 0));
    tStmts.addKid(tAssgn);
    tAssgn.addKid(new OffsetTree(new ParseNode("a", 0))); 
    tAssgn.addKid(new OffsetTree(new ParseNode(":=", 0)));
    OffsetTree tRestStmts= new OffsetTree(new ParseNode("restStmts", 0));
    tStmts.addKid(tRestStmts);
    tRestStmts.addKid(new OffsetTree(new ParseNode(";", 0)));
    forest.addElement(tStmts); 

    OffsetTree x= 
      new OffsetTree(new ParseNode("stmts", 0),
        new OffsetTree(new ParseNode("assgnStmt", 0), 
	  new OffsetTree(new ParseNode("a", 0)),
          new OffsetTree(new ParseNode(":=", 0)),
          new OffsetTree(new ParseNode("expr", 0),
            new OffsetTree(new ParseNode("term", 0),
              new OffsetTree(new ParseNode("factor", 0),
                new OffsetTree(new ParseNode("1", 0)))))),
        new OffsetTree(new ParseNode(";", 0)),
        new OffsetTree(new ParseNode("stmts", 0),
          new OffsetTree(new ParseNode("assgnStmt", 0),
            new OffsetTree(new ParseNode("b", 0)),
            new OffsetTree(new ParseNode(":=", 0)),
	    new OffsetTree(new ParseNode("expr", 0),
              new OffsetTree(new ParseNode("expr", 0),
                new OffsetTree(new ParseNode("term", 0),
                  new OffsetTree(new ParseNode("factor", 0),
                    new OffsetTree(new ParseNode("1", 0))))),
              new OffsetTree(new ParseNode("+", 0)),
              new OffsetTree(new ParseNode("term", 0),
                new OffsetTree(new ParseNode("factor", 0),
                  new OffsetTree(new ParseNode("2", 0))))))));
    forest.addElement(x);

    TreeCanvas treeCanvas= new TreeCanvas(200, 200, "Test tree");
    treeCanvas.setForest(forest);
    Frame frame= new Frame("TreeCanvas Test");
    frame.setLayout(new FlowLayout());
    frame.add(treeCanvas.getComponent());
    frame.resize(400, 400);
    frame.show();
    treeCanvas.repaint();

  }

    
}



