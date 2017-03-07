/*
Dim.h: STATIC DIMENSIONAL ANALYSIS IN C++: Version 1.02.	

*************************************************************************
*									*
* Copyright (C) 1994, 1997 Zerksis D. Umrigar					*
*									*
*                        LICENSE AGREEMENT.                             *
*									*
* Everybody is granted permission to use and/or distribute verbatim or  *
* modified copies of this package provided this license agreement is    *
* retained, such distribution is not for profit, and if modified, the   *
* modified package carries a prominent notice stating that it has been  *
* modified.								*
*									*
* This package is distributed in the hope that it will be useful, but   *
* WITHOUT ANY WARRANTY; without even the implied warranty of            *
* merchantability or fitness for a particular purpose.  The entire risk *
* as to the quality and performance of the package is with you.  Should *
* the package prove defective, you assume the cost of all necessary	*
* servicing, repair or correction.  In no event will any copyright      *
* holder or any other party who may modify and/or redistribute the	*
* package as permitted above, be liable to you for ANY damages arising  *
* from the operation or misoperation of this package.			*
*************************************************************************


*/


#ifndef _DIM_H
#define _DIM_H

/*		   	USING THE Dim PACKAGE.

There are two aspects to a dimensional-analysis package: 

1.  Verifying the dimensional integrity of expressions.  This prevents
nonsense like adding mass to time.

2.  Provide automatic scaling between quantities.  Hence if a quantity 
in inches is added to a quantity in centimeters, one of the quantities 
needs to be scaled appropriately before the addition can be performed.

This package provides (1).  It circumvents the need for (2), by
representing all quantities internally using only a single unit along
a particular dimension.  This unit is chosen by the user for each
dimension and is subsequently referred to as the CHOSEN INTERNAL
UNIT.  However the package does provide facilities for automatic
scaling of quantities represented by literals or those used for I/O.
Hence if the chosen internal unit along the Length dimension happens
to be centimeters, then the literal representing 2 inches will be
automatically converted to its internal representation of 5.08 (2.54 *
2) centimeters.

Facilities are provided for evaluating arithmetic and relational
expressions involving dimensioned quantities with (in theory) no
run-time overhead.  Facilities are also provided for specifying
dimensioned literals, and for doing I/O of dimensioned quantities
using the <iostream> library.

The inputs to the package "Dim.h" are the following cpp #defines (which
should be #define'd or #undef'd before Dim.h is #included).

N_DIM: 		# of dimensions desired (defaults to 3, max. value
                currently handled is 10, but can be changed by
		rerunning m4 on the file Dim.h.m4 (see the comments at
		the beginning of Dim.h.m4)).
DIM_VAL_TYPE:	The default QUANTITY TYPE stored in each dimensioned
                quantity (defaults to double). It must permit the usual 
                arithmetic and relational ops (as well as multiplication
		and division by doubles).
DIM_NO_CHECK:	If this preprocessor symbol is defined, then no
                dimensional analysis is done.  This ensures that
		even a mediocre compiler produces efficient code.
DIM_DENOM:	A positive integer giving a common multiple of the 
                denominators of ALL the rational dimensional powers 
		used (defaults to 1).  
DIM_POW:	The name of the function to be used to find the
                power of a dimensioned quantity (it defaults to
		pow() from the math.h library).  Its prototype can be 
		either T DIM_POW(T, double) or T DIM_POW(const T &, double)
		where T is a quantity type.  It should return
		its first argument raised to the power specified
		by the second argument.

In the examples which follow we assume that N_DIM == 3 and the
semantics of the dimensions are (Mass, Length, Time).  We assume that
N_DIM1 == (N_DIM - 1).  We specify a point in the dimension-space by a
vector giving the powers of the respective dimensions --- hence (0, 1,
-2) would be the dimensions for acceleration.  Also the examples
accumulate --- i.e. the declarations in an example may be used in
subsequent examples.

Given a dimensional-vector (p0, p1, ..., pN_DIM1), we refer to its i'th
element as dimension i.  For example, in (0, 1, -2) dimension 0 is 0,
dimension 1 is 1, and dimension 2 is -2.

We now describe the facilities provided by Dim.h to the programmer.

                         BASIC DIMENSIONED TYPE

DIM(p0, p1, ..., pN_DIM1): 
T_DIM(T, p0, p1, ..., pN_DIM1): 
These macros are used to declare a dimensioned-type located at point
(p0, p1, ..., pN_DIM1) in dimension-space.  The p's can be integers or
fractions (if the latter, they should NOT be enclosed in
parentheses).  The DIM macro declares a dimensioned-type with
quantity-type defaulting to DIM_VAL_TYPE, whereas T_DIM allows the
user to specify the underlying quantity type using the first parameter
T.

Examples: typedef DIM(0, 1, -2) Acceleration;
	  DIM(1, -3, 0) density;
          T_DIM(float, 0, 1, -1) speed;

The usual arithmetic and relationals operators are defined for each
dimensioned type.  There is also a compare function which takes two
compatible dimensioned quantities and returns -1, 0, or +1 depending
on whether its 1st argument is less than, equal to, or greater than
its second operand.  For convenience, multiplication and division BY a
DIM_VAL_TYPE (or by types which can be automatically converted to
DIM_VAL_TYPEs) is also permitted.  Hence if we have:

	DIM(1, 0, 0) m;			//Mass.
	DIM(0, 3, 0) v1, v2;		//Volume.
	DIM(1, -3, 0) d;		//Density.

	d= m * 2/v1;			//Ok.
	d= m + v1;			//Error.  Can't add volume & mass.	
	d= (2 * v1 > v2) ? m/v1 : m/v2;	//Ok.
	d= (v1 != m) ? m/v1 : m/v2;	//Error.  Can't compare volume & mass.




                             DECLARING UNIT

UNIT(Name, DimIndex, Conversion, PrintName): 
A macro used to define a basic unit Name along dimension DimIndex.
Conversion should be a number specifying the conversion factor used to
convert Name to the chosen internal unit for dimension DimIndex.
PrintName is a string used to print the unit.

DimIndex must be an integer in {0, ..., N_DIM1} or a preprocessor macro
which expands to such an integer.  It cannot, for example, be an
enum.

Example:
	                                                //DEFINES
	  UNIT(Gm, 0, 1, "gm");		//Chosen unit.	//Gm, Gm_.
	  UNIT(Kg, 0, 1000, "kg");			//Kg, Kg_.
	  UNIT(Cm, 1, 1, "cm");		//Chosen unit.	//Cm, Cm_.
	  UNIT(Inch, 1, 2.54, "inches");		//Inch, Inch_.
	  UNIT(Mile, 1, 2.54 * 12 * 5280, "miles");	//Mile, Mile_.
	  UNIT(Sec, 2, 1, "sec");	//Chosen unit.	//Sec, Sec_.
	  UNIT(Hour, 2, 60*60, "hr");			//Hour, Hour_.

As noted in the comments in the example above, each use of the UNIT
macro defines two objects:

Name: This is a scaling object used to scale literals or quantities
used for I/O.

_Name: This is a units object which contains the scaling factor and
PrintName.  It is used to output dimensional quantities.

Each use of the above macro specifies information about only 1
dimension.  However, the objects defined above can be combined using *
and / to provide units along multiple dimensions.  For example,
Miles/Hour can be used to specify a scaling object for speed; Gm_/(Cm_
* Cm_ * Cm_) can be used to specify a units object for density.

Note that derived units and scales can easily be expressed using
the preprocessor:

#define Dyne 	((Gm * Cm) / (Sec * Sec))
#define Dyne_	((Gm_ * Cm_) / (Sec_ * Sec_))

Each of the above scaling objects X can be thought as specifying 1 X
unit along the specified dimension.   So after the above UNIT declarations,
the identifier Mile represents 1 mile. 

In addition to being combined among themselves with * and /, scaling
objects can also serve as the right-operand to * or / with the left
operand being either a DIM_VAL_TYPE or Dim object.  Specifically if D
is either a literal number or Dim object and S is a scale object, then
D * S and D / S is a dimensioned quantity dimensioned and scaled
appropriately.  For example:

  DIM(0, 1, 0) len= 2 * Inch;	//Declares a length with value 2 inches.
                                //Its value is automatically scaled 
				//internally to 5.08 cm.

  DIM(1, -3, 0) d1= 1 * Kg/(Cm * Cm * Cm);
                                //Declares a density of 1 kg/cc.
                                //Its value is automatically scaled 
				//internally to 1000 gm/cc.

  DIM(0, 1, -1) speed= 60 * Miles/Hour;	//60 mph == 88 ft/sec.
  DIM(0, 0, 0) s= speed / (Feet/Sec);	//s == 88 dimensionless.

                            I/O SUPPORT

The above scaling objects can also be used for scaling numbers used
for I/O.  For example, if we have

  DIM(0, 1, 0) x;

  cin >> Feet >> x;
  cout << Inch << x;

and the input was the number 1, then the output would be 12.  On
input, the 1 is converted to a dimensioned quantity with internal
value 30.48 (12 * 2.54).  On output, this dimensioned quantity is
scaled and output in inches.

We can make the package output the units too by using the units
objects Name_() (defined by UNIT(Name, ...)).  For example,

  DIM(0, 1, -1) v;

  cin >> Mile/Hour >> v;
  cout << Feet_/Sec_ << v;

with input of the number 60, the output will be the string "88 feet/sec".

The output uses a library function

ostream &dimOut(ostream &c, const unsigned nDim, const unsigned denom, 
		const double val, 
		const int dims[], const char *const *names);

with the following specification:

Outputs value val unchanged on output stream c, followed by its
units.  The powers of the dimensions is specified by the elements of
the vector dims[] with each element divided by denom, and the names of
the units by the vector names[].  Both vectors have nDim elements.
Returns &c.

By plugging in their own replacement for dimOut, users can customize
the output.

                        POWER FUNCTION

Dimensioned quantities, scales and units can be raised to arbitrary
rational powers.  To allow this to be done with full dimensional
checking, the power is represented as a power-object constructed using
the macro DIM_POW(num, denom) which represents the rational power
num/denom.  Since C++ has no convenient operator to use for power, the
syntax used is extremely cumbersome, compared to other aspects of the
package.

dimPow(TDim, Power) where Power is a power-object and TDim is
a dimensioned quantity returns a dimensioned quantity representing
TDim raised to Power.  TDim can have an arbitrary quantity type.

scalePow(Scale, Power) where Power is a power-object and Scale is a
scale-object returns a dimensioned quantity representing Scale raised
to Power.

unitPow(Unit, Power) where Power is a power-object and Unit is a
unit-object returns a dimensioned quantity representing Unit raised
to Power.

Note that to use fractional powers, you should ensure that DIM_DENOM
is defined to be a common multiple (usually the least) of the
denominators of all possible rational dimensional powers.

Example:
  //Calculate period of a pendulum of length L given by 
  //T= (2*PI)*(L/g)**0.5.
  const DIM(0, 1, -2) g= 981 * Cm / (Sec * Sec);
  const double PI= 3.142;

  for (DIM(0, 1, 0) len= 0 * Cm; len < 100 * Cm; len+= 10 * Cm) {
    DIM(0, 0, 1) t= ((2 * PI)) * dimPow(len/g, DIM_POWER(1, 2));
    cout << "Length= " << Cm_ << len << "; "
         << "Period= " << Sec_ << t << endl;
  }

Note that for this example DIM_DENOM can be defined as 2, but it is
not necessary, since even though a fractional power is involved, no
fractional dimensional powers are ever produced.

                      DUMMY DECLARATIONS

The implementation makes heavy use of C++ templates.  An ideal
programming environment should automatically figure out which template
instantiations are necessary without any aid from the programmer.  Since
such ideal programming environments may not exist, the programmer can
assist the system to generate necessary instantiations by declaring
dummy objects.

DIM_DUMMY(p0, p1, ..., pN_DIM1, L):
This macro generates all objects required by the system for dimensions
(p0, p1, ..., pN_DIM1).  The programmer should also specify L ---
either a unique integer or identifier which will be used to generate
the names of the dummy objects.

To avoid cluttering up the global name-space of the program with these
dummy objects, these dummy declarations are best put into a dummy
function:

void dummy(void) {
  DIM_DUMMY( 0,  0, -2, 00_2);
  DIM_DUMMY( 0,  0,  0, 000);
  ...
  DIM_DUMMY( 1,  1, -2, 11_2);
}


RESTRICTIONS:

1. The quantity type of the result for the four binary arithmetic 
operators is arbitrarily chosen to be that of the first (left) operand.

2.  Due to a bug in g++, the I/O manipulators work only for DIMs with
quantity-type set to DIM_VAL_TYPE.

3.  Since g++ does not yet appear to implement division in template
arguments, fractional dimensional powers does not yet work (and has
not been tested except with DIM_NO_CHECK).

4.  The package allows the user to initialize a dimensioned-variable
with any expression of the quantity-type (without any scaling units).
In such a case, the units default to the chosen internal units.  For
example:

DIM(0, 1, -1) speed= 10;	//10 Cm/Sec with above chosen internal units.

The user is urged to avoid this feature and to always indicate the
units as part of the initializing expression.

5.  The user should avoid using identifiers starting with Dim or
DIM to avoid clashes with names defined in this package.


*/

