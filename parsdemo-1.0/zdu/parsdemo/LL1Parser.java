/*

File:	 LL1Parser.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:33:42 umrigar"

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

class LL1Parser extends StepParser {

  LL1Parser(Grammar grammar, Table2 tab, Scanner scanner, 
	    ParseDisplay parseDisplay) {
    super(grammar, scanner, parseDisplay);
    this.tab= tab;
  }

  LL1Parser(Grammar grammar, Table2 tab, Scanner scanner) {
    this(grammar, tab, scanner, null);
  }

  protected void parse() throws ParseException, ParseResetException { 
    act= INIT;
    do {
      waitToStep();
      act= LL1_STEP;
    } while (!didLastUpdate);
  }

  protected void update() throws ParseException {
    Terminal terminal= grammar.terminal(tok.getTokNum());
    switch (act) {
      case INIT: {
	NonTerm start= grammar.getStartSym();
	ParseNode n0= new ParseNode(start.toString(), false);
	OffsetTree t0= new OffsetTree(n0);
	LL1ParseStkEntry e= new LL1ParseStkEntry(start, 0, t0);
	stk.pushT(e); forest.addElement(t0);
	xSelect= terminal; ySelect= start; treeSelect= t0;
	extFrameTag= "rule0";
	break;
      }
      case LL1_STEP: {	
        xSelect= ySelect= null;
	if (stk.empty()) {
	  treeSelect= null;
	  didLastUpdate= true;
	}
	else { //!stk.empty()
	  LL1ParseStkEntry e= (LL1ParseStkEntry) stk.popT();
	  GramSym sym= e.getGramSym();
	  if (sym.getType() == GramSym.TERMINAL) {
	    if (tok.getTokNum() == ((Terminal)sym).getTok()) {
	      OffsetTree t= e.getOffsetTree();
	      ParseNode n= (ParseNode)t.getInfo();
	      treeSelect= t;
	      n.setLabel(tok.getText());
	      tok= scanner.nextTok();
	    }
	    else {
	      parseError();
	    }
	  }
	  else { //nonterminal.
	    Rule rule= (Rule) tab.get((NonTerm)sym, 
				      grammar.terminal(tok.getTokNum()));
	    if (rule == null) {
	      xSelect= terminal; ySelect= sym;
	      parseError();
	    }
	    else {
	      OffsetTree parent= e.getOffsetTree();
	      int ruleN= rule.getNum();
	      for (int i= rule.rhsLen() - 1; i >= 0; i--) {
		GramSym s= rule.rhsSym(i);
		ParseNode n= new ParseNode(s.toString(), false);
		OffsetTree t= new OffsetTree(n);
		treeSelect= t;
		parent.addKid(t, -1);
		stk.pushT(new LL1ParseStkEntry(s, ruleN, t));
	      }
	    }
	  } //nonterminal.
	} //else !stk.empty()
	if (stk.size() > 0) {
	  LL1ParseStkEntry llEntry= (LL1ParseStkEntry)stk.peekT();
	  treeSelect= stk.peekT().getOffsetTree();
	  GramSym s= llEntry.getGramSym();
	  int ruleN= llEntry.getRuleN();
	  if (s.getType() == GramSym.NONTERM) {
	    xSelect= grammar.terminal(tok.getTokNum()); ySelect= s;
	  }
	  extFrameTag= "rule" + ruleN;	  
	}
	break;
      }
      default:
	throw new InternalError("LL1Parser: bad act in update()");
    }
  }

  private void parseError() throws ParseException {
    ParseNode n= new ParseNode("ERROR", true);
    OffsetTree t= new OffsetTree(n);
    ParseStkEntry e= new ParseStkEntry(t);
    stk.peekT().getOffsetTree().addKid(t); stk.pushT(e);
    didLastUpdate= true;
    throw new ParseException();
  }

  private int act;
  private Table2 tab;

  static private final int INIT= 0;
  static private final int LL1_STEP= INIT + 1;  
};
