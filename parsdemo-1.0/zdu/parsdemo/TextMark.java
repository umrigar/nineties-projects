/*

File:	 TextMark.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:40:56 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import java.awt.Color;

class TextMark {		//Marks within a line.
  TextMark(int lo, int hi, Color color, TextMark succ) {
    this.lo= lo; this.hi= hi; this.color= color; this.succ= succ;
  }
  int lo;		//Inclusive lower bound in string.
  int hi;		//Exclusive upper bound in string.
  Color color;		//Color to use in this background.
  TextMark succ;	//Next mark in list.
};

