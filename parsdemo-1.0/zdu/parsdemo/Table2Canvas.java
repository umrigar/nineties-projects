/*

File:	 Table2Canvas.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:40:53 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.ScrollableCanvas;
import zdu.parsdemo.Selectable;
import zdu.parsdemo.SparseTable2;
import zdu.parsdemo.Table2;

import java.awt.*;
import java.util.*;

class Table2Canvas extends ScrollableCanvas implements Selectable {

  /** Constructor.
   * @param tab		2-dimensional table to be displayed.
   * @param width	width of canvas in pixels.
   * @param height	height of canvas in pixels.
   */
  Table2Canvas(Table2 tab, int width, int height, String mouseHint) {
    super(width, height, mouseHint);
    table= tab;
    rowStarts= new int[tab.nRows() + 1];
    colStarts= new int[tab.nCols() + 1];
    updateParams();
  }

  Table2Canvas(Table2 tab, int width, int height) {
    this(tab, width, height, null);
  }

  /** Set font of canvas to f.
   * @param font	Font to be used in canvas.
   */
  public void setFont(Font font) {
    super.setFont(font); updateParams(); select();
  }

  public void reset() {
  }

  /** Select a row/col.  If row/col # is < 0, then it is deselected.
   * Selected rows/columns are highlighted.  If both a row and column
   * are selected, then the entry at the intersection is highlighted
   * further.
   * @param colN	# of col (0-origin) to be selected (< 0 to deselect).
   * @param rowN	# of row (0-origin) to be selected (< 0 to deselect).
   */
  public void select(int colN, int rowN) {
    selectedRowN= rowN; selectedColN= colN;
    select();
  }

  /** Select a row/col.  If row/col is null, then it is deselected.
   * Selected rows/columns are highlighted.  If both a row and column
   * are selected, then the entry at the intersection is highlighted
   * further.
   * @param col		col object to be selected (null to deselect).
   * @param row		row object to be selected (null to deselect).
   */
  public void select(Object col, Object row) {
    int rowN= (row == null) ? DESELECT : table.getRowIndex(row); 
    int colN= (col == null) ? DESELECT : table.getColIndex(col);
    select(colN, rowN);
  }

  void select() {
    if (selectedRowN >= 0) {
      vScroll.setValue(rowStarts[selectedRowN - ((selectedRowN > 0) ? 1 : 0)]);
    }
    if (selectedColN >= 0) {
      hScroll.setValue(colStarts[selectedColN - ((selectedColN > 0) ? 1 : 0)]);
    }
  }

  public synchronized void reshape(int x, int y, int w, int h) {
    super.reshape(x, y, w, h); updated= true;
  }

  /** Paint two dimensional table on its canvas.  Since the drawing
   * overdraws certain areas repeatedly it is best not to call this
   * directly to avoid flicker. Instead, this routine is called by
   * super.update() to draw into an offscreen image which is then
   * displayed.  
   * @param g The graphics context within which the table is displayed.  
   */
  public void paint(Graphics g) {
    Dimension s= size();
    int nRows= table.nRows();
    int nCols= table.nCols();
    if (updated) {
      updateScrollbars(hScroll.getValue(), vScroll.getValue());
      updated= false;
    }
    int x= hScroll.getValue();		//x-coord of visible table contents.
    int y= vScroll.getValue();		//y-coord of visible table contents.
    int w= s.width - headWidth;		//visible width of table contents.
    int h= s.height - headHeight;	//visible height of table contents.
    int endX= 				//end x coord of visible contents.
      (colStarts[nCols] < x + w) ? colStarts[nCols] : x + w;
    int endY= 				//end y coord of visible contents.
      (rowStarts[nRows] < y + h) ? rowStarts[nRows] : y + h;

    int startRowN, endRowN;		//indexes (both incl) of vis rows.
    int startColN, endColN;		//indexes (both incl) of vis cols.
    if (y > rowStarts[nRows]) {
      startRowN= nRows - 1;
    }
    else {
      for (startRowN= 0; rowStarts[startRowN+1] < y; startRowN++) ;
    }
    if (x > colStarts[nCols]) {
      startColN= nCols - 1;
    }
    else {
      for (startColN= 0; colStarts[startColN+1] < x; startColN++) ;
    }
    for (endRowN= startRowN; endRowN < nRows && rowStarts[endRowN + 1] < endY;
	 endRowN++) {
    }
    for (endColN= startColN; endColN < nCols && colStarts[endColN + 1] < endX;
	 endColN++) {
    }

    boolean selectedRowIsVisible= 
      (startRowN <= selectedRowN && selectedRowN <= endRowN);
    boolean selectedColIsVisible= 
      (startColN <= selectedColN && selectedColN <= endColN);

    //Fill background with contentsBGColor.
    g.setColor(contentsBGColor); g.fillRect(0, 0, s.width, s.height);
    
    //Highlight backgrounds for selected contents row/col.
    g.setColor(contentsSelBGColor);
    if (selectedRowIsVisible) {
      int y1= rowStarts[selectedRowN];
      g.fillRect(0, y1 + headHeight - y, 
		 s.width, rowStarts[selectedRowN + 1] - y1 - ROW_SEP);
    }
    if (selectedColIsVisible) {
      int x1= colStarts[selectedColN];
      g.fillRect(x1 + headWidth - x, 0, 
		 colStarts[selectedColN + 1] - x1 - COL_SEP, s.height);
    }

    //Highlight background for selected entry.
    if (selectedRowIsVisible && selectedColIsVisible) {
      int x1= colStarts[selectedColN];
      int y1= rowStarts[selectedRowN];
      g.setColor(contentsSel2BGColor);
      g.fillRect(x1 + headWidth - x, y1 + headHeight - y, 
		 colStarts[selectedColN + 1] - x1 - COL_SEP, 
		 rowStarts[selectedRowN + 1] - y1 - ROW_SEP);
    }

    //Output contents of visible portion of table.
    g.setColor(contentsFGColor);
    g.translate(-x + headWidth, -y + headHeight);
    for (int i= startRowN; i <= endRowN; i++) {
      Object row= table.getRow(i);
      for (int j= startColN; j <= endColN; j++) {
	Object col= table.getCol(j);
	Object val= table.get(row, col);
	drawBlock(val == null ? "" : val.toString(),
		  colStarts[j], rowStarts[i], g);
      }
    }
    g.translate(x - headWidth, y - headHeight); //Back to 0.

    //Output background for headings.
    g.setColor(headBGColor);
    g.fillRect(headWidth, 0, s.width, headHeight);
    g.fillRect(0, headHeight, headWidth, s.height);

    //Highlight heading backgrounds for selected row/columns.
    g.setColor(headSelBGColor);
    if (selectedRowIsVisible) {
      g.fillRect(0, rowStarts[selectedRowN] + headHeight - y, headWidth, 
		 rowStarts[selectedRowN + 1] - rowStarts[selectedRowN] - 
		 ROW_SEP);
    }
    if (selectedColIsVisible) {
      g.fillRect(colStarts[selectedColN] + headWidth - x, 0, 
		 colStarts[selectedColN + 1] - colStarts[selectedColN] - 
		 COL_SEP,
		 headHeight);
    }

    //Output headings.
    g.setColor(headFGColor);
    g.translate(0, -y + headHeight);
    for (int i= startRowN; i <= endRowN; i++) {
      drawBlock(table.getRow(i).toString(), 0, rowStarts[i], g);
    }
    g.translate(-x + headWidth, y - headHeight);
    for (int i= startColN; i <= endColN; i++) {
      drawBlock(table.getCol(i).toString(), colStarts[i], 0, g);
    }
    g.translate(x - headWidth, 0); //Back to 0.

    //Clear top-left corner of table at intersection of headings.
    g.setColor(contentsBGColor); g.fillRect(0, 0, headWidth, headHeight);
  }

  /** This routine is called when this canvas is first created or when
   * the canvas font is changed.  It computes the height and width of
   * the table headings, as well as the coordinates at which each row
   * and each column start.  The latter are computed assuming there
   * are no headings; subsequently when painting, heading dimensions
   * are considered.  Finally it sets the size of the super to the
   * size of the table without the headings, with the origin set after
   * the headings.  
   */
  private void updateParams() {
    FontMetrics m= getFontMetrics(getFont());
    int nRows= table.nRows();
    int nCols= table.nCols();

    //Compute max. height for column headings.
    headHeight= 0;
    for (int i= 0; i < nCols; i++) {
      int h= stringHeight(table.getCol(i).toString(), m);
      if (h > headHeight) headHeight= h;
    }
    headHeight+= ROW_SEP;
    headHeight= 
      (int)Math.ceil((double)(headHeight + ROW_DIV - 1)/ROW_DIV * ROW_DIV);

    //Compute max. heights for each row.
    int height= 0;
    for (int i= 0; i < nRows; i++) {
      Object row= table.getRow(i);
      rowStarts[i]= height;
      int hRow= stringHeight(row.toString(), m);
      for (int j= 0; j < nCols; j++) {
	Object col= table.getCol(j);
	Object val= table.get(row, col);
	int valHeight= 
	  (val == null) ? 0 : stringHeight(table.get(row, col).toString(), m);
	if (valHeight > hRow) hRow= valHeight;
      }
      hRow+= ROW_SEP;
      height+= (int)Math.ceil((double)(hRow + ROW_DIV - 1)/ROW_DIV * ROW_DIV);
    }
    rowStarts[nRows]= height;

    //Compute max. width for row headings.
    headWidth= 0;
    for (int i= 0; i < nRows; i++) {
      int w= 
	  stringWidth(table.getRow(i).toString(), m);
      if (w > headWidth) headWidth= w;
    }
    headWidth+= COL_SEP;
    headWidth= 
      (int)Math.ceil((double)(headWidth + COL_DIV - 1)/COL_DIV * COL_DIV);
   
    //Compute max. widths for each col.
    int width= 0;
    for (int i= 0; i < nCols; i++) {
      Object col= table.getCol(i);
      colStarts[i]= width;
      int wCol= stringWidth(col.toString(), m);
      for (int j= 0; j < nRows; j++) {
	Object row= table.getRow(j);
	Object val= table.get(row, col);
	int valWidth= (val == null) ? 0 : stringWidth(val.toString(), m);
	if (valWidth > wCol) wCol= valWidth;
      }
      wCol+= COL_SEP;
      width+= (int)Math.ceil((double)(wCol + COL_DIV - 1)/COL_DIV * COL_DIV);
    }
    colStarts[nCols]= width;
    updated= true;
  }


  private void updateScrollbars(int x, int y) {
    FontMetrics fontMetrics= getFontMetrics(getFont());
    Dimension s= size();
    hScroll.setValues(x, s.width - headWidth, 0, 
		      colStarts[table.nCols()] + SLOP - (s.width - headWidth));
    hScroll.setLineIncrement(fontMetrics.stringWidth("M"));
    hScroll.setPageIncrement(s.width - headWidth - SLOP);
    vScroll.setValues(y, s.height - headHeight, 0, 
		      rowStarts[table.nRows()] + SLOP - 
		      (s.height - headHeight));
    vScroll.setLineIncrement(fontMetrics.getHeight());
    vScroll.setPageIncrement(s.height - headHeight - SLOP);
  }

  /** Compute height of a string (which may contain '\n's) in pixels,
   * using a specified font metrics.
   * @param s	string (which may contain '\n's) whose height is to
   *		be computed.
   * @param m	font metrics to use in the height computation.
   * @return 	height of the string in pixels.
   */
  private int stringHeight(String s, FontMetrics m) {
    int n= 1;
    int len= s.length();
    try {
      for (int index= 0; index < len; index++) {
	if (s.charAt(index) == '\n') n++;
      }
    }
    catch (StringIndexOutOfBoundsException e) {
      throw new InternalError("Table2Canvas.stringHeight() " + e);
    }
    return n*m.getHeight();
  }

  /** Compute width of a string (which may contain '\n's) in pixels,
   * using a specified font metrics.
   * @param s	string (which may contain '\n's) whose width is to
   *		be computed.
   * @param m	font metrics to use in the height computation.
   * @return 	width of the string in pixels.
   */
  private int stringWidth(String s, FontMetrics m) {
    int w= 0;
    int len= s.length();
    int lastIndex= 0;
    try {
      for (int i= 0; i < len; i++) {
	if (s.charAt(i) == '\n' || i == len - 1) {
	  i++;
	  int lineWidth= m.stringWidth(s.substring(lastIndex, i));
	  if (lineWidth > w) w= lineWidth;
	  lastIndex= i;
	}
      }
    }
    catch (StringIndexOutOfBoundsException e) {
      throw new InternalError("Table2Canvas.stringWidth() " + e);
    }
    return w;
  }

  /** Draw a string (which may contain embedded '\n's).
   * @param s	the string to be drawn (any '\n's in it are
   *		suitably interpreted).
   * @param x	the x coordinate of the top left corner where
   *		the string should be drawn.
   * @param y	the y coordinate of the top left corner where
   *		the string should be drawn.
   */
  private void drawBlock(String s, int x, int y, Graphics g) {
    int n= 0;
    int len= s.length();
    int lastIndex= 0;
    FontMetrics m= g.getFontMetrics(g.getFont());
    int h= m.getHeight();
    int ascent= m.getAscent();
    try {
      for (int i= 0; i < len; i++) {
	if (s.charAt(i) == '\n' || i == len - 1) {
	  i++;
	  g.drawString(s.substring(lastIndex, i), x, y + n*h + ascent);
	  n++;
	  lastIndex= i;
	}
      }
    }
    catch (StringIndexOutOfBoundsException e) {
      throw new InternalError("Table2Canvas.stringWidth() " + e);
    }
  }


  private Table2 table;			//the table being displayed.
  private int headHeight= 0;		//computed heading height across cols.
  private int headWidth= 0;		//computed heading width across rows.
  private int[] rowStarts;		//y-coord of start of each row (in
					//pixels); extra sentinel entry.
  private int[] colStarts;		//x-coord of start of each col (in
					//pixels); extra sentinel entry.
  private int selectedRowN= DESELECT;	//currently selected row #.
  private int selectedColN= DESELECT;	//currently selected col #.
  private boolean updated= false;
					    
  static private final int DESELECT= -1;
  static private final int ROW_SEP= 2;
  static private final int COL_SEP= 5;
  static private final int ROW_DIV= 10;  //Must divide height of each row.
  static private final int COL_DIV= 10;  //Must divide width of each col.

  //foreground color to use for table contents.
  static private final Color contentsFGColor= Color.black;

  //background color to use for table contents.
  static private final Color contentsBGColor= Color.white;

  //foreground color to use for table headings.
  static private final Color headFGColor= Color.red;

  //background color to use for table headings.
  static private final Color headBGColor= new Color(220, 220, 220); //grey.

  //background color to use for selected table headings.
  static private final Color headSelBGColor= 
    new Color(180, 180, 180); //dark grey.

  //background color to use for selected row/col.
  static private final Color contentsSelBGColor= 
    new Color(230, 230, 230); //lighter grey than headBGColor.

  //background color for table entry at intersection of selected row/col.
  static private final Color contentsSel2BGColor=
    new Color(170, 170, 170); //darker grey than headSelBGColor.



  //Test program.
  static public void main(String[] args) {
    String[] rows= { 
      "Adam", "Betty", "Bill", "Jack", "John", "Karen", "Newt", 
    };
    String[] cols= { "AGE", "WEIGHT", "HEIGHT", "OCCUPATION", "MISC" };
    String[][] vals= {
      //Adam
      { "44", "180", "6\'",	"bus-boy", 	"Never married\nrents" }, 
      //Betty
      { "32", "120", "5\' 7\"",	"executive", 	"married\n2 kids\nowns home"}, 
      //Bill
      { "51", "200", "6\' 2\"",	"politician", 	"married\n1 kid\nfree home" }, 
      //Jack
      { "38", "170", "6\' 2\"",	"janitor",	"divorced\n3 kids\nrents" }, 
      //John
      { "82", "155", "6\' 1\"",	"retired",	"widowed\n3 kids\nrents" }, 
      //Karen
      { "54", "160", "5\' 8\"",	"librarian",	"divorced\nno kids\nowns" },
      //Newt
      { "53", "210", "6\' 3\"",	"politician", 	"married\n2 kids\nrents" }, 
    };
    Table2 tab= new SparseTable2(rows, cols);
    for (int i= 0; i < rows.length; i++) {
      for (int j= 0; j < cols.length; j++) {
	tab.put(rows[i], cols[j], vals[i][j]);
      }
    }
    Table2Canvas t= new Table2Canvas(tab, 200, 200);
    Frame f= new Frame("TableCanvas Test");
    t.setFont(new Font("Helvetica", 0, 18));
//    t.select("HEIGHT", "Bill");
    f.setLayout(new FlowLayout());
    f.add(t.getComponent());
    f.resize(600, 600);
    f.show();
  }
}
