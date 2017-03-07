/*

File:	 Scanner.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/07/28 00:57:10 zdu"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.zydebug;

import java.io.*;


//Crude scanner.  Assumes no errors in input.  Tokens are basically of two
//types --- KEY tokens or WORD tokens.  A KEY token starts with a % and the
//following character.  A WORD token is anything else and is terminated
//by the first non-quoted ';' or '\n'.  Single quotes are recognized for
//quoting within WORDs.  The ';' is merely regarded as a token
//delimiter like whitespace and is not returned by the scanner.
class Scanner {
  Scanner(InputStream in, PrintStream out) {
    this.in= in;
    buf= new byte[0];
    index= 0; size= 0; isEOF= false;

    try {
      int t;
      //go past initial greeting message and "zydebug> " prompt.
      do { t= in.read(); } while (t != '>'); t= in.read();

      out.println("V");			//turn off verbose mode.
      t= in.read(); t= in.read();	//go past non-verbose prompt "% ".
    }
    catch (IOException e) {
      throw new InternalError("scanner initialization " + e);
    }
  }

  int nextTok(Token tok) {
    int t;
    if (index >= size && !isEOF) refillBuf();
    if (index >= size) {
      if (!isEOF) {
	throw new InternalError("expecting EOF");
      }
      tok.index= index; tok.type= EOF; 
    }
    else { /* we have some chars */
      if (buf[index] == ';') index++;
      while (buf[index] == ' ' || buf[index] == '\t') {
	index++;
      }
      tok.index= index;
      if (buf[index] == '\n') {
	tok.type= NL; index++;
      }
      else if (buf[index] == '\r' && buf[index + 1] == '\n') {
	tok.type= NL; index+= 2;
      }
      else if (buf[index] == '%') {
	index++;
	switch (buf[index++]) {
	  case ' ': 	
	    tok.type= PROMPT; 
	    break;
	  case 'A': 	
	    tok.type= KEY_ACTION; 
	    break;
	  case 'K': 	
	    tok.type= KEY_STACK; 
	    break;
	  case 'L': 	
	    tok.type= KEY_LINE_NUM; 
	    break;
	  case 'M': 	
	    tok.type= KEY_RULE_LEN; 
	    break;
	  case 'N': 	
	    tok.type= KEY_NON_TERM; 
	    break;
	  case 'R': 	
	    tok.type= KEY_RULE_NUM; 
	    break;
	  case 'S': 	
	    tok.type= KEY_STATE; 
	    break;
	  case 'T': 	
	    tok.type= KEY_TOKEN; 
	    break;
	  case 'V': 	
	    tok.type= KEY_VALUE; 
	    break;
	  default:
	    throw new InternalError("bad % specifier");
	} //switch (buf[index++])
      } //else if (buf[index] == '%')
      else { //word
	while (buf[index] != ';' && buf[index] != '\n' && buf[index] != '\r') {
	  if (buf[index] == '\'') {
	    int lastCh;
	    do {
	      lastCh= buf[index];
	      index++;
	    } while (buf[index] != '\'' || lastCh == '\\');
	  }
	  index++;
	} //while
	tok.type= WORD;
      } //else word
    } //else we have some chars
    tok.len= index - tok.index;
    tok.buf= buf; 
    return tok.type;
  }
  


  private void refillBuf() {
    size= 0;
    index= 0;
    int lastCh= '\0';
    int c;
    do {
      try {
	c= in.read();
      }
      catch (IOException e) {
	throw new InternalError("scanner input " + e);
      }
      if (c < 0) { isEOF= true; break; }
      try {
	buf[size]= (byte)c;
      }
      catch (IndexOutOfBoundsException e) {
	byte[] buf1= new byte[buf.length + BUF_SIZE_INC];
	System.arraycopy(buf, 0, buf1, 0, buf.length);
	buf= buf1;
	buf[size]= (byte)c;
      }
      size++;
      if (lastCh == '%' && c == ' ') break;
      lastCh= c;
    } while (c != '\n');
  }


  
  public static final int EOF= 0;
  public static final int KEY_ACTION= EOF + 1;
  public static final int KEY_LINE_NUM= KEY_ACTION + 1;
  public static final int KEY_NON_TERM= KEY_LINE_NUM + 1;
  public static final int KEY_RULE_LEN= KEY_NON_TERM + 1;
  public static final int KEY_RULE_NUM= KEY_RULE_LEN + 1;
  public static final int KEY_STACK= KEY_RULE_NUM + 1;
  public static final int KEY_STATE= KEY_STACK + 1;
  public static final int KEY_TOKEN= KEY_STATE + 1;
  public static final int KEY_VALUE= KEY_TOKEN + 1;
  public static final int NL= KEY_VALUE + 1;
  public static final int PROMPT= NL + 1;
  public static final int WORD= PROMPT + 1;
  

  private static final int BUF_SIZE_INC= 256;
  private InputStream in;	//stream being scanner.
  private byte buf[];		//buffers a line or prompt from in.
  private int index;		//index of next char to be read in buf[].
  private int size;		//number of chars currently in buf[].
  private boolean isEOF;	//true if EOF seen on in.

  //Test program.  Tokenizes standard input.  Must start standard input
  //with garbage followed by the string "> ".  It will then output a 
  //line containing "V".  Follow this with input starting with "% ".
  //Then type in tokens.
  static public void main(String args[]) {
    Scanner scanner= new Scanner(System.in, System.out);
    Token t= new Token();
    for (scanner.nextTok(t); t.type != Scanner.EOF; scanner.nextTok(t)) {
      System.out.println(t.type + ": " + 
			 new String(t.buf, 0, t.index, t.len));
    }
  }

}
