/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include "Map.h"
#include "Camera.h"
#include "NextTurnState.h"
#include "AbortMissionState.h"
#include "BattleState.h"
#include "UnitTurnBState.h"
#include "UnitWalkBState.h"
#include "ProjectileFlyBState.h"
#include "MeleeAttackBState.h"
#include "PsiAttackBState.h"
#include "ExplosionBState.h"
#include "TileEngine.h"
#include "UnitInfoState.h"
#include "UnitDieBState.h"
#include "UnitPanicBState.h"
#include "AlienBAIState.h"
#include "CivilianBAIState.h"
#include "Pathfinding.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Engine/Sound.h"
#include "../Resource/ResourcePack.h"
#include "../Interface/Cursor.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/SoldierDiary.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/RuleInventory.h"
#include "../Ruleset/Armor.h"
#include "../Engine/Options.h"
#include "../Engine/RNG.h"
#include "InfoboxState.h"
#include "InfoboxOKState.h"
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
BattlescapeGame::BattlescapeGame(SavedBattleGame *save, BattlescapeState *parentState) : _save(save), _parentState(parentState), _playerPanicHandled(true), _AIActionCounter(0), _AISecondMove(false), _playedAggroSound(false), _endTurnRequested(false), _endTurnProcessed(false)
{
	
	_currentAction.actor = 0;
	_currentAction.targeting = false;
	_currentAction.type = BA_NONE;

	_debugPlay = false;

	checkForCasualties(0, 0, true);
	cancelCurrentAction();
}


/**
 * Delete BattlescapeGame.
 */
BattlescapeGame::~BattlescapeGame()
{
	for (std::list<BattleState*>::iterator i = _states.begin(); i != _states.end(); ++i)
	{
		delete *i;
	}
	cleanupDeleted();
}

/**
 * Checks for units panicking or falling and so on.
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
					if (_save->selectNextPlayerUnit(true, _AISecondMove) == 0)
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
				_save->getBattleState()->updateSoldierInfo();
			}
		}
		if (_save->getUnitsFalling())
		{
			statePushFront(new UnitFallBState(this));
			_save->setUnitsFalling(false);
		}
	}
}

/**
 * Initializes the Battlescape game.
 */
void BattlescapeGame::init()
{
	if (_save->getSide() == FACTION_PLAYER && _save->getTurn() > 1)
	{
		_playerPanicHandled = false;
	}
}


/**
 * Handles the processing of the AI states of a unit.
 * @param unit Pointer to a unit.
 */
void BattlescapeGame::handleAI(BattleUnit *unit)
{
	std::wostringstream ss;

	if (unit->getTimeUnits() <= 5)
	{
		unit->dontReselect();
	}
	if (_AIActionCounter >= 2 || !unit->reselectAllowed())
	{
		if (_save->selectNextPlayerUnit(true, _AISecondMove) == 0)
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
			_parentState->updateSoldierInfo();
			getMap()->getCamera()->centerOnPosition(_save->getSelectedUnit()->getPosition());
			if (_save->getSelectedUnit()->getId() <= unit->getId())
			{
				_AISecondMove = true;
			}
		}
		_AIActionCounter = 0;
		return;
	}

	unit->setVisible(false);

	_save->getTileEngine()->calculateFOV(unit->getPosition()); // might need this populate _visibleUnit for a newly-created alien
		// it might also help chryssalids realize they've zombified someone and need to move on
		// it should also hide units when they've killed the guy spotting them
		// it's also for good luck

	BattleAIState *ai = unit->getCurrentAIState();
	if (!ai)
	{
		// for some reason the unit had no AI routine assigned..
		if (unit->getFaction() == FACTION_HOSTILE)
			unit->setAIState(new AlienBAIState(_save, unit, 0));
		else
			unit->setAIState(new CivilianBAIState(_save, unit, 0));
		ai = unit->getCurrentAIState();
	}
	_AIActionCounter++;
	if (_AIActionCounter == 1)
	{
		_playedAggroSound = false;
		unit->setHiding(false);
		if (Options::traceAI) { Log(LOG_INFO) << "#" << unit->getId() << "--" << unit->getType(); }
	}
	//AlienBAIState *aggro = dynamic_cast<AlienBAIState*>(ai); // this cast only works when ai was already AlienBAIState at heart

	BattleAction action;
	action.actor = unit;
	action.number = _AIActionCounter;
	unit->think(&action);

	if (action.type == BA_RETHINK)
	{
		_parentState->debug(L"Rethink");
		unit->think(&action);
	}

	_AIActionCounter = action.number;

	if (!unit->getMainHandWeapon() || !unit->getMainHandWeapon()->getAmmoItem())
	{
		if (unit->getOriginalFaction() == FACTION_HOSTILE && unit->getVisibleUnits()->empty())
		{
			findItem(&action);
		}
	}

	if (unit->getCharging() != 0)
	{
		if (unit->getAggroSound() != -1 && !_playedAggroSound)
		{
			getResourcePack()->getSoundByDepth(_save->getDepth(), unit->getAggroSound())->play(-1, getMap()->getSoundAngle(unit->getPosition()));
			_playedAggroSound = true;
		}
	}
	if (action.type == BA_WALK)
	{
		ss << L"Walking to " << action.target;
		_parentState->debug(ss.str());

		if (_save->getTile(action.target))
		{
			_save->getPathfinding()->calculate(action.actor, action.target);//, _save->getTile(action.target)->getUnit());
		}
		if (_save->getPathfinding()->getStartDirection() != -1)
		{
			statePushBack(new UnitWalkBState(this, action));
		}
	}

	if (action.type == BA_SNAPSHOT || action.type == BA_AUTOSHOT || action.type == BA_AIMEDSHOT || action.type == BA_THROW || action.type == BA_HIT || action.type == BA_MINDCONTROL || action.type == BA_PANIC || action.type == BA_LAUNCH)
	{
		ss.clear();
		ss << L"Attack type=" << action.type << " target="<< action.target << " weapon=" << action.weapon->getRules()->getName().c_str();
		_parentState->debug(ss.str());
		action.TU = unit->getActionTUs(action.type, action.weapon);
		if (action.type == BA_MINDCONTROL || action.type == BA_PANIC)
		{
			statePushBack(new PsiAttackBState(this, action));
		}
		else
		{
			statePushBack(new UnitTurnBState(this, action));
			if (action.type == BA_HIT)
			{
				action.weapon = unit->getMeleeWeapon();
				statePushBack(new MeleeAttackBState(this, action));
			}
			else
			{
				statePushBack(new ProjectileFlyBState(this, action));
			}
		}
	}

	if (action.type == BA_NONE)
	{
		_parentState->debug(L"Idle");
		_AIActionCounter = 0;
		if (_save->selectNextPlayerUnit(true, _AISecondMove) == 0)
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
			_parentState->updateSoldierInfo();
			getMap()->getCamera()->centerOnPosition(_save->getSelectedUnit()->getPosition());
			if (_save->getSelectedUnit()->getId() <= unit->getId())
			{
				_AISecondMove = true;
			}
		}
	}
}