/*			MODIFICATION HISTORY



*/


/*			IMPLEMENTATION NOTES.

The code below, tho' acceptable to g++, is not strictly C++.  The ARM
expressly forbids function-template with non-type template arguments.
Unfortunately, it is only by using such forbidden function templates
that static dimensional analysis of expressions involving * and /
is possible.  The statement in the ARM forbidding such function
templates appears to be too strong for its justification.  Simply
requiring that all template arguments appear in the types of the 
function arguments should be sufficient.

The above paragraph, though true when it was originally written, is no
longer the case.  All the features required by this package are now
supported by C++.

Unfortunately, with nested templates not permitted either, I know of
no way to be able to define * and / as member functions.  Most of the
functions defined in the package could have been defined as member
functions.  However, for uniformity, many of the operations have been
made non-member.

Also as many of the operations need to access what should be private
members of classes, and since they cannot be member or friend functions,
I was unable to make C++ enforce access control and merely request the
user not to access certain members --- hence the access control is
extremely clumsy.

All dimensional powers are multiplied by DIM_DENOM internal to the 
package.

The package makes heavy use of the preprocessor.  Initially, there is
an extremely clumsy section which defines several macros in different
ways depending on N_DIM.  After that, DIM_NO_CHECK is the main
discriminant used: if DIM_NO_CHECK is defined, then T_DIM(T,...) is
simply defined to be T and all the dimensional operators omitted; the
scale, units and auxiliary I/O classes are still defined, tho' free
of all templates.

*/


