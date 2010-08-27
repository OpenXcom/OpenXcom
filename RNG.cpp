/*
 * Copyright 2010 Daniel Albano
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
#include <cstdlib>
#include <ctime>

/**
 * Defaults to the current time.
 */
int RNG::seed = (int)time(NULL);

/**
 * Seeds the random generator with a new number.
 * @param seed New seed. Defaults to existing seed if none is set.
 */
void RNG::init(int seed)
{
	if (seed == -1)
		srand(RNG::seed);
	else
		srand(seed);
}

/**
 * Generates a random number within a certain range.
 * @param min Minimum number.
 * @param max Maximum number.
 * @return Generated number.
 */
int RNG::generate(int min, int max)
{
	seed = rand();
	return (seed % (max - min + 1) + min);
}
