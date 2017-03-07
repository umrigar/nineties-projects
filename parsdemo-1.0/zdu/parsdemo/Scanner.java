/*

File:	 Scanner.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:40:30 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.Token;

import java.io.DataInputStream;
import java.io.EOFException;
import java.io.IOException;
import java.util.Hashtable;
import java.lang.InternalError;

abstract class Scanner {
  static public final int EOF_TOK= 0;		//Traditional EOF token.

  public abstract Token nextTok();
  public Scanner() {
    addTok(EOF_TOK, eofName);
  }

  public String tokName(int t) {
    return   (t == 0) ? eofName
           : (t < 256) ? "'" + (new Character((char)t)).toString() + "'"
           : (String) (tokNames.get(new Integer(t)));
  }

  public String currentLine() {
    int i;
    if (input.charAt(index) == SENTINEL_CHAR) {
      return "";
    }
    else {
      for (i= index + 1; 
	   input.charAt(i) != '\n' && input.charAt(i) != SENTINEL_CHAR;
	   i++) {
      }
      return input.substring(lastNLIndex + 1, i);
    }
  }

  public int currentOffset() {
    return index - (lastNLIndex + 1);
  }

  public String restLine() {
    int i;
    if (index >= input.length()) {
      return "";
    }
    else {
      for (i= index; 
	   input.charAt(i) != '\n' && input.charAt(i) != SENTINEL_CHAR;
	   i++) {
      }
      return input.substring(index, i);
    }
  }

  public String lexemeText() {
    return yytext;
  }
       
  public void reset() {
    index= 0;
  }

  public void reset(StringBuffer input) {
    input.append(SENTINEL_CHAR);
    this.input= input.toString();
    reset();
  }

  public void reset(String input) {
    StringBuffer b= new StringBuffer(input);
    reset(b);
  }

  public void reset(DataInputStream s) {
    StringBuffer b= new StringBuffer();
    try {
      while (true) b.append(s.readChar());
    }
    catch (EOFException e) {
    }
    catch (IOException e) {
      throw new InternalError("Scanner: I/O error");
    }
    reset(b);
  }

  public int lineNum() {
    return lineN;
  }

  static protected final char SENTINEL_CHAR= '\0';

  protected final char peek() {
    return input.charAt(index);
  }

  protected void addTok(int tok, String name) {
    tokNames.put(new Integer(tok), name);
  }

  protected final char peek(int i) {
    return 
      (index + i < input.length()) ? input.charAt(index + i ) : SENTINEL_CHAR;
  }

  protected final void advance() {
    if (input.charAt(index) == '\n') {
      lastNLIndex= index; lineN++;
    }
    index++;
  }

  protected void setText(String text) {
    yytext= text;
  }


  protected String yytext= "";			//Current lexeme text.
  private int lineN= 1;				//Current line #.
  private String input= null;			//Input characters.
  private int index= 0;				//Index in input.
  private int lastNLIndex= 0;			//Index of last '\n'.
  private Hashtable tokNames= new Hashtable();  //Map tok #s to tok-names.

  static final String eofName= "<EOF>";
}
  
