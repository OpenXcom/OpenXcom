/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#include <typeinfo>
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
#include "../Ruleset/RuleInventory.h"
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
BattlescapeGame::BattlescapeGame(SavedBattleGame *save, BattlescapeState *parentState) : _save(save), _parentState(parentState), _playedAggroSound(false), _endTurnRequested(false), _kneelReserved(false)
{
	_tuReserved = BA_NONE;
	_playerTUReserved = BA_NONE;
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
							_endTurnRequested = true;
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
	
	_tuReserved = BA_NONE;

	if (unit->getTimeUnits() <= 5)
	{
		if (_save->selectNextPlayerUnit(true, true) == 0)
		{
			if (!_save->getDebugMode())
			{
				_endTurnRequested = true;
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
		_AIActionCounter = 0;
		return;
	}

	if (unit->getMainHandWeapon() && unit->getMainHandWeapon()->getRules()->getBattleType() == BT_FIREARM)
	{
		switch (unit->getAggression())
		{
		case 0:
			_tuReserved = BA_AIMEDSHOT;
			break;
		case 1:
			_tuReserved = BA_AUTOSHOT;
			break;
		case 2:
			_tuReserved = BA_SNAPSHOT;
		default:
			break;
		}
	}

	_save->getTileEngine()->calculateFOV(unit->getPosition()); // might need this populate _visibleUnit for a newly-created alien
        // it might also help chryssalids realize they've zombified someone and need to move on
		// it should also hide units when they've killed the guy spotting them
        // it's also for good luck

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
		if (_save->getTraceSetting()) { Log(LOG_INFO) << "#" << unit->getId() << "--" << unit->getType(); }
	}
	AggroBAIState *aggro = dynamic_cast<AggroBAIState*>(ai); // this cast only works when ai was already AggroBAIState at heart
	
	// psionic or blaster launcher units may attack remotely
	// in bonus round, need to be in "aggro" state to hide; what was that about refactoring?
	// also make sure you're in aggro state if you see units, even if you haven't taken a step yet
	if ((unit->getStats()->psiSkill && _save->getExposedUnits()->size() > 0)
		|| (unit->getMainHandWeapon() && unit->getMainHandWeapon()->getRules()->isWaypoint())
		|| (_AIActionCounter > 2)
        || (unit->getVisibleUnits()->size() != 0)
		|| (unit->_hidingForTurn))
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
    action.number = _AIActionCounter;
	unit->think(&action);
	
	if (action.type == BA_RETHINK)
	{
		_parentState->debug(L"Rethink");
		unit->setAIState(new PatrolBAIState(_save, unit, 0));
		ai = unit->getCurrentAIState();
		unit->think(&action);	
	}

	if (!unit->getMainHandWeapon() || !unit->getMainHandWeapon()->getAmmoItem())
	{
		if (unit->getOriginalFaction() == FACTION_HOSTILE && unit->getVisibleUnits()->size() == 0)
		{
			action.actor = unit;
			findItem(&action);
		}
	}

	if (dynamic_cast<AggroBAIState*>(ai) != 0)
	{
		_tuReserved = BA_NONE;
		if (unit->getAggroSound() != -1 && !_playedAggroSound)
		{
			getResourcePack()->getSound("BATTLE.CAT", unit->getAggroSound())->play();
			_playedAggroSound = true;
		}
	}
	if (action.type == BA_WALK)
	{
		ss << L"Walking to " << action.target.x << " "<< action.target.y << " "<< action.target.z;
		_parentState->debug(ss.str());

		if (_save->getTile(action.target)) {
      _save->getPathfinding()->calculate(action.actor, action.target, _save->getTile(action.target)->getUnit());
		}
    if (_save->getPathfinding()->getStartDirection() == -1)
		{
			PatrolBAIState *pbai = dynamic_cast<PatrolBAIState*>(unit->getCurrentAIState());
			if (pbai) unit->setAIState(new PatrolBAIState(_save, unit, 0)); // can't reach destination, pick someplace else to walk toward
		}


        Position finalFacing(0, 0, INT_MAX);
        bool usePathfinding = false;

        if (unit->_hidingForTurn && _AIActionCounter > 2)
        {
            if (_save->getTile(action.target) && _save->getTile(action.target)->soldiersVisible > 0)
            {
                finalFacing = _save->getTile(action.target)->closestSoldierPos; // be ready for the nearest spotting unit for our destination
                usePathfinding = false;
				if (_save->getTraceSetting()) { Log(LOG_INFO) << "setting final facing direction for closest soldier, " << finalFacing.x << "," << finalFacing.y << "," << finalFacing.z; }
            } else if (aggro != 0)
            {
                finalFacing = aggro->getLastKnownPosition(); // or else be ready for our aggro target
                usePathfinding = true;
				if (_save->getTraceSetting()) { Log(LOG_INFO) << "setting final facing direction for aggro target via pathfinding, " << finalFacing.x << "," << finalFacing.y << "," << finalFacing.z; }
            }
        }

		statePushBack(new UnitWalkBState(this, action, finalFacing, usePathfinding));
	}

	if (action.type == BA_SNAPSHOT || action.type == BA_AUTOSHOT || action.type == BA_AIMEDSHOT || action.type == BA_THROW || action.type == BA_HIT || action.type == BA_MINDCONTROL || action.type == BA_PANIC || action.type == BA_LAUNCH)
	{
		if (action.type == BA_MINDCONTROL || action.type == BA_PANIC)
		{
			action.weapon = new BattleItem(_parentState->getGame()->getRuleset()->getItem("ALIEN_PSI_WEAPON"), _save->getCurrentItemId());
			action.TU = action.weapon->getRules()->getTUUse();
		}
		else
		{
			statePushBack(new UnitTurnBState(this, action));
		}

		ss.clear();
		ss << L"Attack type=" << action.type << " target="<< action.target.x << " "<< action.target.y << " "<< action.target.z << " weapon=" << action.weapon->getRules()->getName().c_str();
		_parentState->debug(ss.str());

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
				_endTurnRequested = true;
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
 * @return If the action succeeded.
 */
bool BattlescapeGame::kneel(BattleUnit *bu)
{
	int tu = bu->isKneeled()?8:4;
	if (bu->getType() == "SOLDIER" && !bu->isFloating() && checkReservedTU(bu, tu))
	{
		if (bu->spendTimeUnits(tu))
		{
			bu->kneel(!bu->isKneeled());
			// kneeling or standing up can reveal new terrain or units. I guess.
			getTileEngine()->calculateFOV(bu);
			getMap()->cacheUnits();
			_parentState->updateSoldierInfo();
			getTileEngine()->checkReactionFire(bu);
			return true;
		}
		else
		{
			_parentState->warning("STR_NOT_ENOUGH_TIME_UNITS");
		}
	}
	return false;
}

/**
 * End turn.
 */
void BattlescapeGame::endTurn()
{
	Position p;

	_tuReserved = _playerTUReserved;
	_debugPlay = false;
	_currentAction.type = BA_NONE;
	getMap()->getWaypoints()->clear();
	_currentAction.waypoints.clear();
	_parentState->showLaunchButton(false);
	_currentAction.targeting = false;

	if (_save->getTileEngine()->closeUfoDoors())
	{
		getResourcePack()->getSound("BATTLE.CAT", 21)->play(); // ufo door closed
	}

	// check for hot grenades on the ground
	for (int i = 0; i < _save->getMapSizeXYZ(); ++i)
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

	// check for terrain explosions
	Tile *t = _save->getTileEngine()->checkForTerrainExplosions();
	if (t)
	{
		Position p = Position(t->getPosition().x * 16, t->getPosition().y * 16, t->getPosition().z * 24);
		statePushNext(new ExplosionBState(this, p, 0, 0, t));
		t = _save->getTileEngine()->checkForTerrainExplosions();
		statePushBack(0);
		return;
	}

	// if all units from either faction are killed - the mission is over.
	int liveAliens = 0;
	int liveSoldiers = 0;
	// we'll tally them NOW, so that any infected units will... change
	tallyUnits(liveAliens, liveSoldiers, true);

	_save->endTurn();

	if (_save->getSide() == FACTION_PLAYER)
	{
		setupCursor();
	}
	else
	{
		getMap()->setCursorType(CT_NONE);
	}

	checkForCasualties(0, 0, false, false);

	// turn off MCed alien lighting.
	_save->getTileEngine()->calculateUnitLighting();

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

	if (_save->getSide() != FACTION_NEUTRAL && _endTurnRequested)
	{
		_parentState->getGame()->pushState(new NextTurnState(_parentState->getGame(), _save, _parentState));
	}
	_endTurnRequested = false;

}


/**
 * Checks for casualties and adjusts morale accordingly.
 * @param murderweapon Need to know this, for a HE explosion there is an instant death.
 * @param murderer This is needed for credits for the kill.
 * @param hiddenExplosion Set to true for the explosions of UFO Power sources at start of battlescape.
 * @param terrainExplosion Set to true for the explosions of terrain.
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
				int modifier = murderer->getFaction() == FACTION_PLAYER ? _save->getMoraleModifier() : 100;

				// if there is a known murderer, he will get a morale bonus if he is of a different faction (what with neutral?)
				if ((victim->getOriginalFaction() == FACTION_PLAYER && murderer->getFaction() == FACTION_HOSTILE) ||
					(victim->getOriginalFaction() == FACTION_HOSTILE && murderer->getFaction() == FACTION_PLAYER))
				{
					murderer->moraleChange(20 * modifier / 100);
				}
				// murderer will get a penalty with friendly fire
				if (victim->getOriginalFaction() == murderer->getOriginalFaction())
				{
					murderer->moraleChange(-(2000 / modifier));
				}
				if (victim->getOriginalFaction() == FACTION_NEUTRAL)
				{
					if (murderer->getOriginalFaction() == FACTION_PLAYER)
					{
						murderer->moraleChange(-(1000 / modifier));
					}
					else
					{
						murderer->moraleChange(10);
					}
				}
			}

			if (victim->getFaction() != FACTION_NEUTRAL)
			{
				int modifier = _save->getMoraleModifier(victim);
				int loserMod = victim->getFaction() == FACTION_HOSTILE ? 100 : _save->getMoraleModifier();
				int winnerMod = victim->getFaction() == FACTION_HOSTILE ? _save->getMoraleModifier() : 100;
				for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
				{
					if (!(*i)->isOut() && (*i)->getArmor()->getSize() == 1)
					{
						// the losing squad all get a morale loss
						if ((*i)->getOriginalFaction() == victim->getOriginalFaction())
						{
							int bravery = (110 - (*i)->getStats()->bravery) / 10;
							(*i)->moraleChange(-(modifier * 200 * bravery / loserMod / 100));

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
						else
						{
							(*i)->moraleChange(10 * winnerMod / 100);
						}
					}
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
						statePushNext(new UnitDieBState(this, (*j), DT_NONE, false));  // DT_NONE = STR_HAS_DIED_FROM_A_FATAL_WOUND
					}
				}
			}
			if ((*j)->getHealth() > 0 && (*j)->getSpecialAbility() == SPECAB_RESPAWN)
			{
				(*j)->setSpecialAbility(SPECAB_NONE);
				convertUnit((*j), (*j)->getSpawnUnit());
			}
		}
		else if ((*j)->getStunlevel() >= (*j)->getHealth() && (*j)->getStatus() != STATUS_DEAD && (*j)->getStatus() != STATUS_UNCONSCIOUS && (*j)->getStatus() != STATUS_COLLAPSING && (*j)->getStatus() != STATUS_TURNING)
		{
			statePushNext(new UnitDieBState(this, (*j), DT_STUN, true));
		}
	}
	BattleUnit *bu = _save->getSelectedUnit();
	if (_save->getSide() == FACTION_PLAYER)
	{
		_parentState->showPsiButton(bu && bu->getOriginalFaction() == FACTION_HOSTILE && bu->getStats()->psiSkill > 0 && !bu->isOut());
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
	if (_save->getTraceSetting())
	{
		Log(LOG_INFO) << "BattlescapeGame::popState() #" << _AIActionCounter << " with " << (_save->getSelectedUnit() ? _save->getSelectedUnit()->getTimeUnits() : -9999) << " TU";
	}
	bool actionFailed = false;

	if (_states.empty()) return;

	BattleAction action = _states.front()->getAction();

	if (action.actor && action.result.length() > 0 && action.actor->getFaction() == FACTION_PLAYER 
    && _playerPanicHandled && (_save->getSide() == FACTION_PLAYER || _debugPlay))
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
				const int AIActionLimit = (action.actor->getMainHandWeapon() && action.actor->getMainHandWeapon()->getRules()->getBattleType() == BT_MELEE) ? 9 : 2;
				 // AI does two (or three?) things per unit + hides?, before switching to the next, or it got killed before doing the second thing
				 // melee get more because chryssalids and reapers need to attack many times to be scary
				if (_AIActionCounter > AIActionLimit || _save->getSelectedUnit() == 0 || _save->getSelectedUnit()->isOut())
				{
					if (_save->getSelectedUnit())
					{
						_save->getSelectedUnit()->setCache(0);
						getMap()->cacheUnit(_save->getSelectedUnit());
					}
					_AIActionCounter = 0;
					if (_save->selectNextPlayerUnit(true, true) == 0 && _states.empty())
					{
						if (!_save->getDebugMode())
						{
							_endTurnRequested = true;
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
bool BattlescapeGame::checkReservedTU(BattleUnit *bu, int tu, bool justChecking)
{
    BattleActionType effectiveTuReserved = _tuReserved; // avoid changing _tuReserved in this method

	if (_save->getSide() != FACTION_PLAYER) // aliens reserve TUs as a percentage rather than just enough for a single action.
	{
		if (_save->getSide() == FACTION_NEUTRAL)
		{
			return tu < bu->getTimeUnits();
		}
		switch (effectiveTuReserved)
		{
		case BA_SNAPSHOT: return tu + (bu->getStats()->tu / 3) < bu->getTimeUnits(); break; // 33%
		case BA_AUTOSHOT: return tu + ((bu->getStats()->tu / 5)*2) < bu->getTimeUnits(); break; // 40%
		case BA_AIMEDSHOT: return tu + (bu->getStats()->tu / 2) < bu->getTimeUnits(); break; // 50%
		default: return tu < bu->getTimeUnits(); break;
		}
	}

	// check TUs against slowest weapon if we have two weapons
	BattleItem *slowestWeapon = bu->getMainHandWeapon(false);
	// if the weapon has no autoshot, reserve TUs for snapshot
	if (bu->getActionTUs(_tuReserved, slowestWeapon) == 0 && _tuReserved == BA_AUTOSHOT)
	{
		effectiveTuReserved = BA_SNAPSHOT;
	}
	const int tuKneel = _kneelReserved ? 4 : 0;
	if ((effectiveTuReserved != BA_NONE || _kneelReserved) &&
		tu + tuKneel + bu->getActionTUs(effectiveTuReserved, slowestWeapon) > bu->getTimeUnits() &&
		tuKneel + bu->getActionTUs(effectiveTuReserved, slowestWeapon) <= bu->getTimeUnits())
	{
		if (!justChecking)
		{
			if (_kneelReserved)
			{
				switch (effectiveTuReserved)
				{
				case BA_NONE: _parentState->warning("STR_TIME_UNITS_RESERVED_FOR_KNEELING"); break;
				default: _parentState->warning("STR_TUS_RESERVED_FOR_KNEELING_AND_FIRING");
				}
			}
			else
			{
				switch (effectiveTuReserved)
				{
				case BA_SNAPSHOT: _parentState->warning("STR_TIME_UNITS_RESERVED_FOR_SNAP_SHOT"); break;
				case BA_AUTOSHOT: _parentState->warning("STR_TIME_UNITS_RESERVED_FOR_AUTO_SHOT"); break;
				case BA_AIMEDSHOT: _parentState->warning("STR_TIME_UNITS_RESERVED_FOR_AIMED_SHOT"); break;
				default: ;
				}
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
		if ((*j)->getFaction() == FACTION_PLAYER && (*j)->getOriginalFaction() == FACTION_PLAYER && handlePanickingUnit(*j))
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
	ss << unit->getName(_parentState->getGame()->getLanguage()) << L'\n';
	if (status == STATUS_PANICKING)
	{
		ss << _parentState->getGame()->getLanguage()->getString("STR_HAS_PANICKED", unit->getGender());
	}
	else
	{
		ss << _parentState->getGame()->getLanguage()->getString("STR_HAS_GONE_BERSERK", unit->getGender());
	}
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
		ba.type = BA_TURN;
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
		// replace the TUs from shooting
		unit->setTimeUnits(unit->getStats()->tu);
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
  * @param bForce Force the action to be cancelled.
  * @return Whether an action was cancelled or not.
  */
bool BattlescapeGame::cancelCurrentAction(bool bForce)
{
	bool bPreviewed = Options::getInt("battleNewPreviewPath") > 0;

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
	bool bPreviewed = Options::getInt("battleNewPreviewPath") > 0;

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
				if (_currentAction.actor->spendTimeUnits(_currentAction.TU))
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
							BattleUnit *unit = _save->getTile(_currentAction.target)->getUnit();
							ss << unit->getName(_parentState->getGame()->getLanguage()) << L'\n' << _parentState->getGame()->getLanguage()->getString("STR_HAS_PANICKED", unit->getGender());
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
			_currentAction.run = false;
			_currentAction.strafe = _save->getStrafeSetting() && (SDL_GetModState() & KMOD_CTRL) != 0 && _save->getSelectedUnit()->getTurretType() == -1;
			if (_currentAction.strafe && _save->getTileEngine()->distance(_currentAction.actor->getPosition(), pos) > 1)
			{
				_currentAction.run = true;
				_currentAction.strafe = false;
			}
			_currentAction.target = pos;
			_save->getPathfinding()->calculate(_currentAction.actor, _currentAction.target);
			if (bPreviewed && !_save->getPathfinding()->previewPath() && _save->getPathfinding()->getStartDirection() != -1)
			{
				_save->getPathfinding()->removePreview();
				bPreviewed = false;
			}

			if (!bPreviewed && _save->getPathfinding()->getStartDirection() != -1)
			{
				//  -= start walking =-
				getMap()->setCursorType(CT_NONE);
				_parentState->getGame()->getCursor()->setVisible(false);
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
	_currentAction.strafe = _save->getStrafeSetting() && (SDL_GetModState() & KMOD_CTRL) != 0 && _save->getSelectedUnit()->getTurretType() > -1;
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
	if (!_endTurnRequested)
	{
		_endTurnRequested = true;
		statePushBack(0);
	}
}

/**
 * Set the TU reserved type.
 * @param tur A battleactiontype
 */
void BattlescapeGame::setTUReserved(BattleActionType tur)
{
	_tuReserved = tur;
	_playerTUReserved = tur;
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
	else if (_save->getSide() != FACTION_PLAYER)
	{
		item->setTurnFlag(true);
	}

	if (removeItem)
	{
		item->moveToOwner(0);
	}
	else if (item->getRules()->getBattleType() != BT_GRENADE && item->getRules()->getBattleType() != BT_PROXIMITYGRENADE)
	{
		item->setOwner(0);
	}

	getTileEngine()->applyGravity(_save->getTile(p));

	if (item->getRules()->getBattleType() == BT_FLARE)
	{
		getTileEngine()->calculateTerrainLighting();
		getTileEngine()->calculateFOV(position);
	}

}

/*
 * Convert a unit into a unit of another type.
 */
BattleUnit *BattlescapeGame::convertUnit(BattleUnit *unit, std::string newType)
{
	getSave()->getBattleState()->showPsiButton(false);
	// in case the unit was unconscious
	getSave()->removeUnconsciousBodyItem(unit);

	unit->instaKill();

	if (Options::getBool("battleNotifyDeath") && unit->getFaction() == FACTION_PLAYER && unit->getOriginalFaction() == FACTION_PLAYER)
	{
		std::wstringstream ss;
		ss << unit->getName(_parentState->getGame()->getLanguage()) << L'\n' << _parentState->getGame()->getLanguage()->getString("STR_HAS_BEEN_KILLED", unit->getGender());
		_parentState->getGame()->pushState(new InfoboxState(_parentState->getGame(), ss.str()));
	}

	for (std::vector<BattleItem*>::iterator i = unit->getInventory()->begin(); i != unit->getInventory()->end(); ++i)
	{
		dropItem(unit->getPosition(), (*i));
		(*i)->setOwner(0);
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
	int difficulty = (int)(_parentState->getGame()->getSavedGame()->getDifficulty());

	BattleUnit *newUnit = new BattleUnit(getRuleset()->getUnit(newType), FACTION_HOSTILE, _save->getUnits()->back()->getId() + 1, getRuleset()->getArmor(newArmor.str()), difficulty);
	
	if (!difficulty)
	{
		newUnit->halveArmor();
	}

	getSave()->getTile(unit->getPosition())->setUnit(newUnit, _save->getTile(unit->getPosition() + Position(0,0,-1)));
	newUnit->setPosition(unit->getPosition());
	newUnit->setDirection(3);
	newUnit->setCache(0);
	newUnit->setTimeUnits(0);
	getSave()->getUnits()->push_back(newUnit);
	getMap()->cacheUnit(newUnit);
	newUnit->setAIState(new PatrolBAIState(getSave(), newUnit, 0));
	BattleItem *bi = new BattleItem(newItem, getSave()->getCurrentItemId());
	bi->moveToOwner(newUnit);
	bi->setSlot(getRuleset()->getInventory("STR_RIGHT_HAND"));
	getSave()->getItems()->push_back(bi);
	getTileEngine()->calculateFOV(newUnit->getPosition());
	getTileEngine()->applyGravity(newUnit->getTile());
	//newUnit->getCurrentAIState()->think();
	return newUnit;

}

/**
 * Get map
 * @return map
 */
Map *BattlescapeGame::getMap()
{
	return _parentState->getMap();
}
/**
 * Get save
 * @return save
 */
SavedBattleGame *BattlescapeGame::getSave()
{
	return _save;
}
/**
 * Get tilengine
 * @return tilengine
 */
TileEngine *BattlescapeGame::getTileEngine()
{
	return _save->getTileEngine();
}
/**
 * Get pathfinding
 * @return pathfinding
 */
Pathfinding *BattlescapeGame::getPathfinding()
{
	return _save->getPathfinding();
}
/**
 * Get resourcepack
 * @return resourcepack
 */
ResourcePack *BattlescapeGame::getResourcePack()
{
	return _parentState->getGame()->getResourcePack();
}
/**
 * Get ruleset
 * @return ruleset
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
    int w = _save->getMapSizeX();
    int h = _save->getMapSizeZ();
    int l = _save->getMapSizeY();

    Tile **tiles = _save->getTiles();

    // Log(LOG_INFO) << w*h*l << " tiles!";

	if (_save->getTraceSetting())
	{
		for (int i = 0; i < w * l * h; ++i) if (tiles[i]->soldiersVisible != -1) { tiles[i]->setMarkerColor(0); } // clear old tile markers
	}

    for (int i = 0; i < w * l * h; ++i)
    {
		tiles[i]->soldiersVisible = Tile::NOT_CALCULATED;    // -1 for "not calculated"; actual calculations will take place as needed
		tiles[i]->closestSoldierDSqr = Tile::NOT_CALCULATED; // for most of the tiles most of the time, this data is not needed
    }

}


/**
 * Try and find an item and pick it up if possible
 */
void BattlescapeGame::findItem(BattleAction *action)
{
	// terrorists don't have hands.
	if (action->actor->getRankString() != "STR_TERRORIST")
	{
		// pick the best available item
		BattleItem *targetItem = surveyItems(action);
		// make sure it's worth taking
		if (targetItem && worthTaking(targetItem, action))
		{
			// if we're already standing on it...
			if (targetItem->getTile()->getPosition() == action->actor->getPosition())
			{
				// try to pick it up
				if (takeItemFromGround(targetItem, action) == 0)
				{
					// if it isn't loaded or it is ammo
					if (!targetItem->getAmmoItem())
					{
						// try to load our weapon
						action->actor->checkAmmo();
					}
				}
			}
			else if (!targetItem->getTile()->getUnit() || targetItem->getTile()->getUnit()->isOut())
			{
				// if we're not standing on it, we should try to get to it.
				action->target = targetItem->getTile()->getPosition();
				action->type = BA_WALK;
			}
		}
	}
}


/**
 * Search through items on the map that were dropped on an alien turn, then pick the most "attractive" one
 * @param action a pointer to the action being performed.
 * @return the item to attempt to take.
 */
BattleItem *BattlescapeGame::surveyItems(BattleAction *action)
{
	std::vector<BattleItem*> droppedItems;

	// first fill a vector with items on the ground that were dropped on the alien turn, and have an attraction value.
	for (std::vector<BattleItem*>::iterator i = _save->getItems()->begin(); i != _save->getItems()->end(); ++i)
	{
		if ((*i)->getSlot() && (*i)->getSlot()->getId() == "STR_GROUND" && (*i)->getTile() && (*i)->getTurnFlag() && (*i)->getRules()->getAttraction())
		{
			droppedItems.push_back(*i);
		}
	}

	BattleItem *targetItem = 0;
	int maxWorth = 0;

	// now select the most suitable candidate depending on attractiveness and distance
	// (are we still talking about items?)
	for (std::vector<BattleItem*>::iterator i = droppedItems.begin(); i != droppedItems.end(); ++i)
	{
		int currentWorth = (*i)->getRules()->getAttraction() / ((_save->getTileEngine()->distance(action->actor->getPosition(), (*i)->getTile()->getPosition()) * 2)+1);
		if (currentWorth > maxWorth)
		{
			maxWorth = currentWorth;
			targetItem = *i;
		}
	}

	return targetItem;
}


/**
 * Assess whether this item is worth trying to pick up, taking into account how many units we see,
 * whether or not the Weapon has ammo, and if we have ammo FOR it,
 * or if it's ammo, check if we have the weapon to go with it,
 * assess the attraction value of the item and compare it with the distance to the object,
 * then return false anyway.
 * @param item the item to attempt to take.
 * @param action a pointer to the action being performed.
 * @return false.
 */
bool BattlescapeGame::worthTaking(BattleItem* item, BattleAction *action)
{
	int worthToTake = 0;

	// don't even think about making a move for that gun if you can see a target, for some reason 
	// (maybe this should check for enemies spotting the tile the item is on?)
	if (action->actor->getVisibleUnits()->size() == 0)
	{
		// retrieve an insignificantly low value from the ruleset.
		worthToTake = item->getRules()->getAttraction();

		// it's always going to be worth while to try and take a blaster launcher, apparently
		if (!item->getRules()->isWaypoint() && item->getRules()->getBattleType() != BT_AMMO)
		{
			// we only want weapons that HAVE ammo, or weapons that we have ammo FOR
			bool ammoFound = true;
			if (!item->getAmmoItem())
			{
				ammoFound = false;
				for (std::vector<BattleItem*>::iterator i = action->actor->getInventory()->begin(); i != action->actor->getInventory()->end() && !ammoFound; ++i)
				{
					if ((*i)->getRules()->getBattleType() == BT_AMMO)
					{
						for (std::vector<std::string>::iterator j = item->getRules()->getCompatibleAmmo()->begin(); j != item->getRules()->getCompatibleAmmo()->end() && !ammoFound; ++j)
						{
							if ((*i)->getRules()->getName() == *j)
							{
								ammoFound = true;
								break;
							}
						}
					}
				}
			}
			if (!ammoFound)
			{
				return false;
			}
		}

		if ( item->getRules()->getBattleType() == BT_AMMO)
		{
			// similar to the above, but this time we're checking if the ammo is suitable for a weapon we have.
			bool weaponFound = false;
			for (std::vector<BattleItem*>::iterator i = action->actor->getInventory()->begin(); i != action->actor->getInventory()->end() && !weaponFound; ++i)
			{
				if ((*i)->getRules()->getBattleType() == BT_FIREARM)
				{
					for (std::vector<std::string>::iterator j = (*i)->getRules()->getCompatibleAmmo()->begin(); j != (*i)->getRules()->getCompatibleAmmo()->end() && !weaponFound; ++j)
					{
						if ((*i)->getRules()->getName() == *j)
						{
							weaponFound = true;
							break;
						}
					}
				}
			}
			if (!weaponFound)
			{
				return false;
			}
		}
	}

    if (worthToTake)
    {
		// use bad logic to determine if we'll have room for the item
		int freeSlots = 25;
		for (std::vector<BattleItem*>::iterator i = action->actor->getInventory()->begin(); i != action->actor->getInventory()->end(); ++i)
		{
			freeSlots -= (*i)->getRules()->getInventoryHeight() * (*i)->getRules()->getInventoryWidth();
		}
		int size = item->getRules()->getInventoryHeight() * item->getRules()->getInventoryWidth();
		if (freeSlots < size)
		{
			return false;
		}
	}

	// return false for any item that we aren't standing directly on top of with an attraction value less than 6 (aka always)
	return (worthToTake - (_save->getTileEngine()->distance(action->actor->getPosition(), item->getTile()->getPosition())*2)) > 5;
}


/**
 * at this point we've decided it's worth our while to grab this item, so we try to do just that.
 * first we check to make sure we have time units, then that we have space (using horrifying logic)
 * then we attempt to actually recover the item.
 * @param item the item to attempt to take.
 * @param action a pointer to the action being performed.
 * @return 0 if successful, 1 for no TUs, 2 for not enough room, 3 for "won't fit" and -1 for "something went horribly wrong"
 */
int BattlescapeGame::takeItemFromGround(BattleItem* item, BattleAction *action)
{
	const int unhandledError = -1;
	const int success = 0;
	const int notEnoughTimeUnits = 1;
	const int notEnoughSpace = 2;
	const int couldNotFit = 3;
	int freeSlots = 25;

	// make sure we have time units
	if (action->actor->getTimeUnits() < 6)
	{
		return notEnoughTimeUnits;
	}
	else
	{
		// check to make sure we have enough space by checking all the sizes of items in our inventory 
		for (std::vector<BattleItem*>::iterator i = action->actor->getInventory()->begin(); i != action->actor->getInventory()->end(); ++i)
		{
			freeSlots -= (*i)->getRules()->getInventoryHeight() * (*i)->getRules()->getInventoryWidth();
		}
		if (freeSlots < item->getRules()->getInventoryHeight() * item->getRules()->getInventoryWidth())
		{
			return notEnoughSpace;
		}
		else
		{
			// check that the item will fit in our inventory, and if so, take it
			if (takeItem(item, action))
			{
				action->actor->spendTimeUnits(6);
				item->getTile()->removeItem(item);
				return success;
			}
			else
			{
				return couldNotFit;
			}
		}
	}
	// shouldn't ever end up here
	return unhandledError;
}


/**
 * try to fit an item into the unit's inventory, return false if you can't.
 * @param item the item to attempt to take.
 * @param action a pointer to the action being performed.
 * @return whether or not the item was successfully retrieved.
 */
bool BattlescapeGame::takeItem(BattleItem* item, BattleAction *action)
{
	bool placed = false;
	Ruleset* rules = _parentState->getGame()->getRuleset();
	switch (item->getRules()->getBattleType())
	{
	case BT_AMMO:
		// find equipped weapons that can be loaded with this ammo
		if (action->actor->getItem("STR_RIGHT_HAND") && action->actor->getItem("STR_RIGHT_HAND")->getAmmoItem() == 0)
		{
			if (action->actor->getItem("STR_RIGHT_HAND")->setAmmoItem(item) == 0)
			{
				placed = true;
			}
		}
		else
		{
			for (int i = 0; i != 4; ++i)
			{
				if (!action->actor->getItem("STR_BELT", i))
				{
					item->moveToOwner(action->actor);
					item->setSlot(rules->getInventory("STR_BELT"));
					item->setSlotX(i);
					placed = true;
					break;
				}
			}
		}
		break;
	case BT_GRENADE:
	case BT_PROXIMITYGRENADE:
		for (int i = 0; i != 4; ++i)
		{
			if (!action->actor->getItem("STR_BELT", i))
			{
				item->moveToOwner(action->actor);
				item->setSlot(rules->getInventory("STR_BELT"));
				item->setSlotX(i);
				placed = true;
				break;
			}
		}
		break;
	case BT_FIREARM:
	case BT_MELEE:
		if (!action->actor->getItem("STR_RIGHT_HAND"))
		{
			item->moveToOwner(action->actor);
			item->setSlot(rules->getInventory("STR_RIGHT_HAND"));
			placed = true;
		}
		break;
	case BT_MEDIKIT:
	case BT_SCANNER:
		if (!action->actor->getItem("STR_BACK_PACK"))
		{
			item->moveToOwner(action->actor);
			item->setSlot(rules->getInventory("STR_BACK_PACK"));
			placed = true;
		}
		break;
	case BT_MINDPROBE:
		if (!action->actor->getItem("STR_LEFT_HAND"))
		{
			item->moveToOwner(action->actor);
			item->setSlot(rules->getInventory("STR_LEFT_HAND"));
			placed = true;
		}
		break;
	default: break;
	}
	return placed;
}

/*
 * @return the type of action that is reserved.
 */
BattleActionType BattlescapeGame::getReservedAction()
{
	return _tuReserved;
}

/*
 * tally the living units in the game, and if required, convert units into their spawn unit.
 * @param &liveAliens the integer in which to store the live alien tally
 * @param &liveSoldiers the integer in which to store the live XCom tally
 * @param convert should we convert infected units?
 */
void BattlescapeGame::tallyUnits(int &liveAliens, int &liveSoldiers, bool convert)
{
	bool psiCapture = Options::getBool("allowPsionicCapture");
	for (std::vector<BattleUnit*>::iterator j = _save->getUnits()->begin(); j != _save->getUnits()->end(); ++j)
	{
		if (convert && (*j)->getHealth() > 0 && (*j)->getSpecialAbility() == SPECAB_RESPAWN)
		{
			(*j)->setSpecialAbility(SPECAB_NONE);
			convertUnit((*j), (*j)->getSpawnUnit());
		}
		if (!(*j)->isOut())
		{
			if ((*j)->getOriginalFaction() == FACTION_HOSTILE)
			{
				if (!psiCapture || (*j)->getFaction() != FACTION_PLAYER)
				{
					liveAliens++;
				}
			}
			else if ((*j)->getOriginalFaction() == FACTION_PLAYER && (*j)->getFaction() == FACTION_PLAYER)
			{
				liveSoldiers++;
			}
		}
	}
}

/*
 * sets the kneel reservation setting.
 * @param reserved should we reserve an extra 4 TUs to kneel?
 */
void BattlescapeGame::setKneelReserved(bool reserved)
{
	_kneelReserved = reserved;
}

/*
 * @return kneel reservation setting.
 */
bool BattlescapeGame::getKneelReserved()
{
	if (_save->getSelectedUnit() && _save->getSelectedUnit()->getGeoscapeSoldier())
	{
		return _kneelReserved;
	}
	return false;
}
}
