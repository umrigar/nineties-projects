/*

File:	 GridLocatorXEnumeration.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/07/26 20:37:07 zdu"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.zydebug;

import zdu.zydebug.List;
import zdu.zydebug.BoundedObj;
import zdu.zydebug.XEnumeration;

import java.util.NoSuchElementException;

class GridLocatorXEnumeration implements XEnumeration {

  GridLocatorXEnumeration(int x, int y, List list) {
    this.x= x; this.y= y;
    if (list == null) {
      list= new List();
    }
    listEnum= list.elements();
    listEnumFilter();
  }

  public boolean hasMoreElements() {
    return (listEnum.hasMoreElements());
  }

  public Object getXEnumerationElement() {
    return (BoundedObj)(listEnum.getXEnumerationElement());
  }

  public void stepXEnumeration() {
    listEnum.stepXEnumeration(); 
    listEnumFilter();
  }

  public Object nextElement() {
    Object obj= getXEnumerationElement();
    stepXEnumeration();
    return obj;
  }

  //Filter out the prefix of elements in listEnum until an object is found
  //which contains (x, y).
  private void listEnumFilter() {
    while (listEnum.hasMoreElements()) {
      if (((BoundedObj)(listEnum.getXEnumerationElement())).isInside(x, y)) {
	break;
      }
      listEnum.stepXEnumeration();
    }
  }

  private int x, y;
  private XEnumeration listEnum;

}
