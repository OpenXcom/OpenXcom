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
#ifndef OPENXCOM_RULEALIENDEPLOYMENT_H
#define OPENXCOM_RULEALIENDEPLOYMENT_H

#include <vector>
#include <string>
#include "yaml.h"

namespace OpenXcom
{

class RuleTerrain;
class Ruleset;

struct ItemSet
{
	std::vector<std::string> items;
};

struct DeploymentData
{
	int alienRank;
	int lowQty, highQty, dQty;
	int percentageOutsideUFO;
	std::vector<ItemSet> itemSets;
};

/**
 * Represents a specific type of Alien Deployment.
 * Contains constant info about a Alien Deployment like
 * the number of aliens for each alien type and what items they carry 
 * (itemset depends on alien technology advancement level 0, 1 or 2).
 * - deployment type can be a craft's name, but also alien base or cydonia.
 * - alienRank is used to check which nodeRanks can be used to deploy this unit
 *   + to match to a specific unit (=race/rank combination) that should be deployed.
 * @sa Node
 */
class RuleAlienDeployment
{
private:
	std::string _type;
	std::vector<DeploymentData> _data;
public:
	/// Creates a blank Alien Deployment ruleset.
	RuleAlienDeployment(const std::string &type);
	/// Cleans up the Alien Deployment ruleset.
	~RuleAlienDeployment();
	/// Loads Alien Deployment data from YAML.
	void load(const YAML::Node& node, const Ruleset *ruleset);
	/// Saves the Alien Deployment data to YAML.
	void save(YAML::Emitter& out) const;
	/// Gets the Alien Deployment's type.
	std::string getType() const;
	/// Gets a pointer to the data.
	std::vector<DeploymentData>* getDeploymentData();
};

}

#endif
