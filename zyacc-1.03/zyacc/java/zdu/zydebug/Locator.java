/*

File:	 Locator.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/07/26 21:23:50 zdu"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.zydebug;

import zdu.zydebug.XEnumeration;
import zdu.zydebug.BoundedObj;

import java.awt.Rectangle;


/** Interface meant to locate objects which contain a given point. */
interface Locator {
  void locatorRegister(Rectangle bounds, Object obj);
  XEnumeration locatorXEnumeration(int x, int y);
  BoundedObj locate(int x, int y);
  void locatorReset();
}

