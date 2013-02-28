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
#include <sstream>
#include "BattlescapeGame.h"
#include "BattlescapeState.h"
#include "../Engine/Timer.h"

#include "Map.h"
#include "Camera.h"
#include "NextTurnState.h"
#include "AbortMissionState.h"
#include "BattleState.h"
#include "UnitTurnBState.h"
#include "UnitWalkBState.h"
#include "ProjectileFlyBState.h"
#include "ExplosionBState.h"
#include "TileEngine.h"
#include "ActionMenuState.h"
#include "UnitInfoState.h"
#include "UnitDieBState.h"
#include "UnitPanicBState.h"
#include "InventoryState.h"
#include "AggroBAIState.h"
#include "PatrolBAIState.h"
#include "Pathfinding.h"
#include "../Engine/Game.h"
#include "../Engine/Music.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Screen.h"
#include "../Engine/Sound.h"
#include "../Engine/Action.h"
#include "../Resource/ResourcePack.h"
#include "../Interface/Cursor.h"
#include "../Interface/FpsCounter.h"
#include "../Interface/Text.h"
#include "../Interface/Bar.h"
#include "../Interface/ImageButton.h"
#include "../Interface/NumberText.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/BattleItem.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/Armor.h"
#include "../Engine/Options.h"
#include "WarningMessage.h"
#include "BattlescapeOptionsState.h"
#include "DebriefingState.h"
#include "../Engine/RNG.h"
#include "InfoboxState.h"
#include "InfoboxOKState.h"
#include "MiniMapState.h"
#include "UnitFallBState.h"
#include "../Engine/Logger.h"

