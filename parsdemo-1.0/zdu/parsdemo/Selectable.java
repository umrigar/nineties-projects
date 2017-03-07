/*

File:	 Selectable.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:40:41 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import java.awt.Color;
import java.awt.Component;

/** This interface is implemented by widgets which can have a portion
 * of them selected.
 */
public interface Selectable {

  /** select portion of widget specified by (xSpec, ySpec).
   * The meanings of the coordinates may depend on the widget.
   * @param xSpec specifies the x-coordinate of the portion to be selected.
   * @param ySpec specifies the y-coordinate of the portion to be selected.
   */
  public void select(Object xSpec, Object ySpec);

};
