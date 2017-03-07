/*

File:	 Trace.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/07/21 16:39:15 zdu"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.zydebug;

class Trace {
  Trace(String stk, String input, String act) {
    this.stk= stk; this.input= input; this.act= act;
  }
  String stk, input, act;		//Contents of line.
};


