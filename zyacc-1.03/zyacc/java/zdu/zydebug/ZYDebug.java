/*

File:	 ZYDebug.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/07/28 16:49:01 zdu"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.zydebug;

import zdu.zydebug.Act;
import zdu.zydebug.AppletFrame;
import zdu.zydebug.Attention;
import zdu.zydebug.ListCanvas;
import zdu.zydebug.OffsetForest;
import zdu.zydebug.Parser;
import zdu.zydebug.Scanner;
import zdu.zydebug.Token;
import zdu.zydebug.TreeCanvas;

import java.applet.*;
import java.awt.*;
import java.io.*;
import java.net.*;

/**

*/

public class ZYDebug extends Applet implements Attention {

  //Necessary applet constructor.
  public ZYDebug() {
    applet= this;
  }

  //Use this constructor when starting this within a full-frame.
  public ZYDebug(Applet applet) {
    this.applet= applet;
  }

  //this constructor can be used from the command-line.
  public ZYDebug(String args[]) {
    port= -1;
    int argc1= args.length;
    int i= 0;
    if (argc1 > 0 && args[0].charAt(0) == '-' && args[0].charAt(1) == 's') {
      if (args[0].length() > 2) { srcName= args[0].substring(2); i++; }
      else if (argc1 > 1) { srcName= args[1]; i+= 2; }
      else usage();
    }
    switch (argc1 - i) {
      case 1: 
	portString= args[i];
	break;
      case 2:
	host= args[i];
	portString= args[i + 1];
	break;
      default:
	usage();
	break;
    }
  }

  private void usage() {
    System.err.println("usage: zydebug [-s SRCNAME] [HOST] PORT");
    System.exit(1);
  }