#include <iostream.h>

extern "C" {
#include <assert.h>
#include <string.h>
#include <math.h>
}

#define DIM_VERSION	"1.02"

//Define this if compiler unable to handle non-member insertion and
//extraction operators.
//#undef DIM_GPP_BUG	/* GPP bug --- hence work-around with restrictions. */

typedef double DimScaleType;	//Type used for scaling factors.

/* Auxiliary macro to ensure expansion of catenation arguments. */
#define DIM_CAT_X(a, b)	a ## b
#define DIM_CAT(a, b) DIM_CAT_X(a, b)


/*                         DEFAULT MACRO VALUES                         */

#ifndef DIM_VAL_TYPE	/* Default base type of quantities stored. */
#define DIM_VAL_TYPE double
#endif  /* ifndef DIM_VAL_TYPE */

#ifndef N_DIM		/* # of integral dimensions in template. */
#define N_DIM 3
#endif  /* ifndef N_DIM */

#ifndef DIM_DENOM	/* Common denom. of rational dim. powers. */ 
#define DIM_DENOM 1
#endif /* ifndef DIM_DENOM */

#ifndef DIM_POW		/* Name of routine used to do power. */
#define DIM_POW pow	/* Use routine from <math.h>. */
#endif /* ifndef DIM_POWER */

#ifndef DIM_UNIT_SUFFIX	/* Suffix to put on Scale name to get Unit name. */
#define DIM_UNIT_SUFFIX _
#endif /* ifndef DIM_UNIT_SUFFIX */



/*                      MACROS DEPENDING ON N_DIM.                      */

#define DIM_MAX 10	/* Max. # of allowed dimensions. */

//Apply macro f to DIM_MAX arguments.
#define DIM_NUMS(f)							\
  DIM_PREFIX(f(0), f(1), f(2), f(3), f(4), f(5), f(6), f(7), f(8), f(9))

/* For each possible N_DIM in 1...DIM_MAX, define DIM_PREFIX(...) 
 * to return first N_DIM of DIM_MAX arguments. 
 * Define DIM, DIM_T and DIM_DUMMY macros to each take N_DIM args. 
 */
#if  (N_DIM == 1)
#define DIM_PREFIX(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9) \
  e0
#define DIM(e0) \
  DIM_TYPE(DIM_VAL_TYPE, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)
#define DIM_T(T, e0) \
  DIM_TYPE(T, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)
#define DIM_DUMMY(e0, L) \
  DIM_DUMMY_X(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, L)

#elif  (N_DIM == 2)
#define DIM_PREFIX(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9) \
  e0, e1
#define DIM(e0, e1) \
  DIM_TYPE(DIM_VAL_TYPE, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)
#define DIM_T(T, e0, e1) \
  DIM_TYPE(T, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)
#define DIM_DUMMY(e0, e1, L) \
  DIM_DUMMY_X(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, L)

#elif  (N_DIM == 3)
#define DIM_PREFIX(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9) \
  e0, e1, e2
#define DIM(e0, e1, e2) \
  DIM_TYPE(DIM_VAL_TYPE, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)
#define DIM_T(T, e0, e1, e2) \
  DIM_TYPE(T, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)
#define DIM_DUMMY(e0, e1, e2, L) \
  DIM_DUMMY_X(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, L)

#elif  (N_DIM == 4)
#define DIM_PREFIX(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9) \
  e0, e1, e2, e3
#define DIM(e0, e1, e2, e3) \
  DIM_TYPE(DIM_VAL_TYPE, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)
#define DIM_T(T, e0, e1, e2, e3) \
  DIM_TYPE(T, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)
#define DIM_DUMMY(e0, e1, e2, e3, L) \
  DIM_DUMMY_X(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, L)

#elif  (N_DIM == 5)
#define DIM_PREFIX(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9) \
  e0, e1, e2, e3, e4
#define DIM(e0, e1, e2, e3, e4) \
  DIM_TYPE(DIM_VAL_TYPE, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)
#define DIM_T(T, e0, e1, e2, e3, e4) \
  DIM_TYPE(T, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)
#define DIM_DUMMY(e0, e1, e2, e3, e4, L) \
  DIM_DUMMY_X(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, L)

#elif  (N_DIM == 6)
#define DIM_PREFIX(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9) \
  e0, e1, e2, e3, e4, e5
#define DIM(e0, e1, e2, e3, e4, e5) \
  DIM_TYPE(DIM_VAL_TYPE, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)
#define DIM_T(T, e0, e1, e2, e3, e4, e5) \
  DIM_TYPE(T, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)
#define DIM_DUMMY(e0, e1, e2, e3, e4, e5, L) \
  DIM_DUMMY_X(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, L)

#elif  (N_DIM == 7)
#define DIM_PREFIX(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9) \
  e0, e1, e2, e3, e4, e5, e6
#define DIM(e0, e1, e2, e3, e4, e5, e6) \
  DIM_TYPE(DIM_VAL_TYPE, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)
#define DIM_T(T, e0, e1, e2, e3, e4, e5, e6) \
  DIM_TYPE(T, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)
#define DIM_DUMMY(e0, e1, e2, e3, e4, e5, e6, L) \
  DIM_DUMMY_X(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, L)

#elif  (N_DIM == 8)
#define DIM_PREFIX(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9) \
  e0, e1, e2, e3, e4, e5, e6, e7
