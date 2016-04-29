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
#include "WeightedOptions.h"
#include "../Engine/RNG.h"

namespace OpenXcom
{

/**
 * Select a random choice from among the contents.
 * This MUST be called on non-empty objects.
 * Each time this is called, the returned value can be different.
 * @return The key of the selected choice.
 */
std::string WeightedOptions::choose() const
{
	if (_totalWeight == 0)
	{
		return "";
	}
	size_t var = RNG::generate(0, _totalWeight);
	std::map<std::string, size_t>::const_iterator ii = _choices.begin();
	for (; ii != _choices.end(); ++ii)
	{
		if (var <= ii->second)
			break;
		var -= ii->second;
	}
	// We always have a valid iterator here.
	return ii->first;
}

/**
 * Set an option's weight.
 * If @a weight is set to 0, the option is removed from the list of choices.
 * If @a id already exists, the new weight replaces the old one, otherwise
 * @a id is added to the list of choices, with @a weight as the weight.
 * @param id The option name.
 * @param weight The option's new weight.
 */
void WeightedOptions::set(const std::string &id, size_t weight)
{
	std::map<std::string, size_t>::iterator option = _choices.find(id);
	if (option != _choices.end())
	{
		_totalWeight -= option->second;
		if (0 != weight)
		{
			option->second = weight;
			_totalWeight += weight;
		}
		else
		{
			_choices.erase(option);
		}
	}
	else if (0 != weight)
	{
		_choices.insert(std::make_pair(id, weight));
		_totalWeight += weight;
	}
}

/**
 * Add the weighted options from a YAML::Node to a WeightedOptions.
 * The weight option list is not replaced, only values in @a nd will be added /
 * changed.
 * @param nd The YAML node (containing a map) with the new values.
 */
void WeightedOptions::load(const YAML::Node &nd)
{
	for (YAML::const_iterator val = nd.begin(); val != nd.end(); ++val)
	{
		std::string id = val->first.as<std::string>();
		size_t w = val->second.as<size_t>();
		set(id, w);
	}
}

/**
 * Send the WeightedOption contents to a YAML::Emitter.
 * @return YAML node.
 */
YAML::Node WeightedOptions::save() const
{
	YAML::Node node;
	for (std::map<std::string, size_t>::const_iterator ii = _choices.begin(); ii != _choices.end(); ++ii)
	{
		node[ii->first] = ii->second;
	}
	return node;
}

/**
 * Get the list of strings associated with these weights.
 * @return the list of strings in these weights.
 */
std::vector<std::string> WeightedOptions::getNames()
{
	std::vector<std::string> names;
	for (std::map<std::string, size_t>::const_iterator ii = _choices.begin(); ii != _choices.end(); ++ii)
	{
		names.push_back((*ii).first);
	}
	return names;
}

}
