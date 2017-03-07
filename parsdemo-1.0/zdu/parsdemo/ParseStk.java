/*

File:	 ParseStk.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:40:07 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.OffsetTree;
import zdu.parsdemo.ParseStkEntry;

import java.util.EmptyStackException;
import java.util.Stack;
import java.util.Vector;

class ParseStk extends Stack
{
  ParseStk() {
    super();
  }

  void pushT(ParseStkEntry e) {
    OffsetTree t= e.getOffsetTree();
    ParseNode n= (ParseNode) t.getInfo();
    n.setClosed(false);
    push(e);
  }

  ParseStkEntry popT() {
    ParseStkEntry e= null;
    try {
      e= (ParseStkEntry)pop();
      OffsetTree t= e.getOffsetTree();
      ParseNode n= (ParseNode) t.getInfo();
      n.setClosed(true);
    }
    catch (EmptyStackException ex) {
      throw new InternalError("empty stack" + ex);
    }
    return e;
  }

  ParseStkEntry peekT() {
    ParseStkEntry e= null;
    try {
      e= (ParseStkEntry) peek();
    }
    catch (EmptyStackException ex) {
      throw new InternalError("empty stack" + ex);
    }
    return e;
  }

  public String toStringT() {
    StringBuffer b= new StringBuffer();
    try {
      int n= size();
      for (int i= 0; i < n; i++) {
	ParseStkEntry e= (ParseStkEntry) elementAt(i);
	b.append(e.toString() + " ");
      }
    }
    catch (ArrayIndexOutOfBoundsException ex) {
      throw new InternalError("empty stack" + ex);
    }
    return b.toString();
  }

}
