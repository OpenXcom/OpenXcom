/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include <cstdlib>
#include <ctime>

namespace OpenXcom
{
namespace RNG
{

int _seed = 0;

/**
 * Seeds the random generator with a new number.
 * Defaults to the current time if none is set.
 * @param seed New seed.
 */
void init(int seed)
{
	if (seed == -1)
	{
		_seed = (int)time(NULL);
	}
	else
	{
		_seed = seed;
	}
	srand(_seed);
}

/**
 * Returns the last seed used by the generator.
 * @return Generator seed.
 */
int getSeed()
{
	return _seed;
}

/**
 * Generates a random integer number within a certain range.
 * @param min Minimum number.
 * @param max Maximum number.
 * @return Generated number.
 */
int generate(int min, int max)
{
	_seed = rand();
	return (_seed % (max - min + 1) + min);
}

/**
 * Generates a random decimal number within a certain range.
 * @param min Minimum number.
 * @param max Maximum number.
 * @return Generated number.
 */
double generate(double min, double max)
{
	_seed = rand();
	return (_seed * (max - min) / RAND_MAX + min);
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

}
}