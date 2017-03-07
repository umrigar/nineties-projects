/*

File:	 StepParser.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:40:50 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.Grammar;
import zdu.parsdemo.OffsetForest;
import zdu.parsdemo.ParseStk;
import zdu.parsdemo.ParseDisplay;
import zdu.parsdemo.ParseException;
import zdu.parsdemo.ParseResetException;
import zdu.parsdemo.ParseNode;
import zdu.parsdemo.Scanner;

import java.util.Stack;
import java.lang.ArrayIndexOutOfBoundsException;
import java.io.*;
import java.lang.InternalError;

/** Abstract interface to a parser which can be stepped.  Meant to be
 * run in a separate thread.
 */

abstract class StepParser implements Runnable {

  public StepParser(Grammar grammar, Scanner scanner, 
		    ParseDisplay parseDisplay) {
    inParse= available= false;
    this.grammar= grammar; this.scanner= scanner;
    this.parseDisplay= parseDisplay;
  }

  public StepParser(Grammar grammar, Scanner scanner) {
    this(grammar, scanner, null);
  }

  public void run() {
    setInParse(true);
    tok= scanner.nextTok();		//initialize lookahead token.
    try {
      parse();
    }
    catch (ParseResetException e) {
//      System.err.println("Parse reset");
    }      
    catch (ParseException e) {
//      System.err.println("Parse error");
    }
    finally {
      setInParse(false);
    }
  }

  //Prevent more than 1 thread from using parser concurrently.
  public synchronized void setInParse(boolean cond) {
    while (inParse == cond) {
      try {
	wait();
      } catch (InterruptedException e) {
      }
    }
    inParse= cond;
    if (cond) reset2();
    notify();
  }


  public synchronized void reset() {
    doReset= true;
  }

  protected void reset2() {
    doReset= available= didLastUpdate= false;
    stk= new ParseStk(); forest= new OffsetForest();
  }

  /** Step parser by 1 parse step.  
   */
  public synchronized boolean step() {
    while (available == false) {
      try {
	wait();
      } catch (InterruptedException e) {
      }
    }
    available= false;
    notify();
    return !didLastUpdate;
  }
  
  protected abstract void parse() throws ParseException, ParseResetException;

  protected abstract void update() throws ParseException;

  protected synchronized void waitToStep() 
       throws ParseException, ParseResetException {
    while (available == true  && !doReset) {
      try {
	wait();
      } catch (InterruptedException e) {
      }
    }
    if (doReset) {
      doReset= false;
      throw new ParseResetException();
    }
    boolean hadParseException= false;
    try {
      update();
    }
    catch (ParseException e) {
      hadParseException= true;
    }
    finally {
      if (parseDisplay != null) {
	parseDisplay.update(forest, xSelect, ySelect, treeSelect, 
			    stk.toStringT(), 
			    tok.getText() + scanner.restLine(), extFrameTag);
      }
      else {
	record(forest, xSelect, ySelect, stk.toStringT() + " :: " + 
	       tok.getText() + scanner.restLine());
      }
    }
    available= true;
    notify();
    if (hadParseException) throw new ParseException();
  }
  
  private void record(OffsetForest forest, Object xSelect, Object ySelect,
		      String trace) {
    StringBuffer b= new StringBuffer();
    try {
      for (int i= 0; i < forest.size(); i++) {
	b.append("Tree(" + i + ")=\n");
	b.append(((Tree)(forest.elementAt(i))).treeString() + "\n");
      }
    }
    catch (ArrayIndexOutOfBoundsException e) {
      throw new InternalError("empty forest in StepParser");
    }
    b.append("Select= (" + xSelect + ", " + ySelect + ")\n");
    b.append(trace + "\n");
    log= b.toString();
  }

  protected Scanner scanner= null;
  protected Token tok= null;
  protected ParseStk stk;
  protected OffsetForest forest;
  protected Grammar grammar;
  protected Object xSelect;
  protected Object ySelect;
  protected OffsetTree treeSelect;
  protected String extFrameTag;
  protected boolean didLastUpdate;
  private String log;			//log last step when no parseDisplay.
  private boolean doReset= false;
  private boolean inParse= false;
  private boolean available= false;
  private ParseDisplay parseDisplay= null;

  //cannot handle parse errors.
  public static void main(String[] args)
  {
    String input= "a:= 1";
    int algN= 0;
    int nAlgs= 2;
    switch (args.length) {
      case 0:
	break;
      case 1:
	try {
	  algN= Integer.parseInt(args[0]);
	} 
	catch (NumberFormatException e) {
	  algN= 0; input= args[0];
	}
	break;
      case 2:
	try {
	  algN= Integer.parseInt(args[0]);
	  input= args[1];
	} 
	catch (NumberFormatException e) {
	  System.err.println("usage: StepParser [algN] [input]");
	  System.exit(1);
	}
	break;
      default: 
	System.err.println("usage: StepParser [algN] [input]");
	System.exit(1);
	break;
    }
    SimpCompScanner scanner= new SimpCompScanner();
    StepParser parser= null;
    switch (algN) {
      case 0: {
        SimpCompLL1Gram grammar= new SimpCompLL1Gram();
	parser= new SimpCompRecDescent(grammar, scanner);
	break;
      }
      case 1: {
        SimpCompLL1Gram grammar= new SimpCompLL1Gram();
	parser= new LL1Parser(grammar, new SimpCompLL1Table(grammar), scanner);
	break;
      }
      case 2: {
        SimpCompSRGram grammar= new SimpCompSRGram();
	parser= new SRParser(grammar, new SimpCompSRTable(grammar), scanner);
	break;
      }
      default:
	System.err.println("illegal algorithm # (0: rec-descent; 1: LL(1))");
	System.exit(1);
    }
    scanner.reset(input);
    Thread parserThread= new Thread(parser, "Parser");
    DataInputStream in= new DataInputStream(System.in);
    parserThread.start();
    boolean continueParse= true;
    do {
      System.out.print("Step? "); System.out.flush();
      try {
        if (in.readLine().length() > 1) {
          parser.reset(); break;
        }
      }
      catch(EOFException e) {
        System.err.println("EOF exception in SimpCompRecDescent.main");
      }
      catch(IOException e) {
        System.err.println("I/O exception in SimpCompRecDescent.main");
      }
      continueParse= parser.step();
      System.out.println(parser.log);
    } while (continueParse);
    System.out.println("Parser is " + 
	               (parserThread.isAlive() ? "alive" : "dead"));
    try {
      Thread.currentThread().sleep(2000);
    }
    catch(InterruptedException e) {
    }
    System.out.println("Parser is " + 
	               (parserThread.isAlive() ? "alive" : "dead"));
  }
}
