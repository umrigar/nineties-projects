/*

File:	 List.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/07/26 17:05:59 zdu"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.zydebug;

import java.util.Enumeration;
import java.util.NoSuchElementException;

public class List implements XEnumeration {

  public List() {
    obj= null; tail= null;
  }



  /** Adds the specified object obj to the list.
   * @param	obj - the component to be added.
   */
  public void addElement(Object  obj) {
    List newList= new List();
    newList.obj= obj; newList.tail= tail;
    tail= newList;
  }

  /** Returns true iff the list contains Object obj (using equals). */
  public boolean contains(Object  obj) {
    for (List p= tail; p != null; p= p.tail) {
      if (obj.equals(p.obj)) return true;
    }
    return false;
  }

  public Object elementAt(int  index) {
    int i= 0;
    for (List p= tail; p != null; p= p.tail) {
      if (i == index) return p.obj;
      i++;
    }
    throw new NoSuchElementException("List.elementAt()");
  }

  public XEnumeration elements() {
    List list= new List();
    list.tail= tail;
    return list;
  }
  

  public Object firstElement() {
    if (tail != null) { 
      return tail.obj;
    }
    else {
      throw new NoSuchElementException("List.firstElement()");
    }
  }

  public List restElements() {
    if (tail != null) { 
      return tail;
    }
    else {
      throw new NoSuchElementException("List.firstElement()");
    }
  }
    
  public boolean isEmpty() {
    return tail == null;
  }

  public int size() {
    int i= 0;
    for (List p= tail; p != null; p= p.tail) i++;
    return i;
  }

  public String toString() {
    int n= 5;
    int i= 0;
    StringBuffer b= new StringBuffer();
    List p;
    for (p= tail; p != null && i < n; p= p.tail, i++) {
      b.append(p.obj.toString() + " ");
    }
    if (p != null) b.append("...");
    return super.toString() + "(" + size() + ")" + "[" + b.toString() + "]";
  }

  public boolean hasMoreElements() {
    return tail != null;
  }

  public Object getXEnumerationElement() {
    if (tail != null) return tail.obj;
    else throw new NoSuchElementException("List.getXEnumerationElement()");
  }

  public void stepXEnumeration() {
    if (tail != null) tail= tail.tail;
    else throw new NoSuchElementException("List.getXEnumerationElement()");
  }

  public Object nextElement() {
    Object obj= getXEnumerationElement();
    stepXEnumeration();
    return obj;
  }

  private Object obj;
  private List tail;
}
