/*

File:	 RowCol.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:40:14 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

class RowCol {

  RowCol(Object row, Object col) {
    this.row= row; this.col= col;
  }

  public int hashCode() {
    return row.hashCode() ^ col.hashCode();
  }

  public boolean equals(Object rowCol) {
    return (rowCol instanceof RowCol) && 
           row.equals(((RowCol)rowCol).row) && 
           col.equals(((RowCol)rowCol).col);
  }

  private Object row;
  private Object col;
}

