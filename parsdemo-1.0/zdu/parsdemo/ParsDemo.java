/*

File:	 ParsDemo.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:37:26 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.AppletFrame;
import zdu.parsdemo.ExtFrame;
import zdu.parsdemo.FileCanvas;
import zdu.parsdemo.ParseDisplay;
import zdu.parsdemo.Selectable;
import zdu.parsdemo.StepParser;
import zdu.parsdemo.TraceCanvas;
import zdu.parsdemo.TreeCanvas;

import java.applet.*;
import java.awt.*;
import java.io.*;
import java.net.*;

/**
Play and display different parsing algorithms in action.

*/

/** Accepts a single argument specifying which algorithm to run.
 *
 * The argument is somewhat more verbose, if this is run totally as an
 * applet, so that the message output by the browser is more meaningful.
 *
 * If run as an applet, the NAME argument is either RecParser, 
 * LL1Parser or SRParser.
 *
 * If run either as a sub-applet of a full-frame applet or from the
 * command line, the argument is either Rec, LL1 or SR.
 */
public class ParsDemo extends Applet implements Runnable {

  //Necessary applet constructor.
  public ParsDemo() {
    context= FULL_APPLET_CONTEXT;
  }

  //this constructor can be used from the command-line.
  public ParsDemo(String args[]) {
    this(args, CMD_LINE_CONTEXT);
  }

  //use this constructor when run as a sub-applet of a full-screen applet.
  public ParsDemo(String args[], int context) {
    this.context= context;
    algorithm= -1;
    if (args.length == 0) {
      algorithm= REC_DESCENT_PARSER;
    }
    else if (args.length == 1) {
      String alg= args[0];
      algorithm= 
        alg.equals("Rec") ? REC_DESCENT_PARSER 
      : alg.equals("LL1") ? LL_PARSER 
      : alg.equals("SR") ? SR_PARSER : -1;
    }
    if (algorithm < 0) {
      System.err.println(
      "usage: java ParsDemo ALGORITHM where ALGORITHM is\n" +
      "Rec, LL1 or SR"
      );
      System.exit(1);
    }
  }