  public void init() {
    Dimension s= size();
    int w= INIT_WIDTH;
    int h= INIT_HEIGHT;
    String htmlbase= ".";
    if (applet != null) {
      portString= applet.getParameter("PORT");
      srcName= applet.getParameter("SRC");
      host= applet.getCodeBase().getHost();
      String htmlbaseEnv= applet.getParameter("HTMLBASE");
      if (htmlbaseEnv != null) htmlbase= htmlbaseEnv;
      appletContext= applet.getAppletContext();
    }
    try {
      port= Integer.parseInt(portString);
    }
    catch (NumberFormatException e) {
      System.err.println("invalid port specification " + portString);
      throw new InternalError("bad port specification " + e);
    }
    try {
      socket= new Socket(host, port);
      in= new BufferedInputStream(socket.getInputStream());
      out= new PrintStream(socket.getOutputStream());
    }
    catch (UnknownHostException e) {
      throw new InternalError("unknown host " + host + " " + e);
    }
    catch (IOException e) {
      throw new InternalError("socket open " + e);
    }

    setFrame();
    scanner= new Scanner(in, out);
    parser= new Parser(scanner);

    Font font= new Font("Courier", Font.PLAIN, 16);
    super.setFont(font);

    acts[actN].type= Act.REDUCE;
    acts[actN].nonTerm= "$S"; acts[actN].state= 0; acts[actN].ruleLen= 0;

    forest= new OffsetForest();
    treeCanvas= new TreeCanvas((int)(TREE_WIDTH*w), (int)(TREE_HEIGHT*h),
			       "Parse Forest");
    treeCanvas.setBackground(Color.white);
    treeCanvas.setForest(forest);

    treeCanvas.setShadows(true);
    shadowsCheckbox.setState(true);

    traceCanvas= new TraceCanvas((int)(TRACE_WIDTH*w), (int)(TRACE_HEIGHT*h));
    traceCanvas.setBackground(Color.white);

    String docBase= "";
    if (applet != null) {
      docBase= getDocumentBase().toString();
      int lastSlash= docBase.lastIndexOf('/');
      if (lastSlash >= 0) {
	docBase= docBase.substring(0, lastSlash + 1);
      }
      int lastDot= srcName.lastIndexOf('.');
      String srcRoot= (lastDot > 0) ? srcName.substring(0, lastDot) : srcName;
      updateURL= new String(docBase + htmlbase + "/" + srcRoot + "_2.html");
    }
    {
      DataInputStream d= new DataInputStream(in);
      out.println("f"); out.flush();
      srcCanvas= 
	 new FileCanvas(d, (int)(SRC_WIDTH*w), (int)(SRC_HEIGHT*h),
	           	"Parser source program " + srcName);
      if (scanner.nextTok(tok) != Scanner.PROMPT) {
	throw new InternalError("prompt after source file not found");
      }
      srcCanvas.setBackground(Color.white);
    }
    {
      out.println("M"); out.flush();
      scanner.nextTok(tok);
      if (tok.type != Scanner.WORD) {
	throw new InternalError("input error: number expected");
      }
      String maxWidthString= new String(tok.buf, 0, tok.index, tok.len);
      if (scanner.nextTok(tok) != Scanner.NL ||
	  scanner.nextTok(tok) != Scanner.PROMPT) {
	throw new InternalError(
          "input error: max semantics width not properly ended");
      }
      maxSemLen= Integer.parseInt(maxWidthString);
      FontMetrics m= getFontMetrics(font);
      int maxWidth= maxSemLen * m.stringWidth("M");
      breaks= new ListCanvas((int)(LIST_WIDTH*w), (int)(LIST_HEIGHT*h),
			     "Breakpoints Set/Reset");
      breaks.setBackground(Color.white);
      breaks.setAttention(this);
      breaks.addItem("All shifts"); breaks.addItem("All reductions");
      out.println("l *"); out.flush();
      for (scanner.nextTok(tok); tok.type != Scanner.PROMPT; 
	   scanner.nextTok(tok)) {
	if (tok.type != Scanner.NL) {
	  String itemString= new String(tok.buf, 0, tok.index, tok.len);
	  breaks.addItem(itemString);
	  int itemWidth= m.stringWidth(itemString);
	  if (itemWidth > maxWidth) maxWidth= itemWidth;
	}
      }
      treeCanvas.setColSep(maxWidth + X_COL_SEP);
    }

    GridBagLayout layout= new GridBagLayout();
    setLayout(layout);
    GridBagConstraints c= new GridBagConstraints();

    c.gridx= 0; c.gridy= 0; 
    c.weightx= 100; c.weighty= 100;
    c.gridheight= 50; c.gridwidth= 70;
    c.fill= GridBagConstraints.BOTH;
    layout.setConstraints(treeCanvas.getComponent(), c);
    add(treeCanvas.getComponent());

    c.gridx= 70; c.gridy= 0; 
    c.weightx= 20; c.weighty= 100;
    c.gridheight= 50; c.gridwidth= 30;
    c.fill= GridBagConstraints.BOTH;
    layout.setConstraints(traceCanvas.getComponent(), c);
    add(traceCanvas.getComponent());

    c.gridx= 0; c.gridy= 50; 
    c.weightx= 100; c.weighty= 50;
    c.gridheight= 40; c.gridwidth= 90;
    c.fill= GridBagConstraints.BOTH;
    layout.setConstraints(srcCanvas.getComponent(), c);
    add(srcCanvas.getComponent());

    c.gridx= 90; c.gridy= 50; 
    c.weightx= 0; c.weighty= 50;
    c.gridheight= 40; c.gridwidth= 10;
    c.fill= GridBagConstraints.BOTH;
    layout.setConstraints(breaks.getComponent(), c);
    add(breaks.getComponent());

    c.gridx= 0; c.gridy= 95;
    c.weightx= 0; c.weighty= 0;
    c.gridheight= 8; c.gridwidth= 8;
    c.fill= GridBagConstraints.NONE;
    c.anchor= GridBagConstraints.WEST;
    layout.setConstraints(shadowsCheckbox, c); add(shadowsCheckbox);

    if (applet != null) {
      c.gridx= 20; c.gridy= 95;
      c.weightx= 0; c.weighty= 0;
      c.gridheight= 8; c.gridwidth= 8;
      c.fill= GridBagConstraints.NONE;
      c.anchor= GridBagConstraints.WEST;
      layout.setConstraints(updateCheckbox, c); add(updateCheckbox);
    }

    c.gridx= 70; c.gridy= 95;
    c.weightx= 0; c.weighty= 0;
    c.gridheight= 10; c.gridwidth= 8;
    c.fill= GridBagConstraints.NONE;
    c.anchor= GridBagConstraints.EAST;
    layout.setConstraints(stepButton, c); add(stepButton);

    c.gridx= 80; c.gridy= 95;
    c.weightx= 0; c.weighty= 0;
    c.gridheight= 5; c.gridwidth= 8;
    c.fill= GridBagConstraints.NONE;
    c.anchor= GridBagConstraints.EAST;
    layout.setConstraints(nextButton, c); add(nextButton);

    c.gridx= 90; c.gridy= 95;
    c.weightx= 0; c.weighty= 0;
    c.gridheight= 5; c.gridwidth= 8;
    c.fill= GridBagConstraints.NONE;
    c.anchor= GridBagConstraints.EAST;
    layout.setConstraints(continueButton, c); add(continueButton);


  }

