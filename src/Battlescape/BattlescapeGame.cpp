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
#include "InventoryState.h"
#include "AggroBAIState.h"
#include "PatrolBAIState.h"
#include "Pathfinding.h"
#include "../Engine/Game.h"
#include "../Engine/Music.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Screen.h"
#include "../Engine/SoundSet.h"
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
#include "../Ruleset/RuleArmor.h"
#include "../Engine/Options.h"
#include "WarningMessage.h"
#include "BattlescapeOptionsState.h"
#include "DebriefingState.h"
#include "../Engine/RNG.h"
#include "InfoboxState.h"
#include "InfoboxOKState.h"
#include "MiniMapState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Battlescape screen.
 * @param save Pointer to the save game.
 * @param parentState Pointer to the parent battlescape state.
 */
BattlescapeGame::BattlescapeGame(SavedBattleGame *save, BattlescapeState *parentState) : _save(save), _parentState(parentState)
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
	BattleAIState *ai = unit->getCurrentAIState();
	if (!ai)
	{
		// for some reason the unit had no AI routine assigned..
		unit->setAIState(new PatrolBAIState(_save, unit, 0));
		ai = unit->getCurrentAIState();
	}

	_AIActionCounter++;

	AggroBAIState *aggro = dynamic_cast<AggroBAIState*>(ai);
	PatrolBAIState *patrol = dynamic_cast<PatrolBAIState*>(ai);
	
	BattleAction action;
	unit->think(&action);
	if (action.type == BA_WALK)
	{
 		_save->getPathfinding()->calculate(action.actor, action.target);
		statePushBack(new UnitWalkBState(this, action));
	}

	if (action.type == BA_SNAPSHOT || action.type == BA_AUTOSHOT || action.type == BA_THROW)
	{
		statePushBack(new ProjectileFlyBState(this, action));
	}

	if (action.type == BA_NONE)
	{
		_AIActionCounter = 0;
		if (aggro != 0)
		{
			// we lost aggro
			unit->setAIState(new PatrolBAIState(_save, unit, 0));
		}
		if (_save->selectNextPlayerUnit(true) == 0)
		{
			if (!_save->getDebugMode())
			{
				statePushBack(0); // end AI turn
			}
			else
			{
				_save->selectNextPlayerUnit(false);
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
		if (bu->spendTimeUnits(tu, _save->getDebugMode()))
		{
			bu->kneel(!bu->isKneeled());
			// kneeling or standing up can reveil new terrain or units. I guess.
			getTileEngine()->calculateFOV(bu);
			getMap()->cacheUnits();
			_parentState->updateSoldierInfo();
			BattleAction action;
			if (getTileEngine()->checkReactionFire(bu, &action, 0, false))
			{
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
				p.z = _save->getTiles()[i]->getPosition().z*24 + _save->getTiles()[i]->getTerrainLevel();
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
		getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(21)->play(); // ufo door closed
	}

	_save->endTurn();
	bool bBattleIsOver = checkForCasualties(0, 0, false, false);
	if (bBattleIsOver)
	{
		return;
	}

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
	}

	bBattleIsOver = checkForCasualties(0, 0, false, true);
	if (bBattleIsOver)
	{
		return;
	}

	_parentState->updateSoldierInfo();

	if (playableUnitSelected())
	{
		getMap()->getCamera()->centerOnPosition(_save->getSelectedUnit()->getPosition());
		setupCursor();
	}

	_parentState->getGame()->pushState(new NextTurnState(_parentState->getGame(), _save, _parentState));

}


/**
 * Checks for casualties and adjusts morale accordingly.
 * @param murderweapon
 * @param murderer
 * @return Whether the battle is finished.
 */
bool BattlescapeGame::checkForCasualties(BattleItem *murderweapon, BattleUnit *murderer, bool hiddenExplosion, bool terrainExplosion)
{
	for (std::vector<BattleUnit*>::iterator j = _save->getUnits()->begin(); j != _save->getUnits()->end(); ++j)
	{
		if ((*j)->getHealth() == 0 && (*j)->getStatus() != STATUS_DEAD && (*j)->getStatus() != STATUS_FALLING)
		{
			BattleUnit *victim = (*j);

			if (murderer)
			{
				// if there is a known murderer, he will get a morale bonus if he is of a different faction (what with neutral?)
				if ((victim->getFaction() == FACTION_PLAYER && murderer->getFaction() == FACTION_HOSTILE) ||
					(victim->getFaction() == FACTION_HOSTILE && murderer->getFaction() == FACTION_PLAYER))
				{
					murderer->moraleChange(+20);
					Soldier *s = dynamic_cast<Soldier*>(murderer->getUnit());
					if (s != 0)
					{
						s->addKillCount();
					}
				}
				// murderer will get a penalty with friendly fire
				if (victim->getFaction() == murderer->getFaction())
				{
					murderer->moraleChange(-20);
				}
			}

			for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
			{
				// the losing squad all get a morale loss
				if ((*i)->getFaction() == victim->getFaction())
				{
					(*i)->moraleChange(-(22 - ((*i)->getUnit()->getBravery() / 10)*2));

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
								int d = _save->getTileEngine()->distance(victim->getPosition(), (*h)->getPosition());
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
						if (revenger && (revenger->getUnit()->getAggression() == 2 || (revenger->getUnit()->getAggression() == 1 && revenge)))
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
				if ((*i)->getFaction() != victim->getFaction())
				{
					(*i)->moraleChange(+10);
				}
			}

			if (murderweapon)
			{
				statePushNext(new UnitDieBState(this, (*j), murderweapon->getRules()->getDamageType(), false));
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
					}
					else
					{
						// no murderer, and no terrain explosion, must be fatal wounds
						statePushNext(new UnitDieBState(this, (*j), DT_AP, false));  // STR_HAS_DIED_FROM_A_FATAL_WOUND
						// show a little infobox with the name of the unit and "... is panicking"
						_parentState->getGame()->pushState(new InfoboxOKState(_parentState->getGame(), (*j)->getUnit()->getName(_parentState->getGame()->getLanguage()), "STR_HAS_DIED_FROM_A_FATAL_WOUND"));
					}
				}
			}
		}
		else if ((*j)->getStunlevel() >= (*j)->getHealth() && (*j)->getStatus() != STATUS_DEAD && (*j)->getStatus() != STATUS_UNCONSCIOUS && (*j)->getStatus() != STATUS_FALLING && (*j)->getStatus() != STATUS_TURNING)
		{
			
			if (!murderer)
			{
				// fell unconscious from stun level
				statePushNext(new UnitDieBState(this, (*j), DT_STUN, true));  // STR_HAS_BECOME_UNCONSCIOUS
				// show a little infobox with the name of the unit and "... is panicking"
				_parentState->getGame()->pushState(new InfoboxOKState(_parentState->getGame(), (*j)->getUnit()->getName(_parentState->getGame()->getLanguage()), "STR_HAS_BECOME_UNCONSCIOUS"));
			}
			else
			{
				statePushNext(new UnitDieBState(this, (*j), DT_STUN, true));
			}
		}
	}

	return false;
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
			if (_currentAction.actor->spendTimeUnits(_currentAction.TU, dontSpendTUs()))
			{
				_currentAction.weapon->setExplodeTurn(_save->getTurn() + _currentAction.value);
			}
			else
			{
				_parentState->warning("STR_NOT_ENOUGH_TIME_UNITS");
			}
		}
		if (_currentAction.type == BA_USE)
		{
			if (_currentAction.result.length() > 0)
			{
				_parentState->warning(_currentAction.result);
				_currentAction.result = "";
			}
			_save->reviveUnconsciousUnits();
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
			getMap()->setCursorType(CT_NORMAL, _currentAction.actor->getUnit()->getArmor()->getSize());
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
		_states.front()->think();
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

	if (action.result.length() > 0 && action.actor->getFaction() == FACTION_PLAYER && _playerPanicHandled)
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
			// spend TUs of "target triggered actions" (shooting, throwing)
			if (action.targeting && _save->getSelectedUnit() && !actionFailed)
			{
				action.actor->spendTimeUnits(action.TU, dontSpendTUs());
			}
			if (_save->getSide() == FACTION_PLAYER)
			{
				// after throwing the cursor returns to default cursor, after shooting it stays in targeting mode and the player can shoot again in the same mode (autoshot,snap,aimed)
				if (action.type == BA_THROW && !actionFailed)
				{
					cancelCurrentAction();
				}
				_parentState->getGame()->getCursor()->setVisible(true);
				setupCursor();
			}
		}
		else
		{
			if (_save->getSide() != FACTION_PLAYER && !_debugPlay)
			{
				action.actor->spendTimeUnits(action.TU, false);
				 // AI does two things per unit, before switching to the next, or it got killed before doing the second thing
				if (_AIActionCounter > 1 || _save->getSelectedUnit() == 0 || _save->getSelectedUnit()->isOut())
				{
					_AIActionCounter = 0;
					if (_save->selectNextPlayerUnit(true) == 0)
					{
						if (!_save->getDebugMode())
						{
							statePushBack(0); // end AI turn
						}
						else
						{
							_save->selectNextPlayerUnit(false);
							_debugPlay = true;
						}
					}
					if (_save->getSelectedUnit())
					{
						getMap()->getCamera()->centerOnPosition(_save->getSelectedUnit()->getPosition());
					}
				}
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
		ss << action.actor->getUnit()->getName(_parentState->getGame()->getLanguage()) << L'\n' << _parentState->getGame()->getLanguage()->getString("STR_HAS_BECOME_UNCONSCIOUS");
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
	if (dontSpendTUs() || _save->getSide() != FACTION_PLAYER) return true; // aliens don't reserve TUs

	if (_tuReserved != BA_NONE &&
		tu + bu->getActionTUs(_tuReserved, bu->getMainHandWeapon()) > bu->getTimeUnits() &&
		bu->getActionTUs(_tuReserved, bu->getMainHandWeapon()) <= bu->getTimeUnits())
	{
		if (_save->getSide() == FACTION_PLAYER)
		{
			switch (_tuReserved)
			{
			case BA_SNAPSHOT: _parentState->warning("STR_TIME_UNITS_RESERVED_FOR_SNAP_SHOT"); break;
			case BA_AUTOSHOT: _parentState->warning("STR_TIME_UNITS_RESERVED_FOR_AUTO_SHOT"); break;
			case BA_AIMEDSHOT: _parentState->warning("STR_TIME_UNITS_RESERVED_FOR_AIMED_SHOT"); break;
			default: ;
			}
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
		if (handlePanickingUnit(*j))
			return false;
	}
	return true;
}

/**
 * Cmmon function for panicking units.
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
	ss << unit->getUnit()->getName(_parentState->getGame()->getLanguage()) << L'\n' << _parentState->getGame()->getLanguage()->getString(status==STATUS_PANICKING?"STR_HAS_PANICKED":"STR_HAS_GONE_BERSERK");
	_parentState->getGame()->pushState(new InfoboxState(_parentState->getGame(), ss.str()));

	unit->abortTurn(); //makes the unit go to status STANDING :p

	int flee = RNG::generate(0,100);
	switch (status)
	{
	case STATUS_PANICKING: // 1/2 chance to freeze and 1/2 chance try to flee
		if (flee <= 50)
		{
			BattleItem *item = unit->getItem("STR_RIGHT_HAND");
			if (item)
			{
				dropItem(unit->getPosition(), item);
				item->moveToOwner(0);
			}
			item = unit->getItem("STR_LEFT_HAND");
			if (item)
			{
				dropItem(unit->getPosition(), item);
				item->moveToOwner(0);
			}
			unit->setCache(0);
			BattleAction ba;
			ba.actor = unit;
			ba.target = Position(unit->getPosition().x + RNG::generate(-5,5), unit->getPosition().y + RNG::generate(-5,5), unit->getPosition().z);
			if (_save->getTile(ba.target)) // only walk towards it when the place exists
			{
				_save->getPathfinding()->calculate(ba.actor, ba.target);
				statePushBack(new UnitWalkBState(this, ba));
			}
		}
		break;
	case STATUS_BERSERK: // berserk - do some weird turning around and then aggro towards an enemy unit or shoot towards random place
		BattleAction ba;
		for (int i= 0; i < 4; i++)
		{
			ba.actor = unit;
			ba.target = Position(unit->getPosition().x + RNG::generate(-5,5), unit->getPosition().y + RNG::generate(-5,5), unit->getPosition().z);
			statePushBack(new UnitTurnBState(this, ba));
		}
		for (std::vector<BattleUnit*>::iterator j = unit->getVisibleUnits()->begin(); j != unit->getVisibleUnits()->end(); ++j)
		{
			ba.target = (*j)->getPosition();
			statePushBack(new UnitTurnBState(this, ba));
		}
		ba.type = BA_SNAPSHOT;
		ba.weapon = unit->getMainHandWeapon();
		for (int i= 0; i < 10; i++)
		{
			statePushBack(new ProjectileFlyBState(this, ba));
		}
		ba.type = BA_NONE;
		break;
	}
	unit->setTimeUnits(0);
	unit->moraleChange(+15);

	return true;
}

/**
 * TUs are not spent when handling panicking mode or in debug mode.
 * @return Whether TUs are spent or not.
 */
bool BattlescapeGame::dontSpendTUs()
{
	if (_save->getDebugMode())
		return true;
	if (!_playerPanicHandled)
		return true;

	return false;
}

/**
  * This will cancel the current action the user had selected (firing, throwing,..)
  */
bool BattlescapeGame::cancelCurrentAction()
{
	bool bPreviewed = Options::getBool("battlePreviewPath");

	if (_save->getPathfinding()->removePreview() && bPreviewed) return true;

	if (_states.empty())
	{
		if (_currentAction.targeting)
		{
			_currentAction.targeting = false;
			_currentAction.type = BA_NONE;
			setupCursor();
			_parentState->getGame()->getCursor()->setVisible(true);
			return true;
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
 * @param Position on the map.
 */
void BattlescapeGame::primaryAction(const Position &pos)
{
	bool bPreviewed = Options::getBool("battlePreviewPath");

	if (_currentAction.targeting && _save->getSelectedUnit())
	{
		_currentAction.target = pos;
		getMap()->setCursorType(CT_NONE);
		_parentState->getGame()->getCursor()->setVisible(false);
		_states.push_back(new ProjectileFlyBState(this, _currentAction));
		statePushFront(new UnitTurnBState(this, _currentAction)); // first of all turn towards the target
	}
	else
	{
		BattleUnit *unit = _save->selectUnit(pos);
		if (unit && unit != _save->getSelectedUnit())
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
 * @param Position on the map.
 */
void BattlescapeGame::secondaryAction(const Position &pos)
{
	//  -= turn to or open door =-
	_currentAction.target = pos;
	statePushBack(new UnitTurnBState(this, _currentAction));
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
 */
void BattlescapeGame::dropItem(const Position &position, BattleItem *item, bool newItem)
{
	Position p = position;

	// don't spawn anything outside of bounds
	if (_save->getTile(p) == 0)
		return;

	while (_save->getTile(p)->getMapData(MapData::O_FLOOR) == 0 && p.z > 0)
	{
		p.z--;
	}

	_save->getTile(p)->addItem(item);

	if(newItem)
	{
		_save->getItems()->push_back(item);
	}

	item->setSlot(getRuleset()->getInventory("STR_GROUND"));
	item->setOwner(0);

	if (item->getRules()->getBattleType() == BT_FLARE)
	{
		getTileEngine()->calculateTerrainLighting();
	}

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
Ruleset *BattlescapeGame::getRuleset()
{
	return _parentState->getGame()->getRuleset();
}

}
