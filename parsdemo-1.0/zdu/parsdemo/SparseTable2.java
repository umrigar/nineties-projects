/*

File:	 SparseTable2.java
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

Table2: A sparse 2-dimensional table class.  Row, column indexes can be
arbitrary objects, but they must correctly implement hashcode() and
equals().  Table values can be any objects.  Provides routines for
adding entries to the table, looking up entries and enumerating all
row and column indexes as well as values.

*/

import zdu.parsdemo.RowCol;
import zdu.parsdemo.Table2;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

class SparseTable2 extends Table2 {

  SparseTable2() {
  }

  SparseTable2(Object rows[], Object cols[]) {
    super(rows, cols);
  }

  SparseTable2(Vector rows, Vector cols) {
    super(rows, cols);
  }

  void put(Object row, Object col, Object val) {
    if (rows.get(row) == null) {
      throw new InternalError("Table2: invalid row " + row + " in put");
    }
    if (cols.get(col) == null) {
      throw new InternalError("Table2: invalid col " + col + " in put");
    }
    vals.put(new RowCol(row, col), val);
  }

  Object get(Object row, Object col) {
    if (rows.get(row) == null) {
      throw new InternalError("Table2: invalid row " + row + " in get");
    }
    if (cols.get(col) == null) {
      throw new InternalError("Table2: invalid col " + col + " in get");
    }
    return vals.get(new RowCol(row, col));
  }

  private Hashtable vals= new Hashtable();

  public static void main(String[] args) {
    String[] rows= { "Betty", "Jack", "Karen" };
    String[] cols= { "AGE", "WEIGHT", "HEIGHT", "OCCUPATION" };
    String[][] vals= {
      { "32",	"120",	"5\' 7\"",	"executive" }, 
      { "38",	"170",	"6\' 2\"",	"janitor" }, 
      { "54",	"160",	"5\' 8\"",	"librarian" }
    };
    SparseTable2 tab= new SparseTable2(rows, cols);
    for (int i= 0; i < rows.length; i++) {
      for (int j= 0; j < cols.length; j++) {
	tab.put(rows[i], cols[j], vals[i][j]);
      }
    }
    System.out.print(tab.toString());
  }

}