#define DIM(e0, e1, e2, e3, e4, e5, e6, e7) \
  DIM_TYPE(DIM_VAL_TYPE, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)
#define DIM_T(T, e0, e1, e2, e3, e4, e5, e6, e7) \
  DIM_TYPE(T, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)
#define DIM_DUMMY(e0, e1, e2, e3, e4, e5, e6, e7, L) \
  DIM_DUMMY_X(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, L)

#elif  (N_DIM == 9)
#define DIM_PREFIX(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9) \
  e0, e1, e2, e3, e4, e5, e6, e7, e8
#define DIM(e0, e1, e2, e3, e4, e5, e6, e7, e8) \
  DIM_TYPE(DIM_VAL_TYPE, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)
#define DIM_T(T, e0, e1, e2, e3, e4, e5, e6, e7, e8) \
  DIM_TYPE(T, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)
#define DIM_DUMMY(e0, e1, e2, e3, e4, e5, e6, e7, e8, L) \
  DIM_DUMMY_X(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, L)

#elif  (N_DIM == 10)
#define DIM_PREFIX(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9) \
  e0, e1, e2, e3, e4, e5, e6, e7, e8, e9
#define DIM(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9) \
  DIM_TYPE(DIM_VAL_TYPE, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)
#define DIM_T(T, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9) \
  DIM_TYPE(T, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)
#define DIM_DUMMY(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, L) \
  DIM_DUMMY_X(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, L)

#else
#error N_DIM value not supported.
#endif



/*                             UNIT MACRO.                              */

#define UNIT(name, dim, conversion, printName)				\
const DIM_SCALE(DIM_CAT(DIM_U, dim)) name(conversion);			\
const DIM_UNIT(DIM_CAT(DIM_U, dim)) 					\
  DIM_CAT(name, DIM_UNIT_SUFFIX)(conversion, dim, printName)

/* Define DIM_Ui macros to consist of all 0s except for i which is 
 * DIM_DENOM.
 */
#define DIM_U0 DIM_PREFIX(DIM_DENOM, 0, 0, 0, 0, 0, 0, 0, 0, 0)
#define DIM_U1 DIM_PREFIX(0, DIM_DENOM, 0, 0, 0, 0, 0, 0, 0, 0)
#define DIM_U2 DIM_PREFIX(0, 0, DIM_DENOM, 0, 0, 0, 0, 0, 0, 0)
#define DIM_U3 DIM_PREFIX(0, 0, 0, DIM_DENOM, 0, 0, 0, 0, 0, 0)
#define DIM_U4 DIM_PREFIX(0, 0, 0, 0, DIM_DENOM, 0, 0, 0, 0, 0)
#define DIM_U5 DIM_PREFIX(0, 0, 0, 0, 0, DIM_DENOM, 0, 0, 0, 0)
#define DIM_U6 DIM_PREFIX(0, 0, 0, 0, 0, 0, DIM_DENOM, 0, 0, 0)
#define DIM_U7 DIM_PREFIX(0, 0, 0, 0, 0, 0, 0, DIM_DENOM, 0, 0)
#define DIM_U8 DIM_PREFIX(0, 0, 0, 0, 0, 0, 0, 0, DIM_DENOM, 0)
#define DIM_U9 DIM_PREFIX(0, 0, 0, 0, 0, 0, 0, 0, 0, DIM_DENOM)



/*                           TEMPLATE MACROS                            */

//Macro which produces N_DIM comma-separates zeros.
#define DIM_ZERO	DIM_PREFIX(0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

#ifdef DIM_NO_CHECK	/* No dimensional checking by compiler. */

#define DIM_TYPE(T, a, b, c, d, e, f, g, h, i, j) T 	/* Type simply T. */
#define DIM_DUMMY_X(a, b, c, d, e, f, g, h, i, j, L)	/* No dummy decs. */
#define DIM_X(T, TempArgs)		T
#define DIM_TEMPLATE_1(a)
#define DIM_TEMPLATE_2(a, b)
#define DIM_TEMPLATE_3(a, b, c)
#define DIM_TEMPLATE_4(a, b, c, d)	template<a>
#define DIM_SCALE(Args)			DimScale
#define DIM_UNIT(Args)			DimUnit
#define DIM_UNITX(Args)			DimUnitX
#define DIM_IN(Args)			DimIn
#define DIM_UNIT_OUT(Args)		DimUnitOut
#define DIM_SCALE_OUT(Args)		DimScaleOut
#define DIM_VALUE(d)			(d)

#else	/* !ifdef DIM_NO_CHECK */

/* There may be a bug in G++ when instantiated multiplied template
 * args. occur within templates.  Avoid the bug for the common case of
 * DIM_DENOM == 1, by avoiding the multiplication.  The code can be
 * retained unchanged even after things get sorted out. 
 */
#if (DIM_DENOM == 1) 
#define DIM_MULT
#else
#define DIM_MULT DIM_DENOM *
#endif
//Scale all external dimensional exponents by DIM_DENOM internally 
//to permit rational external dimensional exponents.
#define DIM_FRAC(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9) \
  DIM_PREFIX(DIM_MULT e0, DIM_MULT e1, DIM_MULT e2, DIM_MULT e3, DIM_MULT e4, \
  DIM_MULT e5, DIM_MULT e6, DIM_MULT e7, DIM_MULT e8, DIM_MULT e9)

#define DIM_TYPE(T, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9) \
  Dim<T, DIM_FRAC(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)>

//Macro which produces all dummy declarations for a given dim. vector.
#define DIM_DUMMY_X(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, L) \
  Dim<DIM_VAL_TYPE, DIM_FRAC(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)> \
    dimD ## L; \
  DimScale<DIM_FRAC(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)> \
    dimS ## L; \
  DimUnit<DIM_FRAC(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)> \
    dimU ## L; \
  DimUnitX<DIM_FRAC(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)> \
    dimUX ## L; \
  DimIn<DIM_FRAC(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)> \
    dimI ## L;	\
  DimScaleOut<DIM_FRAC(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)> \
    dimSO ## L; \
  DimUnitOut<DIM_FRAC(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9)> \
    dimUO ## L

#define DIM_X(T, TempArgs)		Dim<T, TempArgs>
#define DIM_TEMPLATE_1(a)		template<a>
#define DIM_TEMPLATE_2(a, b)		template<a, b>
#define DIM_TEMPLATE_3(a, b, c)		template<a, b, c>
#define DIM_TEMPLATE_4(a, b, c, d)	template<a, b, c, d>
#define DIM_SCALE(Args)			DimScale<Args>
#define DIM_UNIT(Args)			DimUnit<Args>
#define DIM_UNITX(Args)			DimUnitX<Args>
#define DIM_IN(Args)			DimIn<Args>
#define DIM_UNIT_OUT(Args)		DimUnitOut<Args>
#define DIM_SCALE_OUT(Args)		DimScaleOut<Args>
#define DIM_VALUE(d)			(d).value()

