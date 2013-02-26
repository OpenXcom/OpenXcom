/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include "TileEngine.h"
#include "AggroBAIState.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Node.h"
#include "../Engine/RNG.h"
#include "../Engine/Logger.h"
#include "../Engine/Options.h"
#include "../Ruleset/Armor.h"
#include "../Savegame/Tile.h"

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


	if (Options::getBool("traceAI")) 
	{
		Log(LOG_INFO) << "PatrolBAIState::think() #" << action->number;
	}
	
	if (_unit->_hidingForTurn) 
	{
		action->type = BA_NONE;
		action->TU = 0;
		return;
	}
	
	if (_toNode != 0 && _unit->getPosition() == _toNode->getPosition())
	{
		// destination reached
		// take a peek through window before walking to the next node
		int dir = _game->getTileEngine()->faceWindow(_unit->getPosition());
		if (dir != -1 && dir != _unit->getDirection())
		{
			_unit->lookAt(dir);
			while (_unit->getStatus() == STATUS_TURNING)
			{
				_unit->turn();
			}
			action->TU = 0; // tus are already decreased while walking
			return;
		}
		else
		{
			// head off to next patrol node
			_fromNode = _toNode;
			_toNode->free();
			_toNode = 0;
		}
	}

	if (_fromNode == 0)
	{
		// assume closest node as "from node"
		// on same level to avoid strange things, and the node has to match unit size or it will freeze
		int closest = 1000000;
		for (std::vector<Node*>::iterator i = _game->getNodes()->begin(); i != _game->getNodes()->end(); ++i)
		{
			node = *i;
			int d = _game->getTileEngine()->distance(_unit->getPosition(), node->getPosition());
			if (_unit->getPosition().z == node->getPosition().z 
				&& d < closest 
				&& (!(node->getType() & Node::TYPE_SMALL) || _unit->getArmor()->getSize() == 1))
			{
				_fromNode = node;
				closest = d;
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
			if (_game->getTurn() > 20 || _fromNode->getRank() == 0)
			{
				scout = true;
			}
			else
			{
				scout = false;
			}
		}

		// in base defense missions, the smaller aliens walk towards target nodes - or if there, shoot objects around them
		if (_game->getMissionType() == "STR_BASE_DEFENSE" && _unit->getArmor()->getSize() == 1)
		{
			// can i shoot an object?
			if (_fromNode->isTarget() && _unit->getMainHandWeapon() && _unit->getMainHandWeapon()->getAmmoItem()->getRules()->getDamageType() != DT_HE)
			{
				// scan this room for objects to destroy
				int x = (_unit->getPosition().x/10)*10;
				int y = (_unit->getPosition().y/10)*10;
				for (int i = x; i < x+9; i++)
				for (int j = y; j < y+9; j++)
				{
					MapData *md = _game->getTile(Position(i, j, 1))->getMapData(MapData::O_OBJECT);
					if (md && md->getDieMCD() && md->getArmor() < 60 )
					{
						action->actor = _unit;
						action->target = Position(i, j, 1);
						action->weapon = action->actor->getMainHandWeapon();
						action->type = BA_SNAPSHOT;
						action->TU = action->actor->getActionTUs(action->type, action->weapon);
						_unit->lookAt(action->target);
						while (_unit->getStatus() == STATUS_TURNING)
						{
							_unit->turn();
						}
						return;
					}
				}
			}
			else
			{
				// find closest target which is not already allocated
				int closest = 1000000;
				for (std::vector<Node*>::iterator i = _game->getNodes()->begin(); i != _game->getNodes()->end(); ++i)
				{
					if ((*i)->isTarget() && !(*i)->isAllocated())
					{
						node = *i;
						int d = _game->getTileEngine()->distance(_unit->getPosition(), node->getPosition());
						if (d < closest)
						{
							_toNode = node;
							closest = d;
						}
					}
				}
			}
		}

		if (_toNode == 0)
		{
			_toNode = _game->getPatrolNode(scout, _unit, _fromNode);
		}
	}

	if (_toNode != 0)
	{
		_toNode->allocate();
		action->actor = _unit;
		action->type = BA_WALK;
		action->target = _toNode->getPosition();
		_unit->lookAt(action->target);
		while (_unit->getStatus() == STATUS_TURNING)
		{
			_unit->turn();
		}
		action->TU = 0; // tus are already decreased while walking
	}
	else
	{
		action->type = BA_NONE;
		action->TU = 0;
	}

}

}
