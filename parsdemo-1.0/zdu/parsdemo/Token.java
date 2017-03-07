/*

File:	 Token.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:40:56 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

class Token {

  Token(int tokNum, String text) {
    this.tokNum= tokNum; this.text= text;
  }

  String getText() {
    return text;
  }

  int getTokNum() {
    return tokNum;
  }


  private String text;		//matched lexeme.
  private int tokNum;		//token number.

}

