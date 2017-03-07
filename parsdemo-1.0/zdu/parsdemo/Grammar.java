/*

File:	 Grammar.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:33:08 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.GramSym;
import zdu.parsdemo.NonTerm;
import zdu.parsdemo.Rule;
import zdu.parsdemo.Terminal;

import java.util.*;

abstract class Grammar {

  Terminal terminal(int tokNum) {
    Terminal t= (Terminal) terminalsByTok.get(new Integer(tokNum));
    return (t == null) ? ERR : t;
  }

  Rule rule(int ruleN) {
    return (Rule) rules.elementAt(ruleN);
  }


  protected Terminal terminal(String name) {
    Terminal t= null;
    try {
      t= terminal(name, (int)name.charAt(1));
    }
    catch (StringIndexOutOfBoundsException e) {
      throw new InternalError("Empty terminal name " + e);
    }
    return t;
  }

  protected Terminal terminal(String name, int tok) {
    GramSym sym= (GramSym) gramSyms.get(name);
    if (sym == null) {
      sym= new Terminal(name, terminals.size(), tok);
      gramSyms.put(name, sym);
      terminalsByTok.put(new Integer(tok), sym);
      terminals.addElement(sym);
    }
    else if (sym.getType() != GramSym.TERMINAL) {
      throw new InternalError("non-terminal symbol " + name + 
			      "used as terminal");
    }
    return (Terminal)sym;
  }

  protected NonTerm nonTerm(String name) {
    GramSym sym= (GramSym) gramSyms.get(name);
    if (sym == null) {
      sym= new NonTerm(name, nonTerms.size());
      gramSyms.put(name, sym);
      nonTerms.addElement(sym);
    }
    else if (sym.getType() != GramSym.NONTERM) {
      throw new InternalError("terminal symbol " + name + 
			      "used as non-terminal");
    }
    if (startSym == null) startSym= (NonTerm)sym;
    return (NonTerm)sym;
  }

  void setStartSym(NonTerm start) {
    startSym= start;
  }

  NonTerm getStartSym() {
    return startSym;
  }

  GramSym getItem(int itemN) {
    GramSym sym= null;
    try {
      sym= (GramSym) rhsItems.elementAt(itemN);
    }
    catch (ArrayIndexOutOfBoundsException e) {
      throw new InternalError("bad item # " + e);
    }
    return sym;
  }

  Vector getTerminals() {
    return terminals;
  }

  Vector getNonTerms() {
    return nonTerms;
  }

  Vector getRules() {
    return rules;
  }

  Enumeration rulesEnum() {
    return rules.elements();
  }

  Enumeration terminalsEnum() {
    return terminals.elements();
  }

  Enumeration nonTermsEnum() {
    return nonTerms.elements();
  }

  public String toString() {
    StringBuffer b= new StringBuffer();
    for (Enumeration r= rulesEnum(); r.hasMoreElements(); ) {
      Rule rule= (Rule) r.nextElement();
      b.append(rule.getText() + "\n");
    }
    return b.toString();
  }

  protected void beginRule(NonTerm lhs) {
    if (rhsItems.size() == 0) {  //no rules added so far: add augmenting rule.
      rhsItems.addElement(startSym); 
      rhsItems.addElement(EOF);
      Rule rule= new Rule(augmentedStartSym, 0, 2, rules.size(), this);
      rules.addElement(rule); rhsItems.addElement(rule);
    }
    currentLHS= lhs; currentLC= rhsItems.size();
  }

  protected void endRule() {
    Rule rule= new Rule(currentLHS, currentLC, rhsItems.size(), rules.size(),
			this);
    rules.addElement(rule); rhsItems.addElement(rule); 
    currentLHS= null; currentLC= -1;
  }

  protected void addRHS(GramSym sym) {
    rhsItems.addElement(sym);
  }

  private NonTerm startSym= null;
  private Hashtable gramSyms= new Hashtable();
  private Hashtable terminalsByTok= new Hashtable();
  private Vector rules= new Vector();
  private Vector rhsItems= new Vector();
  private Vector terminals= new Vector();
  private Vector nonTerms= new Vector();
  private NonTerm currentLHS= null;	//non-term of rule being added.
  private int currentLC= -1;		//left-corner of current rule.

  final private NonTerm augmentedStartSym= nonTerm("$S");
  final Terminal EOF= terminal(Scanner.eofName, Scanner.EOF_TOK);
  final Terminal ERR= terminal("ERR_TOK", -1);

};