/* Helper macros to plug into above DIM_NUMS macro. */
#define	DIM_A(n)	DA ## n
#define DIM_B(n)	DB ## n
#define DIM_INT_A(n)	int DIM_A(n)
#define DIM_INT_B(n)	int DIM_B(n)
#define DIM_ADD(n)	DIM_A(n) + DIM_B(n)
#define DIM_SUB(n)	DIM_A(n) - DIM_B(n)

/* 		DEFINITIONS FOR TEMPLATE ARGUMENTS.			*/

/* Declaration for N_DIM arguments: int DA0, int DA1, ... */
#define DIM_DEC_A	DIM_NUMS(DIM_INT_A)

/* Declaration for 2 * N_DIM arguments: 
 * int D0_A, int D1_A, ..., int D0_B, int D1_B, ... 
 */
#define DIM_DEC_AB	DIM_NUMS(DIM_INT_A) , DIM_NUMS(DIM_INT_B)

/* Template arguments DA0, DA1, ... */
#define DIM_ARGS_A	DIM_NUMS(DIM_A)

/* Template arguments DB0, DB1, ... */
#define DIM_ARGS_B	DIM_NUMS(DIM_B)

/* Template args add: DA0 + DB0, DA1 + DB1, ... */
#define DIM_ARGS_ADD		DIM_NUMS(DIM_ADD)

/* Template args add: DA0 - DB0, DA1 - DB1, ... */
#define DIM_ARGS_SUB		DIM_NUMS(DIM_SUB)

#endif /* ifdef DIM_NO_CHECK */


/*                              DIM CLASS.                              */

#ifndef DIM_NO_CHECK

/* We can't use C++'s access control since because of template syntax
 * restrictions, many functions (which can't be declared friends) need
 * to access innards of class.  Hence we simply declare that certain
 * functions should not be used directly by the user.
 */

template <class T, DIM_DEC_A> class Dim {
private:
  T val;
public: //The routines in this section should not be used directly by the user.
  Dim(T v) { val= v; }			//*** NOT to be used by user. ***
  void setValue(T v) { val= v; }	//*** NOT to be used by user. ***	
  T value() const { return val; }	//*** NOT to be used by user. ***
public:
  Dim() { }	//Allow declaration without initialization.
  Dim(const Dim<T, DIM_ARGS_A> &d)  { val= d.val; }
  Dim<T, DIM_ARGS_A> &operator=(const Dim<T, DIM_ARGS_A> &d) {
    val= d.val;  return *this;
  }
};


//Unary + accesses underlying type.
template <class T>
static inline
T operator+(const Dim<T, DIM_ZERO> &d) {
  return d.value();
}

//Print out dimensions for debugging.
template<class T, DIM_DEC_A>
Dim<T, DIM_ARGS_A> &dimPrint(const Dim<T, DIM_ARGS_A> &d) {
  static int dims[N_DIM]= { DIM_ARGS_A }; 
  cout << "< ";
  for (unsigned i= 0; i < N_DIM; i++) cout << dims[i] << ' ';
  cout << ">\n";
  return d;
}


/*                     BASIC ARITHMETIC OPERATORS.                      */

/* Binary operators return result with underlying quantity type to be 
 * that of the first (left) operand.
 * Otherwise it is impossible to decide (at template expansion time)
 * whether the QtyType of the result should be that of the left-operand
 * or the right-operand.
 */
template<class T1, class T2, DIM_DEC_A>
static inline 
Dim<T1, DIM_ARGS_A> 
operator+(const Dim<T1, DIM_ARGS_A> &d1, const Dim<T2, DIM_ARGS_A> &d2) {
  return Dim<T1, DIM_ARGS_A>(d1.value() + d2.value());
}

template<class T1, class T2, DIM_DEC_A>
static inline 
Dim<T1, DIM_ARGS_A> 
operator-(const Dim<T1, DIM_ARGS_A> &d1, const Dim<T2, DIM_ARGS_A> &d2) {
  return Dim<T1, DIM_ARGS_A>(d1.value() - d2.value());
}

template<class T, DIM_DEC_A>
static inline 
Dim<T, DIM_ARGS_A> operator-(const Dim<T, DIM_ARGS_A> &d1) {
  return Dim<T, DIM_ARGS_A>(-d1.value());
}

template<class T1, class T2, DIM_DEC_AB>
static inline 
Dim<T1, DIM_ARGS_ADD> 
operator*(const Dim<T1, DIM_ARGS_A> &d1, const Dim<T2, DIM_ARGS_B> &d2) {
  return Dim<T1, DIM_ARGS_ADD>(d1.value() * d2.value());
}

template<class T1, class T2, DIM_DEC_AB>
static inline 
Dim<T1, DIM_ARGS_SUB> 
operator/(const Dim<T1, DIM_ARGS_A> &d1, const Dim<T2, DIM_ARGS_B> &d2) {
  return Dim<T1, DIM_ARGS_SUB>(d1.value() / d2.value());
}

//For convenience define * and / for DIM_VAL_TYPE.	
template<class T, DIM_DEC_A>
static inline 
Dim<T, DIM_ARGS_A> 
operator*(const Dim<T, DIM_ARGS_A> &d1, const DIM_VAL_TYPE v) {
  return Dim<T, DIM_ARGS_A>(d1.value() * v);
}

template<class T, DIM_DEC_A>
static inline 
Dim<T, DIM_ARGS_A> 
operator*(const DIM_VAL_TYPE v, const Dim<T, DIM_ARGS_A> &d1) {
  return Dim<T, DIM_ARGS_A>(v * d1.value());
}

template<class T, DIM_DEC_A>
static inline 
Dim<T, DIM_ARGS_A> 
operator/(const Dim<T, DIM_ARGS_A> &d1, const DIM_VAL_TYPE &v) {
  return Dim<T, DIM_ARGS_A>(d1.value() / v);
}



/*                         ASSIGNMENT OPERATORS                         */

//For all except =, which C++ requires to be a member function.

template<class T1, class T2, DIM_DEC_A>
Dim<T1, DIM_ARGS_A> &
operator+=(Dim<T1, DIM_ARGS_A> &d1, const Dim<T2, DIM_ARGS_A> &d2) {
    d1.setValue(d1.value() + d2.value());  return d1;
}

template<class T1, class T2, DIM_DEC_A>
Dim<T1, DIM_ARGS_A> &
operator-=(Dim<T1, DIM_ARGS_A> &d1, const Dim<T2, DIM_ARGS_A> &d2) {
    d1.setValue(d1.value() - d2.value());  return d1;
}

