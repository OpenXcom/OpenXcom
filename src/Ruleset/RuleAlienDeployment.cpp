/*
 * Copyright 2010 OpenXcom Developers.
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
#include "RuleAlienDeployment.h"

namespace OpenXcom
{

void operator >> (const YAML::Node& node, ItemSet& s)
{
	node[0] >> s.items;
}

YAML::Emitter& operator << (YAML::Emitter& out, const ItemSet& s)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << s.items << YAML::EndSeq;
	return out;
}

void operator >> (const YAML::Node& node, DeploymentData& s)
{
	node[0] >> s.alienRank;
	node[1] >> s.lowQty;
	node[2] >> s.highQty;
	node[3] >> s.dQty;
	node[4] >> s.percentageOutsideUFO;
	node[5] >> s.itemSets;
}

YAML::Emitter& operator << (YAML::Emitter& out, const DeploymentData& s)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << s.alienRank << s.lowQty << s.highQty << s.dQty << s.percentageOutsideUFO << s.itemSets << YAML::EndSeq;
	return out;
}

/**
 * Creates a blank ruleset for a certain
 * type of deployment data.
 * @param type String defining the type.
 */
RuleAlienDeployment::RuleAlienDeployment(const std::string &type) : _type(type), _data()
{
}

/**
 *
 */
RuleAlienDeployment::~RuleAlienDeployment()
{
}

/**
 * Loads the Deployment from a YAML file.
 * @param node YAML node.
 * @param rule Ruleset for the craft.
 */
void RuleAlienDeployment::load(const YAML::Node &node, const Ruleset *ruleset)
{
	for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
	{
		std::string key;
		i.first() >> key;
		if (key == "type")
		{
			i.second() >> _type;
		}
		else if (key == "data")
		{
			i.second() >> _data;
		}
	}
}

/**
 * Saves the Deployment to a YAML file.
 * @param out YAML emitter.
 */
void RuleAlienDeployment::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "type" << YAML::Value << _type;
	out << YAML::Key << "data" << YAML::Value << _data;
	out << YAML::EndMap;
}


/**
 * Returns the language string that names
 * this deployment. Each deployment type has a unique name.
 * @return Deployment name.
 */
std::string RuleAlienDeployment::getType() const
{
	return _type;
}

	/// Gets a pointer to the data.
std::vector<DeploymentData>* RuleAlienDeployment::getDeploymentData()
{
	return &_data;
}

}