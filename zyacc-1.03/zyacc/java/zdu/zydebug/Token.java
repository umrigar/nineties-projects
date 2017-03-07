/*

File:	 Token.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/07/21 08:31:09 zdu"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.zydebug;

class Token {
  int type;			//type of the token.
  int index;			//index in buf of start of lexeme.
  int len;			//length of lexeme. 
  byte buf[];			//buffer containing lexeme chars.
}
