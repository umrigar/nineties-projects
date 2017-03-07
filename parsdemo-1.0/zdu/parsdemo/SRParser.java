/*

File:	 SRParser.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:40:27 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.Grammar;
import zdu.parsdemo.GramSym;
import zdu.parsdemo.NonTerm;
import zdu.parsdemo.ParseDisplay;
import zdu.parsdemo.Rule;
import zdu.parsdemo.StepParser;
import zdu.parsdemo.Terminal;

class SRParser extends StepParser {

  SRParser(Grammar grammar, SRTable tab, Scanner scanner, 
	    ParseDisplay parseDisplay) {
    super(grammar, scanner, parseDisplay);
    this.tab= tab;
  }

  SRParser(Grammar grammar, SRTable tab, Scanner scanner) {
    this(grammar, tab, scanner, null);
  }

  protected void parse() throws ParseException, ParseResetException { 
    actState= INIT;
    do {
      waitToStep();
    } while (!didLastUpdate);
  }

  protected void update() throws ParseException {
    Terminal terminal= grammar.terminal(tok.getTokNum());
    switch (actState) {
      case INIT: {
	ParseNode n0= new ParseNode("$S", false);
	OffsetTree t0= new OffsetTree(n0);
	SRState start= new SRState(0);
	SRParseStkEntry e= new SRParseStkEntry(start, t0);
	stk.pushT(e);
	xSelect= terminal; ySelect= start; treeSelect= t0;
	extFrameTag= "State-" + start + "-without-closure-items";
	actState= SR_STEP;
	break; 
      }
      case SR_STEP: {	
        xSelect= ySelect= null;
	if (stk.empty()) {
	  treeSelect= null;
	  didLastUpdate= true;
	}
	else { //!stk.empty()
	  SRParseStkEntry e= (SRParseStkEntry) stk.peekT();
	  SRState state0= e.getState();
	  SRAct act= (SRAct) tab.get(state0, terminal);
	  if (act == null) {
	    ParseNode n= new ParseNode("ERROR", true);
	    OffsetTree t= new OffsetTree(n);
	    treeSelect= t;
	    ParseStkEntry err= new ParseStkEntry(t);
	    stk.peekT().getOffsetTree().addKid(t); stk.pushT(err);
	    extFrameTag= "State-" + state0 + "-without-closure-items";
	    didLastUpdate= true;
	    throw new ParseException();
	  }
	  else {
	    switch (act.getType()) {
	      case SRAct.SHIFT: {
		ParseNode n= new ParseNode(tok.getText(), false);
		OffsetTree t= new OffsetTree(n);
		SRState state= act.getState();
		stk.pushT(new SRParseStkEntry(act.getState(), t));
		forest.addElement(t);
		tok= scanner.nextTok();
		xSelect= grammar.terminal(tok.getTokNum()); 
		ySelect= state; treeSelect= t;
		extFrameTag= "State-" + state + "-without-closure-items";
		actState= SR_STEP;
		break;
	      }
	      case SRAct.REDUCE: {
		Rule rule= act.getRule();
		if (rule.getNum() == 0) { //accept
		  forest.pop();  //pop EOF from forest.
		  stk.popT();    //pop EOF from stk.
		  stk.popT();    //pop full parse tree from stk.
		  stk.popT();    //pop $S.
		  xSelect= ySelect= null;
		  didLastUpdate= true; treeSelect= null;
		}
		else {
		  gotoNonTerm= rule.getLHS();
		  ParseNode n= new ParseNode(gotoNonTerm.toString(), true);
		  OffsetTree t= new OffsetTree(n);
		  int len= rule.rhsLen();
		  for (int i= 0; i < len; i++) {
		    OffsetTree kid= stk.popT().getOffsetTree();
		    forest.pop();
		    t.addKid(kid, -1);
		  }
		  forest.push(t);
		  actState= GOTO_STEP;
		  xSelect= gotoNonTerm;
		  ySelect= ((SRParseStkEntry)(stk.peekT())).getState();
		  treeSelect= t;
		  extFrameTag= "State-" + ySelect + "-without-closure-items";
		}
		break;
	      }
  	      default:
	        throw new InternalError("SRParser: shift/reduce expected");
	    }
	  }
	} //else !stk.empty()
	break;
      }
      case GOTO_STEP: {
	SRAct gotoAct= (SRAct)
	  tab.get(((SRParseStkEntry)(stk.peekT())).getState(), gotoNonTerm);
	SRState state= gotoAct.getState();
	stk.pushT(new SRParseStkEntry(state, (OffsetTree)(forest.peek())));
	xSelect= grammar.terminal(tok.getTokNum()); 
	ySelect= state;
	treeSelect= stk.peekT().getOffsetTree();
	extFrameTag= "State-" + state + "-without-closure-items";
	actState= SR_STEP;
	break;
      }
      default:
	throw new InternalError("LL1Parser: bad act in update()");
    }
  }

  private SRTable tab;
  private NonTerm gotoNonTerm;	//nonterm for goto transition.
  private int actState;		//one of following constants.

  static private final int INIT= 0;
  static private final int SR_STEP= INIT + 1;  
  static private final int GOTO_STEP= SR_STEP + 1;
};
