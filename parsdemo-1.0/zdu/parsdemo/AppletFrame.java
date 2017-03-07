/*

File:	 AppletFrame.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:32:42 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import java.applet.*;
import java.awt.*;
import java.net.URL;
import java.util.Enumeration;

/** A frame containing a applet, so that applets can be run from the
 * command-line.  Adapted from "Core Java" by Cornell & Horstmann.
 */
class AppletFrame extends Frame 
  implements AppletContext, AppletStub 
{

  /** Constructor for a frame in which a applet is displayed.
   * This constructor is meant to be invoked in a non-applet context.
   * @param applet	applet to be displayed.
   * @param width	width of frame.
   * @param height	height of frame.
   */
  AppletFrame(Applet applet, int x, int y) {
    this.invoker= null;
    setTitle(applet.getClass().getName());
    resize(x, y);
    add("Center", applet);
    applet.setStub(this);
    applet.init();
    show();
    applet.start();
  }

  /** Constructor for a frame in which a applet is displayed.
   * This constructor is meant to be invoked by an applet invoker.
   * @param applet	applet to be displayed.
   * @param width	width of frame.
   * @param height	height of frame.
   * @param invoker	applet creating this frame.
   */
  AppletFrame(Applet applet, int x, int y, Applet invoker) {
    this.invoker= invoker;
    setTitle(applet.getClass().getName());
    resize(x, y);
    add("Center", applet);
    applet.setStub(this);
    applet.init();
    show();
    applet.start();
  }

  /** Handles WINDOW_DESTROY event by exiting.  Otherwise lets
   * super handle it.
   * @param evt		event to be processed.
   * @return		value returned by super.handleEvent() if not
   *			WINDOW_DESTROY.
   */
  public boolean handleEvent(Event evt) {
    if (evt.id == Event.WINDOW_DESTROY) {
      this.dispose();
      if (invoker == null) System.exit(0);
    }
    return super.handleEvent(evt);
  }

  /** Applet context methods: if this instance was created by an
   * applet, then use the context of the creator; else do something
   * reasonable.  
   */
  public AudioClip getAudioClip(URL url) { 
    return (invoker == null) ? null 
                             : invoker.getAppletContext().getAudioClip(url); 
  }

  public Image getImage(URL url) { 
    return (invoker == null) ? null 
                             : invoker.getAppletContext().getImage(url); 
  }

  public Applet getApplet(String name) {
    return (invoker == null) ? null 
                             : invoker.getAppletContext().getApplet(name); 
  }
  public Enumeration getApplets() { 
    return (invoker == null) ? null 
                             : invoker.getAppletContext().getApplets(); 
  }
  public void showDocument(URL url) { 
    if (invoker != null) {
      invoker.getAppletContext().showDocument(url);
    }
  }
  public void showDocument(URL url, String target) {
    if (invoker != null) {
      invoker.getAppletContext().showDocument(url, target);
    }
  }

  public void showStatus(String status) { 
    if (invoker != null) {
      invoker.getAppletContext().showStatus(status);
    }
  }


  /** Applet stub methods: if this instance was created by an applet,
   * then use the method provided by the context of the creator; else
   * do something reasonable.  
   */
  public boolean isActive() { 
    return invoker == null ? true : invoker.isActive(); 
  }

  public URL getDocumentBase() { 
    return invoker == null ? null : invoker.getDocumentBase(); 
  }

  public URL getCodeBase() { 
    return invoker == null ? null : invoker.getCodeBase(); 
  }

  public String getParameter(String name) { 
    return invoker == null ? null : invoker.getParameter(name); 
  }

  public AppletContext getAppletContext() { 
    return invoker == null ? this : invoker.getAppletContext(); 
  }

  public void appletResize(int width, int height) { 
    resize(width, height); 
  }

  private Applet invoker;

}
    
