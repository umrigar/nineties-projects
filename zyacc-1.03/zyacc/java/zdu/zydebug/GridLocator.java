/*

File:	 GridLocator.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/07/26 21:55:45 zdu"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.zydebug;

import zdu.zydebug.List;
import zdu.zydebug.Locator;
import zdu.zydebug.BoundedObj;
import zdu.zydebug.XEnumeration;

import java.awt.Rectangle;	//for unit test.
import java.io.*;		//"""

class GridLocator implements Locator {

  GridLocator(int x, int y, int width, int height, 
	      int gridWidth, int gridHeight) {
    origin[COORD_X]= x; origin[COORD_Y]= y;
    dim[COORD_X]= width; dim[COORD_Y]= height;
    gridDim[COORD_X]= gridWidth; gridDim[COORD_Y]= gridHeight;
    nGrids[COORD_X]= width/gridWidth; nGrids[COORD_Y]= height/gridHeight;
    grid= new List[nGrids[COORD_X] + 2][nGrids[COORD_Y] + 2];
  }

  public void locatorRegister(Rectangle bounds, Object obj) {
    BoundedObj boundedObj= new BoundedObj(bounds, obj);
    int 
      x= bounds.x,
      y= bounds.y,
      width= bounds.width,
      height= bounds.height;
    int hiX= gridNum(COORD_X, x + width);
    for (int i= gridNum(COORD_X, x); i <= hiX; i++) {
      int hiY= gridNum(COORD_Y, y + height);
      for (int j= gridNum(COORD_Y, y); j <= hiY; j++) {
	if (grid[i][j] == null) grid[i][j]= new List();
	grid[i][j].addElement(boundedObj);
      }
    }
  }

  public XEnumeration locatorXEnumeration(int x, int y) {
    int gridXNum= gridNum(COORD_X, x);
    int gridYNum= gridNum(COORD_Y, y);
    return new GridLocatorXEnumeration(x, y, grid[gridXNum][gridYNum]);
  }

  public BoundedObj locate(int x, int y) {
    XEnumeration e= locatorXEnumeration(x, y);
    return 
      (e.hasMoreElements()) ? (BoundedObj)(e.getXEnumerationElement()) : null;
  }

  public void locatorReset() {
    for (int i= 0; i < nGrids[COORD_X] + 2; i++) {
      for (int j= 0; j < nGrids[COORD_Y] + 2; j++) {
	grid[i][j]= null;
      }
    }
  }

  public Rectangle getBounds() {
    return new Rectangle(origin[COORD_X], origin[COORD_Y], 
			 dim[COORD_X], dim[COORD_Y]);
  }


  private int gridNum(int coord, int val) {
    int i= (val - origin[coord])/gridDim[coord];
    return (i < 0) ? 0 : (i >= nGrids[coord] ? nGrids[coord] + 1 : i + 1);
  }
    
    
  static private final int COORD_X= 0;
  static private final int COORD_Y= 1;

  int[] origin= new int[2];
  int[] dim= new int[2];
  int[] gridDim= new int[2];
  int[] nGrids= new int[2];
  List[][] grid;

  //unit test.
  static public void main(String args[]) {
    int xMin= 73, yMin= 73, width= 950, height= 950;
    int gridWidth= 25, gridHeight= 50;
    GridLocator loc= new GridLocator(xMin, yMin, width, height,
				     gridWidth, gridHeight);
    int overshoot= 100;
    int rect1Width= 50, rect1Height= 80;
    int rect1XDisp= 25, rect1YDisp= 40;
    for (int x= 0; x < xMin + width + overshoot; x+= rect1XDisp) {
      for (int y= 0; y < yMin + height + overshoot; y+= rect1YDisp) {
	Rectangle rect= new Rectangle(x, y, rect1Width, rect1Height);
	loc.locatorRegister(rect, rect);
      }
    }
    StreamTokenizer in= new StreamTokenizer(System.in);
    int[] coords= new int[2];
    while (true) {
      System.out.print(
	"Enter coords (2 numbers separated by space, negative to quit): ");
      System.out.flush();
      for (int n= 0; n < 2; n++) {
	try {
	  int t;
	  while ((t= in.nextToken()) != StreamTokenizer.TT_NUMBER) {
	    System.err.println("must be a number");
	  }
	}
	catch(IOException e) {
	  System.err.println("i/o exception");
	}
	coords[n]= (int) in.nval;
	if (in.nval < 0) break;
      }
      if (coords[0] < 0 || coords[1] < 0) break;
      for (XEnumeration e= loc.locatorXEnumeration(coords[0], coords[1]);
	   e.hasMoreElements(); e.stepXEnumeration()) {
	Rectangle rect= 
	  (Rectangle)(((BoundedObj)(e.getXEnumerationElement())).getObject());
	System.out.println(rect);
      }
    }

  }
}
