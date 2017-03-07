/*

File:	 ExtFrame.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:32:58 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import java.applet.AppletContext;
import java.net.*;

/** A class to display a document in an external frame. */
class ExtFrame {

  /** Constructor to use when applet is running in a non-browser context. */
  ExtFrame() {
    context= null; doUpdate= false;
  }

  /** Constructor to use when running in a browser context. 
   * @param context	context of applet.
   * @param url		url of document to be displayed.
   * @param window	name of window in which document should be displayed.
   */
  ExtFrame(AppletContext context, String url, String window) {
    this.context= context; 
    this.url= url;
    this.window= window;
    doUpdate= true;
  }

  /** Constructor to use when running in a browser context, when document
   * should be displayed in the "default" window.
   * @param context	context of applet.
   * @param url		url of document to be displayed.
   */
  ExtFrame(AppletContext context, String url) {
    this(context, url, null);
  }

  void setUpdate(boolean doUpdate) {
    this.doUpdate= doUpdate;
  }

  /** Show document corresponding to the URL specified in this's constructor
   * at the fragment identifier specified by relTag.
   * @param relTag	fragment identifier appended to URL (after '#' 
   *			character) specifying fragment of document which 
   *			should be displayed.
   */
  void show(String relTag) {
    String url0= url + "#" + relTag;
    if (doUpdate) {
      URL url1= null;
      try {
	url1= new URL(url0); 
      }
      catch (MalformedURLException e) {
	throw new InternalError("bad URL " + url + e);
      }
      if (window == null) {
	context.showDocument(url1);
      }
      else {
	context.showDocument(url1, window);
      }
    }
  }

  private AppletContext context;//context of applet (null if none).
  private String url;		//url of document to be displayed.
  private String window;	//window in which it should be displayed.
  private boolean doUpdate;	//true if window should be updated.
}
