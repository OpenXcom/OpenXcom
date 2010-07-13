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
#ifndef OPENXCOM__RNG_H
#define OPENXCOM__RNG_H

/**
 * Random Number Generator used throughout the game
 * for all your randomness needs. It's really just the
 * standard C generator, but wrapped in a way that we
 * can store its seed for later use if necessary.
 */
class RNG
{
private:
public:
	RNG();
	~RNG();
	/// Seed used for random number generation.
	static int seed;
	/// Initializes the generator.
	static void init(int seed = -1);
	/// Generates a random number.
	static int generate(int min, int max);
};

#endif
