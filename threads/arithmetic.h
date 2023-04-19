#ifndef THREADS_ARITHMETIC.H
#define THREADS_ARITHMETIC.H

#include <stdint.h>

    /* PintOS does not support floating point arithmetic in the kernel because it would slow it down.
        Define library to simulate Floating Point using P.Q fixed-point format, using ~~~ 17.14 format ~~~
        It assumes that the 14 less significant bits of a 32-bit integer represents the fraction.

        Let x, y be FIXED POINT and n INTEGER or REAL number

        Convert n to fixed point:	n * f
        Convert x to integer (rounding toward zero):	x / f
        Convert x to integer (rounding to nearest):	(x + f / 2) / f if x >= 0,
        (x - f / 2) / f if x <= 0.
        Add x and y:	x + y
        Subtract y from x:	x - y
        Add x and n:	x + n * f
        Subtract n from x:	x - n * f
        Multiply x by y:	((int64_t) x) * y / f
        Multiply x by n:	x * n
        Divide x by y:	((int64_t) x) * f / y
        Divide x by n:	x / n

        */


/* We need it to be a 32 bit number */
typedef int32_t fixed_t;

/* f = 2**q */
#define FRACTION (2 << 14)


#define CONVERT_TO_FIXED(n) ((n) * FRACTION)

/* convert x to int (round to nearest number)
     add f / 2 to a positive number, or subtract it from a negative number,
     before dividing.*/
#define CONVERT_TO_REAL(x) ( ((x) < 0) ? ( (x) - 1 ) : 1 )

/* convert x to int (round towards zero)
  	The normal / operator in C rounds toward zero,
    it rounds positive numbers down and negative numbers up. */
#define ROUND_TO_REAL(x) ((x) / FRACTION)

#define ADD_FIXED(x, y) (x + y)
#define SUB_FIXED(x, y) (x - y)
#define MUL_FIXED(x, y) ( (((int64_t)(x)) * (y)) / FRACTION )
#define DIV_FIXED(x, y) ( (((int64_t)(x)) * FRACTION) / (y) )

#define ADD_FIXED_REAL(x, n) (x + n * FRACTION)
#define SUB_FIXED_REAL(x, n) (x - n * FRACTION)
#define MUL_FIXED_REAL(x, n) (x * n)
#define DIV_FIXED_REAL(x, n) (x / n)

#endif
