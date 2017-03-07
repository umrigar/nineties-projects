/*

Block sliding down a plane.  Adapted from papers by Hilfinger and Gehani.

Buggy version which contains dimensional errors, so it won't compile.

Bugs indicated by !!.

*/

#include <iostream.h>

extern "C" {
#include "math.h"
};

#define N_DIM 3
#define DIM_VAL_TYPE double
//#define DIM_NO_CHECK

#include "Dim.h"

#ifndef DIM_NO_CHECK

void dimDummy(void) {
//Define intermediate classes which g++ should 
//really figure out the need for.

DIM_DUMMY( 0,  0, -2, 00_2);
DIM_DUMMY( 0,  0,  0, 000);
DIM_DUMMY( 0,  0,  1, 001);
DIM_DUMMY( 0,  0,  2, 002);
DIM_DUMMY( 0,  1, -2, 01_2);
DIM_DUMMY( 0,  1, -1, 01_1);
DIM_DUMMY( 0,  1,  0, 010);
DIM_DUMMY( 0,  1,  1, 011);
DIM_DUMMY( 0,  2,  0, 020);
DIM_DUMMY( 1,  1,  0, 110);
DIM_DUMMY( 1,  1, -2, 11_2);

#undef DUMMY
}
#endif



//Name each dimensional axis.
#define MASS_DIM 	0
#define LENGTH_DIM 	1
#define TIME_DIM	2


UNIT(Gm, MASS_DIM, 1.0, "gm");
UNIT(Cm, LENGTH_DIM, 1.0, "cm");
UNIT(Feet, LENGTH_DIM, 2.54*12, "feet");
UNIT(Sec, TIME_DIM, 1.0, "sec");

typedef DIM( 1,  0,  0) Mass;
typedef DIM( 0,  1,  0) Length;
typedef DIM( 0,  0,  1) Time;
typedef DIM( 0,  1, -2) Acceleration;
typedef DIM( 1,  0, -1) Friction;
typedef DIM( 0,  1, -1) Velocity;
typedef DIM( 0,  0,  0) Number;

static Length height(Length x) {
  return 0.5 * x;
}

int main() {
  const Acceleration g= 980.7 * Cm/(Sec);	//!!Should be Cm/(Sec * Sec)
  const Mass blockMass= 1000.0 * Gm;
  const Length deltaX= 0.01 * Cm;
  const Friction friction= 20.0 * Gm/Sec;
  Length x, y;	//Initialized to 0.
  const Time deltaT= 0.1*Sec;
  Velocity v;
  Time maxTime;

  cout << "Enter initial velocity (cm/sec): " << flush;
  cin >> Cm/Sec >> v;

  cout << "Enter time limit (sec): " << flush;
  cin >> Sec*Sec >> maxTime;			//!!Should be simply Sec.

  for (Time t= 0.0*Sec; t < maxTime; t+= deltaT) {
    Number slope= (height(x + deltaX) - y)/deltaX;
    double cosAngle= 1.0/sqrt(1 + +(slope * slope));
    x+= v * deltaT * cosAngle * deltaT;		//!!Omit last deltaT.
    y= height(x);
    v+= deltaT * (g * slope * cosAngle - v * friction / blockMass);
    cout << "At T= " << Sec_ << t << " ";
    cout << "X= " << Feet_ << x << "; ";
    cout << " Y= " << Feet_ << y << "; ";
    cout << " and V= " << Feet_ << v << "; ";	//!!Should be Feet_/Sec_.
    cout << '\n';
  }

  return 0;
}
