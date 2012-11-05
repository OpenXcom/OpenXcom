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

unsigned int _seed = 0;
long _count = 0;

/**
 * Seeds the random generator with a new number.
 * Defaults to the current time if none is set.
 * @param seed New seed.
 */
void init(long count, unsigned int seed)
{
	_seed = seed;
	_count = count;
	if (count == -1)
	{
		_seed = (unsigned int)time(NULL);
		_count = 0;
		srand(_seed);
	}
	else
	{
		srand(_seed);
		for (long i = 0; i < count; ++i)
		{
			rand();
		}
	}
}

/**
 * Loads the RNG from a YAML file.
 * @param node YAML node.
 */
void load(const YAML::Node &node)
{
	int count, seed;
	if (node.FindValue("rngCount") != 0)
	{
		node["rngCount"] >> count;
		node["rngSeed"] >> seed;
		init(count, seed);
	}
}

/**
 * Saves the RNG to a YAML file.
 * @param out YAML emitter.
 */
void save(YAML::Emitter &out)
{
	out << YAML::Key << "rngCount" << YAML::Value << _count;
	out << YAML::Key << "rngSeed" << YAML::Value << _seed;
}

/**
 * Generates a random integer number within a certain range.
 * @param min Minimum number.
 * @param max Maximum number.
 * @return Generated number.
 */
int generate(int min, int max)
{
	_count++;
	int num = rand();
	return (num % (max - min + 1) + min);
}

/**
 * Generates a random decimal number within a certain range.
 * @param min Minimum number.
 * @param max Maximum number.
 * @return Generated number.
 */
double generate(double min, double max)
{
	_count++;
	int num = rand();
	return (num * (max - min) / RAND_MAX + min);
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