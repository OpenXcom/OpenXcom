/*
 * Copyright 2010-2013 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "RNG.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <ctime>
#include <climits>

/*Period parameters */
#define CMATH_N 624
#define CMATH_M 397
#define CMATH_MATRIX_A 0x9908b0df   /*constant vector a */
#define CMATH_UPPER_MASK 0x80000000 /*most significant w-r bits */
#define CMATH_LOWER_MASK 0x7fffffff /*least significant r bits */

/*Tempering parameters */
#define CMATH_TEMPERING_MASK_B 0x9d2c5680
#define CMATH_TEMPERING_MASK_C 0xefc60000
#define CMATH_TEMPERING_SHIFT_U(y)  (y >> 11)
#define CMATH_TEMPERING_SHIFT_S(y)  (y << 7)
#define CMATH_TEMPERING_SHIFT_T(y)  (y << 15)
#define CMATH_TEMPERING_SHIFT_L(y)  (y >> 18)

namespace OpenXcom
{
namespace RNG
{

unsigned int rseed = 1;
unsigned long mt[CMATH_N];    /*the array for the state vector */
int mti = CMATH_N+1;    /*mti==N+1 means mt [N ] is not initialized **/

/**
* Returns the current seed in use by the generator.
* @return Current seed.
*/
unsigned int getSeed()
{
	return rseed;
}

/**
* Changes the current seed in use by the generator.
* @param n New seed.
*/
void setSeed(unsigned int n)
{
	/* setting initial seeds to mt[N] using         */
	/* the generator Line 25 of Table 1 in          */
	/* [KNUTH 1981, The Art of Computer Programming */
	/*    Vol. 2 (2nd Ed.), pp102]                  */
	mt[0]= n & 0xffffffff;
	for (mti=1; mti<CMATH_N; mti++)
		mt[mti] = (69069 * mt[mti-1]) & 0xffffffff;

	rseed = n;
}

/**
 * Generates a number using a Mersenne Twister
 * pseudorandom number generator.
 * @return Random unsigned 32-bit number (0-LONG_MAX)
 */
unsigned long random()
{
	unsigned long y;
    static unsigned long mag01[2]={0x0, CMATH_MATRIX_A};

    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= CMATH_N) { /* generate N words at one time */
        int kk;

        if (mti == CMATH_N+1)   /* if sgenrand() has not been called, */
            setSeed(4357);      /* a default initial seed is used   */

        for (kk=0;kk<CMATH_N-CMATH_M;kk++) {
            y = (mt[kk]&CMATH_UPPER_MASK)|(mt[kk+1]&CMATH_LOWER_MASK);
            mt[kk] = mt[kk+CMATH_M] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        for (;kk<CMATH_N-1;kk++) {
            y = (mt[kk]&CMATH_UPPER_MASK)|(mt[kk+1]&CMATH_LOWER_MASK);
            mt[kk] = mt[kk+(CMATH_M-CMATH_N)] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        y = (mt[CMATH_N-1]&CMATH_UPPER_MASK)|(mt[0]&CMATH_LOWER_MASK);
        mt[CMATH_N-1] = mt[CMATH_M-1] ^ (y >> 1) ^ mag01[y & 0x1];

        mti = 0;
    }
  
    y = mt[mti++];
    y ^= CMATH_TEMPERING_SHIFT_U(y);
    y ^= CMATH_TEMPERING_SHIFT_S(y) & CMATH_TEMPERING_MASK_B;
    y ^= CMATH_TEMPERING_SHIFT_T(y) & CMATH_TEMPERING_MASK_C;
    y ^= CMATH_TEMPERING_SHIFT_L(y);

	return y;
}

/**
* Seeds the random generator with the current time.
*/
void init()
{
	setSeed((unsigned int) time(NULL));
}

/**
* Seeds the random generator with a new number.
* @param seed New seed.
*/
void init(unsigned int seed)
{
	setSeed(seed);
}

/**
 * Generates a random integer number within a certain range.
 * @param min Minimum number.
 * @param max Maximum number.
 * @return Generated number.
 */
int generate(int min, int max)
{
	unsigned long num = random();
	return (int)(num % (max - min + 1) + min);
}

/**
 * Generates a random decimal number within a certain range.
 * @param min Minimum number.
 * @param max Maximum number.
 * @return Generated number.
 */
double generate(double min, double max)
{
	double num = random();
	return (double)(num / ((double)LONG_MAX / (max - min)) + min);
}

/**
 * Normal random variate generator
 * @param m mean
 * @param s standard deviation
 * @return normally distributed value.
 */
double boxMuller(double m, double s)
{
	double y1;
	static double y2;
	static int use_last = 0;

	if (use_last)				/* use value from previous call */
	{
		y1 = y2;
		use_last = 0;
	}
	else
	{
		double x1, x2, w;
		do {
			x1 = 2.0 * generate(0.0, 1.0) - 1.0;
			x2 = 2.0 * generate(0.0, 1.0) - 1.0;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0 );

		w = sqrt( (-2.0 * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}

	return( m + y1 * s );
}

/**
 * Generates a random percent chance of an event occuring,
 * and returns the result
 * @param value Value percentage (0-100%)
 * @return True if the chance succeeded.
 */
bool percent(int value)
{
	return (generate(0, 99) < value);
}

}
}
