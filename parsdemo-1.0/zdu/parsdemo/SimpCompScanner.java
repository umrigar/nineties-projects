/*

File:	 SimpCompScanner.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:40:50 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import java.util.Hashtable;


/*

During initialization, we enter all reserved words into the identifier
table in such a way that their IDs will be less than some quantity.
Then when we see something which looks like an identifier, we check
whether its ID is less than the same quantity: if so, it is a reserved
word; else it is indeed an identifier.

*/

/*			     DATA					*/

class SimpCompScanner extends Scanner
{
  //Reserved word tokens.
  static private final int RESERVED_LO= 256;	//Compound token #s > this.
  static private final int N_RESERVED= 2;	//# of reserved words.
  static final int DIV_TOK= RESERVED_LO;
  static final int MOD_TOK= DIV_TOK + 1;

  //Other compound tokens.
  static final int ASSGN_TOK= RESERVED_LO + N_RESERVED;
  static final int ID_TOK= ASSGN_TOK + 1;
  static final int NUM_TOK= ID_TOK + 1;

  //Hashtable for storing reserved words and identifiers.
  private Hashtable ids= new Hashtable();

  SimpCompScanner()
  {
    super();
    initIDs();
    initToks();
  }

  private void initToks() {
    addTok(DIV_TOK, "DIV");
    addTok(MOD_TOK, "MOD");
    addTok(ASSGN_TOK, ":=");
    addTok(ID_TOK, "ID");
    addTok(NUM_TOK, "NUM");
  }

  private void initIDs()
  {  
    ids.put("div", new Integer(ids.size())); 
    ids.put("mod", new Integer(ids.size()));
  }

  public int getID(String text)
  {  
    Integer val= (Integer)ids.get(text);
    if (val == null) {
      val= new Integer(ids.size());
      ids.put(text, val);
    }
    return val.intValue();
  }

  private int getIDTok(String text)
  {
    int id= getID(text);
    if (id < N_RESERVED) {
      return id + RESERVED_LO;
    }
    else {
      return ID_TOK;
    }
  }

  public Token nextTok()
  {
    int t;
    char c= peek();
    StringBuffer tokBuf= new StringBuffer();
    while (true) {
      if (c == SENTINEL_CHAR) {
	t= EOF_TOK; tokBuf.append(eofName);
	break;
      }
      for (c= peek(); Character.isSpace(c); 
	   advance() , c= peek()) {
      }
      if (Character.isLetter(c)) {
	do {
	  tokBuf.append(c);
	  advance(); c= peek();
	} while (Character.isLetterOrDigit(c));
	t= getIDTok(tokBuf.toString());
	break;
      }
      else if (Character.isDigit(c)) {
	do {
	  tokBuf.append(c);
	  advance(); c= peek();
	} while (Character.isDigit(c));
	t= NUM_TOK;
	break;
      }
      else if (c == ':' && peek(1) == '=') {
	tokBuf.append(":="); advance(); advance();
	t= ASSGN_TOK;
	break;
      }
      else if (c == '#') {
	do {
	  advance();
	} while (peek() != '\n');
	advance();
      }
      else {
	tokBuf.append(c);
	t= c; advance();
	break;
      }
    }
    setText(tokBuf.toString());
    return new Token(t, yytext);
  }

  public static void main(String[] args) 	//Test program.
  {
    if (args.length != 1) {
      System.err.println("usage: Scanner STRING");
      System.exit(1);
    }
    else {
      Scanner scanner= new SimpCompScanner();
      scanner.reset(args[0]);
      for (Token t= scanner.nextTok(); t.getTokNum() != Scanner.EOF_TOK; 
	   t= scanner.nextTok()) {
	switch (t.getTokNum()) {
	  case DIV_TOK:
	    System.out.print("DIV_TOK");
	    break;
	  case MOD_TOK:
	    System.out.print("MOD_TOK");
	    break;
	  case ID_TOK:
	    System.out.print("ID_TOK: " + t.getText());
	    break;
	  case NUM_TOK:
	    System.out.print("NUM_TOK: " + t.getText());
	    break;
	  case ASSGN_TOK:
	    System.out.print(":=");
	    break;
	  default:
	    System.out.print("'" + t.getText() + "'");
	    break;
        }
	System.out.println(" restline= `" + scanner.restLine() + "'");
      }
    }
  }
}