  public void init() {
    Dimension s= size();
    int w= s.width;
    int h= s.height;
    if (context != CMD_LINE_CONTEXT) {
      String alg= getParameter("name");
      if (alg != null) {
	algorithm= 
	  alg.equals("LL1Parser") ? LL_PARSER : alg.equals("SRParser") 
	                                        ? SR_PARSER 
	                                        : REC_DESCENT_PARSER;
      }
    }

    Font font= new Font("Courier", Font.PLAIN, 16);
    super.setFont(font);
    traceCanvas= new TraceCanvas((int)(TRACE_WIDTH*w), (int)(TRACE_HEIGHT*h));
    treeCanvas= new TreeCanvas((int)(TREE_WIDTH*w), (int)(TREE_HEIGHT*h),
			       algorithm == SR_PARSER ? "Parse Forest" 
			                              : "Parse Tree");

    scanner= new SimpCompScanner();
    String docBase= null;
    if (context != CMD_LINE_CONTEXT) {
      docBase= getDocumentBase().toString();
      int lastSlash= docBase.lastIndexOf('/');
      if (lastSlash >= 0) {
	docBase= docBase.substring(0, lastSlash + 1);
      }
    }
    switch (algorithm) {
      case REC_DESCENT_PARSER: {
	InputStream i= System.in;
	try {
	  if (context == CMD_LINE_CONTEXT) {
	    i= new FileInputStream(fName);
	  }
	  else {
	    i= (new URL(getCodeBase(), fName)).openStream();
	  }
	}
	catch (IOException e) {
	  System.err.println("Could not open file " + fName);
	  System.exit(1);
	}
	DataInputStream d= new DataInputStream(i);
	prgTableCanvas= 
	  new FileCanvas(d, (int)(SELECT_WIDTH*w), (int)(SELECT_HEIGHT*h),
			 "Recursive-Descent Parsing Program");
	SimpCompLL1Gram grammar= new SimpCompLL1Gram();
	extFrame= 
          (context == CMD_LINE_CONTEXT) 
	  ? new ExtFrame()
	  : new ExtFrame(getAppletContext(), 
			 docBase + "simpcompLL1Gram.html", "left");
	parseDisplay= 
	  new ParseDisplay(treeCanvas, prgTableCanvas, traceCanvas, extFrame);
	parser= new SimpCompRecDescent(grammar, scanner, parseDisplay);
	break;
      }
      case LL_PARSER: {
	SimpCompLL1Gram grammar= new SimpCompLL1Gram();
	Table2 llTab= new SimpCompLL1Table(grammar);
	prgTableCanvas= new Table2Canvas(llTab, 
			   (int)(SELECT_WIDTH*w), (int)(SELECT_HEIGHT*h),
				       "LL(1) Parsing Table");
	extFrame= 
          (context == CMD_LINE_CONTEXT) 
	  ? new ExtFrame()
	  : new ExtFrame(getAppletContext(), 
			 docBase + "simpcompLL1Gram.html", "left");
	parseDisplay=
	  new ParseDisplay(treeCanvas, prgTableCanvas, traceCanvas, extFrame);
	parser= new LL1Parser(grammar, llTab, scanner, parseDisplay);
	break;
      }
      case SR_PARSER: {
	SimpCompSRGram grammar= new SimpCompSRGram();
	SRTable srTab= new SimpCompSRTable(grammar);
	prgTableCanvas= new Table2Canvas(srTab, 
			   (int)(SELECT_WIDTH*w), (int)(SELECT_HEIGHT*h),
				       "Shift-Reduce Parsing Table");
        extFrame= (context == CMD_LINE_CONTEXT) 
	  ? new ExtFrame()
	  : new ExtFrame(getAppletContext(), 
			 docBase + "simpcomp_2.html", "left");
	parseDisplay=
	  new ParseDisplay(treeCanvas, prgTableCanvas, traceCanvas, extFrame);
	parser= new SRParser(grammar, srTab, scanner, parseDisplay);
	break;
      }
    }
    extFrame.setUpdate(false);

    updateButton= new Button("update");
    if (context == CMD_LINE_CONTEXT) updateButton.disable();
    inputField= new TextField(INIT_INPUT, 20);
    startStepButton= new Button("start"); 
    runStopButton= new Button("run");
    buttonBG= startStepButton.getBackground();
    runDelayScrollbar= 
      new Scrollbar(Scrollbar.HORIZONTAL, 10, 1, 7, 13);
    updateRunDelay();

    treeCanvas.setBackground(Color.white);
    prgTableCanvas.setBackground(Color.white);
    traceCanvas.setBackground(Color.white);

    GridBagLayout layout= new GridBagLayout();
    setLayout(layout);
    GridBagConstraints c= new GridBagConstraints();

    c.gridx= 0; c.gridy= 0; 
    c.weightx= 100; c.weighty= 100;
    c.gridheight= 40; c.gridwidth= GridBagConstraints.REMAINDER;
    c.fill= GridBagConstraints.BOTH;
    layout.setConstraints(treeCanvas.getComponent(), c);
    add(treeCanvas.getComponent());

    c.gridx= 0; c.gridy= 50;
    c.weightx= 100; c.weighty= 100;
    c.gridheight= 40; c.gridwidth= 50;
    c.fill= GridBagConstraints.BOTH;
    layout.setConstraints(prgTableCanvas.getComponent(), c);
    add(prgTableCanvas.getComponent());

    c.gridx= 55; c.gridy= 50;
    c.weightx= 100; c.weighty= 100;
    c.gridheight= 40; c.gridwidth= GridBagConstraints.REMAINDER;
    c.fill= GridBagConstraints.BOTH;
    layout.setConstraints(traceCanvas.getComponent(), c);
    add(traceCanvas.getComponent());

    c.gridx= 0; c.gridy= 95;
    c.weightx= 0; c.weighty= 0;
    c.gridheight= 5; c.gridwidth= 8;
    c.fill= GridBagConstraints.NONE;
    c.anchor= GridBagConstraints.WEST;
    layout.setConstraints(updateButton, c); add(updateButton);

    c.gridx= 10; c.gridy= 95;
    c.weightx= 100; c.weighty= 0;
    c.gridheight= 5; c.gridwidth= 65;
    c.fill= GridBagConstraints.HORIZONTAL;
    layout.setConstraints(inputField, c); add(inputField);

    c.gridx= 70; c.gridy= 95;
    c.weightx= 0; c.weighty= 0;
    c.gridheight= 5; c.gridwidth= 8;
    c.fill= GridBagConstraints.NONE;
    c.anchor= GridBagConstraints.WEST;
    layout.setConstraints(startStepButton, c); add(startStepButton);

    c.gridx= 80; c.gridy= 95;
    c.weightx= 0; c.weighty= 0;
    c.gridheight= 5; c.gridwidth= 8;
    c.fill= GridBagConstraints.NONE;
    c.anchor= GridBagConstraints.WEST;
    layout.setConstraints(runStopButton, c); add(runStopButton);
    
    c.gridx= 90; c.gridy= 95;
    c.weightx= 0; c.weighty= 0;
    c.gridheight= 5; c.gridwidth= 8;
    c.fill= GridBagConstraints.HORIZONTAL;
    c.anchor= GridBagConstraints.WEST;
    layout.setConstraints(runDelayScrollbar, c); add(runDelayScrollbar);
    
  }

  public void paint(Graphics g) {
  }