  public boolean action(Event evt, Object arg) {
    if (evt.target == shadowsCheckbox) {
      treeCanvas.setShadows(shadowsCheckbox.getState());
      treeCanvas.repaint();
    }
    else if (!isEOF) {
      if (evt.target == updateCheckbox) {
	isUpdate= updateCheckbox.getState();
      }
      else if (evt.target == stepButton) {
	out.println("s");
	updateTraces();
	return true;
      }
      else if (evt.target == nextButton) {
	out.println("n");
	updateTraces();
	return true;
      }
      else if (evt.target == continueButton) {
	out.println("c");
	updateTraces();
	return true;
      }
    }
    return false;
  }

  public void attention() {
    if (!isEOF) {
      int itemN= breaks.lastClickItemN();
      String text=   (itemN == 0) ? "%t" 
	: (itemN == 1) ? "%n"
	: breaks.getItemText(itemN);
      promptResponseCommand((breaks.isSelected(itemN) ? "b " : "B ") + text);
    }
  }

  private void doUpdate(int state) {
    String url0= updateURL + "#State-" + state + "-without-closure-items";
    URL url1= null;
    try {
      url1= new URL(url0); 
    }
    catch (MalformedURLException e) {
      throw new InternalError("bad URL " + url0 + e);
    }
    appletContext.showDocument(url1, "left");
  }

  private void promptResponseCommand(String cmd) {
    out.println(cmd);
    if (scanner.nextTok(tok) != Scanner.PROMPT) {
      System.out.println("tok= " + tok.type);
      throw new InternalError("prompt not found");
    }    
  }