/**
 * Toggles the Kneel/Standup status of the unit.
 * @param bu Pointer to a unit.
 * @return If the action succeeded.
 */
bool BattlescapeGame::kneel(BattleUnit *bu)
{
	int tu = bu->isKneeled()?8:4;
	if (bu->getType() == "SOLDIER" && !bu->isFloating() && ((!bu->isKneeled() && _save->getKneelReserved()) || checkReservedTU(bu, tu)))
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
 * Ends the turn.
 */
void BattlescapeGame::endTurn()
{

	Position p;

	_debugPlay = false;
	_currentAction.type = BA_NONE;
	getMap()->getWaypoints()->clear();
	_currentAction.waypoints.clear();
	_parentState->showLaunchButton(false);
	_currentAction.targeting = false;
	_AISecondMove = false;

	if (!_endTurnProcessed)
	{
		if (_save->getTileEngine()->closeUfoDoors() && ResourcePack::SLIDING_DOOR_CLOSE != -1)
		{
			getResourcePack()->getSoundByDepth(_save->getDepth(), ResourcePack::SLIDING_DOOR_CLOSE)->play(); // ufo door closed
		}

		// check for hot grenades on the ground
		for (int i = 0; i < _save->getMapSizeXYZ(); ++i)
		{
			for (std::vector<BattleItem*>::iterator it = _save->getTiles()[i]->getInventory()->begin(); it != _save->getTiles()[i]->getInventory()->end(); )
			{
				if ((*it)->getRules()->getBattleType() == BT_GRENADE && (*it)->getFuseTimer() == 0)  // it's a grenade to explode now
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
	}
	// check for terrain explosions
	Tile *t = _save->getTileEngine()->checkForTerrainExplosions();
	if (t)
	{
		Position p = Position(t->getPosition().x * 16, t->getPosition().y * 16, t->getPosition().z * 24);
		statePushNext(new ExplosionBState(this, p, 0, 0, t));
		statePushBack(0);
		return;
	}
	
	if (!_endTurnProcessed)
	{
		if (_save->getSide() != FACTION_NEUTRAL)
		{
			for (std::vector<BattleItem*>::iterator it = _save->getItems()->begin(); it != _save->getItems()->end(); ++it)
			{
					if (((*it)->getRules()->getBattleType() == BT_GRENADE || (*it)->getRules()->getBattleType() == BT_PROXIMITYGRENADE) && (*it)->getFuseTimer() > 0)
					{
						(*it)->setFuseTimer((*it)->getFuseTimer() - 1);
					}
			}
		}


		_save->endTurn();
		t = _save->getTileEngine()->checkForTerrainExplosions();
		if (t)
		{
			Position p = Position(t->getPosition().x * 16, t->getPosition().y * 16, t->getPosition().z * 24);
			statePushNext(new ExplosionBState(this, p, 0, 0, t));
			statePushBack(0);
			_endTurnProcessed = true;
			return;
		}
	}

	_endTurnProcessed = false;

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

	// if all units from either faction are killed - the mission is over.
	int liveAliens = 0;
	int liveSoldiers = 0;

	tallyUnits(liveAliens, liveSoldiers);

	if (_save->allObjectivesDestroyed())
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

	bool battleComplete = liveAliens == 0 || liveSoldiers == 0;

	if ((_save->getSide() != FACTION_NEUTRAL || battleComplete)
		&& _endTurnRequested)
	{
		_parentState->getGame()->pushState(new NextTurnState(_save, _parentState));
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
	std::string killStatRank, killStatRace, killStatWeapon, killStatWeaponAmmo;
	int killStatMission, killStatTurn;

	// If the victim was killed by the murderer's death explosion, fetch who killed the murderer and make HIM the murderer!
	if (murderer && !murderer->getGeoscapeSoldier() && murderer->getUnitRules()->getSpecialAbility() == SPECAB_EXPLODEONDEATH && murderer->getStatus() == STATUS_DEAD && murderer->getMurdererId() != 0)
	{
		for (std::vector<BattleUnit*>::const_iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
		{
			if ((*i)->getId() == murderer->getMurdererId())
			{
				murderer = (*i);
			}
		}
	}

	killStatWeapon = "STR_WEAPON_UNKNOWN";
	killStatWeaponAmmo = "STR_WEAPON_UNKNOWN";
	killStatMission = _save->getGeoscapeSave()->getMissionStatistics()->size();
	if (_save->getSide() == FACTION_PLAYER)
	{
		killStatTurn = _save->getTurn()*3 + 0;
	}
	else if (_save->getSide() == FACTION_HOSTILE)
	{
		killStatTurn = _save->getTurn()*3 + 1;
	}
	else if (_save->getSide() == FACTION_NEUTRAL)
	{
		killStatTurn = _save->getTurn()*3 + 2;
	}

	// Fetch the murder weapon
	if (murderer && murderer->getGeoscapeSoldier())
	{
		if (murderweapon)
		{
			killStatWeaponAmmo = murderweapon->getRules()->getName();
			killStatWeapon = killStatWeaponAmmo;
		}

		BattleItem *weapon = murderer->getItem("STR_RIGHT_HAND");
		if (weapon)
		{
			for (std::vector<std::string>::iterator c = weapon->getRules()->getCompatibleAmmo()->begin(); c != weapon->getRules()->getCompatibleAmmo()->end(); ++c)
			{
				if ((*c) == killStatWeaponAmmo)
				{
					killStatWeapon = weapon->getRules()->getName();
				}
			}
		}
		weapon = murderer->getItem("STR_LEFT_HAND");
		if (weapon)
		{
			for (std::vector<std::string>::iterator c = weapon->getRules()->getCompatibleAmmo()->begin(); c != weapon->getRules()->getCompatibleAmmo()->end(); ++c)
			{
				if ((*c) == killStatWeaponAmmo)
				{
					killStatWeapon = weapon->getRules()->getName();
				}
			}
		}
	}
	
	for (std::vector<BattleUnit*>::iterator j = _save->getUnits()->begin(); j != _save->getUnits()->end(); ++j)
	{
		BattleUnit *victim = (*j);
		
		/// Decide victim race and rank
		// Soldiers
		if (victim->getGeoscapeSoldier() && victim->getOriginalFaction() == FACTION_PLAYER)
		{
			if (victim->getUnitRules() != NULL && victim->getUnitRules()->getRank() != "")
			{
				killStatRank = victim->getGeoscapeSoldier()->getRankString();
			}
			else
			{
				killStatRank = "STR_LIVE_SOLDIER";
			}
			if (victim->getUnitRules() != NULL && victim->getUnitRules()->getRace() != "")
			{
				killStatRace = victim->getUnitRules()->getRace();
			}
			else
			{
				killStatRace = "STR_HUMAN";								
			}
		}
		// HWPs
		else if (victim->getOriginalFaction() == FACTION_PLAYER)
		{
			if (victim->getUnitRules() != NULL && victim->getUnitRules()->getRank() != "")
			{
				killStatRank = victim->getUnitRules()->getRank();
			}
			else
			{
				killStatRank = "STR_HEAVY_WEAPONS_PLATFORM_LC";
			}
			if (victim->getUnitRules() != NULL && victim->getUnitRules()->getRace() != "")
			{
				killStatRace = victim->getUnitRules()->getRace();
			}
			else
			{
				killStatRace = "STR_TANK";								
			}
		}
		// Aliens
		else if (victim->getOriginalFaction() == FACTION_HOSTILE)
		{
			if (victim->getUnitRules() != NULL && victim->getUnitRules()->getRank() != "")
			{
				killStatRank = victim->getUnitRules()->getRank();
			}
			else
			{
				killStatRank = "STR_UNKNOWN";
			}
			if (victim->getUnitRules() != NULL && victim->getUnitRules()->getRace() != "")
			{
				killStatRace = victim->getUnitRules()->getRace();
			}
			else
			{
				killStatRace = "STR_LIVE_SOLDIER";								
			}
		}
		// Civilians
		else if (victim->getOriginalFaction() == FACTION_NEUTRAL)
		{
			if (victim->getUnitRules() != NULL && victim->getUnitRules()->getRank() != "")
			{
				killStatRank = victim->getUnitRules()->getRank();
			}
			else
			{
				killStatRank = "STR_CIVILIAN";
			}
			if (victim->getUnitRules() != NULL && (victim->getUnitRules()->getRace() != "" || victim->getUnitRules()->getRace() != "STR_CIVILIAN"))
			{
				killStatRace = victim->getUnitRules()->getRace();
			}
			else
			{
				killStatRace = "STR_HUMAN";
			}
		}
		// Error
		else
		{
			killStatRank = "STR_UNKNOWN";
			killStatRace = "STR_UNKNOWN";
		}
		
		if ((*j)->getHealth() == 0 && (*j)->getStatus() != STATUS_DEAD && (*j)->getStatus() != STATUS_COLLAPSING)
		{
			// Assume that, in absence of a murderer and an explosion, fire killed victim.
			// Assume that the fire that killed victim was started by the unit who hit victim with DT_IN.
			if (!murderer && !terrainExplosion)
			{
				for (std::vector<BattleUnit*>::const_iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
				{
					if ((*i)->getId() == victim->getMurdererId())
					{
						murderer = (*i);
						// Now find a plausible weapon. First a gun loaded with IN ammo, then a gun that can use IN ammo found in the inventory,
						// then simlpy the IN thing.
						for (std::vector<BattleItem*>::iterator it = murderer->getInventory()->begin(); it != murderer->getInventory()->end(); ++it)
						{
							if ((*it)->getRules()->getBattleType() == BT_FIREARM && (*it)->getAmmoItem()->getRules()->getDamageType() == DT_IN)
							{
								killStatWeaponAmmo = (*it)->getAmmoItem()->getRules()->getName();
								killStatWeapon = (*it)->getRules()->getName();
								break;
							}
							else if ((*it)->getRules()->getDamageType() == DT_IN)
							{
								killStatWeaponAmmo = (*it)->getRules()->getName();
								if ((*it)->getRules()->getBattleType() == BT_AMMO)
								{
									for (std::vector<std::string>::iterator c = (*it)->getRules()->getCompatibleAmmo()->begin(); c != (*it)->getRules()->getCompatibleAmmo()->end(); ++c)
									{
										if ((*c) == killStatWeaponAmmo)
										{
											killStatWeapon = (*it)->getRules()->getName();
										}
									}
								}
								else if ((*it)->getRules()->getBattleType() == BT_GRENADE || (*it)->getRules()->getBattleType() == BT_PROXIMITYGRENADE)
								{
									killStatWeapon = (*it)->getRules()->getName();
								}
							}
						}
					}
				}
			}

			if (murderer)
			{
				if (murderer->getGeoscapeSoldier() && murderer->getFaction() == FACTION_PLAYER)
				{
					murderer->getStatistics()->kills.push_back(new BattleUnitKills(killStatRank, killStatRace, killStatWeapon, killStatWeaponAmmo, victim->getFaction(), STATUS_DEAD, killStatMission, killStatTurn, victim->getFatalShotSide(), victim->getFatalShotBodyPart(), victim->getId() ));
					victim->setMurdererId(murderer->getId());
				}
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
							int bravery = (110 - (*i)->getBaseStats()->bravery) / 10;
							(*i)->moraleChange(-(modifier * 200 * bravery / loserMod / 100));

							if (victim->getFaction() == FACTION_HOSTILE && murderer)
							{
								murderer->setTurnsSinceSpotted(0);
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
		}
		else if ((*j)->getStunlevel() >= (*j)->getHealth() && (*j)->getStatus() != STATUS_DEAD && (*j)->getStatus() != STATUS_UNCONSCIOUS && (*j)->getStatus() != STATUS_COLLAPSING && (*j)->getStatus() != STATUS_TURNING)
		{
			if (murderer && murderer->getGeoscapeSoldier() && murderer->getFaction() == FACTION_PLAYER) 
			{
                    murderer->getStatistics()->kills.push_back(new BattleUnitKills(killStatRank, killStatRace, killStatWeapon, killStatWeaponAmmo, victim->getFaction(), STATUS_UNCONSCIOUS, killStatMission, killStatTurn, victim->getFatalShotSide(), victim->getFatalShotBodyPart(), victim->getId() ));
			}
			if (victim && victim->getGeoscapeSoldier())
			{
				victim->getStatistics()->wasUnconcious = true;
			}
			statePushNext(new UnitDieBState(this, (*j), DT_STUN, true));
		}
	}

	BattleUnit *bu = _save->getSelectedUnit();
	if (_save->getSide() == FACTION_PLAYER)
	{
		_parentState->showPsiButton(bu && bu->getSpecialWeapon(BT_PSIAMP) && !bu->isOut());
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
		_currentAction.cameraPosition = Position(0,0,-1);
		if (_currentAction.type == BA_PRIME && _currentAction.value > -1)
		{
			if (_currentAction.actor->spendTimeUnits(_currentAction.TU))
			{
				_parentState->warning("STR_GRENADE_IS_ACTIVATED");
				_currentAction.weapon->setFuseTimer(_currentAction.value);
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
					statePushBack(new MeleeAttackBState(this, _currentAction));
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
 * Sets the cursor according to the selected action.
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
 * Determines whether a playable unit is selected. Normally only player side units can be selected, but in debug mode one can play with aliens too :)
 * Is used to see if stats can be displayed and action buttons will work.
 * @return Whether a playable unit is selected.
 */
bool BattlescapeGame::playableUnitSelected()
{
	return _save->getSelectedUnit() != 0 && (_save->getSide() == FACTION_PLAYER || _save->getDebugMode());
}

/**
 * Gives time slice to the front state.
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
		getMap()->invalidate(); // redraw map
	}
}

/**
 * Pushes a state to the front of the queue and starts it.
 * @param bs Battlestate.
 */
void BattlescapeGame::statePushFront(BattleState *bs)
{
	_states.push_front(bs);
	bs->init();
}

/**
 * Pushes a state as the next state after the current one.
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
 * Pushes a state to the back.
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
 * Removes the current state.
 *
 * This is a very important function. It is called by a BattleState (walking, projectile is flying, explosions,...) at the moment this state has finished its action.
 * Here we check the result of that action and do all the aftermath.
 * The state is popped off the list.
 */
void BattlescapeGame::popState()
{
	if (Options::traceAI)
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
	_deleted.push_back(_states.front());
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
				// AI does three things per unit, before switching to the next, or it got killed before doing the second thing
				if (_AIActionCounter > 2 || _save->getSelectedUnit() == 0 || _save->getSelectedUnit()->isOut())
				{
					if (_save->getSelectedUnit())
					{
						_save->getSelectedUnit()->setCache(0);
						getMap()->cacheUnit(_save->getSelectedUnit());
					}
					_AIActionCounter = 0;
					if (_states.empty() && _save->selectNextPlayerUnit(true) == 0)
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
			while (!_states.empty())
			{
				if (_states.front() == 0)
					_states.pop_front();
				else
					break;
			}
			if (_states.empty())
			{
				endTurn();
				return;
			}
			else
			{
				_states.push_back(0);
			}
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

/**
 * Determines whether there are any actions pending for the given unit.
 * @param bu BattleUnit.
 * @return True if there are no actions pending.
 */
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
 * Checks against reserved time units.
 * @param bu Pointer to the unit.
 * @param tu Number of time units to check.
 * @param justChecking True to suppress error messages, false otherwise.
 * @return bool Whether or not we got enough time units.
 */
bool BattlescapeGame::checkReservedTU(BattleUnit *bu, int tu, bool justChecking)
{
	BattleActionType effectiveTuReserved = _save->getTUReserved(); // avoid changing _tuReserved in this method

	if (_save->getSide() != bu->getFaction() || _save->getSide() == FACTION_NEUTRAL)
	{
		return tu <= bu->getTimeUnits();
	}

	if (_save->getSide() == FACTION_HOSTILE && !_debugPlay) // aliens reserve TUs as a percentage rather than just enough for a single action.
	{
		AlienBAIState *ai = dynamic_cast<AlienBAIState*>(bu->getCurrentAIState());
		if (ai)
		{
			effectiveTuReserved = ai->getReserveMode();
		}
		switch (effectiveTuReserved)
		{
		case BA_SNAPSHOT: return tu + (bu->getBaseStats()->tu / 3) <= bu->getTimeUnits(); break; // 33%
		case BA_AUTOSHOT: return tu + ((bu->getBaseStats()->tu / 5)*2) <= bu->getTimeUnits(); break; // 40%
		case BA_AIMEDSHOT: return tu + (bu->getBaseStats()->tu / 2) <= bu->getTimeUnits(); break; // 50%
		default: return tu <= bu->getTimeUnits(); break;
		}
	}

	// check TUs against slowest weapon if we have two weapons
	BattleItem *slowestWeapon = bu->getMainHandWeapon(false);
	// if the weapon has no autoshot, reserve TUs for snapshot
	if (bu->getActionTUs(effectiveTuReserved, slowestWeapon) == 0 && effectiveTuReserved == BA_AUTOSHOT)
	{
		effectiveTuReserved = BA_SNAPSHOT;
	}
	// likewise, if we don't have a snap shot available, try aimed.
	if (bu->getActionTUs(effectiveTuReserved, slowestWeapon) == 0 && effectiveTuReserved == BA_SNAPSHOT)
	{
		effectiveTuReserved = BA_AIMEDSHOT;
	}
	const int tuKneel = (_save->getKneelReserved() && !bu->isKneeled()  && bu->getType() == "SOLDIER") ? 4 : 0;
	// no aimed shot available? revert to none.
	if (bu->getActionTUs(effectiveTuReserved, slowestWeapon) == 0 && effectiveTuReserved == BA_AIMEDSHOT)
	{
		if (tuKneel > 0)
		{
			effectiveTuReserved = BA_NONE;
		}
		else
		{
			return true;
		}
	}

	if ((effectiveTuReserved != BA_NONE || _save->getKneelReserved()) &&
		tu + tuKneel + bu->getActionTUs(effectiveTuReserved, slowestWeapon) > bu->getTimeUnits() &&
		(tuKneel + bu->getActionTUs(effectiveTuReserved, slowestWeapon) <= bu->getTimeUnits() || justChecking))
	{
		if (!justChecking)
		{
			if (tuKneel)
			{
				switch (effectiveTuReserved)
				{
				case BA_NONE: _parentState->warning("STR_TIME_UNITS_RESERVED_FOR_KNEELING"); break;
				default: _parentState->warning("STR_TIME_UNITS_RESERVED_FOR_KNEELING_AND_FIRING");
				}
			}
			else
			{
				switch (_save->getTUReserved())
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
 * Picks the first soldier that is panicking.
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
 * Common function for hanlding panicking units.
 * @return False when unit not in panicking mode.
 */
bool BattlescapeGame::handlePanickingUnit(BattleUnit *unit)
{
	UnitStatus status = unit->getStatus();
	if (status != STATUS_PANICKING && status != STATUS_BERSERK) return false;
	_save->setSelectedUnit(unit);
	_parentState->getMap()->setCursorType(CT_NONE);

	// show a little infobox with the name of the unit and "... is panicking"
	Game *game = _parentState->getGame();
	if (unit->getVisible() || !Options::noAlienPanicMessages)
	{
		getMap()->getCamera()->centerOnPosition(unit->getPosition());
		if (status == STATUS_PANICKING)
		{
			game->pushState(new InfoboxState(game->getLanguage()->getString("STR_HAS_PANICKED", unit->getGender()).arg(unit->getName(game->getLanguage()))));
		}
		else
		{
			game->pushState(new InfoboxState(game->getLanguage()->getString("STR_HAS_GONE_BERSERK", unit->getGender()).arg(unit->getName(game->getLanguage()))));
		}
	}

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
			// let's try a few times to get a tile to run to.
			for (int i= 0; i < 20; i++)
			{
				ba.target = Position(unit->getPosition().x + RNG::generate(-5,5), unit->getPosition().y + RNG::generate(-5,5), unit->getPosition().z);

				if (i >= 10 && ba.target.z > 0) // if we've had more than our fair share of failures, try going down.
				{
					ba.target.z--;
					if (i >= 15 && ba.target.z > 0) // still failing? try further down.
					{
						ba.target.z--;
					}
				}
				if (_save->getTile(ba.target)) // sanity check the tile.
				{
					_save->getPathfinding()->calculate(ba.actor, ba.target);
					if (_save->getPathfinding()->getStartDirection() != -1) // sanity check the path.
					{
						statePushBack(new UnitWalkBState(this, ba));
						break;
					}
				}
			}
		}
		break;
	case STATUS_BERSERK: // berserk - do some weird turning around and then aggro towards an enemy unit or shoot towards random place
		ba.type = BA_TURN;
		for (int i= 0; i < 4; i++)
		{
			ba.target = Position(unit->getPosition().x + RNG::generate(-5,5), unit->getPosition().y + RNG::generate(-5,5), unit->getPosition().z);
			statePushBack(new UnitTurnBState(this, ba, false));
		}
		for (std::vector<BattleUnit*>::iterator j = unit->getVisibleUnits()->begin(); j != unit->getVisibleUnits()->end(); ++j)
		{
			ba.target = (*j)->getPosition();
			statePushBack(new UnitTurnBState(this, ba, false));
		}
		if (_save->getTile(ba.target) != 0)
		{
			ba.weapon = unit->getMainHandWeapon();
			if (ba.weapon && (_save->getDepth() != 0 || ba.weapon->getRules()->isWaterOnly() == false))
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
					if (ba.weapon->getFuseTimer() == -1)
					{
						ba.weapon->setFuseTimer(0);
					}
					ba.type = BA_THROW;
					statePushBack(new ProjectileFlyBState(this, ba));
				}
			}
		}
		// replace the TUs from shooting
		unit->setTimeUnits(unit->getBaseStats()->tu);
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
  * Cancels the current action the user had selected (firing, throwing,..)
  * @param bForce Force the action to be cancelled.
  * @return Whether an action was cancelled or not.
  */
bool BattlescapeGame::cancelCurrentAction(bool bForce)
{
	bool bPreviewed = Options::battleNewPreviewPath != PATH_NONE;

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
				if (Options::battleConfirmFireMode && !_currentAction.waypoints.empty())
				{
					_currentAction.waypoints.pop_back();
					getMap()->getWaypoints()->pop_back();
					return true;
				}
				_currentAction.targeting = false;
				_currentAction.type = BA_NONE;
				setupCursor();
				_parentState->getGame()->getCursor()->setVisible(true);
				return true;
			}
		}
	}
	else if (!_states.empty() && _states.front() != 0)
	{
		_states.front()->cancel();
		return true;
	}

	return false;
}
/**
 * Gets a pointer to access action members directly.
 * @return Pointer to action.
 */
BattleAction *BattlescapeGame::getCurrentAction()
{
	return &_currentAction;
}

/**
 * Determines whether an action is currently going on?
 * @return true or false.
 */
bool BattlescapeGame::isBusy()
{
	return !_states.empty();
}

/**
 * Activates primary action (left click).
 * @param pos Position on the map.
 */
void BattlescapeGame::primaryAction(const Position &pos)
{
	bool bPreviewed = Options::battleNewPreviewPath != PATH_NONE;

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
			if (_save->selectUnit(pos) && _save->selectUnit(pos)->getFaction() != _save->getSelectedUnit()->getFaction() && _save->selectUnit(pos)->getVisible())
			{
				if (!_currentAction.weapon->getRules()->isLOSRequired() ||
					std::find(_currentAction.actor->getVisibleUnits()->begin(), _currentAction.actor->getVisibleUnits()->end(), _save->selectUnit(pos)) != _currentAction.actor->getVisibleUnits()->end())
				{
					if (_currentAction.actor->spendTimeUnits(_currentAction.TU))
					{
						_parentState->getGame()->getResourcePack()->getSoundByDepth(_save->getDepth(), _currentAction.weapon->getRules()->getHitSound())->play(-1, getMap()->getSoundAngle(pos));
						_parentState->getGame()->pushState (new UnitInfoState(_save->selectUnit(pos), _parentState, false, true));
						cancelCurrentAction();
					}
					else
					{
						_parentState->warning("STR_NOT_ENOUGH_TIME_UNITS");
					}
				}
				else
				{
					_parentState->warning("STR_NO_LINE_OF_FIRE");
				}
			}
		}
		else if (_currentAction.type == BA_PANIC || _currentAction.type == BA_MINDCONTROL)
		{
			if (_save->selectUnit(pos) && _save->selectUnit(pos)->getFaction() != _save->getSelectedUnit()->getFaction() && _save->selectUnit(pos)->getVisible())
			{
				_currentAction.TU = _currentAction.actor->getActionTUs(_currentAction.type, _currentAction.weapon);
				_currentAction.target = pos;
				if (!_currentAction.weapon->getRules()->isLOSRequired() ||
					std::find(_currentAction.actor->getVisibleUnits()->begin(), _currentAction.actor->getVisibleUnits()->end(), _save->selectUnit(pos)) != _currentAction.actor->getVisibleUnits()->end())
				{
					// get the sound/animation started
					getMap()->setCursorType(CT_NONE);
					_parentState->getGame()->getCursor()->setVisible(false);
					_currentAction.cameraPosition = getMap()->getCamera()->getMapOffset();
					statePushBack(new PsiAttackBState(this, _currentAction));
				}
				else
				{
					_parentState->warning("STR_NO_LINE_OF_FIRE");
				}
			}
		}
		else if (Options::battleConfirmFireMode && (_currentAction.waypoints.empty() || pos != _currentAction.waypoints.front()))
		{
			_currentAction.waypoints.clear();
			_currentAction.waypoints.push_back(pos);
			getMap()->getWaypoints()->clear();
			getMap()->getWaypoints()->push_back(pos);
		}
		else
		{
			_currentAction.target = pos;
			getMap()->setCursorType(CT_NONE);
			
			if (Options::battleConfirmFireMode)
			{
				_currentAction.waypoints.clear();
				getMap()->getWaypoints()->clear();
			}

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
			bool modifierPressed = (SDL_GetModState() & KMOD_CTRL) != 0;
			if (bPreviewed &&
				(_currentAction.target != pos || (_save->getPathfinding()->isModifierUsed() != modifierPressed)))
			{
				_save->getPathfinding()->removePreview();
			}
			_currentAction.run = false;
			_currentAction.strafe = Options::strafe && modifierPressed && _save->getSelectedUnit()->getTurretType() == -1;
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
 * Activates secondary action (right click).
 * @param pos Position on the map.
 */
void BattlescapeGame::secondaryAction(const Position &pos)
{
	//  -= turn to or open door =-
	_currentAction.target = pos;
	_currentAction.actor = _save->getSelectedUnit();
	_currentAction.strafe = Options::strafe && (SDL_GetModState() & KMOD_CTRL) != 0 && _save->getSelectedUnit()->getTurretType() > -1;
	statePushBack(new UnitTurnBState(this, _currentAction));
}

/**
 * Handler for the blaster launcher button.
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
 * Handler for the psi button.
 */
void BattlescapeGame::psiButtonAction()
{
	_currentAction.weapon = _save->getSelectedUnit()->getSpecialWeapon(BT_PSIAMP);
	_currentAction.targeting = true;
	_currentAction.type = BA_PANIC;
	_currentAction.TU = 25;
	setupCursor();
}


/**
 * Moves a unit up or down.
 * @param unit The unit.
 * @param dir Direction DIR_UP or DIR_DOWN.
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
 * Requests the end of the turn (waits for explosions etc to really end the turn).
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
 * Sets the TU reserved type.
 * @param tur A battleactiontype.
 * @param player is this requested by the player?
 */
void BattlescapeGame::setTUReserved(BattleActionType tur)
{
	_save->setTUReserved(tur);
}

/**
 * Drops an item to the floor and affects it with gravity.
 * @param position Position to spawn the item.
 * @param item Pointer to the item.
 * @param newItem Bool whether this is a new item.
 * @param removeItem Bool whether to remove the item from the owner.
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

	if (item->getUnit())
	{
		item->getUnit()->setPosition(p);
	}

	if (newItem)
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

/**
 * Converts a unit into a unit of another type.
 * @param unit The unit to convert.
 * @param newType The type of unit to convert to.
 * @return Pointer to the new unit.
 */
BattleUnit *BattlescapeGame::convertUnit(BattleUnit *unit, const std::string &newType)
{
	bool visible = unit->getVisible();
	getSave()->getBattleState()->showPsiButton(false);
	// in case the unit was unconscious
	getSave()->removeUnconsciousBodyItem(unit);

	unit->instaKill();

	for (std::vector<BattleItem*>::iterator i = unit->getInventory()->begin(); i != unit->getInventory()->end(); ++i)
	{
		dropItem(unit->getPosition(), (*i));
		(*i)->setOwner(0);
	}

	unit->getInventory()->clear();

	// remove unit-tile link
	unit->setTile(0);

	getSave()->getTile(unit->getPosition())->setUnit(0);
	std::ostringstream newArmor;
	newArmor << getRuleset()->getUnit(newType)->getArmor();
	std::string terroristWeapon = getRuleset()->getUnit(newType)->getRace().substr(4);
	terroristWeapon += "_WEAPON";
	RuleItem *newItem = getRuleset()->getItem(terroristWeapon);
	int difficulty = (int)(_parentState->getGame()->getSavedGame()->getDifficulty());

	BattleUnit *newUnit = new BattleUnit(getRuleset()->getUnit(newType), FACTION_HOSTILE, _save->getUnits()->back()->getId() + 1, getRuleset()->getArmor(newArmor.str()), difficulty, getDepth());

	if (!difficulty)
	{
		newUnit->halveArmor();
	}

	getSave()->getTile(unit->getPosition())->setUnit(newUnit, _save->getTile(unit->getPosition() + Position(0,0,-1)));
	newUnit->setPosition(unit->getPosition());
	newUnit->setDirection(unit->getDirection());
	newUnit->setCache(0);
	newUnit->setTimeUnits(0);
	newUnit->setSpecialWeapon(getSave(), getRuleset());
	getSave()->getUnits()->push_back(newUnit);
	newUnit->setAIState(new AlienBAIState(getSave(), newUnit, 0));
	BattleItem *bi = new BattleItem(newItem, getSave()->getCurrentItemId());
	bi->moveToOwner(newUnit);
	bi->setSlot(getRuleset()->getInventory("STR_RIGHT_HAND"));
	getSave()->getItems()->push_back(bi);
	newUnit->setVisible(visible);
	getTileEngine()->calculateFOV(newUnit->getPosition());
	getTileEngine()->applyGravity(newUnit->getTile());
	newUnit->dontReselect();
	getMap()->cacheUnit(newUnit);
	//newUnit->getCurrentAIState()->think();
	return newUnit;

}

/**
 * Gets the map.
 * @return map.
 */
Map *BattlescapeGame::getMap()
{
	return _parentState->getMap();
}
/**
 * Gets the save.
 * @return save.
 */
SavedBattleGame *BattlescapeGame::getSave()
{
	return _save;
}
/**
 * Gets the tilengine.
 * @return tilengine.
 */
TileEngine *BattlescapeGame::getTileEngine()
{
	return _save->getTileEngine();
}
/**
 * Gets the pathfinding.
 * @return pathfinding.
 */
Pathfinding *BattlescapeGame::getPathfinding()
{
	return _save->getPathfinding();
}
/**
 * Gets the resourcepack.
 * @return resourcepack.
 */
ResourcePack *BattlescapeGame::getResourcePack()
{
	return _parentState->getGame()->getResourcePack();
}
/**
 * Gets the ruleset.
 * @return ruleset.
 */
const Ruleset *BattlescapeGame::getRuleset() const
{
	return _parentState->getGame()->getRuleset();
}


/**
 * Tries to find an item and pick it up if possible.
 */
void BattlescapeGame::findItem(BattleAction *action)
{
	// terrorists don't have hands.
	if (action->actor->getRankString() != "STR_LIVE_TERRORIST")
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
 * Searches through items on the map that were dropped on an alien turn, then picks the most "attractive" one.
 * @param action A pointer to the action being performed.
 * @return The item to attempt to take.
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
 * Assesses whether this item is worth trying to pick up, taking into account how many units we see,
 * whether or not the Weapon has ammo, and if we have ammo FOR it,
 * or, if it's ammo, checks if we have the weapon to go with it,
 * assesses the attraction value of the item and compares it with the distance to the object,
 * then returns false anyway.
 * @param item The item to attempt to take.
 * @param action A pointer to the action being performed.
 * @return false.
 */
bool BattlescapeGame::worthTaking(BattleItem* item, BattleAction *action)
{
	int worthToTake = 0;

	// don't even think about making a move for that gun if you can see a target, for some reason
	// (maybe this should check for enemies spotting the tile the item is on?)
	if (action->actor->getVisibleUnits()->empty())
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
 * Picks the item up from the ground.
 *
 * At this point we've decided it's worth our while to grab this item, so we try to do just that.
 * First we check to make sure we have time units, then that we have space (using horrifying logic)
 * then we attempt to actually recover the item.
 * @param item The item to attempt to take.
 * @param action A pointer to the action being performed.
 * @return 0 if successful, 1 for no TUs, 2 for not enough room, 3 for "won't fit" and -1 for "something went horribly wrong".
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
 * Tries to fit an item into the unit's inventory, return false if you can't.
 * @param item The item to attempt to take.
 * @param action A pointer to the action being performed.
 * @return Whether or not the item was successfully retrieved.
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

/**
 * Returns the action type that is reserved.
 * @return The type of action that is reserved.
 */
BattleActionType BattlescapeGame::getReservedAction()
{
	return _save->getTUReserved();
}

/**
 * Tallies the living units in the game and, if required, converts units into their spawn unit.
 * @param &liveAliens The integer in which to store the live alien tally.
 * @param &liveSoldiers The integer in which to store the live XCom tally.
 * @param convert Should we convert infected units?
 */
void BattlescapeGame::tallyUnits(int &liveAliens, int &liveSoldiers)
{
	liveSoldiers = 0;
	liveAliens = 0;

	for (std::vector<BattleUnit*>::iterator j = _save->getUnits()->begin(); j != _save->getUnits()->end(); ++j)
	{
		if (!(*j)->isOut())
		{
			if ((*j)->getOriginalFaction() == FACTION_HOSTILE)
			{
				if (!Options::allowPsionicCapture || (*j)->getFaction() != FACTION_PLAYER)
				{
					liveAliens++;
				}
			}
			else if ((*j)->getOriginalFaction() == FACTION_PLAYER)
			{
				if ((*j)->getFaction() == FACTION_PLAYER)
				{
					liveSoldiers++;
				}
				else
				{
					liveAliens++;
				}
			}
		}
	}
}

bool BattlescapeGame::convertInfected()
{
	bool retVal = false;
	for (std::vector<BattleUnit*>::iterator j = _save->getUnits()->begin(); j != _save->getUnits()->end(); ++j)
	{
		if ((*j)->getHealth() > 0 && (*j)->getRespawn())
		{
			retVal = true;
			(*j)->setRespawn(false);
			convertUnit((*j), (*j)->getSpawnUnit());
			j = _save->getUnits()->begin();
		}
	}
	return retVal;
}
/**
 * Sets the kneel reservation setting.
 * @param reserved Should we reserve an extra 4 TUs to kneel?
 */
void BattlescapeGame::setKneelReserved(bool reserved)
{
	_save->setKneelReserved(reserved);
}

/**
 * Gets the kneel reservation setting.
 * @return Kneel reservation setting.
 */
bool BattlescapeGame::getKneelReserved()
{
	return _save->getKneelReserved();
}

/**
 * Checks if a unit has moved next to a proximity grenade.
 * Checks one tile around the unit in every direction.
 * For a large unit we check every tile it occupies.
 * @param unit Pointer to a unit.
 * @return True if a proximity grenade was triggered.
 */
bool BattlescapeGame::checkForProximityGrenades(BattleUnit *unit)
{
	int size = unit->getArmor()->getSize() - 1;
	for (int x = size; x >= 0; x--)
	{
		for (int y = size; y >= 0; y--)
		{
			for (int tx = -1; tx < 2; tx++)
			{
				for (int ty = -1; ty < 2; ty++)
				{
					Tile *t = _save->getTile(unit->getPosition() + Position(x,y,0) + Position(tx,ty,0));
					if (t)
					{
						for (std::vector<BattleItem*>::iterator i = t->getInventory()->begin(); i != t->getInventory()->end(); ++i)
						{
							if ((*i)->getRules()->getBattleType() == BT_PROXIMITYGRENADE && (*i)->getFuseTimer() == 0)
							{
								Position p;
								p.x = t->getPosition().x*16 + 8;
								p.y = t->getPosition().y*16 + 8;
								p.z = t->getPosition().z*24 + t->getTerrainLevel();
								statePushNext(new ExplosionBState(this, p, (*i), (*i)->getPreviousOwner()));
								getSave()->removeItem(*i);
								unit->setCache(0);
								getMap()->cacheUnit(unit);
								return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

/**
 * Cleans up all the deleted states.
 */
void BattlescapeGame::cleanupDeleted()
{
	for (std::list<BattleState*>::iterator i = _deleted.begin(); i != _deleted.end(); ++i)
	{
		delete *i;
	}
	_deleted.clear();
}

/**
 * Gets the depth of the battlescape.
 * @return the depth of the battlescape.
 */
const int BattlescapeGame::getDepth() const
{
	return _save->getDepth();
}

}