  public boolean action(Event evt, Object arg) {
    if (evt.target == updateButton) {
      if ("update".equals(updateButton.getLabel())) {
	updateButton.setLabel("ignore"); extFrame.setUpdate(true);
      }
      else {
	updateButton.setLabel("update"); extFrame.setUpdate(false);
      }
    }
    else if (evt.target == startStepButton) {
      if (startStepButton.getLabel().equals("step")) {
	doStep();
      }
      else {
	doStart();
      }
      return true;
    }
    else if (evt.target == runStopButton) {
      if (runStopButton.getLabel().equals("run")) {
	doRun();
      }
      else {
	doStop();
      }
      return true;
    }
    else if (evt.target == inputField) {
      doStart();
    }
    return false;
  }

  public boolean handleEvent(Event event) {
    if (event.target == runDelayScrollbar) {
      updateRunDelay();
      return true;
    } 
    else {
      return super.handleEvent(event);
    }
  }
  
  private void updateRunDelay() {
    runDelay= 1000*(1<<10)/(1 << runDelayScrollbar.getValue());
  }

  private void doRun() {
    startStepButton.disable();
    runStopButton.setLabel("stop");
    if (!isStarted) doStart();
    Thread loopThread= new Thread(this);
    loopThread.start();
  }

  public void run() {
    if (Thread.currentThread() == flashThread) {
      doFlash();
    }
    else {
      doLoop();
    }
  }

  void doLoop() {
    do {
      try {
	Thread.sleep(runDelay);
      }
      catch (InterruptedException e) {
      }
      doStep();
    } while (isStarted && runStopButton.getLabel().equals("stop"));
    startStepButton.enable();
    startStepButton.setLabel(isStarted ? "step" : "start");
    runStopButton.setLabel("run");
  }

  void doFlash() {
    startStepButton.disable(); runStopButton.disable();
    startStepButton.setLabel("start"); 
    for (int i= 0; i < N_FLASH; i++) {
      try {
	startStepButton.setBackground(Color.red);
	//Somehow in Netscape 3.0 system's UI thread 
	//doesn't always get the disable message.
	startStepButton.disable(); 
	repaint();
	Thread.sleep(FLASH_DELAY);
	startStepButton.setBackground(buttonBG);
	startStepButton.disable(); 
	repaint();
	Thread.sleep(FLASH_DELAY);
      }
      catch (InterruptedException e) {
      }
    }
    startStepButton.enable(); runStopButton.enable();
  }

  private void doStart() {
    if (isStarted) {
      parser.reset(); doStep();
    }
    startStepButton.setLabel("step");
    parseDisplay.reset();
    scanner.reset(inputField.getText());
    isStarted= true;
    runStopButton.enable();
    parserThread= new Thread(parser, "Parser");
    parserThread.start();
  }

  private void doStop() {
    runStopButton.setLabel("run");
  }

  private void doStep() {
    isStarted= parser.step();
    if (!isStarted) { 
      flashThread= new Thread(this);
      flashThread.start();
    }
  }

  static public void main(String args[]) {
    ParsDemo parsDemo= new ParsDemo(args);
    new AppletFrame(parsDemo, 500, 300);
  }

  static final int FULL_APPLET_CONTEXT= 0;    //full applet context available.
  static final int PARTIAL_APPLET_CONTEXT= 1; //only read rec-descent file.
  static final int CMD_LINE_CONTEXT= 2;	      //invoked from command line.

  private int context= FULL_APPLET_CONTEXT;
  private int algorithm= REC_DESCENT_PARSER;
  private final String fName= "zdu/parsdemo/SimpCompRecDescent.jpx";
  private int runDelay;
  private ScrollableCanvas prgTableCanvas;
  private TreeCanvas treeCanvas;
  private TraceCanvas traceCanvas;
  private Button updateButton;
  private TextField inputField;
  private Button startStepButton;
  private Button runStopButton;
  private Scrollbar runDelayScrollbar;
  private StepParser parser;
  private Thread parserThread;
  private Scanner scanner;
  private ParseDisplay parseDisplay;
  private ExtFrame extFrame;
  private boolean isStarted;
  private Thread flashThread;
  private Color buttonBG;

  private static final int REC_DESCENT_PARSER= 0;
  private static final int LL_PARSER= 1;
  private static final int SR_PARSER= 2;

  private static final double TREE_WIDTH= 0.9;
  private static final double TREE_HEIGHT= 0.5;

  private static final double SELECT_WIDTH= 0.6;
  private static final double SELECT_HEIGHT= 0.3;

  private static final double TRACE_WIDTH= 0.3;
  private static final double TRACE_HEIGHT= 0.3;

  private static final String INIT_INPUT= "a:= 1; b:= 1 + 2";

  private static final int FLASH_DELAY= 1000;
  private static final int N_FLASH= 2;
}