template<class T1, class T2, DIM_DEC_A>
Dim<T1, DIM_ARGS_A> &
operator*=(Dim<T1, DIM_ARGS_A> &d1, const Dim<T2, DIM_ARGS_A> &d2) {
    d1.setValue(d1.value() * d2.value());  return d1;
}

template<class T1, class T2, DIM_DEC_A>
Dim<T1, DIM_ARGS_A> &
operator/=(Dim<T1, DIM_ARGS_A> &d1, const Dim<T2, DIM_ARGS_A> &d2) {
    d1.setValue(d1.value() / d2.value());  return d1;
}



/*                        RELATIONAL OPERATORS.                         */

//Allow different quantity-types in left & right operands.

template<class T1, class T2, DIM_DEC_A>
static inline
int operator==(const Dim<T1, DIM_ARGS_A> &d1, const Dim<T2, DIM_ARGS_A> &d2) {
  return d1.value() == d2.value();
}

template<class T1, class T2, DIM_DEC_A>
static inline
int operator!=(const Dim<T1, DIM_ARGS_A> &d1, const Dim<T2, DIM_ARGS_A> &d2) {
  return d1.value() != d2.value();
}

template<class T1, class T2, DIM_DEC_A>
static inline
int operator<=(const Dim<T1, DIM_ARGS_A> &d1, const Dim<T2, DIM_ARGS_A> &d2) {
  return d1.value() <= d2.value();
}

template<class T1, class T2, DIM_DEC_A>
static inline
int operator<(const Dim<T1, DIM_ARGS_A> &d1, const Dim<T2, DIM_ARGS_A> &d2) {
  return d1.value() < d2.value();
}

template<class T1, class T2, DIM_DEC_A>
static inline
int operator>=(const Dim<T1, DIM_ARGS_A> &d1, const Dim<T2, DIM_ARGS_A> &d2) {
  return d1.value() >= d2.value();
}

template<class T1, class T2, DIM_DEC_A>
static inline
int operator>(const Dim<T1, DIM_ARGS_A> &d1, const Dim<T2, DIM_ARGS_A> &d2) {
  return d1.value() > d2.value();
}

//Return 1 (0) [-1] if 1st arg. is > (==) [<] second arg.
template<class T1, class T2, DIM_DEC_A>
static inline
int compare(const Dim<T1, DIM_ARGS_A> &d1, const Dim<T2, DIM_ARGS_A> &d2) {
  return 
    (d1.value() > d2.value()) 
    ? 1
    : ( (d1.value() < d2.value()) ? -1 : 0 );
}

#endif	/* ifndef DIM_NO_CHECK */

/*     				Scale Class.				*/

//Used for literal dimensioned quantities and I/O.

DIM_TEMPLATE_1(DIM_DEC_A)
class DimScale {  			//Should not be directly used by user.
  DimScaleType convert;			//Conversion factor.
public:
  DimScale(const DIM_SCALE(DIM_ARGS_A) &s) { convert= s.convert; }
  DimScale(DimScaleType s= 1) : convert(s) { }
  DimScaleType scale() const { return convert; }
};

//Convert DimValType * DimScale into a Dim.
DIM_TEMPLATE_1(DIM_DEC_A)
inline static 
DIM_X(DIM_VAL_TYPE, DIM_ARGS_A)
operator*(DIM_VAL_TYPE v, const DIM_SCALE(DIM_ARGS_A) &s) {
  return DIM_X(DIM_VAL_TYPE, DIM_ARGS_A)(v * s.scale());
}

//Convert DimValType / DimScale into a Dim.
DIM_TEMPLATE_1(DIM_DEC_A)
inline static 
DIM_X(DIM_VAL_TYPE, DIM_ARGS_A)
operator/(DIM_VAL_TYPE v, const DIM_SCALE(DIM_ARGS_A) &s) {
  return DIM_X(DIM_VAL_TYPE, DIM_ARGS_A)(v / s.scale());
}

#ifndef DIM_NO_CHECK	/* When DIM_NO_CHECK Dim and DimValType are the same. */
//Allow Dim * DimScale.
template<class T, DIM_DEC_AB> 
inline static 
Dim<T, DIM_ARGS_ADD> operator*(const Dim<T, DIM_ARGS_A> &d, 
			       const DimScale<DIM_ARGS_B> &s) {
  return Dim<T, DIM_ARGS_ADD>(d.value() * s.scale());
}

//Allow Dim / DimScale.
template<class T, DIM_DEC_AB> 
inline static 
Dim<T, DIM_ARGS_SUB> operator/(const Dim<T, DIM_ARGS_A> &d, 
			       const DimScale<DIM_ARGS_B> &s) {
  return Dim<T, DIM_ARGS_SUB>(d.value() / s.scale());
}

#endif /* ifndef DIM_NO_CHECK */

//Combine scales using * and /.
DIM_TEMPLATE_1(DIM_DEC_AB)
static inline 
DIM_SCALE(DIM_ARGS_ADD) operator*(const DIM_SCALE(DIM_ARGS_A) &s1, 
				  const DIM_SCALE(DIM_ARGS_B) &s2) {
  return DIM_SCALE(DIM_ARGS_ADD) (s1.scale() * s2.scale());
}

DIM_TEMPLATE_1(DIM_DEC_AB)
static inline 
DIM_SCALE(DIM_ARGS_SUB)  operator/(const DIM_SCALE(DIM_ARGS_A)  &s1, 
				 const DIM_SCALE(DIM_ARGS_B)  &s2) {
  return DIM_SCALE(DIM_ARGS_SUB) (s1.scale() / s2.scale());
}

/*     				Units Class.				*/

// Used for output.  DimUnit is the class used for storing information
// provided by the user.  DimUnitX is the class used to do the actual
// output.  Hence during output, DimUnit is converted to DimUnitX.  
// For convenience, the user is allowed to apply * and / on any combination
// of DimUnit & DimUnitX. 

DIM_TEMPLATE_1(DIM_DEC_A) 
class DimUnit  { 		//Should not be directly used by user.
  DimScaleType convert;		//Conversion factor.
  unsigned dimIndex;		//Which dimension index # this unit is for.
  char *name;			//Print name of unit.
public:
  DimUnit() { }			//For declaring dummy units to satisfy gcc.
  DimUnit(DimScaleType s, unsigned dimI, char *printName) 
    : convert(s), dimIndex(dimI), name(printName) { }
  DimScaleType scale() const { return convert; }
  unsigned dimNum() const { return dimIndex; }
  char *printName() const { return name; }
};

