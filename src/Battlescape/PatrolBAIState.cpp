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
#define _USE_MATH_DEFINES
#include <cmath>
#include "PatrolBAIState.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Node.h"
#include "../Engine/RNG.h"
#include "../Ruleset/RuleArmor.h"

namespace OpenXcom
{

/**
 * Sets up a PatrolBAIState.
 */
PatrolBAIState::PatrolBAIState(SavedBattleGame *game, BattleUnit *unit, Node *node) : BattleAIState(game, unit), _fromNode(node), _toNode(0)
{

}

/**
 * Deletes the PatrolBAIState.
 */
PatrolBAIState::~PatrolBAIState()
{

}

/**
 * Loads the AI state from a YAML file.
 * @param node YAML node.
 */
void PatrolBAIState::load(const YAML::Node &node)
{
	int fromnodeID, tonodeID;
	node["fromnode"] >> fromnodeID;
	node["tonode"] >> tonodeID;
	if (fromnodeID != -1)
	{
		_fromNode = _game->getNodes()->at(fromnodeID);
	}
	if (tonodeID != -1)
	{
		_toNode = _game->getNodes()->at(tonodeID);
	}
}

/**
 * Saves the AI state to a YAML file.
 * @param out YAML emitter.
 */
void PatrolBAIState::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "state" << YAML::Value << "PATROL";
	if (_fromNode)
	{
		out << YAML::Key << "fromnode" << YAML::Value << _fromNode->getID();
	}
	else
	{
		out << YAML::Key << "fromnode" << YAML::Value << -1;
	}
	if (_toNode)
	{
		out << YAML::Key << "tonode" << YAML::Value << _toNode->getID();
	}
	else
	{
		out << YAML::Key << "tonode" << YAML::Value << -1;
	}
	out << YAML::EndMap;
}

/**
 * Enters the current AI state.
 */
void PatrolBAIState::enter()
{

}


/**
 * Exits the current AI state.
 */
void PatrolBAIState::exit()
{
	if (_toNode) _toNode->free();
}

/**
 * Runs any code the state needs to keep updating every
 * AI cycle.
 */
void PatrolBAIState::think(BattleAction *action)
{
	/*
	Patrolling is mainly walking from one node to another node until bumping into hostiles
	There are a few rules:
	- if the from node is a rank 0 node (scout) the to node must be a rank 0 node (scouts stay outside)

	- a to-node can only be allocated for one unit, if it's already allocated, it will walk towards a random connected node, if no free connected node, stand still

	*/

	Node *node;
	bool bFound = false;

	if (_toNode != 0 && _unit->getPosition() == _toNode->getPosition())
	{
		// destination reached
		_fromNode = _toNode;
		_toNode->free();
		_toNode = 0;
	}

	if (_fromNode == 0)
	{
		// assume closest node as "from node"
		int closest = 1000000;
		for (std::vector<Node*>::iterator i = _game->getNodes()->begin(); i != _game->getNodes()->end(); ++i)
		{
			node = *i;
			int x = abs(_unit->getPosition().x - node->getPosition().x);
			int y = abs(_unit->getPosition().y - node->getPosition().y);
			int distance = int(floor(sqrt(float(x*x + y*y)) + 0.5));
			if (distance < closest)
			{
				_fromNode = node;
			}
		}
	}

	if (_toNode == 0)
	{
		// look for a new node to walk towards
		bool scout = true;
		if (_game->getMissionType() == "STR_UFO_CRASH_RECOVERY"
			|| _game->getMissionType() == "STR_UFO_GROUND_ASSAULT")
		{
			// after turn 20 or if the morale is low, everyone moves out the UFO and scout
			if (_unit->getMorale() < 50 || _game->getTurn() > 20 || _fromNode->getRank() == 0)
			{
				scout = true;
			}
			else
			{
				scout = false;
			}
		}
		_toNode = _game->getPatrolNode(scout, _unit, _fromNode);
	}

	if (_toNode != 0)
	{
		_toNode->allocate();
		action->actor = _unit;
		action->type = BA_WALK;
		action->target = _toNode->getPosition();
	}
	else
	{
		action->type = BA_NONE;
	}

}

}
