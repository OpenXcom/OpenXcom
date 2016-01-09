#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include <string>
#include <map>
#include <vector>
#include <yaml-cpp/yaml.h>


namespace OpenXcom
{

/**
 * Holds pairs of relative weights and IDs.
 * It is used to store options and make a random choice between them.
 */
class WeightedOptions
{
public:
	/// Create an empty set.
	WeightedOptions() : _totalWeight(0) { /* Empty by design. */ }
	/// Select from among the items.
	std::string choose() const;
	/// Set an option's weight.
	void set(const std::string &id, size_t weight);
	/// Is this empty?
	bool empty() const { return 0 == _totalWeight; }
	/// Remove all entries.
	void clear() { _totalWeight = 0; _choices.clear(); }
	/// Update our list with data from YAML.
	void load(const YAML::Node &node);
	/// Store our list in YAML.
	YAML::Node save() const;
	/// Get the list of strings associated with these weights.
	std::vector<std::string> getNames();
private:
	std::map<std::string, size_t> _choices; //!< Options and weights
	size_t _totalWeight; //!< The total weight of all options.
};

}
