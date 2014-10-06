/*
 * Copyright 2012 OpenXcom Developers.
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
#include "RuleAlienMission.h"
#include "../Savegame/WeightedOptions.h"

namespace YAML
{
	template<>
	struct convert<OpenXcom::MissionWave>
	{
		static Node encode(const OpenXcom::MissionWave& rhs)
		{
			Node node;
			node["ufo"] = rhs.ufoType;
			node["count"] = rhs.ufoCount;
			node["trajectory"] = rhs.trajectory;
			node["timer"] = rhs.spawnTimer;
			return node;
		}

		static bool decode(const Node& node, OpenXcom::MissionWave& rhs)
		{
			if (!node.IsMap())
				return false;

			rhs.ufoType = node["ufo"].as<std::string>();
			rhs.ufoCount = node["count"].as<size_t>();
			rhs.trajectory = node["trajectory"].as<std::string>();
			rhs.spawnTimer = node["timer"].as<size_t>();
			return true;
		}
	};
}

namespace OpenXcom
{

RuleAlienMission::RuleAlienMission(const std::string &type) : _type(type), _points(0)
{
}

/**
 * Loads the mission data from a YAML node.
 * @param node YAML node.
 */
void RuleAlienMission::load(const YAML::Node &node)
{
	_type = node["type"].as<std::string>(_type);
	_points = node["points"].as<int>(_points);
	_waves = node["waves"].as< std::vector<MissionWave> >(_waves);
	//Only allow full replacement of mission racial distribution.
	if (const YAML::Node &weights = node["raceWeights"])
	{
		typedef std::map<size_t, WeightedOptions*> Associative;
		typedef std::vector<std::pair<size_t, WeightedOptions*> > Linear;

		Associative assoc;
		//Place in the associative container so we can index by month and keep entries sorted.
		for (Linear::iterator ii = _raceDistribution.begin(); ii != _raceDistribution.end(); ++ii)
		{
			assoc.insert(*ii);
		}

		// Now go through the node contents and merge with existing data.
		for (YAML::const_iterator nn = weights.begin(); nn != weights.end(); ++nn)
		{
			size_t month = nn->first.as<size_t>();
			Associative::iterator existing = assoc.find(month);
			if (assoc.end() == existing)
			{
				// New entry, load and add it.
				std::auto_ptr<WeightedOptions> nw(new WeightedOptions);
				nw->load(nn->second);
				assoc.insert(std::make_pair(month, nw.release()));
			}
			else
			{
				// Existing entry, update it.
				existing->second->load(nn->second);
			}
		}

		// Now replace values in our actual member variable!
		_raceDistribution.clear();
		_raceDistribution.reserve(assoc.size());
		for (Associative::iterator ii = assoc.begin(); ii != assoc.end(); ++ii)
		{
			if (ii->second->empty())
			{
				// Don't keep empty lists.
				delete ii->second;
			}
			else
			{
				// Place it
				_raceDistribution.push_back(*ii);
			}
		}
	}
}

/**
 * Chooses one of the available races for this mission.
 * The racial distribution may vary based on the current game date.
 * @param monthsPassed The number of months that have passed in the game world.
 * @return The string id of the race.
 */
const std::string RuleAlienMission::generateRace(const size_t monthsPassed) const
{
	std::vector<std::pair<size_t, WeightedOptions*> >::const_reverse_iterator rc = _raceDistribution.rbegin();
	while (monthsPassed < rc->first)
		++rc;
	return rc->second->choose();
}


const std::string RuleAlienMission::getTopRace(const size_t monthsPassed) const
{
	std::vector<std::pair<size_t, WeightedOptions*> >::const_iterator rc = _raceDistribution.begin();	
	return rc->second->top();
}

/**
 * Ensures the allocated memory is released.
 */
RuleAlienMission::~RuleAlienMission()
{
	for (std::vector<std::pair<size_t, WeightedOptions*> >::const_iterator ii = _raceDistribution.begin(); ii != _raceDistribution.end(); ++ii)
	{
		delete ii->second;
	}
}

/**
 * Returns the Alien score for this mission.
 * @return Amount of points.
 */
int RuleAlienMission::getPoints() const
{
	return _points;
}

}
