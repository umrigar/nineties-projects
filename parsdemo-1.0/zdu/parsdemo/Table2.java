/*

File:	 Table2.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:40:50 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

/**

Table2: A 2-dimensional table abstract class.  Row, column indexes can be
arbitrary objects, but they must correctly implement hashcode() and
equals().  Table values can be any objects.  Provides routines for
adding entries to the table, looking up entries and enumerating all
row and column indexes as well as values.

*/

import zdu.parsdemo.RowCol;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

abstract class Table2 {

  Table2() {
  }

  Table2(Object rows[], Object cols[]) {
    for (int i= 0; i < rows.length; i++) {
      addRow(rows[i]);
    }
    for (int i= 0; i < cols.length; i++) {
      addCol(cols[i]);
    }
  }

  Table2(Vector rows, Vector cols) {
    for (int i= 0; i < rows.size(); i++) {
      addRow(rows.elementAt(i));
    }
    for (int i= 0; i < cols.size(); i++) {
      addCol(cols.elementAt(i));
    }
  }

  void addRow(Object row) {
    if (rows.get(row) == null) {
      rows.put(row, new Integer(rowVec.size()));
      rowVec.addElement(row);
    }
  }

  void addCol(Object col) {
    if (cols.get(col) == null) {
      cols.put(col, new Integer(colVec.size()));
      colVec.addElement(col);
    }
  }

  abstract void put(Object row, Object col, Object val);

  abstract Object get(Object row, Object col);

  Enumeration enumerateRows() {
    return rowVec.elements();
  }

  Enumeration enumerateCols() {
    return colVec.elements();
  }

  int getRowIndex(Object row) {
    Integer index= (Integer) rows.get(row);
    return (index == null) ? -1 : index.intValue();
  }

  int getColIndex(Object col) {
    Integer index= (Integer) cols.get(col);
    return (index == null) ? -1 : index.intValue();
  }

  Object getRow(int i) {
    Object row= null;
    try {
      row= rowVec.elementAt(i);
    }
    catch (ArrayIndexOutOfBoundsException e) {
      throw new InternalError("Table2.getRow() " + e);
    }
    return row;
  }

  Object getCol(int i) {
    Object col= null;
    try {
      col= colVec.elementAt(i);
    }
    catch (ArrayIndexOutOfBoundsException e) {
      throw new InternalError("Table2.getCol() " + e);
    }
    return col;
  }

  int nRows() {
    return rowVec.size();
  }

  int nCols() {
    return colVec.size();
  }

  public String toString() {
    StringBuffer b= new StringBuffer();
    for (Enumeration e= enumerateCols(); e.hasMoreElements();) {
      b.append("\t" + e.nextElement().toString());
    }
    b.append("\n");
    for (Enumeration rEnum= enumerateRows(); rEnum.hasMoreElements();) {
      Object row= rEnum.nextElement();
      b.append(row.toString());
      if (row.toString().length() < 8) b.append("\t");
      for (Enumeration cEnum= enumerateCols(); cEnum.hasMoreElements();) {
	Object val= get(row, cEnum.nextElement());
	if (val != null) {
	  b.append(val.toString());
	}
	b.append("\t");
      }
      b.append("\n");
    }
    return b.toString();
  }

  protected Hashtable rows= new Hashtable();
  protected Hashtable cols= new Hashtable();
  /* Redundant Vectors to ensure rows and cols are enumerated in order
   * they were entered into the table.  (Another implementation could
   * construct these vectors dynamically when an enumeration is requested).
   */
  private Vector rowVec= new Vector();
  private Vector colVec= new Vector();

}
