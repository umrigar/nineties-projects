/*

File:	 RecParser.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:40:12 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.Grammar;
import zdu.parsdemo.OffsetTree;
import zdu.parsdemo.ParseNode;
import zdu.parsdemo.Scanner;

import java.util.Stack;
import java.util.EmptyStackException;

abstract class RecParser extends StepParser {
  
  RecParser(Grammar grammar, Scanner scanner, ParseDisplay parseDisplay) {
    super(grammar, scanner, parseDisplay);
    reset2();
  }

  protected void reset2() {
    super.reset2();
  }

  protected void update() throws ParseException {
    switch (act) {
      case ACCEPT: {
	stk.popT();
	ySelect= new Integer(lineNum); treeSelect= null;
	didLastUpdate= true;
	break;
      }
      case CALL: {
	GramSym nonTerm= gramSym;
	ParseNode n= new ParseNode(nonTerm.toString(), false);
	OffsetTree t= new OffsetTree(n);
	ParseStkEntry e= new ParseStkEntry(t);
	if (stk.empty()) {
	  forest.addElement(t);
	}
	else {
	  stk.peekT().getOffsetTree().addKid(t);
	}
	stk.pushT(e);
	ySelect= new Integer(lineNum);
	treeSelect= t;
	break;
      }
      case MATCH: {
	Terminal terminal= (Terminal)gramSym;
	if (tok.getTokNum() == terminal.getTok()) {
	  String name= tok.getText();
	  ParseNode n= new ParseNode(name, true);
	  OffsetTree t= new OffsetTree(n);
	  ParseStkEntry e= new ParseStkEntry(t);
	  stk.peekT().getOffsetTree().addKid(t);
	  ySelect= new Integer(lineNum);
	  tok= scanner.nextTok();
	  treeSelect= t;
	}
	else {
	  ParseNode n= new ParseNode("ERROR", true);
	  OffsetTree t= new OffsetTree(n);
	  ParseStkEntry e= new ParseStkEntry(t);
	  stk.peekT().getOffsetTree().addKid(t); stk.pushT(e);
	  ySelect= new Integer(lineNum);
	  didLastUpdate= true;
	  throw new ParseException();
	}
	break;
      }
      case RETURN: {
	treeSelect= stk.popT().getOffsetTree();
	ySelect= new Integer(lineNum);
	break;
      }
      default:
	throw new InternalError("RecParser: bad act in update()");
    }
  }

  protected void call(NonTerm nonTerm, int ruleN, int lineNum) 
    throws ParseException, ParseResetException
  {
    this.act= CALL; this.lineNum= lineNum; this.gramSym= nonTerm;
    extFrameTag= "rule" + ruleN;
    waitToStep();
  }

  protected void match(Terminal terminal, int ruleN, int lineNum) 
    throws ParseException, ParseResetException 
  {
    this.act= MATCH; this.lineNum= lineNum; this.gramSym= terminal;
    extFrameTag= "rule" + ruleN;
    waitToStep();
  }


  protected void ret(int ruleN, int lineNum) 
    throws ParseException, ParseResetException
  {
    this.act= RETURN; this.lineNum= lineNum;
    extFrameTag= "rule" + ruleN;
    waitToStep();
  }

  protected void accept(int ruleN, int lineNum) 
    throws ParseException, ParseResetException
  {
    this.act= ACCEPT; this.lineNum= lineNum;
    extFrameTag= "rule" + ruleN;
    waitToStep();
  }


  private GramSym gramSym;		//grammar symbol being processed.
  private int lineNum;			//current line #.
  private int act;			//current update action.

  static private final int ACCEPT= 0;
  static private final int CALL= ACCEPT + 1;
  static private final int MATCH= CALL + 1;
  static private final int RETURN= MATCH + 1;

}