namespace OpenXcom
{

bool BattlescapeGame::_debugPlay = false;

/**
 * Initializes all the elements in the Battlescape screen.
 * @param save Pointer to the save game.
 * @param parentState Pointer to the parent battlescape state.
 */
BattlescapeGame::BattlescapeGame(SavedBattleGame *save, BattlescapeState *parentState) : _save(save), _parentState(parentState), _playedAggroSound(false)
{
	_tuReserved = BA_NONE;
	_debugPlay = false;
	_playerPanicHandled = true;
	_AIActionCounter = 0;
	_currentAction.actor = 0;

	checkForCasualties(0, 0, true);
	cancelCurrentAction();
	_currentAction.targeting = false;
	_currentAction.type = BA_NONE;
}


/**
 * Delete BattlescapeGame.
 */
BattlescapeGame::~BattlescapeGame()
{
}

/**
  * think
  */
void BattlescapeGame::think()
{
	// nothing is happening - see if we need some alien AI or units panicking or what have you
	if (_states.empty())
	{
		// it's a non player side (ALIENS or CIVILIANS)
		if (_save->getSide() != FACTION_PLAYER)
		{
			if (!_debugPlay)
			{
				if (_save->getSelectedUnit())
				{
					if (!handlePanickingUnit(_save->getSelectedUnit()))
						handleAI(_save->getSelectedUnit());
				}
				else
				{
					if (_save->selectNextPlayerUnit(true, true) == 0)
					{
						if (!_save->getDebugMode())
						{
							statePushBack(0); // end AI turn
						}
						else
						{
							_save->selectNextPlayerUnit();
							_debugPlay = true;
						}
					}
				}
			}
		}
		else
		{
			// it's a player side && we have not handled all panicking units
			if (!_playerPanicHandled)
			{
				_playerPanicHandled = handlePanickingPlayer();
			}
		}
		if (_save->getUnitsFalling())
		{
			statePushFront(new UnitFallBState(this));
			_save->setUnitsFalling(false);
		}
	}

}

void BattlescapeGame::init()
{
	if (_save->getSide() == FACTION_PLAYER)
	{
		_playerPanicHandled = false;
	}
}


/**
 * Handles the processing of the AI states of a unit.
 * @param unit Pointer to an unit.
 */
void BattlescapeGame::handleAI(BattleUnit *unit)
{
	std::wstringstream ss;
	BattleAIState *ai = unit->getCurrentAIState();
	if (!ai)
	{
		// for some reason the unit had no AI routine assigned..
		unit->setAIState(new PatrolBAIState(_save, unit, 0));
		ai = unit->getCurrentAIState();
	}
	_AIActionCounter++;
	if (_AIActionCounter == 1 && _playedAggroSound)
	{
		_playedAggroSound = false;
	}
	if(_AIActionCounter == 1)
	{
		unit->_hidingForTurn = 0;
		unit->_desperatelySeekingCover = 0;
		if (Options::getBool("traceAI")) { Log(LOG_INFO) << "#" << unit->getId() << "--" << unit->getType(); }
	}
	AggroBAIState *aggro = dynamic_cast<AggroBAIState*>(ai); // this cast only works when ai was already AggroBAIState at heart
	
	// psionic or blaster launcher units may attack remotely
	// in bonus round, need to be in "aggro" state to hide; what was that about refactoring?
	if (unit->getStats()->psiSkill
		|| (unit->getMainHandWeapon() && unit->getMainHandWeapon()->getRules()->isWaypoint())
		|| (_AIActionCounter > 2))
	{
		if (aggro == 0)
		{
			aggro = new AggroBAIState(_save, unit);
			unit->setAIState(aggro);
		}
		ai = unit->getCurrentAIState();
		_parentState->debug(L"Aggro");
	}

	BattleAction action;
	action.diff = _parentState->getGame()->getSavedGame()->getDifficulty();
    action.number = _AIActionCounter;
	unit->think(&action);
	
	if (action.type == BA_RETHINK)
	{
		_parentState->debug(L"Rethink");
		unit->setAIState(new PatrolBAIState(_save, unit, 0));
		ai = unit->getCurrentAIState();
		unit->think(&action);	
	}

	if (action.type == BA_WALK)
	{
		ss << L"Walking to " << action.target.x << " "<< action.target.y << " "<< action.target.z;
		_parentState->debug(ss.str());
		if (unit->getAggroSound() && aggro && !_playedAggroSound)
		{
			getResourcePack()->getSound("BATTLE.CAT", unit->getAggroSound())->play();
			_playedAggroSound = true;
		}
 		_save->getPathfinding()->calculate(action.actor, action.target);
		statePushBack(new UnitWalkBState(this, action));
	}

	if (action.type == BA_SNAPSHOT || action.type == BA_AUTOSHOT || action.type == BA_THROW || action.type == BA_HIT || action.type == BA_MINDCONTROL || action.type == BA_PANIC || action.type == BA_LAUNCH)
	{
		if (action.type == BA_MINDCONTROL || action.type == BA_PANIC)
		{
			action.weapon = new BattleItem(_parentState->getGame()->getRuleset()->getItem("ALIEN_PSI_WEAPON"), _save->getCurrentItemId());
			action.TU = action.weapon->getRules()->getTUUse();
		}

		ss.clear();
		ss << L"Attack type=" << action.type << " target="<< action.target.x << " "<< action.target.y << " "<< action.target.z << " weapon=" << action.weapon->getRules()->getName().c_str();
		_parentState->debug(ss.str());

		action.actor->lookAt(action.target);
		while (action.actor->getStatus() == STATUS_TURNING)
			action.actor->turn();
		statePushBack(new ProjectileFlyBState(this, action));
		if (action.type == BA_MINDCONTROL || action.type == BA_PANIC)
		{
			bool success = _save->getTileEngine()->psiAttack(&action);
			if (success && action.type == BA_MINDCONTROL)
			{
				_save->updateExposedUnits();
				// show a little infobox with the name of the unit and "... is under alien control"
				std::wstringstream ss;
				ss << _save->getTile(action.target)->getUnit()->getName(_parentState->getGame()->getLanguage()) << L'\n' << _parentState->getGame()->getLanguage()->getString("STR_IS_UNDER_ALIEN_CONTROL");
				_parentState->getGame()->pushState(new InfoboxState(_parentState->getGame(), ss.str()));
			}
			_save->removeItem(action.weapon);
		}
	}

	if (action.type == BA_NONE)
	{
		_parentState->debug(L"Idle");
		_AIActionCounter = 0;
		if (aggro != 0)
		{
			// we lost aggro
			unit->setAIState(new PatrolBAIState(_save, unit, 0));
			_parentState->debug(L"Lost aggro");
		}
		if (_save->selectNextPlayerUnit(true, true) == 0)
		{
			if (!_save->getDebugMode())
			{
				statePushBack(0); // end AI turn
			}
			else
			{
				_save->selectNextPlayerUnit();
				_debugPlay = true;
			}
		}
		if (_save->getSelectedUnit())
		{
			getMap()->getCamera()->centerOnPosition(_save->getSelectedUnit()->getPosition());
		}
	}
}

/**
 * Kneel/Standup.
 * @param bu Pointer to a unit.
 */
void BattlescapeGame::kneel(BattleUnit *bu)
{
	int tu = bu->isKneeled()?8:4;
	if (checkReservedTU(bu, tu))
	{
		if (bu->spendTimeUnits(tu))
		{
			bu->kneel(!bu->isKneeled());
			// kneeling or standing up can reveal new terrain or units. I guess.
			getTileEngine()->calculateFOV(bu);
			getMap()->cacheUnits();
			_parentState->updateSoldierInfo();
			BattleAction action;
			if (getTileEngine()->checkReactionFire(bu, &action, 0, false))
			{
				action.cameraPosition = getMap()->getCamera()->getMapOffset();
				statePushBack(new ProjectileFlyBState(this, action));
			}
		}
		else
		{
			_parentState->warning("STR_NOT_ENOUGH_TIME_UNITS");
		}
	}
}

/**
 * End turn.
 */
void BattlescapeGame::endTurn()
{
	Position p;

	_debugPlay = false;
	_currentAction.type = BA_NONE;

	// check for hot grenades on the ground
	for (int i = 0; i < _save->getWidth() * _save->getLength() * _save->getHeight(); ++i)
	{
		for (std::vector<BattleItem*>::iterator it = _save->getTiles()[i]->getInventory()->begin(); it != _save->getTiles()[i]->getInventory()->end(); )
		{
			if ((*it)->getRules()->getBattleType() == BT_GRENADE && (*it)->getExplodeTurn() > 0 && (*it)->getExplodeTurn() <= _save->getTurn())  // it's a grenade to explode now
			{
				p.x = _save->getTiles()[i]->getPosition().x*16 + 8;
				p.y = _save->getTiles()[i]->getPosition().y*16 + 8;
				p.z = _save->getTiles()[i]->getPosition().z*24 - _save->getTiles()[i]->getTerrainLevel();
				statePushNext(new ExplosionBState(this, p, (*it), (*it)->getPreviousOwner()));
				_save->removeItem((*it));
				statePushBack(0);
				return;
			}
			++it;
		}
	}

	if (_save->getTileEngine()->closeUfoDoors())
	{
		getResourcePack()->getSound("BATTLE.CAT", 21)->play(); // ufo door closed
	}

	_save->endTurn();

	if (_save->getSide() == FACTION_PLAYER)
	{
		setupCursor();
	}
	else
	{
		getMap()->setCursorType(CT_NONE);
	}

	// check for terrain explosions
	Tile *t = _save->getTileEngine()->checkForTerrainExplosions();
	if (t)
	{
		Position p = Position(t->getPosition().x * 16, t->getPosition().y * 16, t->getPosition().z * 24);
		statePushNext(new ExplosionBState(this, p, 0, 0, t));
		checkForCasualties(0, 0, false, true);
	}

	checkForCasualties(0, 0, false, false);

	// if all units from either faction are killed - the mission is over.
	int liveAliens = 0;
	int liveSoldiers = 0;
	for (std::vector<BattleUnit*>::iterator j = _save->getUnits()->begin(); j != _save->getUnits()->end(); ++j)
	{
		if ((*j)->getHealth() > 0 && (*j)->getHealth() > (*j)->getStunlevel())
		{
			if ((*j)->getOriginalFaction() == FACTION_HOSTILE)
				liveAliens++;
			if ((*j)->getOriginalFaction() == FACTION_PLAYER)
				liveSoldiers++;
		}
	}

	if (_save->isObjectiveDestroyed())
	{
		_parentState->finishBattle(false,liveSoldiers);
		return;
	}


	if (liveAliens > 0 && liveSoldiers > 0)
	{
		showInfoBoxQueue();

		_parentState->updateSoldierInfo();

		if (playableUnitSelected())
		{
			getMap()->getCamera()->centerOnPosition(_save->getSelectedUnit()->getPosition());
			setupCursor();
		}
	}

    if (_save->getSide() == FACTION_HOSTILE)
    {
        resetSituationForAI();
    }

	if (_save->getSide() != FACTION_NEUTRAL)
	{
		_parentState->getGame()->pushState(new NextTurnState(_parentState->getGame(), _save, _parentState));
	}

}


/**
 * Checks for casualties and adjusts morale accordingly.
 * @param murderweapon Need to know this, for a HE explosion there is an instant death.
 * @param murderer This is needed for credits for the kill.
 * @param hiddenExplosion Set to true for the explosions of UFO Power sources at start of battlescape.
 * @param terrainExplosion Set to true for the explosions of terrain.
 * @return Whether the battle is finished.
 */
void BattlescapeGame::checkForCasualties(BattleItem *murderweapon, BattleUnit *murderer, bool hiddenExplosion, bool terrainExplosion)
{
	for (std::vector<BattleUnit*>::iterator j = _save->getUnits()->begin(); j != _save->getUnits()->end(); ++j)
	{
		if ((*j)->getHealth() == 0 && (*j)->getStatus() != STATUS_DEAD && (*j)->getStatus() != STATUS_COLLAPSING)
		{
			BattleUnit *victim = (*j);

			if (murderer)
			{
				murderer->addKillCount();
				victim->killedBy(murderer->getFaction());
				// if there is a known murderer, he will get a morale bonus if he is of a different faction (what with neutral?)
				if ((victim->getOriginalFaction() == FACTION_PLAYER && murderer->getFaction() == FACTION_HOSTILE) ||
					(victim->getOriginalFaction() == FACTION_HOSTILE && murderer->getFaction() == FACTION_PLAYER))
				{
					murderer->moraleChange(+20);
				}
				// murderer will get a penalty with friendly fire
				if (victim->getOriginalFaction() == murderer->getFaction())
				{
					murderer->moraleChange(-20);
				}
			}

			for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
			{
				// the losing squad all get a morale loss
				if ((*i)->getFaction() == victim->getOriginalFaction())
				{
					(*i)->moraleChange(-(22 - ((*i)->getStats()->bravery / 10)*2));

					// revenge procedure:
					// if the victim is hostile, the nearest other hostile will aggro if he wasn't already
					if (victim->getFaction() == FACTION_HOSTILE && murderer)
					{
						int closest = 1000000;
						BattleUnit *revenger = 0;
						bool revenge = RNG::generate(0,100) < 50;
						for (std::vector<BattleUnit*>::iterator h = _save->getUnits()->begin(); h != _save->getUnits()->end(); ++h)
						{
							if ((*h)->getFaction() == FACTION_HOSTILE && !(*h)->isOut() && (*h) != victim)
							{
								int d = _save->getTileEngine()->distanceSq(victim->getPosition(), (*h)->getPosition());
								if (d < closest)
								{
									revenger = (*h);
									closest = d;
								}
							}
						}
						// aliens with aggression level 2 always revenge
						// aliens with aggression level 1 have 50% chance to revenge
						// aliens with aggression level 0 never revenge
						if (revenger && (revenger->getAggression() == 2 || (revenger->getAggression() == 1 && revenge)))
						{
							AggroBAIState *aggro = dynamic_cast<AggroBAIState*>(revenger->getCurrentAIState());
							if (aggro == 0)
							{
								aggro = new AggroBAIState(_save, revenger);
								revenger->setAIState(aggro);
							}
							aggro->setAggroTarget(murderer);
						}
					}
				}
				// the winning squad all get a morale increase
				if ((*i)->getFaction() != victim->getOriginalFaction())
				{
					(*i)->moraleChange(+10);
				}
			}

			if (murderweapon)
			{
				statePushNext(new UnitDieBState(this, (*j), murderweapon->getRules()->getDamageType(), false));
				if (Options::getBool("battleNotifyDeath") && (*j)->getFaction() == FACTION_PLAYER && (*j)->getOriginalFaction() == FACTION_PLAYER)
				{
					std::wstringstream ss;
					ss << (*j)->getName(_parentState->getGame()->getLanguage()) << L'\n' << _parentState->getGame()->getLanguage()->getString("STR_HAS_BEEN_KILLED");
					_parentState->getGame()->pushState(new InfoboxState(_parentState->getGame(), ss.str()));
				}
			}
			else
			{
				if (hiddenExplosion)
				{
					// this is instant death from UFO powersources, without screaming sounds
					statePushNext(new UnitDieBState(this, (*j), DT_HE, true)); 
				}
				else
				{
					if (terrainExplosion)
					{
						// terrain explosion
						statePushNext(new UnitDieBState(this, (*j), DT_HE, false));
						if (Options::getBool("battleNotifyDeath") && (*j)->getFaction() == FACTION_PLAYER && (*j)->getOriginalFaction() == FACTION_PLAYER)
						{
							std::wstringstream ss;
							ss << (*j)->getName(_parentState->getGame()->getLanguage()) << L'\n' << _parentState->getGame()->getLanguage()->getString("STR_HAS_BEEN_KILLED");
							_parentState->getGame()->pushState(new InfoboxState(_parentState->getGame(), ss.str()));
						}
					}
					else
					{
						// no murderer, and no terrain explosion, must be fatal wounds
						statePushNext(new UnitDieBState(this, (*j), DT_AP, false));  // STR_HAS_DIED_FROM_A_FATAL_WOUND
						// show a little infobox with the name of the unit and "... is panicking"
						_infoboxQueue.push_back(new InfoboxOKState(_parentState->getGame(), (*j)->getName(_parentState->getGame()->getLanguage()), "STR_HAS_DIED_FROM_A_FATAL_WOUND"));
					}
				}
			}
		}
		else if ((*j)->getStunlevel() >= (*j)->getHealth() && (*j)->getStatus() != STATUS_DEAD && (*j)->getStatus() != STATUS_UNCONSCIOUS && (*j)->getStatus() != STATUS_COLLAPSING && (*j)->getStatus() != STATUS_TURNING)
		{
			statePushNext(new UnitDieBState(this, (*j), DT_STUN, true));
			if ((*j)->getFaction() == FACTION_PLAYER)
			{
				_infoboxQueue.push_back(new InfoboxOKState(_parentState->getGame(), (*j)->getName(_parentState->getGame()->getLanguage()), "STR_HAS_BECOME_UNCONSCIOUS"));
			}
		}
	}
}

/**
 * Shows the infoboxes in the queue (if any).
 */
void BattlescapeGame::showInfoBoxQueue()
{
	for (std::vector<InfoboxOKState*>::iterator i = _infoboxQueue.begin(); i != _infoboxQueue.end(); ++i)
	{
		_parentState->getGame()->pushState(*i);
	}

	_infoboxQueue.clear();
}

/**
 * Handles the result of non target actions, like priming a grenade.
 */
void BattlescapeGame::handleNonTargetAction()
{
	if (!_currentAction.targeting)
	{
		if (_currentAction.type == BA_PRIME && _currentAction.value > -1)
		{
			if (_currentAction.actor->spendTimeUnits(_currentAction.TU))
			{
				_currentAction.weapon->setExplodeTurn(_save->getTurn() + _currentAction.value);
			}
			else
			{
				_parentState->warning("STR_NOT_ENOUGH_TIME_UNITS");
			}
		}
		if (_currentAction.type == BA_USE || _currentAction.type == BA_LAUNCH)
		{
			if (_currentAction.result.length() > 0)
			{
				_parentState->warning(_currentAction.result);
				_currentAction.result = "";
			}
			_save->reviveUnconsciousUnits();
		}
		if (_currentAction.type == BA_HIT)
		{
			if (_currentAction.result.length() > 0)
			{
				_parentState->warning(_currentAction.result);
				_currentAction.result = "";
			}
			else
			{
				if (_currentAction.actor->spendTimeUnits(_currentAction.TU))
				{
					Position p;
					Pathfinding::directionToVector(_currentAction.actor->getDirection(), &p);
					Tile * tile (_save->getTile(_currentAction.actor->getPosition() + p));
					for (int x = 0; x != _currentAction.actor->getArmor()->getSize(); ++x)
					{
						for (int y = 0; y != _currentAction.actor->getArmor()->getSize(); ++y)
						{
							tile = _save->getTile(Position(_currentAction.actor->getPosition().x + x, _currentAction.actor->getPosition().y + y, _currentAction.actor->getPosition().z) + p);
							if (tile->getUnit() && tile->getUnit() != _currentAction.actor)
							{
								Position voxel = Position(tile->getPosition().x*16,tile->getPosition().y*16,tile->getPosition().z*24);
								voxel.x += 8;voxel.y += 8;voxel.z += 8;
								statePushNext(new ExplosionBState(this, voxel, _currentAction.weapon, _currentAction.actor));
								break;
							}
						}
						if (tile->getUnit() && tile->getUnit() != _currentAction.actor)
							break;
					}
				}
				else
				{
					_parentState->warning("STR_NOT_ENOUGH_TIME_UNITS");
				}
			}
		}
		_currentAction.type = BA_NONE;
		_parentState->updateSoldierInfo();
	}

	setupCursor();
}

/**
 * Set the cursor according to the selected action.
 */
void BattlescapeGame::setupCursor()
{
	if (_currentAction.targeting)
	{
		if (_currentAction.type == BA_THROW)
		{
			getMap()->setCursorType(CT_THROW);
		}
		else if (_currentAction.type == BA_MINDCONTROL || _currentAction.type == BA_PANIC || _currentAction.type == BA_USE)
		{
			getMap()->setCursorType(CT_PSI);
		}
		else if (_currentAction.type == BA_LAUNCH)
		{
			getMap()->setCursorType(CT_WAYPOINT);
		}
		else
		{
			getMap()->setCursorType(CT_AIM);
		}
	}
	else
	{
		_currentAction.actor = _save->getSelectedUnit();
		if (_currentAction.actor)
		{
			getMap()->setCursorType(CT_NORMAL, _currentAction.actor->getArmor()->getSize());
		}
	}
}

/**
 * Whether a playable unit is selected. Normally only player side units can be selected, but in debug mode one can play with aliens too :)
 * Is used to see if stats can be displayed and action buttons will work.
 * @return whether a playable unit is selected.
 */
bool BattlescapeGame::playableUnitSelected()
{
	return _save->getSelectedUnit() != 0 && (_save->getSide() == FACTION_PLAYER || _save->getDebugMode());
}

/**
 * Give time slice to the front state.
 */
void BattlescapeGame::handleState()
{
	if (!_states.empty())
	{
		// end turn request?
		if (_states.front() == 0)
		{
			_states.pop_front();
			endTurn();
			return;
		}
		else
		{
			_states.front()->think();
		}
		getMap()->draw(); // redraw map
	}
}

/**
 * Push a state at the front of the queue and start it.
 * @param bs Battlestate.
 */
void BattlescapeGame::statePushFront(BattleState *bs)
{
	_states.push_front(bs);
	bs->init();
}

/**
 * Push a state as the next state after the current one.
 * @param bs Battlestate.
 */
void BattlescapeGame::statePushNext(BattleState *bs)
{
	if (_states.empty())
	{
		_states.push_front(bs);
		bs->init();
	}
	else
	{
		_states.insert(++_states.begin(), bs);
	}

}

/**
 * Push a state at the back.
 * @param bs Battlestate.
 */
void BattlescapeGame::statePushBack(BattleState *bs)
{
	if (_states.empty())
	{
		_states.push_front(bs);
		// end turn request?
		if (_states.front() == 0)
		{
			_states.pop_front();
			endTurn();
			return;
		}
		else
		{
			bs->init();
		}
	}
	else
	{
		_states.push_back(bs);
	}
}

/**
 * This is a very important function. It is called by a BattleState (walking, projectile is flying, explosions,...) at the moment this state has finished it's action.
 * Here we check the result of that action and do all the aftermath.
 * The state is popped off the list.
 */
void BattlescapeGame::popState()
{
	bool actionFailed = false;

	if (_states.empty()) return;

	BattleAction action = _states.front()->getAction();

	if (action.result.length() > 0 && action.actor->getFaction() == FACTION_PLAYER && _playerPanicHandled && (_save->getSide() == FACTION_PLAYER || _debugPlay))
	{
		_parentState->warning(action.result);
		actionFailed = true;
	}
	_states.pop_front();

	// handle the end of this unit's actions
	if (action.actor && noActionsPending(action.actor))
	{
		if (action.actor->getFaction() == FACTION_PLAYER)
		{
			// spend TUs of "target triggered actions" (shooting, throwing) only
			// the other actions' TUs (healing,scanning,..) are already take care of
			if (action.targeting && _save->getSelectedUnit() && !actionFailed)
			{
				action.actor->spendTimeUnits(action.TU);
			}
			if (_save->getSide() == FACTION_PLAYER)
			{
				// after throwing the cursor returns to default cursor, after shooting it stays in targeting mode and the player can shoot again in the same mode (autoshot,snap,aimed)
				if ((action.type == BA_THROW || action.type == BA_LAUNCH) && !actionFailed)
				{
					// clean up the waypoints
					if (action.type == BA_LAUNCH)
					{
						_currentAction.waypoints.clear();
					}

					cancelCurrentAction(true);
				}
				_parentState->getGame()->getCursor()->setVisible(true);
				setupCursor();
			}
		}
		else
		{
			// spend TUs
			action.actor->spendTimeUnits(action.TU);
			if (_save->getSide() != FACTION_PLAYER && !_debugPlay)
			{
				 // AI does two things per unit + hides?, before switching to the next, or it got killed before doing the second thing
				if (_AIActionCounter > 3 || _save->getSelectedUnit() == 0 || _save->getSelectedUnit()->isOut())
				{
					if (_save->getSelectedUnit())
					{
						_save->getSelectedUnit()->setCache(0);
						getMap()->cacheUnit(_save->getSelectedUnit());
					}
					_AIActionCounter = 0;
					if (_save->selectNextPlayerUnit(true, true) == 0)
					{
						if (!_save->getDebugMode())
						{
							statePushBack(0); // end AI turn
						}
						else
						{
							_save->selectNextPlayerUnit();
							_debugPlay = true;
						}
					}
					if (_save->getSelectedUnit())
					{
						getMap()->getCamera()->centerOnPosition(_save->getSelectedUnit()->getPosition());
					}
				}
			}
			else if (_debugPlay)
			{
				_parentState->getGame()->getCursor()->setVisible(true);
				setupCursor();
			}
		}
	}
	
	if (!_states.empty())
	{
		// end turn request?
		if (_states.front() == 0)
		{
			_states.pop_front();
			endTurn();
			return;
		}
		// init the next state in queue
		_states.front()->init();
	}

	// the currently selected unit died or became unconscious or disappeared inexplicably
	if (_save->getSelectedUnit() == 0 || _save->getSelectedUnit()->isOut())
	{
		cancelCurrentAction();
		getMap()->setCursorType(CT_NORMAL, 1);
		_parentState->getGame()->getCursor()->setVisible(true);
		_save->setSelectedUnit(0);
	}
	_parentState->updateSoldierInfo();

	// the unit became unconscious - show popup
	if (action.actor && action.actor->getStatus() == STATUS_UNCONSCIOUS)
	{
		std::wstringstream ss;
		ss << action.actor->getName(_parentState->getGame()->getLanguage()) << L'\n' << _parentState->getGame()->getLanguage()->getString("STR_HAS_BECOME_UNCONSCIOUS");
		_parentState->getGame()->pushState(new InfoboxState(_parentState->getGame(), ss.str()));
	}
	

}

bool BattlescapeGame::noActionsPending(BattleUnit *bu)
{
	if (_states.empty()) return true;

	for (std::list<BattleState*>::iterator i = _states.begin(); i != _states.end(); ++i)
	{
		if ((*i) != 0 && (*i)->getAction().actor == bu)
			return false;
	}

	return true;
}
/**
 * Sets the timer interval for think() calls of the state.
 * @param interval An interval in ms.
 */
void BattlescapeGame::setStateInterval(Uint32 interval)
{
	_parentState->setStateInterval(interval);
}


/**
 * Check against reserved time units.
 * @param bu Pointer to the unit.
 * @param tu Number of time units to check.
 * @return bool Whether or not we got enough time units.
 */
bool BattlescapeGame::checkReservedTU(BattleUnit *bu, int tu)
{
    BattleActionType effectiveTuReserved = _tuReserved; // avoid changing _tuReserved in this method

	if (_save->getSide() != FACTION_PLAYER) return true; // aliens don't reserve TUs

	// check TUs against slowest weapon if we have two weapons
	BattleItem *slowestWeapon = bu->getMainHandWeapon(false);
	// if the weapon has no autoshot, reserve TUs for snapshot
	if (bu->getActionTUs(_tuReserved, slowestWeapon) == 0 && _tuReserved == BA_AUTOSHOT)
	{
		effectiveTuReserved = BA_SNAPSHOT;
	}

	if (effectiveTuReserved != BA_NONE &&
		tu + bu->getActionTUs(effectiveTuReserved, slowestWeapon) > bu->getTimeUnits() &&
		bu->getActionTUs(effectiveTuReserved, slowestWeapon) <= bu->getTimeUnits())
	{
		switch (effectiveTuReserved)
		{
		case BA_SNAPSHOT: _parentState->warning("STR_TIME_UNITS_RESERVED_FOR_SNAP_SHOT"); break;
		case BA_AUTOSHOT: _parentState->warning("STR_TIME_UNITS_RESERVED_FOR_AUTO_SHOT"); break;
		case BA_AIMEDSHOT: _parentState->warning("STR_TIME_UNITS_RESERVED_FOR_AIMED_SHOT"); break;
		default: ;
		}
		return false;
	}

	return true;
}



/**
 * Pick the first soldier from the list in status panic.
 * @return True when all panicking is over.
 */
bool BattlescapeGame::handlePanickingPlayer()
{
	for (std::vector<BattleUnit*>::iterator j = _save->getUnits()->begin(); j != _save->getUnits()->end(); ++j)
	{
		if ((*j)->getFaction() == FACTION_PLAYER && handlePanickingUnit(*j))
			return false;
	}
	return true;
}

/**
 * Common function for panicking units.
 * @return False when unit not in panicking mode.
 */
bool BattlescapeGame::handlePanickingUnit(BattleUnit *unit)
{
	UnitStatus status = unit->getStatus();
	if (status != STATUS_PANICKING && status != STATUS_BERSERK) return false;
	unit->setVisible(true);
	getMap()->getCamera()->centerOnPosition(unit->getPosition());
	_save->setSelectedUnit(unit);

	// show a little infobox with the name of the unit and "... is panicking"
	std::wstringstream ss;
	ss << unit->getName(_parentState->getGame()->getLanguage()) << L'\n' << _parentState->getGame()->getLanguage()->getString(status==STATUS_PANICKING?"STR_HAS_PANICKED":"STR_HAS_GONE_BERSERK");
	_parentState->getGame()->pushState(new InfoboxState(_parentState->getGame(), ss.str()));

	unit->abortTurn(); //makes the unit go to status STANDING :p

	int flee = RNG::generate(0,100);
	BattleAction ba;
	ba.actor = unit;
	switch (status)
	{
	case STATUS_PANICKING: // 1/2 chance to freeze and 1/2 chance try to flee
		if (flee <= 50)
		{
			BattleItem *item = unit->getItem("STR_RIGHT_HAND");
			if (item)
			{
				dropItem(unit->getPosition(), item, false, true);
			}
			item = unit->getItem("STR_LEFT_HAND");
			if (item)
			{
				dropItem(unit->getPosition(), item, false, true);
			}
			unit->setCache(0);
			ba.target = Position(unit->getPosition().x + RNG::generate(-5,5), unit->getPosition().y + RNG::generate(-5,5), unit->getPosition().z);
			if (_save->getTile(ba.target)) // only walk towards it when the place exists
			{
				_save->getPathfinding()->calculate(ba.actor, ba.target);
				statePushBack(new UnitWalkBState(this, ba));
			}
		}
		break;
	case STATUS_BERSERK: // berserk - do some weird turning around and then aggro towards an enemy unit or shoot towards random place
		for (int i= 0; i < 4; i++)
		{
			ba.target = Position(unit->getPosition().x + RNG::generate(-5,5), unit->getPosition().y + RNG::generate(-5,5), unit->getPosition().z);
			statePushBack(new UnitTurnBState(this, ba));
		}
		for (std::vector<BattleUnit*>::iterator j = unit->getVisibleUnits()->begin(); j != unit->getVisibleUnits()->end(); ++j)
		{
			ba.target = (*j)->getPosition();
			statePushBack(new UnitTurnBState(this, ba));
		}
		if (_save->getTile(ba.target) != 0)
		{
			ba.weapon = unit->getMainHandWeapon();
			if(ba.weapon)
			{
				if (ba.weapon->getRules()->getBattleType() == BT_FIREARM)
				{
					ba.type = BA_SNAPSHOT;
					int tu = ba.actor->getActionTUs(ba.type, ba.weapon);
					for (int i= 0; i < 10; i++)
					{
						// fire shots until unit runs out of TUs
						if (!ba.actor->spendTimeUnits(tu))
							break;
						statePushBack(new ProjectileFlyBState(this, ba));
					}
				}
				else if (ba.weapon->getRules()->getBattleType() == BT_GRENADE)
				{
					if (ba.weapon->getExplodeTurn() == 0)
					{
						ba.weapon->setExplodeTurn(_save->getTurn());
					}
					ba.type = BA_THROW;
					statePushBack(new ProjectileFlyBState(this, ba));
				}
			}
		}
		// Add some time units for the turning
		unit->setTimeUnits(15);
		ba.type = BA_NONE;
		break;
	default: break;
	}
	// Time units can only be reset after everything else occurs
	statePushBack(new UnitPanicBState(this, ba.actor));
	unit->moraleChange(+15);

	return true;
}

/**
  * This will cancel the current action the user had selected (firing, throwing,..)
  * @return Whether an action was cancelled or not.
  */
bool BattlescapeGame::cancelCurrentAction(bool bForce)
{
	bool bPreviewed = Options::getBool("battlePreviewPath");

	if (_save->getPathfinding()->removePreview() && bPreviewed) return true;

	if (_states.empty() || bForce)
	{
		if (_currentAction.targeting)
		{
			if (_currentAction.type == BA_LAUNCH && !_currentAction.waypoints.empty())
			{
				_currentAction.waypoints.pop_back();
				if (!getMap()->getWaypoints()->empty())
				{
					getMap()->getWaypoints()->pop_back();
				}
				if (_currentAction.waypoints.empty())
				{
					_parentState->showLaunchButton(false);
				}
				return true;
			}
			else
			{
				_currentAction.targeting = false;
				_currentAction.type = BA_NONE;
				setupCursor();
				_parentState->getGame()->getCursor()->setVisible(true);
				return true;
			}
		}
	}
	else
	{
		_states.front()->cancel();
		return true;
	}

	return false;
}
/**
 * Get pointer to access action members directly.
 * @return pointer to action.
 */
BattleAction *BattlescapeGame::getCurrentAction()
{
	return &_currentAction;
}

/**
 * Is there currently an action going on.
 * @return true or false.
 */
bool BattlescapeGame::isBusy()
{
	return !_states.empty();
}

/**
 * Activate primary action (left click)
 * @param pos Position on the map.
 */
void BattlescapeGame::primaryAction(const Position &pos)
{
	bool bPreviewed = Options::getBool("battlePreviewPath");

	if (_currentAction.targeting && _save->getSelectedUnit())
	{
		if (_currentAction.type == BA_LAUNCH)
		{
			_parentState->showLaunchButton(true);
			_currentAction.waypoints.push_back(pos);
			getMap()->getWaypoints()->push_back(pos);
		}
		else if (_currentAction.type == BA_USE && _currentAction.weapon->getRules()->getBattleType() == BT_MINDPROBE)
		{
			if (_save->selectUnit(pos) && _save->selectUnit(pos)->getFaction() != _save->getSelectedUnit()->getFaction())
			{
				if (_currentAction.actor->spendTimeUnits(_currentAction.TU, false))
				{
					_parentState->getGame()->getResourcePack()->getSound("BATTLE.CAT", _currentAction.weapon->getRules()->getHitSound())->play();
					_parentState->getGame()->pushState (new UnitInfoState (_parentState->getGame(), _save->selectUnit(pos)));
					cancelCurrentAction();
				}
				else
				{
					_parentState->warning("STR_NOT_ENOUGH_TIME_UNITS");
				}
			}
		}
		else if (_currentAction.type == BA_PANIC || _currentAction.type == BA_MINDCONTROL)
		{
			if (_save->selectUnit(pos) && _save->selectUnit(pos)->getFaction() != _save->getSelectedUnit()->getFaction() && _save->selectUnit(pos)->getVisible())
			{
				bool builtinpsi = !_currentAction.weapon;
				if (builtinpsi)
				{
					_currentAction.weapon = new BattleItem(_parentState->getGame()->getRuleset()->getItem("ALIEN_PSI_WEAPON"), _save->getCurrentItemId());
				}
				_currentAction.TU = _currentAction.weapon->getRules()->getTUUse();
				_currentAction.target = pos;
				// get the sound/animation started
				getMap()->setCursorType(CT_NONE);
				_parentState->getGame()->getCursor()->setVisible(false);
				_currentAction.cameraPosition = getMap()->getCamera()->getMapOffset();
				statePushBack(new ProjectileFlyBState(this, _currentAction));
				if (_currentAction.TU <= _currentAction.actor->getTimeUnits())
				{
					if (getTileEngine()->psiAttack(&_currentAction))
					{
						// show a little infobox if it's successful
						std::wstringstream ss;
						if (_currentAction.type == BA_PANIC)
						{
							ss << _save->getTile(_currentAction.target)->getUnit()->getName(_parentState->getGame()->getLanguage()) << L'\n' << _parentState->getGame()->getLanguage()->getString("STR_HAS_PANICKED");
						}
						else if (_currentAction.type == BA_MINDCONTROL)
						{
							ss << _parentState->getGame()->getLanguage()->getString("STR_MIND_CONTROL_SUCCESSFUL");
						}
						_parentState->getGame()->pushState(new InfoboxState(_parentState->getGame(), ss.str()));
						_parentState->updateSoldierInfo();
						_currentAction.targeting = false;
						_currentAction.type = BA_NONE;
						setupCursor();
					}
					if (builtinpsi)
					{
						_save->removeItem(_currentAction.weapon);
						_currentAction.weapon = 0;
					}
				}
			}
		}
		else
		{
			_currentAction.target = pos;
			getMap()->setCursorType(CT_NONE);
			_parentState->getGame()->getCursor()->setVisible(false);
			_currentAction.cameraPosition = getMap()->getCamera()->getMapOffset();
			_states.push_back(new ProjectileFlyBState(this, _currentAction));
			statePushFront(new UnitTurnBState(this, _currentAction)); // first of all turn towards the target
		}
	}
	else
	{
		_currentAction.actor = _save->getSelectedUnit();
		BattleUnit *unit = _save->selectUnit(pos);
		if (unit && unit != _save->getSelectedUnit() && (unit->getVisible() || _debugPlay))
		{
		//  -= select unit =-
			if (unit->getFaction() == _save->getSide())
			{
				_save->setSelectedUnit(unit);
				_parentState->updateSoldierInfo();
				cancelCurrentAction();
				setupCursor();
				_currentAction.actor = unit;
			}
		}
		else if (playableUnitSelected())
		{

			if (_currentAction.target != pos && bPreviewed)
				_save->getPathfinding()->removePreview();
			_currentAction.run = _save->getStrafeSetting() && Game::getShiftKeyDown() && _save->getSelectedUnit()->getTurretType() == -1;
			_currentAction.strafe = !_currentAction.run && _save->getStrafeSetting() && Game::getCtrlKeyDown() && _save->getSelectedUnit()->getTurretType() == -1;
			_currentAction.target = pos;
			_save->getPathfinding()->calculate(_currentAction.actor, _currentAction.target);
			if (bPreviewed && !_save->getPathfinding()->previewPath())
			{
				_save->getPathfinding()->removePreview();
				bPreviewed = false;
			}

			if (!bPreviewed)
			{
				//  -= start walking =-
				getMap()->setCursorType(CT_NONE);
				_parentState->getGame()->getCursor()->setVisible(false);
				if (_save->getSelectedUnit()->isKneeled())
				{
					kneel(_save->getSelectedUnit());
				}
				statePushBack(new UnitWalkBState(this, _currentAction));
			}
		}
	}
}

/**
 * Activate secondary action (right click)
 * @param pos Position on the map.
 */
void BattlescapeGame::secondaryAction(const Position &pos)
{
	//  -= turn to or open door =-
	_currentAction.target = pos;
	_currentAction.actor = _save->getSelectedUnit();
	_currentAction.strafe = _save->getStrafeSetting() && Game::getCtrlKeyDown() && _save->getSelectedUnit()->getTurretType() > -1;
	statePushBack(new UnitTurnBState(this, _currentAction));
}

/**
 * Pressed the blaster launcher button.
 */
void BattlescapeGame::launchAction()
{
	_parentState->showLaunchButton(false);
	getMap()->getWaypoints()->clear();
	_currentAction.target = _currentAction.waypoints.front();
	getMap()->setCursorType(CT_NONE);
	_parentState->getGame()->getCursor()->setVisible(false);
	_currentAction.cameraPosition = getMap()->getCamera()->getMapOffset();
	_states.push_back(new ProjectileFlyBState(this, _currentAction));
	statePushFront(new UnitTurnBState(this, _currentAction)); // first of all turn towards the target
}

/**
 * Pressed the psi button.
 */
void BattlescapeGame::psiAction()
{
	_currentAction.weapon = 0;
	_currentAction.targeting = true;
	_currentAction.type = BA_PANIC;
	_currentAction.TU = 25;
	setupCursor();
}


/**
 * Move a unit up or down.
 * @param unit The unit.
 * @param dir Direction DIR_UP or DIR_DOWN
 */
void BattlescapeGame::moveUpDown(BattleUnit *unit, int dir)
{
	_currentAction.target = unit->getPosition();
	if (dir == Pathfinding::DIR_UP)
	{
		_currentAction.target.z++;
	}
	else
	{
		_currentAction.target.z--;
	}
	getMap()->setCursorType(CT_NONE);
	_parentState->getGame()->getCursor()->setVisible(false);
	if (_save->getSelectedUnit()->isKneeled())
	{
		kneel(_save->getSelectedUnit());
	}
	_save->getPathfinding()->calculate(_currentAction.actor, _currentAction.target);
	statePushBack(new UnitWalkBState(this, _currentAction));
}

/**
 * Request of the end of the turn (wait for explosions etc to really end the turn)
 */
void BattlescapeGame::requestEndTurn()
{
	cancelCurrentAction();
	statePushBack(0);
}

/**
 * Set the TU reserved type.
 * @param tur A battleactiontype
 */
void BattlescapeGame::setTUReserved(BattleActionType tur)
{
	_tuReserved = tur;
}

/**
 * Drop item to the floor & affect with gravity.
 * @param position Position to spawn the item.
 * @param item Pointer to the item.
 * @param newItem Bool whether this is a new item.
 * @param removeItem Bool whether to remove the item from owner.
 */
void BattlescapeGame::dropItem(const Position &position, BattleItem *item, bool newItem, bool removeItem)
{
	Position p = position;

	// don't spawn anything outside of bounds
	if (_save->getTile(p) == 0)
		return;

	// don't ever drop fixed items
	if (item->getRules()->isFixed())
		return;

	_save->getTile(p)->addItem(item, getRuleset()->getInventory("STR_GROUND"));

	if(newItem)
	{
		_save->getItems()->push_back(item);
	}

	if (removeItem)
	{
		item->moveToOwner(0);
	}
	else
	{
		item->setOwner(0);
	}

	getTileEngine()->applyItemGravity(_save->getTile(p));

	if (item->getRules()->getBattleType() == BT_FLARE)
	{
		getTileEngine()->calculateTerrainLighting();
	}

}

/*
 * Convert a unit into a unit of another type.
 */
BattleUnit *BattlescapeGame::convertUnit(BattleUnit *unit, std::string newType)
{
	// in case the unit was unconscious
	getSave()->removeUnconsciousBodyItem(unit);
	unit->instaKill();
	for (std::vector<BattleItem*>::iterator i = unit->getInventory()->begin(); i != unit->getInventory()->end(); ++i)
	{
		dropItem(unit->getPosition(), (*i));
	}

	unit->getInventory()->clear();

	// remove unit-tile link
	unit->setTile(0);

	getSave()->getTile(unit->getPosition())->setUnit(0);
	std::stringstream newArmor;
	newArmor << getRuleset()->getUnit(newType)->getArmor();
	std::string terroristWeapon = getRuleset()->getUnit(newType)->getRace().substr(4);
	terroristWeapon += "_WEAPON";
	RuleItem *newItem = getRuleset()->getItem(terroristWeapon);

	BattleUnit *_newUnit = new BattleUnit(getRuleset()->getUnit(newType), FACTION_HOSTILE, _save->getUnits()->back()->getId() + 1, getRuleset()->getArmor(newArmor.str()));

	getSave()->getTile(unit->getPosition())->setUnit(_newUnit, _save->getTile(unit->getPosition() + Position(0,0,-1)));
	_newUnit->setPosition(unit->getPosition());
	_newUnit->setDirection(3);
	_newUnit->setCache(0);
	getSave()->getUnits()->push_back(_newUnit);
	getMap()->cacheUnit(_newUnit);
	_newUnit->setAIState(new PatrolBAIState(getSave(), _newUnit, 0));
	BattleItem *bi = new BattleItem(newItem, getSave()->getCurrentItemId());
	bi->moveToOwner(_newUnit);
	bi->setSlot(getRuleset()->getInventory("STR_RIGHT_HAND"));
	getSave()->getItems()->push_back(bi);

	return _newUnit;

}


/**
 * Get map
 */
Map *BattlescapeGame::getMap()
{
	return _parentState->getMap();
}
/**
 * Get save
 */
SavedBattleGame *BattlescapeGame::getSave()
{
	return _save;
}
/**
 * Get tilengine
 */
TileEngine *BattlescapeGame::getTileEngine()
{
	return _save->getTileEngine();
}
/**
 * Get pathfinding
 */
Pathfinding *BattlescapeGame::getPathfinding()
{
	return _save->getPathfinding();
}
/**
 * Get resourcepack
 */
ResourcePack *BattlescapeGame::getResourcePack()
{
	return _parentState->getGame()->getResourcePack();
}
/**
 * Get ruleset
 */
const Ruleset *BattlescapeGame::getRuleset() const
{
	return _parentState->getGame()->getRuleset();
}


/** 
 * Evaluate the situation for ease of AI decisions in the following turn.
 */
void BattlescapeGame::resetSituationForAI()
{
    int w = _save->getWidth();
    int h = _save->getHeight();
    int l = _save->getLength();

    Tile **tiles = _save->getTiles();

    // Log(LOG_INFO) << w*h*l << " tiles!";

    for (int i = 0; i < w * l * h; ++i)
    {
       tiles[i]->soldiersVisible = -1;    // -1 for "not calculated"; actual calculations will take place as needed
       tiles[i]->closestSoldierDSqr = -1; // for most of the tiles most of the time, this data is not needed
    }
}


}
