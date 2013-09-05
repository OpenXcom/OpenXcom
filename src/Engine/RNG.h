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
#ifndef OPENXCOM_RNG_H
#define OPENXCOM_RNG_H

#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

/**
 * Random Number Generator used throughout the game
 * for all your randomness needs. It's really just the
 * standard C generator, but wrapped in a way that we
 * can store its seed for later use if necessary.
 */
namespace RNG
{
	/// Initializes the generator.
	void init(long count = -1, unsigned int seed = 0);
	/// Loads the RNG from YAML.
	void load(const YAML::Node& node);
	/// Saves the RNG to YAML.
	void save(YAML::Node& node);
	/// Generates a random integer number.
	int generate(int min, int max);
	/// Generates a random decimal number.
	double generate(double min, double max);
	/// Get normally distributed value.
	double boxMuller(double m = 0, double s = 1);
}

}

#endif
