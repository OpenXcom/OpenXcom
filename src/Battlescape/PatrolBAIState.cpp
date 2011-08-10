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
#include "../Savegame/NodeLink.h"
#include "../Engine/RNG.h"

namespace OpenXcom
{

/**
 * Sets up a PatrolBAIState.
 * @param parent pointer to the parent state.
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
	- generally the rank of the to-node can not be lower than the one of the from-node
		- exception: a soldier can become a scout, the moment another scout is killed by hostiles (the killed scout's to-node will become the new scout's to-node)
	- a to-node can only be allocated for one unit, if it's already allocated, it will walk towards a random connected node, if no free connected node, stand still
	- scouts will always walk in the general direction of the landed x-com craft (CRAFTSEGMENT)
		- exception: when morale is low they run towards the ufo (UFOSEGMENT)
		- probably other rules for terror and base missions...
	*/

	Node *node;
	bool bFound = false;
	int segment;

	if (_toNode != 0 && _unit->getPosition() == _toNode->getPosition())
	{
		// destination reached
		_fromNode = _toNode;
		_toNode = 0;
	}

	if (_toNode == 0)
	{
		if (_game->getMissionType() == MISS_UFORECOVERY
			|| _game->getMissionType() == MISS_UFOASSAULT)
		{
			// look for a new node to walk towards
			if (_unit->getMorale() > 50)
			{
				segment = CRAFTSEGMENT;
			}
			else
			{
				segment = UFOSEGMENT;
			}

			for (std::vector<Node*>::iterator i = _game->getNodes()->begin(); i != _game->getNodes()->end() && !bFound; ++i)
			{
				node = *i;
				if (node->getSegment() == segment
					&& (!_fromNode || (_fromNode && node->getRank() >= _fromNode->getRank()))
					&& _game->selectUnit(node->getPosition()) == 0)
				{
					_toNode = node;
					bFound = true;
					break;
				}
			}
		}

		if (!bFound)
		{
			if (_fromNode)
			{
				// walk towards a connected node
				int iters = 0;
				while(!bFound && iters < 100)
				{
					int i = RNG::generate(0, 4);
					if (_fromNode->getNodeLink(i)->getConnectedNodeID() > -1)
					{
						node = _game->getNodes()->at(_fromNode->getNodeLink(i)->getConnectedNodeID());
						if (node->getRank() >= _fromNode->getRank()-2
							&& _game->selectUnit(node->getPosition()) == 0)
						{
							_toNode = node;
							bFound = true;
							break;
						}
					}
					iters++;
				}
			}
			else
			{
				// TODO : walk towards closest node
				int closest = 1000000;
				for (std::vector<Node*>::iterator i = _game->getNodes()->begin(); i != _game->getNodes()->end(); ++i)
				{
					node = *i;
					int x = abs(_unit->getPosition().x - node->getPosition().x);
					int y = abs(_unit->getPosition().y - node->getPosition().y);
					int distance = int(floor(sqrt(float(x*x + y*y)) + 0.5));
					if (distance < closest)
					{
						_toNode = node;
					}
				}
			}
		}

	}

	if (_toNode != 0)
	{
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
