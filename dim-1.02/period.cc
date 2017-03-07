/*

Calculates period of a simple pendulum.  Currently compiles only if
DIM_NO_CHECK and DIM_GPP_BUG are defined, since gcc does not implement
template-arg mult.  Use 

    make DFLAGS="-DDIM_NO_CHECK -DDIM_GPP_BUG" period


*/

#define DIM_DENOM 2

#include "Dim.h"

void f(void) {
//Define intermediate classes which gpp should really figure out the need for.

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
}


//Name each dimensional axis.
#define MASS_DIM 	0
#define LENGTH_DIM 	1
#define TIME_DIM	2


UNIT(Cm, LENGTH_DIM, 1.0, "cm");
UNIT(Sec, TIME_DIM, 1.0, "sec");

int main() {
  const DIM(0, 1, -2) g= 981.0 * Cm / (Sec * Sec);
  //  const double PI= 3.142;

  for (DIM(0, 1, 0) len= 0.0 * Cm; len < 100.0 * Cm; len+= 10.0 * Cm) {
    DIM(0, 0, 1) t= ((2 * PI)) * dimPow(len/g, DIM_POWER(1, 2));
    cout << "Length= " << Cm_ << len << "; "
         << "Period= " << Sec_ << t << endl;
  }
  return 0;
}
