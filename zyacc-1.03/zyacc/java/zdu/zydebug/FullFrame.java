/*

File:	 FullFrame.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/08/01 15:55:30 zdu"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.zydebug;

import zdu.zydebug.ZYDebug;
import zdu.zydebug.AppletFrame;

import java.applet.Applet;
import java.applet.AppletContext;
import java.awt.BorderLayout;
import java.awt.Button;
import java.awt.Event;

/** This is an inline applet.  If the user clicks in it, then it will
 * display the ZYDebug applet in a full-frame.
 */
public class FullFrame extends Applet {
  
  /** Constructor called during applet initialization. */
  public FullFrame() { 
    setLayout(new BorderLayout());
    add("Center", startButton);
  }

  public boolean action(Event event, Object arg) {
    if (event.target == startButton) {
      startButton.disable(); repaint();
      String args[]= new String[1];
      args[0]= getParameter("APPLET_ALG");
      zydebug= new ZYDebug(this);
      AppletFrame frame= new AppletFrame(zydebug, 600, 400, this);
      frame.setTitle("ZYDebug");
      return true;
    }
    return false;
  }


  private ZYDebug zydebug;
  private Button startButton= new Button("start");
}
