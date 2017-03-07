/*

Test for rational powers.  Currently compiles only if DIM_GPP_BUG and
DIM_NO_CHECK is defined, since gcc does not implement template-arg
mult.  Compile using:

    make DFLAGS="-DDIM_NO_CHECK -DDIM_GPP_BUG" ratPow


*/

#include <iostream.h>

#define N_DIM 2
#define DIM_DENOM 3


#include "Dim.h"

void dummyFn() {
DIM_DUMMY(1/3, 1/3, _11);
DIM_DUMMY(2/3, 2/3, _22);
DIM_DUMMY(2, 2, _66);
DIM_DUMMY(2, 0, _60);
DIM_DUMMY(2, 1, _63);
}


//Name each dimensional axis.
#define MASS_DIM 	0
#define LENGTH_DIM 	1


UNIT(Gm, MASS_DIM, 1.0, "gm");
UNIT(Kg, MASS_DIM, 1000.0, "kg");
UNIT(Cm, LENGTH_DIM, 1.0, "cm");
UNIT(Meter, LENGTH_DIM, 100.0, "meter");
UNIT(Km, LENGTH_DIM, 100.0 * 1000.0, "km");
UNIT(Feet, LENGTH_DIM, 2.54*12.0, "feet");
typedef DIM( 1,  0) Mass;
typedef DIM( 0,  1) Length;
typedef DIM( 0,  0) Number;

main() {
  DIM(1/3, 1/3) x= 12.0;
  DIM(2/3, 2/3) y= 14.0;
  cout << unitPow(Kg_ * Cm_, DIM_POWER(1, 3)) << x << '\n';
  cout << unitPow(Kg_ * Feet_, DIM_POWER(2, 3)) << y << '\n';
  cout << Kg_ * Kg_ * Cm_ * Cm_ << dimPow(x * x + y, DIM_POWER(3, 1)) << '\n';
}
