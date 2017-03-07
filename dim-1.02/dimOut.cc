#include <iostream.h>

static unsigned gcd(unsigned u, unsigned v) {
  if (u < v) 
    return gcd(v, u);
  else if (v == 0)
    return u;
  else {
    int r;
    while ((r= u % v) != 0) { u= v; v= r; }
    return v;
  }
}

ostream &dimOut(ostream &out, const unsigned nDim, const unsigned denom, 
		const double val, 
		const int dims[], const char *const *dimNames) {
  unsigned i;
  int dimMax[2];	// [0]: Max. + power index; [1]: Max. -  power index.
  dimMax[0]= dimMax[1]= -1;
  out << val;
  for (i= 0; i < nDim; i++) {
    //Find max. index for +ve and -ve powers in dimMax[0] & dimMax[1] resp.
    if (dims[i] > 0) dimMax[0]= i;
    else if (dims[i] < 0) dimMax[1]= i;
  }
  unsigned sign= 0;
  do { //Loop at most twice - first over +ve powers; then over -ve powers.
    for (i= 0; i < nDim; i++) {	//Loop over all dimensions.
      int power= (sign == 0) ? 1 * dims[i] : -1 * dims[i];
      if (power > 0) {	//Need to output unit.
	out << ' ' << dimNames[i];	//Output unit-name.
	if (power != denom) {		//Need to output power. 
	  unsigned g= gcd(power, denom);
	  unsigned numerator= power/g;
	  unsigned denominator= denom/g;
	  out << "**";			
	  if (denominator != 1) {	//Need to output fractional power.
	    out << '(' << numerator << '/' << denominator << ')';
	  }
	  else {
	    out << numerator;
	  }
	}
	if (dimMax[sign] != i) out << " *";
      }
    }
    if ((sign= (dimMax[1] !=  -1) - sign)) out << " /";
  } while (sign);
  return out;
}