  private void updateTraces() {
    if (!isEOF) {
    out:
      while (true) {
	//why doesn't this work outside the loop? 
	if (frame != null) frame.setCursor(Frame.WAIT_CURSOR);
	Act lastAct= acts[actN];
	Act newAct= acts[1 - actN];
	parser.nextAct(newAct);
	if (newAct.type == Act.PROMPT) break;
	switch (lastAct.type) {
	  case Act.ACCEPT: {
	    OffsetTree eofTree= forest.pop();
	    OffsetTree startTree= forest.pop();
	    forest.peek().addKid(startTree);
	    forest.peek().addKid(eofTree);
	    treeCanvas.select(forest.peek(), null);
	    break;
	  }
	  case Act.ERROR: 
	    if ("error".equals(lastAct.token)) forest.pop();
	    break;
	  case Act.REDUCE: {
	    ParseNode n= new ParseNode(lastAct.state + "/" + lastAct.nonTerm, 
				       ParseNode.NON_TERM);
	    OffsetTree t= new OffsetTree(n);
	    for (int i= 0; i < lastAct.ruleLen; i++) {
	      t.addKid(forest.pop(), -1);
	    }
	    forest.push(t); treeCanvas.select(t, null);
	    if (isUpdate) doUpdate(lastAct.state);
	    break;
	  }
	  case Act.SHIFT: {
	    String tokStr=
	        (lastAct.value == null) ? lastAct.token
	      : (lastAct.value.length() < maxSemLen) ? lastAct.value
	      : lastAct.value.substring(0, maxSemLen);
	    ParseNode n= new ParseNode(lastAct.state + "/" + tokStr,
				       ("error".equals(lastAct.token)) 
				       ? ParseNode.ERROR 
				       : ParseNode.TERM);
	    OffsetTree t= new OffsetTree(n);
	    forest.push(t); treeCanvas.select(t, null);
	    if (isUpdate) doUpdate(lastAct.state);
	    break;
	  }
	  default:
	    break;
	} // switch (lastAct.type)
	switch (newAct.type) {
	  case Act.EOF:
	    isEOF= true; 
	    break out;
	  case Act.ACCEPT: {
	    srcCanvas.selectLine(0);
	    traceCanvas.addTrace(newAct.stack, newAct.token,  "ACCEPT");
	    break;
	  }
	  case Act.ERROR:
	    srcCanvas.selectLine(0);
	    traceCanvas.addTrace(newAct.stack, newAct.token, "ERROR");
	    break;
	  case Act.REDUCE: {
	    srcCanvas.selectLine(newAct.lineNum);
	    traceCanvas.addTrace(newAct.stack, newAct.token, 
				 "REDUCE " + newAct.ruleN + 
				 "(" + newAct.nonTerm + "); GOTO " +
				 newAct.state);
	    break;
	  }
	  case Act.SHIFT: {
	    srcCanvas.selectLine(0);
	    traceCanvas.addTrace(newAct.stack, newAct.token, 
	      ((newAct.value == null) ? "" : "(" + newAct.value + ") ") 
	      + "SHIFT " + newAct.state);
	    break;
	  }
	  default:
	    throw new InternalError("bad action");
	} // switch (newAct.type) 
	actN= 1 - actN;
      } // while (true)
      if (frame != null) frame.setCursor(Frame.DEFAULT_CURSOR);      
    } // if (!isEOF)
    srcCanvas.repaint(); traceCanvas.repaint(); treeCanvas.repaint(); 
  }

  private void setFrame() {
    Component p;
    for (p= this; p != null; p= p.getParent()) {
      if (p instanceof java.awt.Frame) {
	frame= (Frame)p; break;
      }
    }
  }

  static public void main(String args[]) {
    ZYDebug zydebug= new ZYDebug(args);
    new AppletFrame(zydebug, INIT_WIDTH, INIT_HEIGHT);
  }


  static final int MIN_PORT_NUMBER= 5001;
  static final int MAX_PORT_NUMBER= 65535;						  
  private Frame frame;
  private Applet applet;
  private AppletContext appletContext;
  private String updateURL= null;
  private int port= -1;
  private String portString= null;
  private String host= "localhost";
  private String srcName= null;
  private Socket socket;
  private InputStream in;
  private PrintStream out;
  private Scanner scanner;
  private Parser parser;
  private OffsetForest forest;
  private Token tok= new Token();
  private TreeCanvas treeCanvas;
  private FileCanvas srcCanvas;
  private TraceCanvas traceCanvas;
  private ListCanvas breaks;
  private Checkbox updateCheckbox= new Checkbox("update");
  private Checkbox shadowsCheckbox= new Checkbox("shadows");
  private Button nextButton= new Button("next");
  private Button stepButton= new Button("step");
  private Button continueButton= new Button("continue");
  private Act[] acts= { new Act(), new Act() };
  private int actN= 0;
  private int maxSemLen= 0;
  private boolean isUpdate= false;
  private boolean isEOF;

  private static final int INIT_WIDTH= 600;
  private static final int INIT_HEIGHT= 400;

  private static final double TREE_WIDTH= 0.7;
  private static final double TREE_HEIGHT= 0.5;

  private static final double TRACE_WIDTH= 0.2;
  private static final double TRACE_HEIGHT= 0.5;

  private static final double SRC_WIDTH= 0.7;
  private static final double SRC_HEIGHT= 0.3;

  private static final double LIST_WIDTH= 0.2;
  private static final double LIST_HEIGHT= 0.3;

  private static final int X_COL_SEP= 20;
}