//The following class builds temporary objects during output.
DIM_TEMPLATE_1(DIM_DEC_A)
class DimUnitX  { 		//Should not be directly used by user.
  DimScaleType convert;		//Conversion factor.
  int powers[N_DIM];		//Dimension powers.
  const char *names[N_DIM];	//Print name of unit.
public:
  DimUnitX(const DIM_UNIT(DIM_ARGS_A)  &u) : convert(u.scale()) {
    const unsigned dimN= u.dimNum();
    assert(dimN < N_DIM);
    for (unsigned i= 0; i < N_DIM; i++) { names[i]= 0; powers[i]= 0; }
    names[dimN]= u.printName(); powers[dimN]= DIM_DENOM;
  }
  DimUnitX(DimScaleType s= 0) : convert(s) { 
    for (unsigned i= 0; i < N_DIM; i++) { names[i]= 0; powers[i]= 0; }
  }
  DimScaleType scale() const { return convert; }
  const int *dimPowers() const { return powers; }
  const char *const *printNames() const { return names; }
  void setDimPower(int dimN, int pow) { powers[dimN]= pow; }
  void setDimName(int dimN, const char *name) { names[dimN]= name; }
};

//Combine units using * and /.
DIM_TEMPLATE_1(DIM_DEC_AB)
static inline 
DIM_UNITX(DIM_ARGS_ADD)  operator*(const DIM_UNITX(DIM_ARGS_A)  &u1, 
				   const DIM_UNITX(DIM_ARGS_B)  &u2) {
  DIM_UNITX(DIM_ARGS_ADD)  uz(u1.scale() * u2.scale());
  for (unsigned i= 0; i < N_DIM; i++) {
    assert(!u1.printNames()[i] || !u2.printNames()[i] || 
           strcmp(u1.printNames()[i], u2.printNames()[i]) == 0);
    uz.setDimName(i, u1.printNames()[i] 
		     ? u1.printNames()[i] 
		     : u2.printNames()[i]);
    uz.setDimPower(i, u1.dimPowers()[i] + u2.dimPowers()[i]);
  }
  return uz;
}

DIM_TEMPLATE_1(DIM_DEC_AB)
static inline 
DIM_UNITX(DIM_ARGS_ADD) operator*(const DIM_UNIT(DIM_ARGS_A) &u1, 
				  const DIM_UNIT(DIM_ARGS_B) &u2) {
  return DIM_UNITX(DIM_ARGS_A) (u1) * DIM_UNITX(DIM_ARGS_B) (u2);
}

DIM_TEMPLATE_1(DIM_DEC_AB)
static inline 
DIM_UNITX(DIM_ARGS_ADD) operator*(const DIM_UNITX(DIM_ARGS_A) &u1, 
				  const DIM_UNIT(DIM_ARGS_B) &u2) {
  return u1 * DIM_UNITX(DIM_ARGS_B) (u2);
}

DIM_TEMPLATE_1(DIM_DEC_AB)
static inline 
DIM_UNITX(DIM_ARGS_ADD)  operator*(const DIM_UNIT(DIM_ARGS_A)  &u1, 
				 const DIM_UNITX(DIM_ARGS_B)  &u2) {
  return DIM_UNITX(DIM_ARGS_A) (u1) * u2;
}

DIM_TEMPLATE_1(DIM_DEC_AB)
static inline 
DIM_UNITX(DIM_ARGS_SUB)  operator/(const DIM_UNITX(DIM_ARGS_A)  &u1, 
				   const DIM_UNITX(DIM_ARGS_B)  &u2) {
  DIM_UNITX(DIM_ARGS_SUB)  uz(u1.scale() / u2.scale());
  for (unsigned i= 0; i < N_DIM; i++) {
    assert(!u1.printNames()[i] || !u2.printNames()[i] || 
           strcmp(u1.printNames()[i], u2.printNames()[i]) == 0);
    uz.setDimName(i, u1.printNames()[i] 
		     ? u1.printNames()[i] 
		     : u2.printNames()[i]);
    uz.setDimPower(i, u1.dimPowers()[i] - u2.dimPowers()[i]);
  }
  return uz;
}

DIM_TEMPLATE_1(DIM_DEC_AB)
static inline 
DIM_UNITX(DIM_ARGS_SUB)  operator/(const DIM_UNIT(DIM_ARGS_A)  &u1, 
				 const DIM_UNIT(DIM_ARGS_B)  &u2) {
  return DIM_UNITX(DIM_ARGS_A) (u1) / DIM_UNITX(DIM_ARGS_B) (u2);
}

DIM_TEMPLATE_1(DIM_DEC_AB)
static inline 
DIM_UNITX(DIM_ARGS_SUB)  operator/(const DIM_UNITX(DIM_ARGS_A)  &u1, 
				 const DIM_UNIT(DIM_ARGS_B)  &u2) {
  return u1 / DIM_UNITX(DIM_ARGS_B) (u2);
}

DIM_TEMPLATE_1(DIM_DEC_AB)
static inline 
DIM_UNITX(DIM_ARGS_SUB)  operator/(const DIM_UNIT(DIM_ARGS_A)  &u1, 
				 const DIM_UNITX(DIM_ARGS_B)  &u2) {
  return DIM_UNITX(DIM_ARGS_A) (u1) / u2;
}


/*				POWERS					*/

typedef double DimPowType;

#ifdef DIM_NO_CHECK

#define DIM_POW_TYPE(num, denom)	DimPowType
#define DIM_POWER(n, d)			((double)(n)/(d))
#define DIM_POW_VAL(p)			(p)

#else /* #ifdef DIM_NO_CHECK */

/* Define DimPow class template parameterized by numerator and denominator
 * to make compile-time adjustment of dimensional exponents possible.
 */

#define DIM_POW_TYPE(num, denom)	DimPow<num, denom>
#define DIM_POWER(n, d)			DimPow<n, d>((double)(n)/(d))
#define DIM_POW_VAL(p)			(p.power())
#define DIM_POW_APP(n)			(DA ## n * num)/denom

/* DA0 * num/denom, DA1 * num/denom, ... */
#define DIM_POW_ARGS			DIM_NUMS(DIM_POW_APP)


template<int num, int denom> class DimPow {
  double exp;
public:
  DimPow(double e) : exp(e) { }
  double power() { return exp; }
};

#endif /* ifdef DIM_NO_CHECK */

DIM_TEMPLATE_4(class T, DIM_DEC_A, int num, int denom)
static inline
DIM_X(T, DIM_POW_ARGS) dimPow(const DIM_X(T, DIM_ARGS_A) &d, 
			      const DIM_POW_TYPE(num, denom) &p) {
  return DIM_X(T, DIM_POW_ARGS)(DIM_POW(DIM_VALUE(d), DIM_POW_VAL(p)));
}

