/*

File:	 TextLine.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:40:56 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.TextMark;

import java.awt.Color;

class TextLine {
  TextLine(String text) {
    this.text= text; marks= null;
  }
  void addTextMark(Color c) {
     addTextMark(0, text.length(), c);
  }
  void addTextMark(int lo, int hi, Color c) {
    marks= new TextMark(lo, hi, c, marks);
  }
  String text;		//Contents of TextLine.
  TextMark marks;
};

