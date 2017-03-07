/*

File:	 FullFrame.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:33:03 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.ParsDemo;
import zdu.parsdemo.AppletFrame;

import java.applet.Applet;
import java.applet.AppletContext;
import java.awt.BorderLayout;
import java.awt.Button;
import java.awt.Event;

/** This is an inline applet.  If the user clicks in it, then it will
 * display the ParsDemo applet in a full-frame.  The ParsDemo applet
 * is invoked as a command-line applet with argument set by parameter
 * APPLET_ALG.  
 */
public class FullFrame extends Applet implements Runnable {
  
  /** Constructor called during applet initialization. */
  public FullFrame() { 
    setLayout(new BorderLayout());
    add("Center", startButton);
  }

  public boolean action(Event event, Object arg) {
    if (event.target == startButton) {
      String args[]= new String[1];
      args[0]= getParameter("APPLET_ALG");
      parsDemo= new ParsDemo(args, ParsDemo.PARTIAL_APPLET_CONTEXT);
      AppletFrame frame= new AppletFrame(parsDemo, 500, 300, this);
      frame.setTitle("zdu.ParseDemo." + args[0]);
      startButton.disable(); repaint();
      Thread enableThread= new Thread(this);
      enableThread.start();
      return true;
    }
    return false;
  }

  public void run() {
    try {
      Thread.sleep(ENABLE_DELAY);
    }
    catch (InterruptedException e) {
    }
    startButton.enable(); repaint();
  }

  private ParsDemo parsDemo;
  private Button startButton= new Button("start");
  private static final int ENABLE_DELAY= 10000;
}
