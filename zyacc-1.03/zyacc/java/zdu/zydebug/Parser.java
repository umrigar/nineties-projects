/*

File:	 Parser.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/07/27 22:32:43 zdu"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.zydebug;

import zdu.zydebug.Act;
import zdu.zydebug.Scanner;

//This class parses trace lines from the parser.  Parsing is simple
//because each trace line is assumed to consist of either a prompt
//or a set of key-value pairs terminated by a newline.
class Parser {
  Parser(Scanner scanner) {
    this.scanner= scanner;
    this.tok= new Token();
  }

  void nextAct(Act act) {
    act.value= null;
  out:
    do {
      scanner.nextTok(tok);
      switch (tok.type) {
	case Scanner.NL:
	  break out;
	case Scanner.PROMPT:
	  act.type= Act.PROMPT;
	  break out;
	case Scanner.EOF:
	  act.type= Act.EOF;
	  break out;
	default: { //must be a %-keyword.
	  int t= tok.type;
	  scanner.nextTok(tok);
	  switch(t) {
	    case Scanner.KEY_ACTION:
	      if (tok.len != 1) {
		throw new InternalError("action argument not of length 1");
	      }
	      switch ((int) (tok.buf[tok.index])) {
		case 'A':
		  act.type= Act.ACCEPT;
		  break;
		case 'E':
		  act.type= Act.ERROR;
		  break;
		case 'R':
		  act.type= Act.REDUCE;
		  break;
		case 'S':
		  act.type= Act.SHIFT;
		  break;
		default:
		  throw new InternalError("invalid action type");
	      }
	      break;
	    case Scanner.KEY_LINE_NUM:
	      act.lineNum= 
		Integer.parseInt(new String(tok.buf, 0, tok.index, tok.len));
	      break;
	    case Scanner.KEY_NON_TERM:
	      act.nonTerm= new String(tok.buf, 0, tok.index, tok.len);
	      break;
	    case Scanner.KEY_RULE_LEN:
	      act.ruleLen= 
		Integer.parseInt(new String(tok.buf, 0, tok.index, tok.len));
	      break;
	    case Scanner.KEY_RULE_NUM:
	      act.ruleN= 
		Integer.parseInt(new String(tok.buf, 0, tok.index, tok.len));
	      break;
	    case Scanner.KEY_STACK:
	      act.stack= new String(tok.buf, 0, tok.index, tok.len);
	      break;
	    case Scanner.KEY_STATE:
	      act.state= 
		Integer.parseInt(new String(tok.buf, 0, tok.index, tok.len));
	      break;
	    case Scanner.KEY_TOKEN:
	      act.token= new String(tok.buf, 0, tok.index, tok.len);
	      break;
	    case Scanner.KEY_VALUE:
	      if (tok.len > 0) {
		act.value= new String(tok.buf, 0, tok.index, tok.len);
	      }
	      break;
            default:
	      throw new InternalError("bad key token");
	  } //switch (t)
	} //default:
      } //switch (tok.type)
    } while (true); 
  } 

  private Scanner scanner;
  private Token tok;
}
