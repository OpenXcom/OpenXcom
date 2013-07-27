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

namespace OpenXcom
{

/**
 * Loads the mission data from a YAML node.
 * @param node YAML node.
 */
void RuleAlienMission::load(const YAML::Node &node)
{
	node["type"] >> _type;
	node["points"] >> _points;
	//Only allow full replacement of mission racial distribution.
	if (const YAML::Node *pNode = node.FindValue("raceWeights"))
	{
		typedef std::map<unsigned, WeightedOptions*> Associative;
		typedef std::vector<std::pair<unsigned, WeightedOptions*> > Linear;

		Associative assoc;
		//Place in the associative container so we can index by month and keep entries sorted.
		for (Linear::iterator ii = _raceDistribution.begin(); ii != _raceDistribution.end(); ++ii)
		{
			assoc.insert(*ii);
		}

		// Now go through the node contents and merge with existing data.
		for (YAML::Iterator nn = pNode->begin(); nn != pNode->end(); ++nn)
		{
			unsigned month;
			nn.first() >> month;
			Associative::iterator existing = assoc.find(month);
			if (assoc.end() == existing)
			{
				// New entry, load and add it.
				std::auto_ptr<WeightedOptions> nw(new WeightedOptions);
				nw->load(nn.second());
				assoc.insert(std::make_pair(month, nw.release()));
			}
			else
			{
				// Existing entry, update it.
				existing->second->load(nn.second());
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
	//Only allow full replacement of mission waves.
	if (const YAML::Node *pNode = node.FindValue("waves"))
	{
		*pNode >> _waves;
	}
}

/**
 * Saves the alien mission data to a YAML file.
 * @param out YAML emitter.
 */
void RuleAlienMission::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "type" << YAML::Value <<_type;
	out << YAML::Key << "raceWeights" << YAML::Value;
	out << YAML::BeginMap;
	for (std::vector<std::pair<unsigned, WeightedOptions*> >::const_iterator ii = _raceDistribution.begin();
	     ii != _raceDistribution.end(); ++ii)
	{
		out << YAML::Key << ii->first << YAML::Value;
		ii->second->save(out);
	}
	out << YAML::EndMap;
	out << YAML::Key << "waves" << YAML::Value << _waves;
}

/**
 * Choose one of the available races for this mission.
 * The racial distribution may vary based on the current game date.
 * @param gameTime The current date and time of the game world.
 * @return The string id of the race.
 */
const std::string &RuleAlienMission::generateRace(const unsigned monthsPassed) const
{
	std::vector<std::pair<unsigned, WeightedOptions*> >::const_reverse_iterator rc = _raceDistribution.rbegin();
	while (monthsPassed < rc->first)
		++rc;
	return rc->second->choose();
}

/**
 * Make sure the allocated memory is released.
 */
RuleAlienMission::~RuleAlienMission()
{
	for (std::vector<std::pair<unsigned, WeightedOptions*> >::const_iterator ii = _raceDistribution.begin(); ii != _raceDistribution.end(); ++ii)
	{
		delete ii->second;
	}
}

/**
 * Return the Alien score for this mission.
 */
int RuleAlienMission::getPoints() const
{
	return _points;
}

void operator<<(YAML::Emitter &out, const MissionWave &wave)
{
	out << YAML::BeginMap;
	out << YAML::Key << "ufo" << YAML::Value << wave.ufoType;
	out << YAML::Key << "count" << YAML::Value << wave.ufoCount;
	out << YAML::Key << "trajectory" << YAML::Value << wave.trajectory;
	out << YAML::Key << "timer" << YAML::Value << wave.spawnTimer;
	out << YAML::EndMap;
}

void operator>>(const YAML::Node &node, MissionWave &wave)
{
	node["ufo"] >> wave.ufoType;
	node["count"] >> wave.ufoCount;
	node["trajectory"] >> wave.trajectory;
	node["timer"] >> wave.spawnTimer;
}

}
