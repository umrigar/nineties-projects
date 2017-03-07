/*

File:	 Act.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/07/27 13:16:32 zdu"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.zydebug;

import zdu.zydebug.Scanner;

class Act {
  int type;		//one of the constants below
  String stack;		//string representing stack.
  int state;		//target of shift or goto
  String token;		//lookahead token
  String nonTerm;	//LHS nonterm of string being reduced.
  String value;		//token semantic value.
  int ruleLen;		//length of rule being reduced			    
  int lineNum;		//line # of reduction in .y source file
  int ruleN;		//rule # of reducing rule.
  static final int EOF= 0;		//EOF on input.
  static final int PROMPT= 1;		//waiting for input.
  static final int SHIFT= 2;
  static final int REDUCE= 3;
  static final int ACCEPT= 4;
  static final int ERROR= 5;		//in error recovery.
};
