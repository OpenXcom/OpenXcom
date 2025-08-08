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
#include <climits>
#include <algorithm>
#include "AIModule.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/Node.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/SavedGame.h"
#include "TileEngine.h"
#include "Map.h"
#include "BattlescapeState.h"
#include "../Savegame/Tile.h"
#include "Pathfinding.h"
#include "../Engine/RNG.h"
#include "../Engine/Logger.h"
#include "../Engine/Game.h"
#include "../Mod/Armor.h"
#include "../Mod/Mod.h"
#include "../Mod/RuleItem.h"
#include "../fmath.h"

namespace OpenXcom
{


/**
 * Sets up a BattleAIState.
 * @param save Pointer to the battle game.
 * @param unit Pointer to the unit.
 * @param node Pointer to the node the unit originates from.
 */
AIModule::AIModule(SavedBattleGame *save, BattleUnit *unit, Node *node) : _save(save), _unit(unit), _aggroTarget(0), _knownEnemies(0), _visibleEnemies(0), _spottingEnemies(0),
																				_escapeTUs(0), _ambushTUs(0), _rifle(false), _melee(false), _blaster(false),
																				_didPsi(false), _AIMode(AI_PATROL), _closestDist(100), _fromNode(node), _toNode(0)
{
	_traceAI = Options::traceAI;

	_reserve = BA_NONE;
	_intelligence = _unit->getIntelligence();
	_escapeAction = new BattleAction();
	_ambushAction = new BattleAction();
	_attackAction = new BattleAction();
	_patrolAction = new BattleAction();
	_psiAction = new BattleAction();
	_targetFaction = FACTION_PLAYER;
	if (_unit->getOriginalFaction() == FACTION_NEUTRAL)
	{
		_targetFaction = FACTION_HOSTILE;
	}
}

/**
 * Deletes the BattleAIState.
 */
AIModule::~AIModule()
{
	delete _escapeAction;
	delete _ambushAction;
	delete _attackAction;
	delete _patrolAction;
	delete _psiAction;
}

/**
 * Resets the unsaved AI state.
 */
void AIModule::reset()
{
	// these variables are not saved in save() and also not initiated in think()
	_escapeTUs = 0;
	_ambushTUs = 0;
}

/**
 * Loads the AI state from a YAML file.
 * @param node YAML node.
 */
void AIModule::load(const YAML::Node &node)
{
	int fromNodeID, toNodeID;
	fromNodeID = node["fromNode"].as<int>(-1);
	toNodeID = node["toNode"].as<int>(-1);
	_AIMode = node["AIMode"].as<int>(AI_PATROL);
	_wasHitBy = node["wasHitBy"].as<std::vector<int> >(_wasHitBy);
	// TODO: Figure out why AI are sometimes left with junk nodes
	if (fromNodeID >= 0 && (size_t)fromNodeID < _save->getNodes()->size())
	{
		_fromNode = _save->getNodes()->at(fromNodeID);
	}
	if (toNodeID >= 0 && (size_t)toNodeID < _save->getNodes()->size())
	{
		_toNode = _save->getNodes()->at(toNodeID);
	}
}

/**
 * Saves the AI state to a YAML file.
 * @return YAML node.
 */
YAML::Node AIModule::save() const
{
	int fromNodeID = -1, toNodeID = -1;
	if (_fromNode)
		fromNodeID = _fromNode->getID();
	if (_toNode)
		toNodeID = _toNode->getID();

	YAML::Node node;
	node["fromNode"] = fromNodeID;
	node["toNode"] = toNodeID;
	node["AIMode"] = _AIMode;
	node["wasHitBy"] = _wasHitBy;
	return node;
}

/**
 * Runs any code the state needs to keep updating every AI cycle.
 * @param action (possible) AI action to execute after thinking is done.
 */
void AIModule::think(BattleAction *action)
{
	action->type = BA_RETHINK;
	action->actor = _unit;
	action->weapon = _unit->getMainHandWeapon(false);
	_attackAction->diff = _save->getBattleState()->getGame()->getSavedGame()->getDifficultyCoefficient();
	_attackAction->actor = _unit;
	_attackAction->weapon = action->weapon;
	_attackAction->number = action->number;
	_escapeAction->number = action->number;
	_knownEnemies = countKnownTargets();
	_visibleEnemies = selectNearestTarget();
	_spottingEnemies = getSpottingUnits(_unit->getPosition());
	_melee = (_unit->getMeleeWeapon() != 0);
	_rifle = false;
	_blaster = false;
	_reachable = _save->getPathfinding()->findReachable(_unit, _unit->getTimeUnits());
	_wasHitBy.clear();

	if (_unit->getCharging() && _unit->getCharging()->isOut())
	{
		_unit->setCharging(0);
	}

	if (_traceAI)
	{
		if (_unit->getFaction() == FACTION_HOSTILE)
		{
			Log(LOG_INFO) << "Unit has " << _visibleEnemies << "/" << _knownEnemies << " known enemies visible, " << _spottingEnemies << " of whom are spotting him. ";
		}
		else
		{
			Log(LOG_INFO) << "Civilian Unit has " << _visibleEnemies << " enemies visible, " << _spottingEnemies << " of whom are spotting him. ";
		}
		std::string AIMode;
		switch (_AIMode)
		{
		case AI_PATROL:
			AIMode = "Patrol";
			break;
		case AI_AMBUSH:
			AIMode = "Ambush";
			break;
		case AI_COMBAT:
			AIMode = "Combat";
			break;
		case AI_ESCAPE:
			AIMode = "Escape";
			break;
		}
		Log(LOG_INFO) << "Currently using " << AIMode << " behaviour";
	}

	if (action->weapon)
	{
		RuleItem *rule = action->weapon->getRules();
		if (_save->isItemUsable(action->weapon))
		{
			if (rule->getBattleType() == BT_FIREARM)
			{
				if (rule->getWaypoints() != 0 || (action->weapon->getAmmoItem() && action->weapon->getAmmoItem()->getRules()->getWaypoints() != 0))
				{
					_blaster = true;
					_reachableWithAttack = _save->getPathfinding()->findReachable(_unit, _unit->getTimeUnits() - _unit->getActionTUs(BA_AIMEDSHOT, action->weapon));
				}
				else
				{
					_rifle = true;
					_reachableWithAttack = _save->getPathfinding()->findReachable(_unit, _unit->getTimeUnits() - _unit->getActionTUs(BA_SNAPSHOT, action->weapon));
				}
			}
			else if (rule->getBattleType() == BT_MELEE)
			{
				_melee = true;
				_reachableWithAttack = _save->getPathfinding()->findReachable(_unit, _unit->getTimeUnits() - _unit->getActionTUs(BA_HIT, action->weapon));
			}
		}
		else
		{
			action->weapon = 0;
		}
	}

	if (_spottingEnemies && !_escapeTUs)
	{
		setupEscape();
	}

	if (_knownEnemies && !_melee && !_ambushTUs)
	{
		setupAmbush();
	}

	setupAttack();
	setupPatrol();

	if (_psiAction->type != BA_NONE && !_didPsi)
	{
		_didPsi = true;
		action->type = _psiAction->type;
		action->target = _psiAction->target;
		action->number -= 1;
		action->weapon = _psiAction->weapon;
		return;
	}
	else
	{
		_didPsi = false;
	}

	bool evaluate = false;

	switch (_AIMode)
		{
		case AI_PATROL:
			evaluate = (bool)(_spottingEnemies || _visibleEnemies || _knownEnemies || RNG::percent(10));
			break;
		case AI_AMBUSH:
			evaluate = (!_rifle || !_ambushTUs || _visibleEnemies);
			break;
		case AI_COMBAT:
			evaluate = (_attackAction->type == BA_RETHINK);
			break;
		case AI_ESCAPE:
			evaluate = (!_spottingEnemies || !_knownEnemies);
			break;
			}

	if (_spottingEnemies > 2
		|| _unit->getHealth() < 2 * _unit->getBaseStats()->health / 3
		|| (_aggroTarget && _aggroTarget->getTurnsSinceSpotted() > _intelligence))
	{
		evaluate = true;
	}


	if (_save->isCheating() && _AIMode != AI_COMBAT)
	{
		evaluate = true;
	}

	if (evaluate)
	{
		evaluateAIMode();
		if (_traceAI)
		{
			std::string AIMode;
			switch (_AIMode)
			{
			case AI_PATROL:
				AIMode = "Patrol";
				break;
			case AI_AMBUSH:
				AIMode = "Ambush";
				break;
			case AI_COMBAT:
				AIMode = "Combat";
				break;
			case AI_ESCAPE:
				AIMode = "Escape";
				break;
			}
			Log(LOG_INFO) << "Re-Evaluated, now using " << AIMode << " behaviour";
		}
	}

	_reserve = BA_NONE;

	switch (_AIMode)
	{
	case AI_ESCAPE:
		_unit->setCharging(0);
		action->type = _escapeAction->type;
		action->target = _escapeAction->target;
		// end this unit's turn.
		action->finalAction = true;
		// ignore new targets.
		action->desperate = true;
		// spin 180 at the end of your route.
		_unit->setHiding(true);
		break;
	case AI_PATROL:
		_unit->setCharging(0);
		if (action->weapon && action->weapon->getRules()->getBattleType() == BT_FIREARM)
		{
			switch (_unit->getAggression())
			{
			case 0:
				_reserve = BA_AIMEDSHOT;
				break;
			case 1:
				_reserve = BA_AUTOSHOT;
				break;
			case 2:
				_reserve = BA_SNAPSHOT;
				break;
			default:
				break;
			}
		}
		action->type = _patrolAction->type;
		action->target = _patrolAction->target;
		break;
	case AI_COMBAT:
		action->type = _attackAction->type;
		action->target = _attackAction->target;
		// this may have changed to a grenade.
		action->weapon = _attackAction->weapon;
		if (action->weapon && action->type == BA_THROW && action->weapon->getRules()->getBattleType() == BT_GRENADE)
		{
			_unit->spendTimeUnits(4 + _unit->getActionTUs(BA_PRIME, action->weapon));
		}
		// if this is a firepoint action, set our facing.
		action->finalFacing = _attackAction->finalFacing;
		action->TU = _unit->getActionTUs(_attackAction->type, _attackAction->weapon);
		// if this is a "find fire point" action, don't increment the AI counter.
		if (action->type == BA_WALK && _rifle
			// so long as we can take a shot afterwards.
			&& _unit->getTimeUnits() > _unit->getActionTUs(BA_SNAPSHOT, action->weapon))
		{
			action->number -= 1;
		}
		else if (action->type == BA_LAUNCH)
		{
			action->waypoints = _attackAction->waypoints;
		}
		break;
	case AI_AMBUSH:
		_unit->setCharging(0);
		action->type = _ambushAction->type;
		action->target = _ambushAction->target;
		// face where we think our target will appear.
		action->finalFacing = _ambushAction->finalFacing;
		// end this unit's turn.
		action->finalAction = true;
		break;
	default:
		break;
	}

	if (action->type == BA_WALK)
	{
		// if we're moving, we'll have to re-evaluate our escape/ambush position.
		if (action->target != _unit->getPosition())
		{
			_escapeTUs = 0;
			_ambushTUs = 0;
		}
		else
		{
			action->type = BA_NONE;
		}
	}
}


/*
 * sets the "was hit" flag to true.
 */
void AIModule::setWasHitBy(BattleUnit *attacker)
{
	if (attacker->getFaction() != _unit->getFaction() && !getWasHitBy(attacker->getId()))
		_wasHitBy.push_back(attacker->getId());
}

/*
 * Gets whether the unit was hit.
 * @return if it was hit.
 */
bool AIModule::getWasHitBy(int attacker) const
{
	return std::find(_wasHitBy.begin(), _wasHitBy.end(), attacker) != _wasHitBy.end();
}
/*
 * Sets up a patrol action.
 * this is mainly going from node to node, moving about the map.
 * handles node selection, and fills out the _patrolAction with useful data.
 */
void AIModule::setupPatrol()
{
	Node *node;
	_patrolAction->TU = 0;
	if (_toNode != 0 && _unit->getPosition() == _toNode->getPosition())
	{
		if (_traceAI)
		{
			Log(LOG_INFO) << "Patrol destination reached!";
		}
		// destination reached
		// head off to next patrol node
		_fromNode = _toNode;
		freePatrolTarget();
		_toNode = 0;
		// take a peek through window before walking to the next node
		int dir = _save->getTileEngine()->faceWindow(_unit->getPosition());
		if (dir != -1 && dir != _unit->getDirection())
		{
			_unit->lookAt(dir);
			while (_unit->getStatus() == STATUS_TURNING)
			{
				_unit->turn();
			}
		}
	}

	if (_fromNode == 0)
	{
		// assume closest node as "from node"
		// on same level to avoid strange things, and the node has to match unit size or it will freeze
		int closest = 1000000;
		for (std::vector<Node*>::iterator i = _save->getNodes()->begin(); i != _save->getNodes()->end(); ++i)
		{
			if ((*i)->isDummy())
			{
				continue;
			}
			node = *i;
			int d = _save->getTileEngine()->distanceSq(_unit->getPosition(), node->getPosition());
			if (_unit->getPosition().z == node->getPosition().z
				&& d < closest
				&& (!(node->getType() & Node::TYPE_SMALL) || _unit->getArmor()->getSize() == 1))
			{
				_fromNode = node;
				closest = d;
			}
		}
	}
	int triesLeft = 5;

	while (_toNode == 0 && triesLeft)
	{
		triesLeft--;
		// look for a new node to walk towards
		bool scout = true;
		if (_save->getMissionType() != "STR_BASE_DEFENSE")
		{
			// after turn 20 or if the morale is low, everyone moves out the UFO and scout
			// also anyone standing in fire should also probably move
			if (_save->isCheating() || !_fromNode || _fromNode->getRank() == 0 ||
				(_save->getTile(_unit->getPosition()) && _save->getTile(_unit->getPosition())->getFire()))
			{
				scout = true;
			}
			else
			{
				scout = false;
			}
		}

		// in base defense missions, the smaller aliens walk towards target nodes - or if there, shoot objects around them
		else if (_unit->getArmor()->getSize() == 1 &&
				_attackAction->weapon &&
				_attackAction->weapon->getRules()->getAccuracySnap() &&
				!_attackAction->weapon->getRules()->getArcingShot() &&
				_attackAction->weapon->getAmmoItem() &&
				!_attackAction->weapon->getAmmoItem()->getRules()->getArcingShot() &&
				_attackAction->weapon->getAmmoItem()->getRules()->getDamageType() != DT_HE &&
				_attackAction->weapon->getAmmoItem()->getRules()->getDamageType() != DT_STUN)
		{
			// can i shoot an object?
			if (_fromNode->isTarget()  &&
				_save->getModuleMap()[_fromNode->getPosition().x / 10][_fromNode->getPosition().y / 10].second > 0)
			{
				// scan this room for objects to destroy
				int x = (_unit->getPosition().x/10)*10;
				int y = (_unit->getPosition().y/10)*10;
				for (int i = x; i < x+9; i++)
				for (int j = y; j < y+9; j++)
				{
					MapData *md = _save->getTile(Position(i, j, 1))->getMapData(O_OBJECT);
					if (md && md->isBaseModule())
					{
						_patrolAction->actor = _unit;
						_patrolAction->target = Position(i, j, 1);
						_patrolAction->weapon = _attackAction->weapon;
						_patrolAction->type = BA_SNAPSHOT;
						_patrolAction->TU = _patrolAction->actor->getActionTUs(_patrolAction->type, _patrolAction->weapon);
						return;
					}
				}
			}
			else
			{
				// find closest high value target which is not already allocated
				int closest = 1000000;
				for (std::vector<Node*>::iterator i = _save->getNodes()->begin(); i != _save->getNodes()->end(); ++i)
				{
					if ((*i)->isDummy())
					{
						continue;
					}

					if (_save->getTile((*i)->getPosition())->getUnit() && _save->getTile((*i)->getPosition())->getUnit()->getFaction() == _unit->getFaction())
					{
						continue;
					}

					if ((*i)->isTarget() && !(*i)->isAllocated() && _save->getModuleMap()[(*i)->getPosition().x / 10][(*i)->getPosition().y / 10].second > 0)
					{
						node = *i;
						int d = _save->getTileEngine()->distanceSq(_unit->getPosition(), node->getPosition());
						if (!_toNode ||  (d < closest && node != _fromNode))
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
			_toNode = _save->getPatrolNode(scout, _unit, _fromNode);
			if (_toNode == 0)
			{
				_toNode = _save->getPatrolNode(!scout, _unit, _fromNode);
			}
		}

		if (_toNode != 0)
		{
			_save->getPathfinding()->calculate(_unit, _toNode->getPosition());
			if (_save->getPathfinding()->getStartDirection() == -1)
			{
				_toNode = 0;
			}
			_save->getPathfinding()->abortPath();
		}
	}

	if (_toNode != 0)
	{
		_toNode->allocateNode();
		_patrolAction->actor = _unit;
		_patrolAction->type = BA_WALK;
		_patrolAction->target = _toNode->getPosition();
	}
	else
	{
		_patrolAction->type = BA_RETHINK;
	}
}

/**
 * Try to set up an ambush action
 * The idea is to check within a 11x11 tile square for a tile which is not seen by our aggroTarget,
 * but that can be reached by him. we then intuit where we will see the target first from our covered
 * position, and set that as our final facing.
 * Fills out the _ambushAction with useful data.
 */
void AIModule::setupAmbush()
{
	_ambushAction->type = BA_RETHINK;
	int bestScore = 0;
	_ambushTUs = 0;
	std::vector<int> path;

	if (selectClosestKnownEnemy())
	{
		const int BASE_SYSTEMATIC_SUCCESS = 100;
		const int COVER_BONUS = 25;
		const int FAST_PASS_THRESHOLD = 80;
		Position origin = _save->getTileEngine()->getSightOriginVoxel(_aggroTarget);

		// we'll use node positions for this, as it gives map makers a good degree of control over how the units will use the environment.
		for (std::vector<Node*>::const_iterator i = _save->getNodes()->begin(); i != _save->getNodes()->end(); ++i)
		{
			if ((*i)->isDummy())
			{
				continue;
			}
			Position pos = (*i)->getPosition();
			Tile *tile = _save->getTile(pos);
			if (tile == 0 || _save->getTileEngine()->distance(pos, _unit->getPosition()) > 10 || pos.z != _unit->getPosition().z || tile->getDangerous() ||
				std::find(_reachableWithAttack.begin(), _reachableWithAttack.end(), _save->getTileIndex(pos))  == _reachableWithAttack.end())
				continue; // just ignore unreachable tiles

			if (_traceAI)
			{
				// colour all the nodes in range purple.
				tile->setPreview(10);
				tile->setMarkerColor(13);
			}

			// make sure we can't be seen here.
			Position target;
			if (!_save->getTileEngine()->canTargetUnit(&origin, tile, &target, _aggroTarget, false, _unit) && !getSpottingUnits(pos))
			{
				_save->getPathfinding()->calculate(_unit, pos);
				int ambushTUs = _save->getPathfinding()->getTotalTUCost();
				// make sure we can move here
				if (_save->getPathfinding()->getStartDirection() != -1)
				{
					int score = BASE_SYSTEMATIC_SUCCESS;
					score -= ambushTUs;

					// make sure our enemy can reach here too.
					_save->getPathfinding()->calculate(_aggroTarget, pos);

					if (_save->getPathfinding()->getStartDirection() != -1)
					{
						// ideally we'd like to be behind some cover, like say a window or a low wall.
						if (_save->getTileEngine()->faceWindow(pos) != -1)
						{
							score += COVER_BONUS;
						}
						if (score > bestScore)
						{
							path = _save->getPathfinding()->copyPath();
							bestScore = score;
							_ambushTUs = (pos == _unit->getPosition()) ? 1 : ambushTUs;
							_ambushAction->target = pos;
							if (bestScore > FAST_PASS_THRESHOLD)
							{
								break;
							}
						}
					}
				}
			}
		}

		if (bestScore > 0)
		{
			_ambushAction->type = BA_WALK;
			// i should really make a function for this
			origin = (_ambushAction->target * Position(16,16,24)) +
				// 4 because -2 is eyes and 2 below that is the rifle (or at least that's my understanding)
				Position(8,8, _unit->getHeight() + _unit->getFloatHeight() - _save->getTile(_ambushAction->target)->getTerrainLevel() - 4);
			Position currentPos = _aggroTarget->getPosition();
			_save->getPathfinding()->setUnit(_aggroTarget);
			Position nextPos;
			size_t tries = path.size();
			// hypothetically walk the target through the path.
			while (tries > 0)
			{
				_save->getPathfinding()->getTUCost(currentPos, path.back(), &nextPos, _aggroTarget, 0, false);
				path.pop_back();
				currentPos = nextPos;
				Tile *tile = _save->getTile(currentPos);
				Position target;
				// do a virtual fire calculation
				if (_save->getTileEngine()->canTargetUnit(&origin, tile, &target, _unit, false, _aggroTarget))
				{
					// if we can virtually fire at the hypothetical target, we know which way to face.
					_ambushAction->finalFacing = _save->getTileEngine()->getDirectionTo(_ambushAction->target, currentPos);
					break;
				}
				--tries;
			}
			if (_traceAI)
			{
				Log(LOG_INFO) << "Ambush estimation will move to " << _ambushAction->target;
			}
			return;
		}
	}
	if (_traceAI)
	{
		Log(LOG_INFO) << "Ambush estimation failed";
	}
}

/**
 * Try to set up a combat action
 * This will either be a psionic, grenade, or weapon attack,
 * or potentially just moving to get a line of sight to a target.
 * Fills out the _attackAction with useful data.
 */
void AIModule::setupAttack()
{
	_attackAction->type = BA_RETHINK;
	_psiAction->type = BA_NONE;

	// if enemies are known to us but not necessarily visible, we can attack them with a blaster launcher or psi.
	if (_knownEnemies)
	{
		if (psiAction())
		{
			// at this point we can save some time with other calculations - the unit WILL make a psionic attack this turn.
			return;
		}
		if (_blaster)
		{
			wayPointAction();
		}
	}

	// if we CAN see someone, that makes them a viable target for "regular" attacks.
	if (selectNearestTarget())
	{
		// if we have both types of weapon, make a determination on which to use.
		if (_melee && _rifle)
		{
			selectMeleeOrRanged();
		}

		if (_unit->getGrenadeFromBelt())
		{
			grenadeAction();
		}
		if (_melee)
		{
			meleeAction();
		}
		if (_rifle)
		{
			projectileAction();
		}
	}

	if (_attackAction->type != BA_RETHINK)
	{
		if (_traceAI)
		{
			if (_attackAction->type != BA_WALK)
			{
				Log(LOG_INFO) << "Attack estimation desires to shoot at " << _attackAction->target;
			}
			else
			{
				Log(LOG_INFO) << "Attack estimation desires to move to " << _attackAction->target;
			}
		}
		return;
	}
	else if (_spottingEnemies || _unit->getAggression() < RNG::generate(0, 3))
	{
		// if enemies can see us, or if we're feeling lucky, we can try to spot the enemy.
		if (findFirePoint())
		{
			if (_traceAI)
			{
				Log(LOG_INFO) << "Attack estimation desires to move to " << _attackAction->target;
			}
			return;
		}
	}
	if (_traceAI)
	{
		Log(LOG_INFO) << "Attack estimation failed";
	}
}

/**
 * Attempts to find cover, and move toward it.
 * The idea is to check within a 11x11 tile square for a tile which is not seen by our aggroTarget.
 * If there is no such tile, we run away from the target.
 * Fills out the _escapeAction with useful data.
 */
void AIModule::setupEscape()
{
	int unitsSpottingMe = getSpottingUnits(_unit->getPosition());
	int currentTilePreference = 15;
	int tries = -1;
	bool coverFound = false;
	selectNearestTarget();
	_escapeTUs = 0;

	int dist = _aggroTarget ? _save->getTileEngine()->distance(_unit->getPosition(), _aggroTarget->getPosition()) : 0;

	int bestTileScore = -100000;
	int score = -100000;
	Position bestTile(0, 0, 0);

	Tile *tile = 0;

	// weights of various factors in choosing a tile to which to withdraw
	const int EXPOSURE_PENALTY = 10;
	const int FIRE_PENALTY = 40;
	const int BASE_SYSTEMATIC_SUCCESS = 100;
	const int BASE_DESPERATE_SUCCESS = 110;
	const int FAST_PASS_THRESHOLD = 100; // a score that's good enough to quit the while loop early; it's subjective, hand-tuned and may need tweaking

	std::vector<Position> randomTileSearch = _save->getTileSearch();
	RNG::shuffle(randomTileSearch);

	while (tries < 150 && !coverFound)
	{
		_escapeAction->target = _unit->getPosition(); // start looking in a direction away from the enemy

		if (!_save->getTile(_escapeAction->target))
		{
			_escapeAction->target = _unit->getPosition(); // cornered at the edge of the map perhaps?
		}

		score = 0;

		if (tries == -1)
		{
			// you know, maybe we should just stay where we are and not risk reaction fire...
			// or maybe continue to wherever we were running to and not risk looking stupid
			if (_save->getTile(_unit->lastCover) != 0)
			{
				_escapeAction->target = _unit->lastCover;
			}
		}
		else if (tries < 121)
		{
			// looking for cover
			_escapeAction->target.x += randomTileSearch[tries].x;
			_escapeAction->target.y += randomTileSearch[tries].y;
			score = BASE_SYSTEMATIC_SUCCESS;
			if (_escapeAction->target == _unit->getPosition())
			{
				if (unitsSpottingMe > 0)
				{
					// maybe don't stay in the same spot? move or something if there's any point to it?
					_escapeAction->target.x += RNG::generate(-20,20);
					_escapeAction->target.y += RNG::generate(-20,20);
				}
				else
				{
					score += currentTilePreference;
				}
			}
		}
		else
		{

			if (tries == 121)
			{
				if (_traceAI)
				{
					Log(LOG_INFO) << "best score after systematic search was: " << bestTileScore;
				}
			}

			score = BASE_DESPERATE_SUCCESS; // ruuuuuuun
			_escapeAction->target = _unit->getPosition();
			_escapeAction->target.x += RNG::generate(-10,10);
			_escapeAction->target.y += RNG::generate(-10,10);
			_escapeAction->target.z = _unit->getPosition().z + RNG::generate(-1,1);
			if (_escapeAction->target.z < 0)
			{
				_escapeAction->target.z = 0;
			}
			else if (_escapeAction->target.z >= _save->getMapSizeZ())
			{
				_escapeAction->target.z = _unit->getPosition().z;
			}
		}

		tries++;

		// THINK, DAMN YOU
		tile = _save->getTile(_escapeAction->target);
		int distanceFromTarget = _aggroTarget ? _save->getTileEngine()->distance(_aggroTarget->getPosition(), _escapeAction->target) : 0;
		if (dist >= distanceFromTarget)
		{
			score -= (distanceFromTarget - dist) * 10;
		}
		else
		{
			score += (distanceFromTarget - dist) * 10;
		}
		int spotters = 0;
		if (!tile)
		{
			score = -100001; // no you can't quit the battlefield by running off the map.
		}
		else
		{
			spotters = getSpottingUnits(_escapeAction->target);
			if (std::find(_reachable.begin(), _reachable.end(), _save->getTileIndex(_escapeAction->target))  == _reachable.end())
				continue; // just ignore unreachable tiles

			if (_spottingEnemies || spotters)
			{
				if (_spottingEnemies <= spotters)
				{
					score -= (1 + spotters - _spottingEnemies) * EXPOSURE_PENALTY; // that's for giving away our position
				}
				else
				{
					score += (_spottingEnemies - spotters) * EXPOSURE_PENALTY;
				}
			}
			if (tile->getFire())
			{
				score -= FIRE_PENALTY;
			}
			if (tile->getDangerous())
			{
				score -= BASE_SYSTEMATIC_SUCCESS;
			}

			if (_traceAI)
			{
				tile->setMarkerColor(score < 0 ? 3 : (score < FAST_PASS_THRESHOLD/2 ? 8 : (score < FAST_PASS_THRESHOLD ? 9 : 5)));
				tile->setPreview(10);
				tile->setTUMarker(score);
			}

		}

		if (tile && score > bestTileScore)
		{
			// calculate TUs to tile; we could be getting this from findReachable() somehow but that would break something for sure...
			_save->getPathfinding()->calculate(_unit, _escapeAction->target);
			if (_escapeAction->target == _unit->getPosition() || _save->getPathfinding()->getStartDirection() != -1)
			{
				bestTileScore = score;
				bestTile = _escapeAction->target;
				_escapeTUs = _save->getPathfinding()->getTotalTUCost();
				if (_escapeAction->target == _unit->getPosition())
				{
					_escapeTUs = 1;
				}
				if (_traceAI)
				{
					tile->setMarkerColor(score < 0 ? 7 : (score < FAST_PASS_THRESHOLD/2 ? 10 : (score < FAST_PASS_THRESHOLD ? 4 : 5)));
					tile->setPreview(10);
					tile->setTUMarker(score);
				}
			}
			_save->getPathfinding()->abortPath();
			if (bestTileScore > FAST_PASS_THRESHOLD) coverFound = true; // good enough, gogogo
		}
	}
	_escapeAction->target = bestTile;
	if (_traceAI)
	{
		_save->getTile(_escapeAction->target)->setMarkerColor(13);
	}

	if (bestTileScore <= -100000)
	{
		if (_traceAI)
		{
			Log(LOG_INFO) << "Escape estimation failed.";
		}
		_escapeAction->type = BA_RETHINK; // do something, just don't look dumbstruck :P
		return;
	}
	else
	{
		if (_traceAI)
		{
			Log(LOG_INFO) << "Escape estimation completed after " << tries << " tries, " << _save->getTileEngine()->distance(_unit->getPosition(), bestTile) << " squares or so away.";
		}
		_escapeAction->type = BA_WALK;
	}
}

/**
 * Counts how many targets, both xcom and civilian are known to this unit
 * @return how many targets are known to us.
 */
int AIModule::countKnownTargets() const
{
	int knownEnemies = 0;

	if (_unit->getFaction() == FACTION_HOSTILE)
	{
		for (std::vector<BattleUnit*>::const_iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
		{
			if (validTarget(*i, true, true))
			{
				++knownEnemies;
			}
		}
	}
	return knownEnemies;
}

/*
 * counts how many enemies (xcom only) are spotting any given position.
 * @param pos the Position to check for spotters.
 * @return spotters.
 */
int AIModule::getSpottingUnits(const Position& pos) const
{
	// if we don't actually occupy the position being checked, we need to do a virtual LOF check.
	bool checking = pos != _unit->getPosition();
	int tally = 0;
	for (std::vector<BattleUnit*>::const_iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
	{
		if (validTarget(*i, false, false))
		{
			int dist = _save->getTileEngine()->distance(pos, (*i)->getPosition());
			if (dist > 20) continue;
			Position originVoxel = _save->getTileEngine()->getSightOriginVoxel(*i);
			originVoxel.z -= 2;
			Position targetVoxel;
			if (checking)
			{
				if (_save->getTileEngine()->canTargetUnit(&originVoxel, _save->getTile(pos), &targetVoxel, *i, false, _unit))
				{
					tally++;
				}
			}
			else
			{
				if (_save->getTileEngine()->canTargetUnit(&originVoxel, _save->getTile(pos), &targetVoxel, *i, false))
				{
					tally++;
				}
			}
		}
	}
	return tally;
}

/**
 * Selects the nearest known living target we can see/reach and returns the number of visible enemies.
 * This function includes civilians as viable targets.
 * @return viable targets.
 */
int AIModule::selectNearestTarget()
{
	int tally = 0;
	_closestDist= 100;
	_aggroTarget = 0;
	Position target;
	for (std::vector<BattleUnit*>::const_iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
	{
		if (validTarget(*i, true, _unit->getFaction() == FACTION_HOSTILE) &&
			_save->getTileEngine()->visible(_unit, (*i)->getTile()))
		{
			tally++;
			int dist = _save->getTileEngine()->distance(_unit->getPosition(), (*i)->getPosition());
			if (dist < _closestDist)
			{
				bool valid = false;
				if (_rifle || !_melee)
				{
					BattleAction action;
					action.actor = _unit;
					action.weapon = _attackAction->weapon;
					action.target = (*i)->getPosition();
					Position origin = _save->getTileEngine()->getOriginVoxel(action, 0);
					valid = _save->getTileEngine()->canTargetUnit(&origin, (*i)->getTile(), &target, _unit, false);
				}
				else
				{
					if (selectPointNearTarget(*i, _unit->getTimeUnits()))
					{
						int dir = _save->getTileEngine()->getDirectionTo(_attackAction->target, (*i)->getPosition());
						valid = _save->getTileEngine()->validMeleeRange(_attackAction->target, dir, _unit, *i, 0);
					}
				}
				if (valid)
				{
					_closestDist = dist;
					_aggroTarget = *i;
				}
			}
		}
	}
	if (_aggroTarget)
	{
		return tally;
	}

	return 0;
}

/**
 * Selects the nearest known living Xcom unit.
 * used for ambush calculations
 * @return if we found one.
 */
bool AIModule::selectClosestKnownEnemy()
{
	_aggroTarget = 0;
	int minDist = 255;
	for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
	{
		if (validTarget(*i, true, false))
		{
			int dist = _save->getTileEngine()->distance((*i)->getPosition(), _unit->getPosition());
			if (dist < minDist)
			{
				minDist = dist;
				_aggroTarget = *i;
			}
		}
	}
	return _aggroTarget != 0;
}

/**
 * Selects a random known living Xcom or civilian unit.
 * @return if we found one.
 */
bool AIModule::selectRandomTarget()
{
	int farthest = -100;
	_aggroTarget = 0;

	for (std::vector<BattleUnit*>::const_iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
	{
		if (validTarget(*i, true, _unit->getFaction() == FACTION_HOSTILE))
		{
			int dist = RNG::generate(0,20) - _save->getTileEngine()->distance(_unit->getPosition(), (*i)->getPosition());
			if (dist > farthest)
			{
				farthest = dist;
				_aggroTarget = *i;
			}
		}
	}
	return _aggroTarget != 0;
}

/**
 * Selects a point near enough to our target to perform a melee attack.
 * @param target Pointer to a target.
 * @param maxTUs Maximum time units the path to the target can cost.
 * @return True if a point was found.
 */
bool AIModule::selectPointNearTarget(BattleUnit *target, int maxTUs) const
{
	int size = _unit->getArmor()->getSize();
	int targetsize = target->getArmor()->getSize();
	bool returnValue = false;
	unsigned int distance = 1000;
	for (int z = -1; z <= 1; ++z)
	{
		for (int x = -size; x <= targetsize; ++x)
		{
			for (int y = -size; y <= targetsize; ++y)
			{
				if (x || y) // skip the unit itself
				{
					Position checkPath = target->getPosition() + Position (x, y, z);
					if (_save->getTile(checkPath) == 0 || std::find(_reachable.begin(), _reachable.end(), _save->getTileIndex(checkPath))  == _reachable.end())
						continue;
					int dir = _save->getTileEngine()->getDirectionTo(checkPath, target->getPosition());
					bool valid = _save->getTileEngine()->validMeleeRange(checkPath, dir, _unit, target, 0);
					bool fitHere = _save->setUnitPosition(_unit, checkPath, true);

					if (valid && fitHere && !_save->getTile(checkPath)->getDangerous())
					{
						_save->getPathfinding()->calculate(_unit, checkPath, 0, maxTUs);
						if (_save->getPathfinding()->getStartDirection() != -1 && _save->getPathfinding()->getPath().size() < distance)
						{
							_attackAction->target = checkPath;
							returnValue = true;
							distance = _save->getPathfinding()->getPath().size();
						}
						_save->getPathfinding()->abortPath();
					}
				}
			}
		}
	}
	return returnValue;
}

/**
 * Selects an AI mode based on a number of factors, some RNG and the results of the rest of the determinations.
 */
void AIModule::evaluateAIMode()
{
	if ((_unit->getCharging() && _attackAction->type != BA_RETHINK))
	{
		_AIMode = AI_COMBAT;
		return;
	}
	// don't try to run away as often if we're a melee type, and really don't try to run away if we have a viable melee target, or we still have 50% or more TUs remaining.
	int escapeOdds = 15;
	if (_melee)
	{
		escapeOdds = 12;
	}
	if (_unit->getFaction() == FACTION_HOSTILE && (_unit->getTimeUnits() > _unit->getBaseStats()->tu / 2 || _unit->getCharging()))
	{
		escapeOdds = 5;
	}
	int ambushOdds = 12;
	int combatOdds = 20;
	// we're less likely to patrol if we see enemies.
	int patrolOdds = _visibleEnemies ? 15 : 30;

	// the enemy sees us, we should take retreat into consideration, and forget about patrolling for now.
	if (_spottingEnemies)
	{
		patrolOdds = 0;
		if (_escapeTUs == 0)
		{
			setupEscape();
		}
	}

	// melee/blaster units shouldn't consider ambush
	if (!_rifle || _ambushTUs == 0)
	{
		ambushOdds = 0;
		if (_melee)
		{
			combatOdds *= 1.3;
		}
	}

	// if we KNOW there are enemies around...
	if (_knownEnemies)
	{
		if (_knownEnemies == 1)
		{
			combatOdds *= 1.2;
		}

		if (_escapeTUs == 0)
		{
			if (selectClosestKnownEnemy())
			{
				setupEscape();
			}
			else
			{
				escapeOdds = 0;
			}
		}
	}
	else if (_unit->getFaction() == FACTION_HOSTILE)
	{
		combatOdds = 0;
		escapeOdds = 0;
	}

	// take our current mode into consideration
	switch (_AIMode)
	{
	case AI_PATROL:
		patrolOdds *= 1.1;
		break;
	case AI_AMBUSH:
		ambushOdds *= 1.1;
		break;
	case AI_COMBAT:
		combatOdds *= 1.1;
		break;
	case AI_ESCAPE:
		escapeOdds *= 1.1;
		break;
	}

	// take our overall health into consideration
	if (_unit->getHealth() < _unit->getBaseStats()->health / 3)
	{
		escapeOdds *= 1.7;
		combatOdds *= 0.6;
		ambushOdds *= 0.75;
	}
	else if (_unit->getHealth() < 2 * (_unit->getBaseStats()->health / 3))
	{
		escapeOdds *= 1.4;
		combatOdds *= 0.8;
		ambushOdds *= 0.8;
	}
	else if (_unit->getHealth() < _unit->getBaseStats()->health)
	{
		escapeOdds *= 1.1;
	}

	// take our aggression into consideration
	switch (_unit->getAggression())
	{
	case 0:
		escapeOdds *= 1.4;
		combatOdds *= 0.7;
		break;
	case 1:
		ambushOdds *= 1.1;
		break;
	case 2:
		combatOdds *= 1.4;
		escapeOdds *= 0.7;
		break;
	default:
		combatOdds *= Clamp(1.2 + (_unit->getAggression() / 10.0), 0.1, 2.0);
		escapeOdds *= Clamp(0.9 - (_unit->getAggression() / 10.0), 0.1, 2.0);
		break;
	}

	if (_AIMode == AI_COMBAT)
	{
		ambushOdds *= 1.5;
	}

	// factor in the spotters.
	if (_spottingEnemies)
	{
		escapeOdds = 10 * escapeOdds * (_spottingEnemies + 10) / 100;
		combatOdds = 5 * combatOdds * (_spottingEnemies + 20) / 100;
	}
	else
	{
		escapeOdds /= 2;
	}

	// factor in visible enemies.
	if (_visibleEnemies)
	{
		combatOdds = 10 * combatOdds * (_visibleEnemies + 10) /100;
		if (_closestDist < 5)
		{
			ambushOdds = 0;
		}
	}
	// make sure we have an ambush lined up, or don't even consider it.
	if (_ambushTUs)
	{
		ambushOdds *= 1.7;
	}
	else
	{
		ambushOdds = 0;
	}

	// factor in mission type
	if (_save->getMissionType() == "STR_BASE_DEFENSE")
	{
		escapeOdds *= 0.75;
		ambushOdds *= 0.6;
	}

	// no weapons, not psychic? don't pick combat or ambush
	if (!_melee && !_rifle && !_blaster && !_unit->getGrenadeFromBelt() && _unit->getBaseStats()->psiSkill == 0)
	{
		combatOdds = 0;
		ambushOdds = 0;
	}
	// generate a random number to represent our decision.
	int decision = RNG::generate(1, std::max(1, patrolOdds + ambushOdds + escapeOdds + combatOdds));

	if (decision > escapeOdds)
	{
		if (decision > escapeOdds + ambushOdds)
		{
			if (decision > escapeOdds + ambushOdds + combatOdds)
			{
				_AIMode = AI_PATROL;
			}
			else
			{
				_AIMode = AI_COMBAT;
			}
		}
		else
		{
			_AIMode = AI_AMBUSH;
		}
	}
	else
	{
		_AIMode = AI_ESCAPE;
	}

	// if the aliens are cheating, or the unit is charging, enforce combat as a priority.
	if ((_unit->getFaction() == FACTION_HOSTILE && _save->isCheating()) || _unit->getCharging() != 0)
	{
		_AIMode = AI_COMBAT;
	}


	// enforce the validity of our decision, and try fallback behaviour according to priority.
	if (_AIMode == AI_COMBAT)
	{
		if (_save->getTile(_attackAction->target) && _save->getTile(_attackAction->target)->getUnit())
		{
			if (_attackAction->type != BA_RETHINK)
			{
				return;
			}
			if (findFirePoint())
			{
				return;
			}
		}
		else if (selectRandomTarget() && findFirePoint())
		{
			return;
		}
		_AIMode = AI_PATROL;
	}

	if (_AIMode == AI_PATROL)
	{
		if (_toNode)
		{
			return;
		}
		// base defense mission protocol: patrol action becomes an attack action when base modules are sighted
		if (_patrolAction->type == BA_SNAPSHOT)
		{
			return;
		}
		_AIMode = AI_AMBUSH;
	}

	if (_AIMode == AI_AMBUSH)
	{
		if (_ambushTUs != 0)
		{
			return;
		}
		_AIMode = AI_ESCAPE;
	}
}

/**
 * Find a position where we can see our target, and move there.
 * check the 11x11 grid for a position nearby where we can potentially target him.
 * @return True if a possible position was found.
 */
bool AIModule::findFirePoint()
{
	if (!selectClosestKnownEnemy())
		return false;
	std::vector<Position> randomTileSearch = _save->getTileSearch();
	RNG::shuffle(randomTileSearch);
	Position target;
	const int BASE_SYSTEMATIC_SUCCESS = 100;
	const int FAST_PASS_THRESHOLD = 125;
	int bestScore = 0;
	_attackAction->type = BA_RETHINK;
	for (std::vector<Position>::const_iterator i = randomTileSearch.begin(); i != randomTileSearch.end(); ++i)
	{
		Position pos = _unit->getPosition() + *i;
		Tile *tile = _save->getTile(pos);
		if (tile == 0  ||
			std::find(_reachableWithAttack.begin(), _reachableWithAttack.end(), _save->getTileIndex(pos))  == _reachableWithAttack.end())
			continue;
		int score = 0;
		// i should really make a function for this
		Position origin = (pos * Position(16,16,24)) +
			// 4 because -2 is eyes and 2 below that is the rifle (or at least that's my understanding)
			Position(8,8, _unit->getHeight() + _unit->getFloatHeight() - tile->getTerrainLevel() - 4);

		if (_save->getTileEngine()->canTargetUnit(&origin, _aggroTarget->getTile(), &target, _unit, false))
		{
			_save->getPathfinding()->calculate(_unit, pos);
			// can move here
			if (_save->getPathfinding()->getStartDirection() != -1)
			{
				score = BASE_SYSTEMATIC_SUCCESS - getSpottingUnits(pos) * 10;
				score += _unit->getTimeUnits() - _save->getPathfinding()->getTotalTUCost();
				if (!_aggroTarget->checkViewSector(pos))
				{
					score += 10;
				}
				if (score > bestScore)
				{
					bestScore = score;
					_attackAction->target = pos;
					_attackAction->finalFacing = _save->getTileEngine()->getDirectionTo(pos, _aggroTarget->getPosition());
					if (score > FAST_PASS_THRESHOLD)
					{
						break;
					}
				}
			}
		}
	}

	if (bestScore > 70)
	{
		_attackAction->type = BA_WALK;
		if (_traceAI)
		{
			Log(LOG_INFO) << "Firepoint found at " << _attackAction->target << ", with a score of: " << bestScore;
		}
		return true;
	}
	if (_traceAI)
	{
		Log(LOG_INFO) << "Firepoint failed, best estimation was: " << _attackAction->target << ", with a score of: " << bestScore;
	}

	return false;
}

/**
 * Decides if it worth our while to create an explosion here.
 * @param targetPos The target's position.
 * @param attackingUnit The attacking unit.
 * @param radius How big the explosion will be.
 * @param diff Game difficulty.
 * @param grenade Is the explosion coming from a grenade?
 * @return True if it is worthwhile creating an explosion in the target position.
 */
bool AIModule::explosiveEfficacy(Position targetPos, BattleUnit *attackingUnit, int radius, int diff, bool grenade) const
{
	// i hate the player and i want him dead, but i don't want to piss him off.
	Mod *mod = _save->getBattleState()->getGame()->getMod();
	if ((!grenade && _save->getTurn() < mod->getTurnAIUseBlaster()) ||
		 (grenade && _save->getTurn() < mod->getTurnAIUseGrenade()))
	{
		return false;
	}

	Tile *targetTile = _save->getTile(targetPos);

	// don't throw grenades at flying enemies.
	if (grenade && targetPos.z > 0 && targetTile->hasNoFloor(_save->getTile(targetPos - Position(0,0,1))))
	{
		return false;
	}

	if (diff == -1)
	{
		diff = _save->getBattleState()->getGame()->getSavedGame()->getDifficultyCoefficient();
	}
	int distance = _save->getTileEngine()->distance(attackingUnit->getPosition(), targetPos);
	int injurylevel = attackingUnit->getBaseStats()->health - attackingUnit->getHealth();
	int desperation = (100 - attackingUnit->getMorale()) / 10;
	int enemiesAffected = 0;
	// if we're below 1/3 health, let's assume things are dire, and increase desperation.
	if (injurylevel > (attackingUnit->getBaseStats()->health / 3) * 2)
		desperation += 3;

	int efficacy = desperation;

	// don't go kamikaze unless we're already doomed.
	if (abs(attackingUnit->getPosition().z - targetPos.z) <= Options::battleExplosionHeight && distance <= radius)
	{
		efficacy -= 4;
	}

	// allow difficulty to have its influence
	efficacy += diff/2;

	// account for the unit we're targetting
	BattleUnit *target = targetTile->getUnit();
	if (target && !targetTile->getDangerous())
	{
		++enemiesAffected;
		++efficacy;
	}

	for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
	{
			// don't grenade dead guys
		if (!(*i)->isOut() &&
			// don't count ourself twice
			(*i) != attackingUnit &&
			// don't count the target twice
			(*i) != target &&
			// don't count units that probably won't be affected cause they're out of range
			abs((*i)->getPosition().z - targetPos.z) <= Options::battleExplosionHeight &&
			_save->getTileEngine()->distance((*i)->getPosition(), targetPos) <= radius)
		{
				// don't count people who were already grenaded this turn
			if ((*i)->getTile()->getDangerous() ||
				// don't count units we don't know about
				((*i)->getFaction() == _targetFaction && (*i)->getTurnsSinceSpotted() > _intelligence))
				continue;

			// trace a line from the grenade origin to the unit we're checking against
			Position voxelPosA = Position ((targetPos.x * 16)+8, (targetPos.y * 16)+8, (targetPos.z * 24)+12);
			Position voxelPosB = Position (((*i)->getPosition().x * 16)+8, ((*i)->getPosition().y * 16)+8, ((*i)->getPosition().z * 24)+12);
			std::vector<Position> traj;
			int collidesWith = _save->getTileEngine()->calculateLine(voxelPosA, voxelPosB, false, &traj, target, true, false, *i);

			if (collidesWith == V_UNIT && traj.front() / Position(16,16,24) == (*i)->getPosition())
			{
				if ((*i)->getFaction() == _targetFaction)
				{
					++enemiesAffected;
					++efficacy;
				}
				else if ((*i)->getFaction() == attackingUnit->getFaction() || (attackingUnit->getFaction() == FACTION_NEUTRAL && (*i)->getFaction() == FACTION_PLAYER))
					efficacy -= 2; // friendlies count double
			}
		}
	}
	// don't throw grenades at single targets, unless morale is in the danger zone
	// or we're halfway towards panicking while bleeding to death.
	if (grenade && desperation < 6 && enemiesAffected < 2)
	{
		return false;
	}
	return (efficacy > 0 || enemiesAffected >= 10);
}

/**
 * Attempts to take a melee attack/charge an enemy we can see.
 * Melee targetting: we can see an enemy, we can move to it so we're charging blindly toward an enemy.
 */
void AIModule::meleeAction()
{
	int attackCost = _unit->getActionTUs(BA_HIT, _unit->getMeleeWeapon());
	if (_unit->getTimeUnits() < attackCost)
	{
		// cannot make a melee attack - consider some other behaviour, like running away, or standing motionless.
		return;
	}
	if (_aggroTarget != 0 && !_aggroTarget->isOut())
	{
		if (_save->getTileEngine()->validMeleeRange(_unit, _aggroTarget, _save->getTileEngine()->getDirectionTo(_unit->getPosition(), _aggroTarget->getPosition())))
		{
			meleeAttack();
			return;
		}
	}
	int chargeReserve = _unit->getTimeUnits() - attackCost;
	int distance = (chargeReserve / 4) + 1;
	_aggroTarget = 0;
	for (std::vector<BattleUnit*>::const_iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
	{
		int newDistance = _save->getTileEngine()->distance(_unit->getPosition(), (*i)->getPosition());
		if (newDistance > 20 ||
			!validTarget(*i, true, _unit->getFaction() == FACTION_HOSTILE))
			continue;
		//pick closest living unit that we can move to
		if ((newDistance < distance || newDistance == 1) && !(*i)->isOut())
		{
			if (newDistance == 1 || selectPointNearTarget(*i, chargeReserve))
			{
				_aggroTarget = (*i);
				_attackAction->type = BA_WALK;
				_unit->setCharging(_aggroTarget);
				distance = newDistance;
			}

		}
	}
	if (_aggroTarget != 0)
	{
		if (_save->getTileEngine()->validMeleeRange(_unit, _aggroTarget, _save->getTileEngine()->getDirectionTo(_unit->getPosition(), _aggroTarget->getPosition())))
		{
			meleeAttack();
		}
	}
	if (_traceAI && _aggroTarget) { Log(LOG_INFO) << "AIModule::meleeAction:" << " [target]: " << (_aggroTarget->getId()) << " at: "  << _attackAction->target; }
	if (_traceAI && _aggroTarget) { Log(LOG_INFO) << "CHARGE!"; }
}

/**
 * Attempts to fire a waypoint projectile at an enemy we, or one of our teammates sees.
 *
 * Waypoint targeting: pick from any units currently spotted by our allies.
 */
void AIModule::wayPointAction()
{
	int attackCost = _unit->getActionTUs(BA_LAUNCH, _attackAction->weapon);
	if (_unit->getTimeUnits() < attackCost)
	{
		// cannot make a launcher attack - consider some other behaviour, like running away, or standing motionless.
		return;
	}
	_aggroTarget = 0;
	for (std::vector<BattleUnit*>::const_iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end() && _aggroTarget == 0; ++i)
	{
		if (!validTarget(*i, true, _unit->getFaction() == FACTION_HOSTILE))
			continue;
		_save->getPathfinding()->calculate(_unit, (*i)->getPosition(), *i, -1);
		if (_save->getPathfinding()->getStartDirection() != -1 &&
			explosiveEfficacy((*i)->getPosition(), _unit, (_attackAction->weapon->getAmmoItem()->getRules()->getPower()/20)+1, _attackAction->diff))
		{
			_aggroTarget = *i;
		}
		_save->getPathfinding()->abortPath();
	}

	if (_aggroTarget != 0)
	{
		_attackAction->type = BA_LAUNCH;
		_attackAction->TU = _unit->getActionTUs(BA_LAUNCH, _attackAction->weapon);
		if (_attackAction->TU > _unit->getTimeUnits())
		{
			_attackAction->type = BA_RETHINK;
			return;
		}
		_attackAction->waypoints.clear();

		int PathDirection;
		int CollidesWith;
		int maxWaypoints = _attackAction->weapon->getRules()->getWaypoints();
		if (maxWaypoints == 0)
		{
			maxWaypoints = _attackAction->weapon->getAmmoItem()->getRules()->getWaypoints();
		}
		if (maxWaypoints == -1)
		{
			maxWaypoints = 6 + (_attackAction->diff * 2);
		}
		Position LastWayPoint = _unit->getPosition();
		Position LastPosition = _unit->getPosition();
		Position CurrentPosition = _unit->getPosition();
		Position DirectionVector;

		_save->getPathfinding()->calculate(_unit, _aggroTarget->getPosition(), _aggroTarget, -1);
		PathDirection = _save->getPathfinding()->dequeuePath();
		while (PathDirection != -1 && (int)_attackAction->waypoints.size() < maxWaypoints)
		{
			LastPosition = CurrentPosition;
			_save->getPathfinding()->directionToVector(PathDirection, &DirectionVector);
			CurrentPosition = CurrentPosition + DirectionVector;
			Position voxelPosA ((CurrentPosition.x * 16)+8, (CurrentPosition.y * 16)+8, (CurrentPosition.z * 24)+16);
			Position voxelPosb ((LastWayPoint.x * 16)+8, (LastWayPoint.y * 16)+8, (LastWayPoint.z * 24)+16);
			CollidesWith = _save->getTileEngine()->calculateLine(voxelPosA, voxelPosb, false, 0, _unit, true);
			if (CollidesWith > V_EMPTY && CollidesWith < V_UNIT)
			{
				_attackAction->waypoints.push_back(LastPosition);
				LastWayPoint = LastPosition;
			}
			else if (CollidesWith == V_UNIT)
			{
				BattleUnit* target = _save->getTile(CurrentPosition)->getUnit();
				if (target == _aggroTarget)
				{
					_attackAction->waypoints.push_back(CurrentPosition);
					LastWayPoint = CurrentPosition;
				}
			}
			PathDirection = _save->getPathfinding()->dequeuePath();
		}
		_attackAction->target = _attackAction->waypoints.front();
		if (LastWayPoint != _aggroTarget->getPosition())
		{
			_attackAction->type = BA_RETHINK;
		}
	}
}

/**
 * Attempts to fire at an enemy we can see.
 *
 * Regular targeting: we can see an enemy, we have a gun, let's try to shoot.
 */
void AIModule::projectileAction()
{
	_attackAction->target = _aggroTarget->getPosition();
	if (!_attackAction->weapon->getAmmoItem()->getRules()->getExplosionRadius() ||
		explosiveEfficacy(_aggroTarget->getPosition(), _unit, _attackAction->weapon->getAmmoItem()->getRules()->getExplosionRadius(), _attackAction->diff))
	{
		selectFireMethod();
	}
}

/**
 * Selects a fire method based on range, time units, and time units reserved for cover.
 */
void AIModule::selectFireMethod()
{
	int distance = _save->getTileEngine()->distance(_unit->getPosition(), _attackAction->target);
	_attackAction->type = BA_RETHINK;
	int tuAuto = _attackAction->weapon->getRules()->getTUAuto();
	int tuSnap = _attackAction->weapon->getRules()->getTUSnap();
	int tuAimed = _attackAction->weapon->getRules()->getTUAimed();
	int currentTU = _unit->getTimeUnits();

	if (distance < 4)
	{
		if ( tuAuto && currentTU >= _unit->getActionTUs(BA_AUTOSHOT, _attackAction->weapon) )
		{
			_attackAction->type = BA_AUTOSHOT;
			return;
		}
		if ( !tuSnap || currentTU < _unit->getActionTUs(BA_SNAPSHOT, _attackAction->weapon) )
		{
			if ( tuAimed && currentTU >= _unit->getActionTUs(BA_AIMEDSHOT, _attackAction->weapon) )
			{
				_attackAction->type = BA_AIMEDSHOT;
			}
			return;
		}
		_attackAction->type = BA_SNAPSHOT;
		return;
	}


	if ( distance > 12 )
	{
		if ( tuAimed && currentTU >= _unit->getActionTUs(BA_AIMEDSHOT, _attackAction->weapon) )
		{
			_attackAction->type = BA_AIMEDSHOT;
			return;
		}
		if ( distance < 20
			&& tuSnap
			&& currentTU >= _unit->getActionTUs(BA_SNAPSHOT, _attackAction->weapon) )
		{
			_attackAction->type = BA_SNAPSHOT;
			return;
		}
	}

	if ( tuSnap && currentTU >= _unit->getActionTUs(BA_SNAPSHOT, _attackAction->weapon) )
	{
			_attackAction->type = BA_SNAPSHOT;
			return;
	}
	if ( tuAimed && currentTU >= _unit->getActionTUs(BA_AIMEDSHOT, _attackAction->weapon) )
	{
			_attackAction->type = BA_AIMEDSHOT;
			return;
	}
	if ( tuAuto && currentTU >= _unit->getActionTUs(BA_AUTOSHOT, _attackAction->weapon) )
	{
			_attackAction->type = BA_AUTOSHOT;
	}
}

/**
 * Evaluates whether to throw a grenade at an enemy (or group of enemies) we can see.
 */
void AIModule::grenadeAction()
{
	// do we have a grenade on our belt?
	BattleItem *grenade = _unit->getGrenadeFromBelt();
	int tu = 4; // 4TUs for picking up the grenade
	tu += _unit->getActionTUs(BA_PRIME, grenade);
	tu += _unit->getActionTUs(BA_THROW, grenade);
	// do we have enough TUs to prime and throw the grenade?
	if (tu <= _unit->getTimeUnits())
	{
		BattleAction action;
		action.weapon = grenade;
		action.type = BA_THROW;
		action.actor = _unit;
		if (explosiveEfficacy(_aggroTarget->getPosition(), _unit, grenade->getRules()->getExplosionRadius(), _attackAction->diff, true))
		{
			action.target = _aggroTarget->getPosition();
		}
		else if (!getNodeOfBestEfficacy(&action))
		{
			return;
		}
		Position originVoxel = _save->getTileEngine()->getOriginVoxel(action, 0);
		Position targetVoxel = action.target * Position (16,16,24) + Position (8,8, (2 + -_save->getTile(action.target)->getTerrainLevel()));
		// are we within range?
		if (_save->getTileEngine()->validateThrow(action, originVoxel, targetVoxel))
		{
			_attackAction->weapon = grenade;
			_attackAction->target = action.target;
			_attackAction->type = BA_THROW;
			_attackAction->TU = tu;
			_rifle = false;
			_melee = false;
		}
	}
}

/**
 * Attempts a psionic attack on an enemy we "know of".
 *
 * Psionic targetting: pick from any of the "exposed" units.
 * Exposed means they have been previously spotted, and are therefore "known" to the AI,
 * regardless of whether we can see them or not, because we're psychic.
 * @return True if a psionic attack is performed.
 */
bool AIModule::psiAction()
{
	BattleItem *item = _unit->getSpecialWeapon(BT_PSIAMP);
	if (!item)
	{
		return false;
	}
	RuleItem *psiWeaponRules = item->getRules();
	int cost = psiWeaponRules->getTUUse();
	if (!psiWeaponRules->getFlatRate())
	{
		cost = (int)floor(_unit->getBaseStats()->tu * cost / 100.0f);
	}
	bool LOSRequired = psiWeaponRules->isLOSRequired();

	_aggroTarget = 0;
		// don't let mind controlled soldiers mind control other soldiers.
	if (_unit->getOriginalFaction() == _unit->getFaction()
		// and we have the required 25 TUs and can still make it to cover
		&& _unit->getTimeUnits() > _escapeTUs + cost
		// and we didn't already do a psi action this round
		&& !_didPsi)
	{
		int psiAttackStrength = _unit->getBaseStats()->psiSkill * _unit->getBaseStats()->psiStrength / 50;
		int chanceToAttack = 0;

		for (std::vector<BattleUnit*>::const_iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
		{
			// don't target tanks
			if ((*i)->getArmor()->getSize() == 1 &&
				validTarget(*i, true, false) &&
				// they must be player units
				(*i)->getOriginalFaction() == _targetFaction &&
				(!LOSRequired ||
				std::find(_unit->getVisibleUnits()->begin(), _unit->getVisibleUnits()->end(), *i) != _unit->getVisibleUnits()->end()))
			{
				int chanceToAttackMe = psiAttackStrength
					+ (((*i)->getBaseStats()->psiSkill > 0) ? (*i)->getBaseStats()->psiSkill * -0.4 : 0)
					- _save->getTileEngine()->distance((*i)->getPosition(), _unit->getPosition())
					- ((*i)->getBaseStats()->psiStrength)
					+ RNG::generate(55, 105);

				if (chanceToAttackMe > chanceToAttack)
				{
					chanceToAttack = chanceToAttackMe;
					_aggroTarget = *i;
				}
			}
		}

		if (!_aggroTarget || !chanceToAttack) return false;

		if (_visibleEnemies && _attackAction->weapon && _attackAction->weapon->getAmmoItem())
		{
			if (_attackAction->weapon->getAmmoItem()->getRules()->getPower() >= chanceToAttack)
			{
				return false;
			}
		}
		else if (RNG::generate(35, 155) >= chanceToAttack)
		{
			return false;
		}

		if (_traceAI)
		{
			Log(LOG_INFO) << "making a psionic attack this turn";
		}

		if (chanceToAttack >= 30)
		{
			int controlOdds = 40;
			int morale = _aggroTarget->getMorale();
			int bravery = (110 - _aggroTarget->getBaseStats()->bravery) / 10;
			if (bravery > 6)
				controlOdds -= 15;
			if (bravery < 4)
				controlOdds += 15;
			if (morale >= 40)
			{
				if (morale - 10 * bravery < 50)
					controlOdds -= 15;
			}
			else
			{
				controlOdds += 15;
			}
			if (!morale)
			{
				controlOdds = 100;
			}
			if (RNG::percent(controlOdds))
			{
				_psiAction->type = BA_MINDCONTROL;
				_psiAction->target = _aggroTarget->getPosition();
				_psiAction->weapon = item;
				return true;
			}
		}
		_psiAction->type = BA_PANIC;
		_psiAction->target = _aggroTarget->getPosition();
		_psiAction->weapon = item;
		return true;
	}
	return false;
}

/**
 * Performs a melee attack action.
 */
void AIModule::meleeAttack()
{
	_unit->lookAt(_aggroTarget->getPosition() + Position(_unit->getArmor()->getSize()-1, _unit->getArmor()->getSize()-1, 0), false);
	while (_unit->getStatus() == STATUS_TURNING)
		_unit->turn();
	if (_traceAI) { Log(LOG_INFO) << "Attack unit: " << _aggroTarget->getId(); }
	_attackAction->target = _aggroTarget->getPosition();
	_attackAction->type = BA_HIT;
	_attackAction->weapon = _unit->getMeleeWeapon();
}

/**
 * Validates a target.
 * @param unit the target we want to validate.
 * @param assessDanger do we care if this unit was previously targetted with a grenade?
 * @param includeCivs do we include civilians in the threat assessment?
 * @return whether this target is someone we would like to kill.
 */
bool AIModule::validTarget(BattleUnit *unit, bool assessDanger, bool includeCivs) const
{
		// ignore units that are dead/unconscious
	if (unit->isOut() ||
		// they must be units that we "know" about
		(_unit->getFaction() == FACTION_HOSTILE && _intelligence < unit->getTurnsSinceSpotted()) ||
		// they haven't been grenaded
		(assessDanger && unit->getTile()->getDangerous()) ||
		// and they mustn't be on our side
		unit->getFaction() == _unit->getFaction())
	{
		return false;
	}

	if (includeCivs)
	{
		return true;
	}

	return unit->getFaction() == _targetFaction;
}

/**
 * Checks the alien's reservation setting.
 * @return the reserve setting.
 */
BattleActionType AIModule::getReserveMode()
{
	return _reserve;
}

/**
 * We have a dichotomy on our hands: we have a ranged weapon and melee capability.
 * let's make a determination on which one we'll be using this round.
 */
void AIModule::selectMeleeOrRanged()
{
	RuleItem *rangedWeapon = _attackAction->weapon->getRules();
	RuleItem *meleeWeapon = _unit->getMeleeWeapon() ? _unit->getMeleeWeapon()->getRules() : 0;

	if (!meleeWeapon)
	{
		// no idea how we got here, but melee is definitely out of the question.
		_melee = false;
		return;
	}
	if (!rangedWeapon || _attackAction->weapon->getAmmoItem() == 0)
	{
		_rifle = false;
		return;
	}

	int meleeOdds = 10;

	int dmg = meleeWeapon->getPower();
	if (meleeWeapon->isStrengthApplied())
	{
		dmg += _unit->getBaseStats()->strength;
	}
	dmg *= _aggroTarget->getArmor()->getDamageModifier(meleeWeapon->getDamageType());

	if (dmg > 50)
	{
		meleeOdds += (dmg - 50) / 2;
	}
	if ( _visibleEnemies > 1 )
	{
		meleeOdds -= 20 * (_visibleEnemies - 1);
	}

	if (meleeOdds > 0 && _unit->getHealth() >= 2 * _unit->getBaseStats()->health / 3)
	{
		if (_unit->getAggression() == 0)
		{
			meleeOdds -= 20;
		}
		else if (_unit->getAggression() > 1)
		{
			meleeOdds += 10 * _unit->getAggression();
		}

		if (RNG::percent(meleeOdds))
		{
			_rifle = false;
			_reachableWithAttack = _save->getPathfinding()->findReachable(_unit, _unit->getTimeUnits() - _unit->getActionTUs(BA_HIT, meleeWeapon));
			return;
		}
	}
	_melee = false;
}

/**
 * Checks nearby nodes to see if they'd make good grenade targets
 * @param action contains our details one weapon and user, and we set the target for it here.
 * @return if we found a viable node or not.
 */
bool AIModule::getNodeOfBestEfficacy(BattleAction *action)
{
	// i hate the player and i want him dead, but i don't want to piss him off.
	if (_save->getTurn() < _save->getBattleState()->getGame()->getMod()->getTurnAIUseGrenade())
		return false;

	int bestScore = 2;
	Position originVoxel = _save->getTileEngine()->getSightOriginVoxel(_unit);
	Position targetVoxel;
	for (std::vector<Node*>::const_iterator i = _save->getNodes()->begin(); i != _save->getNodes()->end(); ++i)
	{
		if ((*i)->isDummy())
		{
			continue;
		}
		int dist = _save->getTileEngine()->distance((*i)->getPosition(), _unit->getPosition());
		if (dist <= 20 && dist > action->weapon->getRules()->getExplosionRadius() &&
			_save->getTileEngine()->canTargetTile(&originVoxel, _save->getTile((*i)->getPosition()), O_FLOOR, &targetVoxel, _unit, false))
		{
			int nodePoints = 0;
			for (std::vector<BattleUnit*>::const_iterator j = _save->getUnits()->begin(); j != _save->getUnits()->end(); ++j)
			{
				dist = _save->getTileEngine()->distance((*i)->getPosition(), (*j)->getPosition());
				if (!(*j)->isOut() && dist < action->weapon->getRules()->getExplosionRadius())
				{
					Position targetOriginVoxel = _save->getTileEngine()->getSightOriginVoxel(*j);
					if (_save->getTileEngine()->canTargetTile(&targetOriginVoxel, _save->getTile((*i)->getPosition()), O_FLOOR, &targetVoxel, *j, false))
					{
						if ((_unit->getFaction() == FACTION_HOSTILE && (*j)->getFaction() != FACTION_HOSTILE) ||
							(_unit->getFaction() == FACTION_NEUTRAL && (*j)->getFaction() == FACTION_HOSTILE))
						{
							if ((*j)->getTurnsSinceSpotted() <= _intelligence)
							{
								nodePoints++;
							}
						}
						else
						{
							nodePoints -= 2;
						}
					}
				}
			}
			if (nodePoints > bestScore)
			{
				bestScore = nodePoints;
				action->target = (*i)->getPosition();
			}
		}
	}
	return bestScore > 2;
}

BattleUnit* AIModule::getTarget()
{
	return _aggroTarget;
}

void AIModule::freePatrolTarget()
{
	if (_toNode)
	{
		_toNode->freeNode();
	}
}

}