DIM_TEMPLATE_3(DIM_DEC_A, int num, int denom)
static inline
DIM_SCALE(DIM_POW_ARGS) scalePow(const DIM_SCALE(DIM_ARGS_A) &s,
			       const DIM_POW_TYPE(num, denom) &p) {
  return DIM_SCALE(DIM_POW_ARGS)(DIM_POW(s.scale(), DIM_POW_VAL(p)));
}

DIM_TEMPLATE_3(DIM_DEC_A, int num, int denom)
static inline
DIM_UNIT(DIM_POW_ARGS) unitPow(const DIM_UNIT(DIM_ARGS_A) &u,
			       const DIM_POW_TYPE(num, denom) &p) {
  return DIM_UNIT(DIM_POW_ARGS)(DIM_POW(u.scale(), DIM_POW_VAL(p)),
				u.dimNum(), u.printName());
}

DIM_TEMPLATE_3(DIM_DEC_A, int num, int denom)
static inline
DIM_UNITX(DIM_POW_ARGS) unitPow(const DIM_UNITX(DIM_ARGS_A) &u,
				const DIM_POW_TYPE(num, denom) &p) {
  DIM_UNITX(DIM_POW_ARGS) uz(DIM_POW(u.scale(), DIM_POW_VAL(p)));
  for (unsigned i= 0; i < N_DIM; i++) {
    uz.setDimName(i, u.printNames()[i]);
    uz.setDimPower(i, (int)(u.dimPowers()[i] * DIM_POW_VAL(p)));
  }
  return uz;
}




/*				INPUT.					*/

DIM_TEMPLATE_1(DIM_DEC_A) class DimIn  {
  istream &iStream;
  const DIM_SCALE(DIM_ARGS_A) &convert;
public:
  DimIn() : iStream(cin), convert(DIM_SCALE(DIM_ARGS_A)()) { 
  }	//For initializing dummies.
  DimIn(istream &input, const DIM_SCALE(DIM_ARGS_A)  &s) : 
    iStream(input), convert(s) { }
  const DIM_SCALE(DIM_ARGS_A) &scale() const { return convert; }
  istream &in() const { return iStream; }
#ifdef DIM_GPP_BUG
  istream &operator>>(DIM_X(DIM_VAL_TYPE, DIM_ARGS_A)  &d) {
    DIM_VAL_TYPE v;
    iStream >> v; d= v * convert;
    return iStream;
  }
#endif
};

DIM_TEMPLATE_1(DIM_DEC_A)
static inline
DIM_IN(DIM_ARGS_A)  operator>>(istream &in, const DIM_SCALE(DIM_ARGS_A)  &s) {
  return DIM_IN(DIM_ARGS_A) (in, s);
}

#ifndef DIM_GPP_BUG
DIM_TEMPLATE_2(class T, DIM_DEC_A)
static inline
istream &operator>>(const DIM_IN(DIM_ARGS_A)  &dimIn, 
		    DIM_X(T, DIM_ARGS_A)  &d) {
  T v;
  dimIn.in() >> v; d= v * dimIn.scale();
  return dimIn.in();
}
#endif

/*				OUTPUT.					*/

//Declare library function.
ostream &dimOut(ostream &c, const unsigned nDim, const unsigned denom, 
		const double val, 
		const int dims[], const char *const *dimNames);

//Outputting units as manipulator.
DIM_TEMPLATE_1(DIM_DEC_A) class DimUnitOut {
  DIM_UNITX(DIM_ARGS_A)  u;
  ostream &oStream;
public:
  DimUnitOut() : oStream(cout) { } //For initializing dummies.
  DimUnitOut(ostream &o, const DIM_UNITX(DIM_ARGS_A)  &unit) : 
    oStream(o), u(unit) { }
  const DIM_UNITX(DIM_ARGS_A)  &unit() const { return u; }
  ostream &out() const { return oStream; }
#ifdef DIM_GPP_BUG
  ostream &operator<<(const DIM_X(DIM_VAL_TYPE, DIM_ARGS_A)  &d) {
    return dimOut(oStream, N_DIM, DIM_DENOM, DIM_VALUE(d)/u.scale(),
		  u.dimPowers(), u.printNames());
  }
#endif
};

DIM_TEMPLATE_1(DIM_DEC_A)
static inline
DIM_UNIT_OUT(DIM_ARGS_A)  operator<<(ostream &out, const DIM_UNIT(DIM_ARGS_A)  &u) {
  return DIM_UNIT_OUT(DIM_ARGS_A) (out, DIM_UNITX(DIM_ARGS_A) (u));
}

DIM_TEMPLATE_1(DIM_DEC_A)
static inline
DIM_UNIT_OUT(DIM_ARGS_A)  operator<<(ostream &out, const DIM_UNITX(DIM_ARGS_A)  &u) {
  return DIM_UNIT_OUT(DIM_ARGS_A) (out, u);
}

#ifndef DIM_GPP_BUG
DIM_TEMPLATE_2(class T, DIM_DEC_A)
static inline
ostream &operator<<(const DIM_UNIT_OUT(DIM_ARGS_A)  &dimUO, 
		    const DIM_X(T, DIM_ARGS_A)  &d) {
  return dimOut(dimUO.out(), N_DIM, DIM_DENOM, 
		DIM_VALUE(d) / dimUO.unit().scale(),
                dimUO.unit().dimPowers(), dimUO.unit().printNames());
}
#endif

//Outputting scales as manipulator.
DIM_TEMPLATE_1(DIM_DEC_A) class DimScaleOut {
  DimScaleType convert;
  ostream &oStream;
public:
  DimScaleOut() : oStream(cout) { } //For initializing dummies.
  DimScaleOut(ostream &o, const DIM_SCALE(DIM_ARGS_A)  &s) : 
    oStream(o), convert(s.scale()) { }
  DimScaleType scale() { return convert; }
  ostream &out() { return oStream; }
#ifdef DIM_GPP_BUG
  ostream &operator<<(const DIM_X(DIM_VAL_TYPE, DIM_ARGS_A)  &d) {
    return oStream << DIM_VALUE(d)/convert;
  }
#endif
};

DIM_TEMPLATE_1(DIM_DEC_A)
static inline
DIM_SCALE_OUT(DIM_ARGS_A)  operator<<(ostream &out, 
				   const DIM_SCALE(DIM_ARGS_A)  &s) {
  return DIM_SCALE_OUT(DIM_ARGS_A) (out, s);
}

#ifndef DIM_GPP_BUG
DIM_TEMPLATE_2(class T, DIM_DEC_A)
static inline
ostream &operator<<(const DIM_SCALE_OUT(DIM_ARGS_A)  &dimSO, 
		    const DIM_X(T, DIM_ARGS_A) &d) {
  return dimSO.out() << DIM_VALUE(d) / dimSO.scale();
}
#endif /* ifndef DIM_GPP_BUG */

#endif /* ifndef _DIM_H */
