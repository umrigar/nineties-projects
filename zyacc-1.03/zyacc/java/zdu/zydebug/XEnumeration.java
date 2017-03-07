/*

File:	 GridLocatorEnumeration.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/07/26 12:03:09 zdu"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.zydebug;

import java.util.Enumeration;

/** This interface fixes a problem in that the granularity of 
 * java.util.Enumeration is too coarse, making it impossible
 * to write a enumeration which filters a second enumeration
 * while maintaining the enumeration state in the second
 * enumeration.
 * Should be used as:
 * for (XEnumeration p= (...); p.hasMoreElements(); p.stepXEnumeration()) {
 *   Object obj= p.getXEnumerationElement();
 *   ... process obj ...
 * }
 */
interface XEnumeration extends Enumeration {
  /** Return current element of enumeration. */
  public Object getXEnumerationElement();

  /** Step enumeration to next element. */
  public void stepXEnumeration();

}
