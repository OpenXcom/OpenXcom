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
#include "BattleUnit.h"
#include "BattleItem.h"
#include <sstream>
#include "../Engine/Surface.h"
#include "../Engine/Language.h"
#include "../Engine/Options.h"
#include "../Battlescape/Pathfinding.h"
#include "../Battlescape/BattlescapeGame.h"
#include "../Battlescape/AIModule.h"
#include "Soldier.h"
#include "../Mod/Armor.h"
#include "../Mod/Unit.h"
#include "../Engine/RNG.h"
#include "../Mod/RuleInventory.h"
#include "../Mod/RuleSoldier.h"
#include "../Mod/Mod.h"
#include "Tile.h"
#include "SavedGame.h"
#include "SavedBattleGame.h"
#include "BattleUnitStatistics.h"
#include "../fmath.h"

namespace OpenXcom
{

/**
 * Initializes a BattleUnit from a Soldier
 * @param soldier Pointer to the Soldier.
 * @param depth the depth of the battlefield (used to determine movement type in case of MT_FLOAT).
 */
BattleUnit::BattleUnit(Soldier *soldier, int depth) :
	_faction(FACTION_PLAYER), _originalFaction(FACTION_PLAYER), _killedBy(FACTION_PLAYER), _id(0), _tile(0),
	_lastPos(Position()), _direction(0), _toDirection(0), _directionTurret(0), _toDirectionTurret(0),
	_verticalDirection(0), _status(STATUS_STANDING), _walkPhase(0), _fallPhase(0), _kneeled(false), _floating(false),
	_dontReselect(false), _fire(0), _currentAIState(0), _visible(false), _cacheInvalid(true),
	_expBravery(0), _expReactions(0), _expFiring(0), _expThrowing(0), _expPsiSkill(0), _expPsiStrength(0), _expMelee(0),
	_motionPoints(0), _kills(0), _hitByFire(false), _hitByAnything(false), _moraleRestored(0), _charging(0), _turnsSinceSpotted(255),
	_statistics(), _murdererId(0), _mindControllerID(0), _fatalShotSide(SIDE_FRONT), _fatalShotBodyPart(BODYPART_HEAD),
	_geoscapeSoldier(soldier), _unitRules(0), _rankInt(0), _turretType(-1), _hidingForTurn(false), _respawn(false), _capturable(true)
{
	_name = soldier->getName(true);
	_id = soldier->getId();
	_type = "SOLDIER";
	_rank = soldier->getRankString();
	_stats = *soldier->getCurrentStats();
	_standHeight = soldier->getRules()->getStandHeight();
	_kneelHeight = soldier->getRules()->getKneelHeight();
	_floatHeight = soldier->getRules()->getFloatHeight();
	_deathSound = std::vector<int>(); // this one is hardcoded
	_aggroSound = -1;
	_moveSound = -1;  // this one is hardcoded
	_intelligence = 2;
	_aggression = 1;
	_specab = SPECAB_NONE;
	_armor = soldier->getArmor();
	_movementType = _armor->getMovementType();
	if (_movementType == MT_FLOAT)
	{
		if (depth > 0)
		{
			_movementType = MT_FLY;
		}
		else
		{
			_movementType = MT_WALK;
		}
	}
	else if (_movementType == MT_SINK)
	{
		if (depth == 0)
		{
			_movementType = MT_FLY;
		}
		else
		{
			_movementType = MT_WALK;
		}
	}
	_stats += *_armor->getStats();	// armors may modify effective stats
	_loftempsSet = _armor->getLoftempsSet();
	_gender = soldier->getGender();
	_faceDirection = -1;
	_breathFrame = -1;
	if (_armor->drawBubbles())
	{
		_breathFrame = 0;
	}
	_floorAbove = false;
	_breathing = false;

	int rankbonus = 0;

	switch (soldier->getRank())
	{
	case RANK_SERGEANT:  rankbonus =  1; break;
	case RANK_CAPTAIN:   rankbonus =  3; break;
	case RANK_COLONEL:   rankbonus =  6; break;
	case RANK_COMMANDER: rankbonus = 10; break;
	default:             rankbonus =  0; break;
	}

	_value = soldier->getRules()->getValue() + soldier->getMissions() + rankbonus;

	_tu = _stats.tu;
	_energy = _stats.stamina;
	_health = _stats.health;
	_morale = 100;
	_stunlevel = 0;
	_maxArmor[SIDE_FRONT] = _armor->getFrontArmor();
	_maxArmor[SIDE_LEFT] = _armor->getSideArmor();
	_maxArmor[SIDE_RIGHT] = _armor->getSideArmor();
	_maxArmor[SIDE_REAR] = _armor->getRearArmor();
	_maxArmor[SIDE_UNDER] = _armor->getUnderArmor();
	_currentArmor[SIDE_FRONT] = _maxArmor[SIDE_FRONT];
	_currentArmor[SIDE_LEFT] = _maxArmor[SIDE_LEFT];
	_currentArmor[SIDE_RIGHT] = _maxArmor[SIDE_RIGHT];
	_currentArmor[SIDE_REAR] = _maxArmor[SIDE_REAR];
	_currentArmor[SIDE_UNDER] = _maxArmor[SIDE_UNDER];
	for (int i = 0; i < 6; ++i)
		_fatalWounds[i] = 0;
	for (int i = 0; i < 5; ++i)
		_cache[i] = 0;
	for (int i = 0; i < SPEC_WEAPON_MAX; ++i)
		_specWeapon[i] = 0;

	_activeHand = "STR_RIGHT_HAND";

	lastCover = Position(-1, -1, -1);

	_statistics = new BattleUnitStatistics();

	deriveRank();

	int look = soldier->getGender() + 2 * soldier->getLook();
	setRecolor(look, look, _rankInt);
}

/**
 * Initializes a BattleUnit from a Unit (non-player) object.
 * @param unit Pointer to Unit object.
 * @param faction Which faction the units belongs to.
 * @param id Unique unit ID.
 * @param armor Pointer to unit Armor.
 * @param diff difficulty level (for stat adjustment).
 * @param depth the depth of the battlefield (used to determine movement type in case of MT_FLOAT).
 */
BattleUnit::BattleUnit(Unit *unit, UnitFaction faction, int id, Armor *armor, StatAdjustment *adjustment, int depth) :
	_faction(faction), _originalFaction(faction), _killedBy(faction), _id(id),
	_tile(0), _lastPos(Position()), _direction(0), _toDirection(0), _directionTurret(0),
	_toDirectionTurret(0),  _verticalDirection(0), _status(STATUS_STANDING), _walkPhase(0),
	_fallPhase(0), _kneeled(false), _floating(false), _dontReselect(false), _fire(0), _currentAIState(0),
	_visible(false), _cacheInvalid(true), _expBravery(0), _expReactions(0), _expFiring(0),
	_expThrowing(0), _expPsiSkill(0), _expPsiStrength(0), _expMelee(0), _motionPoints(0), _kills(0), _hitByFire(false), _hitByAnything(false),
	_moraleRestored(0), _charging(0), _turnsSinceSpotted(255),
	_statistics(), _murdererId(0), _mindControllerID(0), _fatalShotSide(SIDE_FRONT),
	_fatalShotBodyPart(BODYPART_HEAD), _armor(armor),  _geoscapeSoldier(0), _unitRules(unit),
	_rankInt(0), _turretType(-1), _hidingForTurn(false), _respawn(false)
{
	_type = unit->getType();
	_rank = unit->getRank();
	_race = unit->getRace();
	_stats = *unit->getStats();
	_standHeight = unit->getStandHeight();
	_kneelHeight = unit->getKneelHeight();
	_floatHeight = unit->getFloatHeight();
	_loftempsSet = _armor->getLoftempsSet();
	_deathSound = unit->getDeathSounds();
	_aggroSound = unit->getAggroSound();
	_moveSound = unit->getMoveSound();
	_intelligence = unit->getIntelligence();
	_aggression = unit->getAggression();
	_specab = (SpecialAbility) unit->getSpecialAbility();
	_spawnUnit = unit->getSpawnUnit();
	_value = unit->getValue();
	_faceDirection = -1;
	_capturable = unit->getCapturable();

	_movementType = _armor->getMovementType();
	if (_movementType == MT_FLOAT)
	{
		if (depth > 0)
		{
			_movementType = MT_FLY;
		}
		else
		{
			_movementType = MT_WALK;
		}
	}
	else if (_movementType == MT_SINK)
	{
		if (depth == 0)
		{
			_movementType = MT_FLY;
		}
		else
		{
			_movementType = MT_WALK;
		}
	}

	_stats += *_armor->getStats();	// armors may modify effective stats

	_breathFrame = -1; // most aliens don't breathe per-se, that's exclusive to humanoids
	if (armor->drawBubbles())
	{
		_breathFrame = 0;
	}
	_floorAbove = false;
	_breathing = false;

	_maxArmor[SIDE_FRONT] = _armor->getFrontArmor();
	_maxArmor[SIDE_LEFT] = _armor->getSideArmor();
	_maxArmor[SIDE_RIGHT] = _armor->getSideArmor();
	_maxArmor[SIDE_REAR] = _armor->getRearArmor();
	_maxArmor[SIDE_UNDER] = _armor->getUnderArmor();

	if (faction == FACTION_HOSTILE)
	{
		adjustStats(*adjustment);
	}

	_tu = _stats.tu;
	_energy = _stats.stamina;
	_health = _stats.health;
	_morale = 100;
	_stunlevel = 0;
	_currentArmor[SIDE_FRONT] = _maxArmor[SIDE_FRONT];
	_currentArmor[SIDE_LEFT] = _maxArmor[SIDE_LEFT];
	_currentArmor[SIDE_RIGHT] = _maxArmor[SIDE_RIGHT];
	_currentArmor[SIDE_REAR] = _maxArmor[SIDE_REAR];
	_currentArmor[SIDE_UNDER] = _maxArmor[SIDE_UNDER];
	for (int i = 0; i < 6; ++i)
		_fatalWounds[i] = 0;
	for (int i = 0; i < 5; ++i)
		_cache[i] = 0;
	for (int i = 0; i < SPEC_WEAPON_MAX; ++i)
		_specWeapon[i] = 0;

	_activeHand = "STR_RIGHT_HAND";
	_gender = GENDER_MALE;

	lastCover = Position(-1, -1, -1);

	_statistics = new BattleUnitStatistics();

	int generalRank = 0;
	if (faction == FACTION_HOSTILE)
	{
		const int max = 7;
		const char* rankList[max] =
		{
			"STR_LIVE_SOLDIER",
			"STR_LIVE_ENGINEER",
			"STR_LIVE_MEDIC",
			"STR_LIVE_NAVIGATOR",
			"STR_LIVE_LEADER",
			"STR_LIVE_COMMANDER",
			"STR_LIVE_TERRORIST",
		};
		for (int i = 0; i < max; ++i)
		{
			if (_rank.compare(rankList[i]) == 0)
			{
				generalRank = i;
				break;
			}
		}
	}
	else if (faction == FACTION_NEUTRAL)
	{
		generalRank = std::rand() % 8;
	}

	setRecolor(std::rand() % 8, std::rand() % 8, generalRank);
}


/**
 *
 */
BattleUnit::~BattleUnit()
{
	for (int i = 0; i < 5; ++i)
		if (_cache[i]) delete _cache[i];
	for (std::vector<BattleUnitKills*>::const_iterator i = _statistics->kills.begin(); i != _statistics->kills.end(); ++i)
	{
		delete *i;
	}
	delete _statistics;
	delete _currentAIState;
}

/**
 * Loads the unit from a YAML file.
 * @param node YAML node.
 */
void BattleUnit::load(const YAML::Node &node)
{
	_id = node["id"].as<int>(_id);
	_faction = _originalFaction = (UnitFaction)node["faction"].as<int>(_faction);
	_status = (UnitStatus)node["status"].as<int>(_status);
	_pos = node["position"].as<Position>(_pos);
	_direction = _toDirection = node["direction"].as<int>(_direction);
	_directionTurret = _toDirectionTurret = node["directionTurret"].as<int>(_directionTurret);
	_tu = node["tu"].as<int>(_tu);
	_health = node["health"].as<int>(_health);
	_stunlevel = node["stunlevel"].as<int>(_stunlevel);
	_energy = node["energy"].as<int>(_energy);
	_morale = node["morale"].as<int>(_morale);
	_kneeled = node["kneeled"].as<bool>(_kneeled);
	_floating = node["floating"].as<bool>(_floating);
	for (int i=0; i < 5; i++)
		_currentArmor[i] = node["armor"][i].as<int>(_currentArmor[i]);
	for (int i=0; i < 6; i++)
		_fatalWounds[i] = node["fatalWounds"][i].as<int>(_fatalWounds[i]);
	_fire = node["fire"].as<int>(_fire);
	_expBravery = node["expBravery"].as<int>(_expBravery);
	_expReactions = node["expReactions"].as<int>(_expReactions);
	_expFiring = node["expFiring"].as<int>(_expFiring);
	_expThrowing = node["expThrowing"].as<int>(_expThrowing);
	_expPsiSkill = node["expPsiSkill"].as<int>(_expPsiSkill);
	_expPsiStrength = node["expPsiStrength"].as<int>(_expPsiStrength);
	_expMelee = node["expMelee"].as<int>(_expMelee);
	_turretType = node["turretType"].as<int>(_turretType);
	_visible = node["visible"].as<bool>(_visible);
	_turnsSinceSpotted = node["turnsSinceSpotted"].as<int>(_turnsSinceSpotted);
	_killedBy = (UnitFaction)node["killedBy"].as<int>(_killedBy);
	_moraleRestored = node["moraleRestored"].as<int>(_moraleRestored);
	_rankInt = node["rankInt"].as<int>(_rankInt);
	_originalFaction = (UnitFaction)node["originalFaction"].as<int>(_originalFaction);
	_kills = node["kills"].as<int>(_kills);
	_dontReselect = node["dontReselect"].as<bool>(_dontReselect);
	_charging = 0;
	_spawnUnit = node["spawnUnit"].as<std::string>(_spawnUnit);
	_motionPoints = node["motionPoints"].as<int>(0);
	_respawn = node["respawn"].as<bool>(_respawn);
	_activeHand = node["activeHand"].as<std::string>(_activeHand);
	if (node["tempUnitStatistics"])
	{
		_statistics->load(node["tempUnitStatistics"]);
	}
	_murdererId = node["murdererId"].as<int>(_murdererId);
	_fatalShotSide = (UnitSide)node["fatalShotSide"].as<int>(_fatalShotSide);
	_fatalShotBodyPart = (UnitBodyPart)node["fatalShotBodyPart"].as<int>(_fatalShotBodyPart);
	_murdererWeapon = node["murdererWeapon"].as<std::string>(_murdererWeapon);
	_murdererWeaponAmmo = node["murdererWeaponAmmo"].as<std::string>(_murdererWeaponAmmo);

	if (const YAML::Node& p = node["recolor"])
	{
		_recolor.clear();
		for (size_t i = 0; i < p.size(); ++i)
		{
			_recolor.push_back(std::make_pair(p[i][0].as<int>(), p[i][1].as<int>()));
		}
	}
	_mindControllerID = node["mindControllerID"].as<int>(_mindControllerID);
}

/**
 * Saves the soldier to a YAML file.
 * @return YAML node.
 */
YAML::Node BattleUnit::save() const
{
	YAML::Node node;

	node["id"] = _id;
	node["genUnitType"] = _type;
	node["genUnitArmor"] = _armor->getType();
	node["faction"] = (int)_faction;
	node["status"] = (int)_status;
	node["position"] = _pos;
	node["direction"] = _direction;
	node["directionTurret"] = _directionTurret;
	node["tu"] = _tu;
	node["health"] = _health;
	node["stunlevel"] = _stunlevel;
	node["energy"] = _energy;
	node["morale"] = _morale;
	node["kneeled"] = _kneeled;
	node["floating"] = _floating;
	for (int i=0; i < 5; i++) node["armor"].push_back(_currentArmor[i]);
	for (int i=0; i < 6; i++) node["fatalWounds"].push_back(_fatalWounds[i]);
	node["fire"] = _fire;
	node["expBravery"] = _expBravery;
	node["expReactions"] = _expReactions;
	node["expFiring"] = _expFiring;
	node["expThrowing"] = _expThrowing;
	node["expPsiSkill"] = _expPsiSkill;
	node["expPsiStrength"] = _expPsiStrength;
	node["expMelee"] = _expMelee;
	node["turretType"] = _turretType;
	node["visible"] = _visible;
	node["turnsSinceSpotted"] = _turnsSinceSpotted;
	node["rankInt"] = _rankInt;
	node["moraleRestored"] = _moraleRestored;
	if (getAIModule())
	{
		node["AI"] = getAIModule()->save();
	}
	node["killedBy"] = (int)_killedBy;
	if (_originalFaction != _faction)
		node["originalFaction"] = (int)_originalFaction;
	if (_kills)
		node["kills"] = _kills;
	if (_faction == FACTION_PLAYER && _dontReselect)
		node["dontReselect"] = _dontReselect;
	if (!_spawnUnit.empty())
		node["spawnUnit"] = _spawnUnit;
	node["motionPoints"] = _motionPoints;
	node["respawn"] = _respawn;
	node["activeHand"] = _activeHand;
	node["tempUnitStatistics"] = _statistics->save();
	node["murdererId"] = _murdererId;
	node["fatalShotSide"] = (int)_fatalShotSide;
	node["fatalShotBodyPart"] = (int)_fatalShotBodyPart;
	node["murdererWeapon"] = _murdererWeapon;
	node["murdererWeaponAmmo"] = _murdererWeaponAmmo;

	for (size_t i = 0; i < _recolor.size(); ++i)
	{
		YAML::Node p;
		p.push_back((int)_recolor[i].first);
		p.push_back((int)_recolor[i].second);
		node["recolor"].push_back(p);
	}
	node["mindControllerID"] = _mindControllerID;

	return node;
}

/**
 * Prepare vector values for recolor.
 * @param basicLook select index for hair and face color.
 * @param utileLook select index for utile color.
 * @param rankLook select index for rank color.
 */
void BattleUnit::setRecolor(int basicLook, int utileLook, int rankLook)
{
	const int colorsMax = 4;
	std::pair<int, int> colors[colorsMax] =
	{
		std::make_pair(_armor->getFaceColorGroup(), _armor->getFaceColor(basicLook)),
		std::make_pair(_armor->getHairColorGroup(), _armor->getHairColor(basicLook)),
		std::make_pair(_armor->getUtileColorGroup(), _armor->getUtileColor(utileLook)),
		std::make_pair(_armor->getRankColorGroup(), _armor->getRankColor(rankLook)),
	};

	for (int i = 0; i < colorsMax; ++i)
	{
		if (colors[i].first > 0 && colors[i].second > 0)
		{
			_recolor.push_back(std::make_pair(colors[i].first << 4, colors[i].second));
		}
	}
}

/**
 * Returns the BattleUnit's unique ID.
 * @return Unique ID.
 */
int BattleUnit::getId() const
{
	return _id;
}

/**
 * Changes the BattleUnit's position.
 * @param pos position
 * @param updateLastPos refresh last stored position
 */
void BattleUnit::setPosition(Position pos, bool updateLastPos)
{
	if (updateLastPos) { _lastPos = _pos; }
	_pos = pos;
}

/**
 * Gets the BattleUnit's position.
 * @return position
 */
Position BattleUnit::getPosition() const
{
	return _pos;
}

/**
 * Gets the BattleUnit's position.
 * @return position
 */
Position BattleUnit::getLastPosition() const
{
	return _lastPos;
}

/**
 * Gets the BattleUnit's destination.
 * @return destination
 */
Position BattleUnit::getDestination() const
{
	return _destination;
}

/**
 * Changes the BattleUnit's (horizontal) direction.
 * Only used for initial unit placement.
 * @param direction new horizontal direction
 */
void BattleUnit::setDirection(int direction)
{
	_direction = direction;
	_toDirection = direction;
	_directionTurret = direction;
}

/**
 * Changes the BattleUnit's (horizontal) face direction.
 * Only used for strafing moves.
 * @param direction new face direction
 */
void BattleUnit::setFaceDirection(int direction)
{
	_faceDirection = direction;
}

/**
 * Gets the BattleUnit's (horizontal) direction.
 * @return horizontal direction
 */
int BattleUnit::getDirection() const
{
	return _direction;
}

/**
 * Gets the BattleUnit's (horizontal) face direction.
 * Used only during strafing moves.
 * @return face direction
 */
int BattleUnit::getFaceDirection() const
{
	return _faceDirection;
}

/**
 * Gets the BattleUnit's turret direction.
 * @return direction
 */
int BattleUnit::getTurretDirection() const
{
	return _directionTurret;
}

/**
 * Gets the BattleUnit's turret To direction.
 * @return toDirectionTurret
 */
int BattleUnit::getTurretToDirection() const
{
	return _toDirectionTurret;
}

/**
 * Gets the BattleUnit's vertical direction. This is when going up or down.
 * @return direction
 */
int BattleUnit::getVerticalDirection() const
{
	return _verticalDirection;
}

/**
 * Gets the unit's status.
 * @return the unit's status
 */
UnitStatus BattleUnit::getStatus() const
{
	return _status;
}

/**
 * Initialises variables to start walking.
 * @param direction Which way to walk.
 * @param destination The position we should end up on.
 * @param tileBelowMe Which tile is currently below the unit.
 * @param cache Update cache?
 */
void BattleUnit::startWalking(int direction, Position destination, Tile *tileBelowMe, bool cache)
{
	if (direction >= Pathfinding::DIR_UP)
	{
		_verticalDirection = direction;
		_status = STATUS_FLYING;
	}
	else
	{
		_direction = direction;
		_status = STATUS_WALKING;
	}
	bool floorFound = false;
	if (!_tile->hasNoFloor(tileBelowMe))
	{
		floorFound = true;
	}
	if (!floorFound || direction >= Pathfinding::DIR_UP)
	{
		_status = STATUS_FLYING;
		_floating = true;
	}
	else
	{
		_floating = false;
	}

	_walkPhase = 0;
	_destination = destination;
	_lastPos = _pos;
	_cacheInvalid = cache;
	_kneeled = false;
	if (_breathFrame >= 0)
	{
		_breathing = false;
		_breathFrame = 0;
	}
}

/**
 * This will increment the walking phase.
 * @param tileBelowMe Pointer to tile currently below the unit.
 * @param cache Refresh the unit cache.
 */
void BattleUnit::keepWalking(Tile *tileBelowMe, bool cache)
{
	int middle, end;
	if (_verticalDirection)
	{
		middle = 4;
		end = 8;
	}
	else
	{
		// diagonal walking takes double the steps
		middle = 4 + 4 * (_direction % 2);
		end = 8 + 8 * (_direction % 2);
		if (_armor->getSize() > 1)
		{
			if (_direction < 1 || _direction > 5)
				middle = end;
			else if (_direction == 5)
				middle = 12;
			else if (_direction == 1)
				middle = 5;
			else
				middle = 1;
		}
	}
	if (!cache)
	{
		_pos = _destination;
		end = 2;
	}

	_walkPhase++;


	if (_walkPhase == middle)
	{
		// we assume we reached our destination tile
		// this is actually a drawing hack, so soldiers are not overlapped by floortiles
		_pos = _destination;
	}

	if (_walkPhase >= end)
	{
		if (_floating && !_tile->hasNoFloor(tileBelowMe))
		{
			_floating = false;
		}
		// we officially reached our destination tile
		_status = STATUS_STANDING;
		_walkPhase = 0;
		_verticalDirection = 0;
		if (_faceDirection >= 0) {
			// Finish strafing move facing the correct way.
			_direction = _faceDirection;
			_faceDirection = -1;
		}

		// motion points calculation for the motion scanner blips
		if (_armor->getSize() > 1)
		{
			_motionPoints += 30;
		}
		else
		{
			// sectoids actually have less motion points
			// but instead of create yet another variable,
			// I used the height of the unit instead (logical)
			if (getStandHeight() > 16)
				_motionPoints += 4;
			else
				_motionPoints += 3;
		}
	}

	_cacheInvalid = cache;
}

/**
 * Gets the walking phase for animation and sound.
 * @return phase will always go from 0-7
 */
int BattleUnit::getWalkingPhase() const
{
	return _walkPhase % 8;
}

/**
 * Gets the walking phase for diagonal walking.
 * @return phase this will be 0 or 8
 */
int BattleUnit::getDiagonalWalkingPhase() const
{
	return (_walkPhase / 8) * 8;
}

/**
 * Look at a point.
 * @param point Position to look at.
 * @param turret True to turn the turret, false to turn the unit.
 */
void BattleUnit::lookAt(Position point, bool turret)
{
	int dir = directionTo (point);

	if (turret)
	{
		_toDirectionTurret = dir;
		if (_toDirectionTurret != _directionTurret)
		{
			_status = STATUS_TURNING;
		}
	}
	else
	{
		_toDirection = dir;
		if (_toDirection != _direction
			&& _toDirection < 8
			&& _toDirection > -1)
		{
			_status = STATUS_TURNING;
		}
	}
}

/**
 * Look at a direction.
 * @param direction Direction to look at.
 * @param force True to reset the direction, false to animate to it.
 */
void BattleUnit::lookAt(int direction, bool force)
{
	if (!force)
	{
		if (direction < 0 || direction >= 8) return;
		_toDirection = direction;
		if (_toDirection != _direction)
		{
			_status = STATUS_TURNING;
		}
	}
	else
	{
		_toDirection = direction;
		_direction = direction;
	}
}

/**
 * Advances the turning towards the target direction.
 * @param turret True to turn the turret, false to turn the unit.
 */
void BattleUnit::turn(bool turret)
{
	int a = 0;

	if (turret)
	{
		if (_directionTurret == _toDirectionTurret)
		{
			abortTurn();
			return;
		}
		a = _toDirectionTurret - _directionTurret;
	}
	else
	{
		if (_direction == _toDirection)
		{
			abortTurn();
			return;
		}
		a = _toDirection - _direction;
	}

	if (a != 0) {
		if (a > 0) {
			if (a <= 4) {
				if (!turret) {
					if (_turretType > -1)
						_directionTurret++;
					_direction++;
				} else _directionTurret++;
			} else {
				if (!turret) {
					if (_turretType > -1)
						_directionTurret--;
					_direction--;
				} else _directionTurret--;
			}
		} else {
			if (a > -4) {
				if (!turret) {
					if (_turretType > -1)
						_directionTurret--;
					_direction--;
				} else _directionTurret--;
			} else {
				if (!turret) {
					if (_turretType > -1)
						_directionTurret++;
					_direction++;
				} else _directionTurret++;
			}
		}
		if (_direction < 0) _direction = 7;
		if (_direction > 7) _direction = 0;
		if (_directionTurret < 0) _directionTurret = 7;
		if (_directionTurret > 7) _directionTurret = 0;
		if (_visible || _faction == FACTION_PLAYER)
			_cacheInvalid = true;
	}

	if (turret)
	{
		 if (_toDirectionTurret == _directionTurret)
		 {
			// we officially reached our destination
			_status = STATUS_STANDING;
		 }
	}
	else if (_toDirection == _direction || _status == STATUS_UNCONSCIOUS)
	{
		// we officially reached our destination
		_status = STATUS_STANDING;
	}
}

/**
 * Stops the turning towards the target direction.
 */
void BattleUnit::abortTurn()
{
	_status = STATUS_STANDING;
}


/**
 * Gets the soldier's gender.
 */
SoldierGender BattleUnit::getGender() const
{
	return _gender;
}

/**
 * Returns the unit's faction.
 * @return Faction. (player, hostile or neutral)
 */
UnitFaction BattleUnit::getFaction() const
{
	return _faction;
}

/**
 * Sets the unit's cache flag.
 * @param cache Pointer to cache surface to use, NULL to redraw from scratch.
 * @param part Unit part to cache.
 */
void BattleUnit::setCache(Surface *cache, int part)
{
	if (cache == 0)
	{
		_cacheInvalid = true;
	}
	else
	{
		_cache[part] = cache;
		_cacheInvalid = false;
	}
}

/**
 * Returns the current cache surface.
 * When the unit changes it needs to be re-cached.
 * @param part Unit part to check.
 * @return Pointer to cache surface used.
 */
Surface *BattleUnit::getCache(int part) const
{
	if (part < 0) part = 0;
	return _cache[part];
}

/**
 * Check if the unit is still cached in the Map cache.
 * When the unit changes it needs to be re-cached.
 * @return True if it needs to be re-cached.
 */
bool BattleUnit::isCacheInvalid() const
{
	return _cacheInvalid;
}

/**
 * Gets values used for recoloring sprites.
 * @param i what value choose.
 * @return Pairs of value, where first is color group to replace and second is new color group with shade.
 */
const std::vector<std::pair<Uint8, Uint8> > &BattleUnit::getRecolor() const
{
	return _recolor;
}

/**
 * Kneel down.
 * @param kneeled to kneel or to stand up
 */
void BattleUnit::kneel(bool kneeled)
{
	_kneeled = kneeled;
	_cacheInvalid = true;
}

/**
 * Is kneeled down?
 * @return true/false
 */
bool BattleUnit::isKneeled() const
{
	return _kneeled;
}

/**
 * Is floating? A unit is floating when there is no ground under him/her.
 * @return true/false
 */
bool BattleUnit::isFloating() const
{
	return _floating;
}

/**
 * Aim. (shows the right hand sprite and weapon holding)
 * @param aiming true/false
 */
void BattleUnit::aim(bool aiming)
{
	if (aiming)
		_status = STATUS_AIMING;
	else
		_status = STATUS_STANDING;

	if (_visible || _faction == FACTION_PLAYER)
		_cacheInvalid = true;
}

/**
 * Returns the direction from this unit to a given point.
 * 0 <-> y = -1, x = 0
 * 1 <-> y = -1, x = 1
 * 3 <-> y = 1, x = 1
 * 5 <-> y = 1, x = -1
 * 7 <-> y = -1, x = -1
 * @param point given position.
 * @return direction.
 */
int BattleUnit::directionTo(Position point) const
{
	double ox = point.x - _pos.x;
	double oy = point.y - _pos.y;
	double angle = atan2(ox, -oy);
	// divide the pie in 4 angles each at 1/8th before each quarter
	double pie[4] = {(M_PI_4 * 4.0) - M_PI_4 / 2.0, (M_PI_4 * 3.0) - M_PI_4 / 2.0, (M_PI_4 * 2.0) - M_PI_4 / 2.0, (M_PI_4 * 1.0) - M_PI_4 / 2.0};
	int dir = 0;

	if (angle > pie[0] || angle < -pie[0])
	{
		dir = 4;
	}
	else if (angle > pie[1])
	{
		dir = 3;
	}
	else if (angle > pie[2])
	{
		dir = 2;
	}
	else if (angle > pie[3])
	{
		dir = 1;
	}
	else if (angle < -pie[1])
	{
		dir = 5;
	}
	else if (angle < -pie[2])
	{
		dir = 6;
	}
	else if (angle < -pie[3])
	{
		dir = 7;
	}
	else if (angle < pie[0])
	{
		dir = 0;
	}
	return dir;
}

/**
 * Returns the soldier's amount of time units.
 * @return Time units.
 */
int BattleUnit::getTimeUnits() const
{
	return _tu;
}

/**
 * Returns the soldier's amount of energy.
 * @return Energy.
 */
int BattleUnit::getEnergy() const
{
	return _energy;
}

/**
 * Returns the soldier's amount of health.
 * @return Health.
 */
int BattleUnit::getHealth() const
{
	return _health;
}

/**
 * Returns the soldier's amount of morale.
 * @return Morale.
 */
int BattleUnit::getMorale() const
{
	return _morale;
}

/**
 * Do an amount of damage.
 * @param relative The relative position of which part of armor and/or bodypart is hit.
 * @param power The amount of damage to inflict.
 * @param type The type of damage being inflicted.
 * @param ignoreArmor Should the damage ignore armor resistance?
 * @return damage done after adjustment
 */
int BattleUnit::damage(Position relative, int power, ItemDamageType type, bool ignoreArmor)
{
	UnitSide side = SIDE_FRONT;
	UnitBodyPart bodypart = BODYPART_TORSO;
	_hitByAnything = true;
	if (power <= 0)
	{
		return 0;
	}

	power = (int)floor(power * _armor->getDamageModifier(type));

	if (type == DT_SMOKE) type = DT_STUN; // smoke doesn't do real damage, but stun damage

	if (!ignoreArmor)
	{
		if (relative == Position(0, 0, 0))
		{
			side = SIDE_UNDER;
		}
		else
		{
			int relativeDirection;
			const int abs_x = abs(relative.x);
			const int abs_y = abs(relative.y);
			if (abs_y > abs_x * 2)
				relativeDirection = 8 + 4 * (relative.y > 0);
			else if (abs_x > abs_y * 2)
				relativeDirection = 10 + 4 * (relative.x < 0);
			else
			{
				if (relative.x < 0)
				{
					if (relative.y > 0)
						relativeDirection = 13;
					else
						relativeDirection = 15;
				}
				else
				{
					if (relative.y > 0)
						relativeDirection = 11;
					else
						relativeDirection = 9;
				}
			}

			switch((relativeDirection - _direction) % 8)
			{
			case 0:	side = SIDE_FRONT; 										break;
			case 1:	side = RNG::generate(0,2) < 2 ? SIDE_FRONT:SIDE_RIGHT; 	break;
			case 2:	side = SIDE_RIGHT; 										break;
			case 3:	side = RNG::generate(0,2) < 2 ? SIDE_REAR:SIDE_RIGHT; 	break;
			case 4:	side = SIDE_REAR; 										break;
			case 5:	side = RNG::generate(0,2) < 2 ? SIDE_REAR:SIDE_LEFT; 	break;
			case 6:	side = SIDE_LEFT; 										break;
			case 7:	side = RNG::generate(0,2) < 2 ? SIDE_FRONT:SIDE_LEFT; 	break;
			}
			if (relative.z >= getHeight())
			{
				bodypart = BODYPART_HEAD;
			}
			else if (relative.z > 4)
			{
				switch(side)
				{
				case SIDE_LEFT:		bodypart = BODYPART_LEFTARM; break;
				case SIDE_RIGHT:	bodypart = BODYPART_RIGHTARM; break;
				default:			bodypart = BODYPART_TORSO;
				}
			}
			else
			{
				switch(side)
				{
				case SIDE_LEFT: 	bodypart = BODYPART_LEFTLEG; 	break;
				case SIDE_RIGHT:	bodypart = BODYPART_RIGHTLEG; 	break;
				default:
					bodypart = (UnitBodyPart) RNG::generate(BODYPART_RIGHTLEG,BODYPART_LEFTLEG);
				}
			}
		}
		power -= getArmor(side);
	}

	if (power > 0)
	{
		if (type == DT_STUN)
		{
			_stunlevel += power;
		}
		else
		{
			// health damage
			_health -= power;
			if (_health < 0)
			{
				_health = 0;
			}

			if (type != DT_IN)
			{
				if (_armor->getDamageModifier(DT_STUN) > 0.0)
				{
					// conventional weapons can cause additional stun damage
					_stunlevel += RNG::generate(0, power / 4);
				}
				// fatal wounds
				if (isWoundable())
				{
					if (RNG::generate(0, 10) < power)
						_fatalWounds[bodypart] += RNG::generate(1,3);

					if (_fatalWounds[bodypart])
						moraleChange(-_fatalWounds[bodypart]);
				}
				// armor damage
				setArmor(getArmor(side) - (power/10) - 1, side);
			}
		}
	}

	setFatalShotInfo(side, bodypart);

	return power < 0 ? 0:power;
}

/**
 * Do an amount of stun recovery.
 * @param power
 */
void BattleUnit::healStun(int power)
{
	_stunlevel -= power;
	if (_stunlevel < 0) _stunlevel = 0;
}

int BattleUnit::getStunlevel() const
{
	return _stunlevel;
}

/**
 * Raises a unit's stun level sufficiently so that the unit is ready to become unconscious.
 * Used when another unit falls on top of this unit.
 * Zombified units first convert to their spawn unit.
 * @param battle Pointer to the battlescape game.
 */
void BattleUnit::knockOut(BattlescapeGame *battle)
{
	if (!_spawnUnit.empty())
	{
		setRespawn(false);
		BattleUnit *newUnit = battle->convertUnit(this);
		newUnit->knockOut(battle);
	}
	else
	{
		_stunlevel = _health;
	}
}

/**
 * Initialises the falling sequence. Occurs after death or stunned.
 */
void BattleUnit::startFalling()
{
	_status = STATUS_COLLAPSING;
	_fallPhase = 0;
	_cacheInvalid = true;
}

/**
 * Advances the phase of falling sequence.
 */
void BattleUnit::keepFalling()
{
	_fallPhase++;
	if (_fallPhase == _armor->getDeathFrames())
	{
		_fallPhase--;
		if (_health == 0)
		{
			_status = STATUS_DEAD;
		}
		else
			_status = STATUS_UNCONSCIOUS;
	}

	_cacheInvalid = true;
}


/**
 * Returns the phase of the falling sequence.
 * @return phase
 */
int BattleUnit::getFallingPhase() const
{
	return _fallPhase;
}

/**
 * Returns whether the soldier is out of combat, dead or unconscious.
 * A soldier that is out, cannot perform any actions, cannot be selected, but it's still a unit.
 * @return flag if out or not.
 */
bool BattleUnit::isOut() const
{
	return _status == STATUS_DEAD || _status == STATUS_UNCONSCIOUS || _status == STATUS_IGNORE_ME;
}

/**
 * Get the number of time units a certain action takes.
 * @param actionType
 * @param item
 * @return TUs
 */
int BattleUnit::getActionTUs(BattleActionType actionType, BattleItem *item)
{
	if (item == 0)
	{
		return 0;
	}
	return getActionTUs(actionType, item->getRules());
}

int BattleUnit::getActionTUs(BattleActionType actionType, RuleItem *item)
{
	if (item == 0)
	{
		return 0;
	}

	int cost = 0;
	switch (actionType)
	{
		case BA_PRIME:
			cost = 50; // maybe this should go in the ruleset
			break;
		case BA_THROW:
			cost = 25;
			break;
		case BA_AUTOSHOT:
			cost = item->getTUAuto();
			break;
		case BA_SNAPSHOT:
			cost = item->getTUSnap();
			break;
		case BA_HIT:
			cost = item->getTUMelee();
			break;
		case BA_LAUNCH:
		case BA_AIMEDSHOT:
			cost = item->getTUAimed();
			break;
		case BA_USE:
		case BA_MINDCONTROL:
		case BA_PANIC:
			cost = item->getTUUse();
			break;
		default:
			cost = 0;
	}

	// if it's a percentage, apply it to unit TUs
	if (!item->getFlatRate() || actionType == BA_THROW || actionType == BA_PRIME)
	{
		cost = (int)floor(getBaseStats()->tu * cost / 100.0f);
	}

	return cost;
}


/**
 * Spend time units if it can. Return false if it can't.
 * @param tu
 * @return flag if it could spend the time units or not.
 */
bool BattleUnit::spendTimeUnits(int tu)
{
	if (tu <= _tu)
	{
		_tu -= tu;
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * Spend energy  if it can. Return false if it can't.
 * @param tu
 * @return flag if it could spend the time units or not.
 */
bool BattleUnit::spendEnergy(int tu)
{
	int eu = tu / 2;

	if (eu <= _energy)
	{
		_energy -= eu;
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * Set a specific number of timeunits.
 * @param tu
 */
void BattleUnit::setTimeUnits(int tu)
{
	_tu = std::max(0, tu);
}

/**
 * Add this unit to the list of visible units. Returns true if this is a new one.
 * @param unit
 * @return
 */
bool BattleUnit::addToVisibleUnits(BattleUnit *unit)
{
	bool add = true;
	for (std::vector<BattleUnit*>::iterator i = _unitsSpottedThisTurn.begin(); i != _unitsSpottedThisTurn.end();++i)
	{
		if ((BattleUnit*)(*i) == unit)
		{
			add = false;
			break;
		}
	}
	if (add)
	{
		_unitsSpottedThisTurn.push_back(unit);
	}
	for (std::vector<BattleUnit*>::iterator i = _visibleUnits.begin(); i != _visibleUnits.end(); ++i)
	{
		if ((BattleUnit*)(*i) == unit)
		{
			return false;
		}
	}
	_visibleUnits.push_back(unit);
	return true;
}

/**
 * Get the pointer to the vector of visible units.
 * @return pointer to vector.
 */
std::vector<BattleUnit*> *BattleUnit::getVisibleUnits()
{
	return &_visibleUnits;
}

/**
 * Clear visible units.
 */
void BattleUnit::clearVisibleUnits()
{
	_visibleUnits.clear();
}

/**
 * Add this unit to the list of visible tiles. Returns true if this is a new one.
 * @param tile
 * @return
 */
bool BattleUnit::addToVisibleTiles(Tile *tile)
{
	_visibleTiles.push_back(tile);
	return true;
}

/**
 * Get the pointer to the vector of visible tiles.
 * @return pointer to vector.
 */
std::vector<Tile*> *BattleUnit::getVisibleTiles()
{
	return &_visibleTiles;
}

/**
 * Clear visible tiles.
 */
void BattleUnit::clearVisibleTiles()
{
	for (std::vector<Tile*>::iterator j = _visibleTiles.begin(); j != _visibleTiles.end(); ++j)
	{
		(*j)->setVisible(-1);
	}

	_visibleTiles.clear();
}

/**
 * Calculate firing accuracy.
 * Formula = accuracyStat * weaponAccuracy * kneelingbonus(1.15) * one-handPenalty(0.8) * woundsPenalty(% health) * critWoundsPenalty (-10%/wound)
 * @param actionType
 * @param item
 * @return firing Accuracy
 */
int BattleUnit::getFiringAccuracy(BattleActionType actionType, BattleItem *item)
{

	int weaponAcc = item->getRules()->getAccuracySnap();
	if (actionType == BA_AIMEDSHOT || actionType == BA_LAUNCH)
		weaponAcc = item->getRules()->getAccuracyAimed();
	else if (actionType == BA_AUTOSHOT)
		weaponAcc = item->getRules()->getAccuracyAuto();
	else if (actionType == BA_HIT)
	{
		if (item->getRules()->isSkillApplied())
		{
			return (getBaseStats()->melee * item->getRules()->getAccuracyMelee() / 100) * getAccuracyModifier(item) / 100;
		}
		return item->getRules()->getAccuracyMelee() * getAccuracyModifier(item) / 100;
	}

	int result = getBaseStats()->firing * weaponAcc / 100;

	if (_kneeled)
	{
		result = result * 115 / 100;
	}

	if (item->getRules()->isTwoHanded())
	{
		// two handed weapon, means one hand should be empty
		if (getItem("STR_RIGHT_HAND") != 0 && getItem("STR_LEFT_HAND") != 0)
		{
			result = result * 80 / 100;
		}
	}

	return result * getAccuracyModifier(item) / 100;
}

/**
 * To calculate firing accuracy. Takes health and fatal wounds into account.
 * Formula = accuracyStat * woundsPenalty(% health) * critWoundsPenalty (-10%/wound)
 * @param item the item we are shooting right now.
 * @return modifier
 */
int BattleUnit::getAccuracyModifier(BattleItem *item)
{
	int wounds = _fatalWounds[BODYPART_HEAD];

	if (item)
	{
		if (item->getRules()->isTwoHanded())
		{
			wounds += _fatalWounds[BODYPART_RIGHTARM] + _fatalWounds[BODYPART_LEFTARM];
		}
		else
		{
			if (getItem("STR_RIGHT_HAND") == item)
			{
				wounds += _fatalWounds[BODYPART_RIGHTARM];
			}
			else
			{
				wounds += _fatalWounds[BODYPART_LEFTARM];
			}
		}
	}
	return std::max(10, 25 * _health / getBaseStats()->health + 75 + -10 * wounds);
}

/**
 * Calculate throwing accuracy.
 * @return throwing Accuracy
 */
double BattleUnit::getThrowingAccuracy()
{
	return (double)(getBaseStats()->throwing * getAccuracyModifier()) / 100.0;
}

/**
 * Set the armor value of a certain armor side.
 * @param armor Amount of armor.
 * @param side The side of the armor.
 */
void BattleUnit::setArmor(int armor, UnitSide side)
{
	if (armor < 0)
	{
		armor = 0;
	}
	_currentArmor[side] = armor;
}

/**
 * Get the armor value of a certain armor side.
 * @param side The side of the armor.
 * @return Amount of armor.
 */
int BattleUnit::getArmor(UnitSide side) const
{
	return _currentArmor[side];
}

/**
 * Get the max armor value of a certain armor side.
 * @param side The side of the armor.
 * @return Amount of armor.
 */
int BattleUnit::getMaxArmor(UnitSide side) const
{
	return _maxArmor[side];
}

/**
 * Get total amount of fatal wounds this unit has.
 * @return Number of fatal wounds.
 */
int BattleUnit::getFatalWounds() const
{
	int sum = 0;
	for (int i = 0; i < 6; ++i)
		sum += _fatalWounds[i];
	return sum;
}


/**
 * Little formula to calculate reaction score.
 * @return Reaction score.
 */
double BattleUnit::getReactionScore()
{
	//(Reactions Stat) x (Current Time Units / Max TUs)
	double score = ((double)getBaseStats()->reactions * (double)getTimeUnits()) / (double)getBaseStats()->tu;
	return score;
}


/**
 * Prepare for a new turn.
 */
void BattleUnit::prepareNewTurn(bool fullProcess)
{
	if (_status == STATUS_IGNORE_ME)
	{
		return;
	}


	_unitsSpottedThisTurn.clear();

	// revert to original faction
	// don't give it back its TUs or anything this round
	// because it's no longer a unit of the team getting TUs back
	if (_faction != _originalFaction)
	{
		_faction = _originalFaction;
		if (_faction == FACTION_PLAYER && _currentAIState)
		{
			delete _currentAIState;
			_currentAIState = 0;
		}
	}
	else
	{
		recoverTimeUnits();
	}
	_dontReselect = false;

	_motionPoints = 0;

	// transition between stages, don't do damage or panic
	if (!fullProcess)
	{
		if (_kneeled)
		{
			// stand up if kneeling
			_kneeled = false;
		}
		return;
	}

	// suffer from fatal wounds
	_health -= getFatalWounds();

	// suffer from fire
	if (!_hitByFire && _fire > 0)
	{
		_health -= _armor->getDamageModifier(DT_IN) * RNG::generate(Mod::FIRE_DAMAGE_RANGE[0], Mod::FIRE_DAMAGE_RANGE[1]);
		_fire--;
	}

	if (_health < 0)
		_health = 0;

	// if unit is dead, AI state should be gone
	if (_health == 0 && _currentAIState)
	{
		delete _currentAIState;
		_currentAIState = 0;
	}

	// recover stun 1pt/turn
	if (_stunlevel > 0 &&
		(_armor->getSize() == 1 || !isOut()))
		healStun(1);

	if (!isOut())
	{
		int chance = 100 - (2 * getMorale());
		if (RNG::generate(1,100) <= chance)
		{
			int type = RNG::generate(0,100);
			_status = (type<=33?STATUS_BERSERK:STATUS_PANICKING); // 33% chance of berserk, panic can mean freeze or flee, but that is determined later
		}
		else
		{
			// successfully avoided panic
			// increase bravery experience counter
			if (chance > 1)
				_expBravery++;
		}
	}
	_hitByFire = false;
}


/**
 * Morale change with bounds check.
 * @param change can be positive or negative
 */
void BattleUnit::moraleChange(int change)
{
	if (!isFearable()) return;

	_morale += change;
	if (_morale > 100)
		_morale = 100;
	if (_morale < 0)
		_morale = 0;
}

/**
 * Mark this unit as not reselectable.
 */
void BattleUnit::dontReselect()
{
	_dontReselect = true;
}

/**
 * Mark this unit as reselectable.
 */
void BattleUnit::allowReselect()
{
	_dontReselect = false;
}


/**
 * Check whether reselecting this unit is allowed.
 * @return bool
 */
bool BattleUnit::reselectAllowed() const
{
	return !_dontReselect;
}

/**
 * Set the amount of turns this unit is on fire. 0 = no fire.
 * @param fire : amount of turns this tile is on fire.
 */
void BattleUnit::setFire(int fire)
{
	if (_specab != SPECAB_BURNFLOOR && _specab != SPECAB_BURN_AND_EXPLODE)
		_fire = fire;
}

/**
 * Get the amount of turns this unit is on fire. 0 = no fire.
 * @return fire : amount of turns this tile is on fire.
 */
int BattleUnit::getFire() const
{
	return _fire;
}

/**
 * Get the pointer to the vector of inventory items.
 * @return pointer to vector.
 */
std::vector<BattleItem*> *BattleUnit::getInventory()
{
	return &_inventory;
}

/**
 * Let AI do their thing.
 * @param action AI action.
 */
void BattleUnit::think(BattleAction *action)
{
	checkAmmo();
	_currentAIState->think(action);
}

/**
 * Changes the current AI state.
 * @param aiState Pointer to AI state.
 */
void BattleUnit::setAIModule(AIModule *ai)
{
	if (_currentAIState)
	{
		delete _currentAIState;
	}
	_currentAIState = ai;
}

/**
 * Returns the current AI state.
 * @return Pointer to AI state.
 */
AIModule *BattleUnit::getAIModule() const
{
	return _currentAIState;
}

/**
 * Set whether this unit is visible.
 * @param flag
 */
void BattleUnit::setVisible(bool flag)
{
	_visible = flag;
}


/**
 * Get whether this unit is visible.
 * @return flag
 */
bool BattleUnit::getVisible() const
{
	if (getFaction() == FACTION_PLAYER)
	{
		return true;
	}
	else
	{
		return _visible;
	}
}

/**
 * Sets the unit's tile it's standing on
 * @param tile Pointer to tile.
 * @param tileBelow Pointer to tile below.
 */
void BattleUnit::setTile(Tile *tile, Tile *tileBelow)
{
	_tile = tile;
	if (!_tile)
	{
		_floating = false;
		return;
	}
	// unit could have changed from flying to walking or vice versa
	if (_status == STATUS_WALKING && _tile->hasNoFloor(tileBelow) && _movementType == MT_FLY)
	{
		_status = STATUS_FLYING;
		_floating = true;
	}
	else if (_status == STATUS_FLYING && !_tile->hasNoFloor(tileBelow) && _verticalDirection == 0)
	{
		_status = STATUS_WALKING;
		_floating = false;
	}
	else if (_status == STATUS_UNCONSCIOUS)
	{
		_floating = _movementType == MT_FLY && _tile->hasNoFloor(tileBelow);
	}
}

/**
 * Gets the unit's tile.
 * @return Tile
 */
Tile *BattleUnit::getTile() const
{
	return _tile;
}

/**
 * Checks if there's an inventory item in
 * the specified inventory position.
 * @param slot Inventory slot.
 * @param x X position in slot.
 * @param y Y position in slot.
 * @return Item in the slot, or NULL if none.
 */
BattleItem *BattleUnit::getItem(RuleInventory *slot, int x, int y) const
{
	// Soldier items
	if (slot->getType() != INV_GROUND)
	{
		for (std::vector<BattleItem*>::const_iterator i = _inventory.begin(); i != _inventory.end(); ++i)
		{
			if ((*i)->getSlot() == slot && (*i)->occupiesSlot(x, y))
			{
				return *i;
			}
		}
	}
	// Ground items
	else if (_tile != 0)
	{
		for (std::vector<BattleItem*>::const_iterator i = _tile->getInventory()->begin(); i != _tile->getInventory()->end(); ++i)
		{
			if ((*i)->occupiesSlot(x, y))
			{
				return *i;
			}
		}
	}
	return 0;
}

/**
 * Checks if there's an inventory item in
 * the specified inventory position.
 * @param slot Inventory slot.
 * @param x X position in slot.
 * @param y Y position in slot.
 * @return Item in the slot, or NULL if none.
 */
BattleItem *BattleUnit::getItem(const std::string &slot, int x, int y) const
{
	// Soldier items
	if (slot != "STR_GROUND")
	{
		for (std::vector<BattleItem*>::const_iterator i = _inventory.begin(); i != _inventory.end(); ++i)
		{
			if ((*i)->getSlot() != 0 && (*i)->getSlot()->getId() == slot && (*i)->occupiesSlot(x, y))
			{
				return *i;
			}
		}
	}
	// Ground items
	else if (_tile != 0)
	{
		for (std::vector<BattleItem*>::const_iterator i = _tile->getInventory()->begin(); i != _tile->getInventory()->end(); ++i)
		{
			if ((*i)->getSlot() != 0 && (*i)->occupiesSlot(x, y))
			{
				return *i;
			}
		}
	}
	return 0;
}

/**
 * Get the "main hand weapon" from the unit.
 * @param quickest Whether to get the quickest weapon, default true
 * @return Pointer to item.
 */
BattleItem *BattleUnit::getMainHandWeapon(bool quickest) const
{
	BattleItem *weaponRightHand = getItem("STR_RIGHT_HAND");
	BattleItem *weaponLeftHand = getItem("STR_LEFT_HAND");

	// ignore weapons without ammo (rules out grenades)
	if (!weaponRightHand || !weaponRightHand->getAmmoItem() || !weaponRightHand->getAmmoItem()->getAmmoQuantity())
		weaponRightHand = 0;
	if (!weaponLeftHand || !weaponLeftHand->getAmmoItem() || !weaponLeftHand->getAmmoItem()->getAmmoQuantity())
		weaponLeftHand = 0;

	// if there is only one weapon, it's easy:
	if (weaponRightHand && !weaponLeftHand)
		return weaponRightHand;
	else if (!weaponRightHand && weaponLeftHand)
		return weaponLeftHand;
	else if (!weaponRightHand && !weaponLeftHand)
		return 0;

	// otherwise pick the one with the least snapshot TUs
	int tuRightHand = weaponRightHand->getRules()->getTUSnap();
	int tuLeftHand = weaponLeftHand->getRules()->getTUSnap();
	BattleItem *weaponCurrentHand = getItem(getActiveHand());
	//prioritize blasters
	if (!quickest && _faction != FACTION_PLAYER)
	{
		if (weaponRightHand->getRules()->getWaypoints() != 0 || weaponRightHand->getAmmoItem()->getRules()->getWaypoints() != 0)
		{
			return weaponRightHand;
		}
		if (weaponLeftHand->getRules()->getWaypoints() != 0 || weaponLeftHand->getAmmoItem()->getRules()->getWaypoints() != 0)
		{
			return weaponLeftHand;
		}
	}
	// if only one weapon has snapshot, pick that one
	if (tuLeftHand <= 0 && tuRightHand > 0)
		return weaponRightHand;
	else if (tuRightHand <= 0 && tuLeftHand > 0)
		return weaponLeftHand;
	// else pick the better one
	else
	{
		if (tuLeftHand >= tuRightHand)
		{
			if (quickest)
			{
				return weaponRightHand;
			}
			else if (_faction == FACTION_PLAYER)
			{
				return weaponCurrentHand;
			}
			else
			{
				return weaponLeftHand;
			}
		}
		else
		{
			if (quickest)
			{
				return weaponLeftHand;
			}
			else if (_faction == FACTION_PLAYER)
			{
				return weaponCurrentHand;
			}
			else
			{
				return weaponRightHand;
			}
		}
	}
}

/**
 * Get a grenade from the belt (used for AI)
 * @return Pointer to item.
 */
BattleItem *BattleUnit::getGrenadeFromBelt() const
{
	for (std::vector<BattleItem*>::const_iterator i = _inventory.begin(); i != _inventory.end(); ++i)
	{
		if ((*i)->getRules()->getBattleType() == BT_GRENADE)
		{
			return *i;
		}
	}
	return 0;
}

/**
 * Check if we have ammo and reload if needed (used for AI).
 * @return Do we have ammo?
 */
bool BattleUnit::checkAmmo()
{
	BattleItem *weapon = getItem("STR_RIGHT_HAND");
	if (!weapon || weapon->getAmmoItem() != 0 || weapon->getRules()->getBattleType() == BT_MELEE || getTimeUnits() < 15)
	{
		weapon = getItem("STR_LEFT_HAND");
		if (!weapon || weapon->getAmmoItem() != 0 || weapon->getRules()->getBattleType() == BT_MELEE || getTimeUnits() < 15)
		{
			return false;
		}
	}
	// we have a non-melee weapon with no ammo and 15 or more TUs - we might need to look for ammo then
	BattleItem *ammo = 0;
	bool wrong = true;
	for (std::vector<BattleItem*>::iterator i = getInventory()->begin(); i != getInventory()->end(); ++i)
	{
		ammo = (*i);
		for (std::vector<std::string>::iterator c = weapon->getRules()->getCompatibleAmmo()->begin(); c != weapon->getRules()->getCompatibleAmmo()->end(); ++c)
		{
			if ((*c) == ammo->getRules()->getType())
			{
				wrong = false;
				break;
			}
		}
		if (!wrong) break;
	}

	if (wrong) return false; // didn't find any compatible ammo in inventory

	spendTimeUnits(15);
	weapon->setAmmoItem(ammo);
	ammo->moveToOwner(0);

	return true;
}

/**
 * Check if this unit is in the exit area.
 * @param stt Type of exit tile to check for.
 * @return Is in the exit area?
 */
bool BattleUnit::isInExitArea(SpecialTileType stt) const
{
	return _tile && _tile->getMapData(O_FLOOR) && (_tile->getMapData(O_FLOOR)->getSpecialType() == stt);
}

/**
 * Check if this unit lies (e.g. unconscious) in the exit area.
 * @param tile Unit's location.
 * @param stt Type of exit tile to check for.
 * @return Is in the exit area?
 */
bool BattleUnit::liesInExitArea(Tile *tile, SpecialTileType stt) const
{
	return tile && tile->getMapData(O_FLOOR) && (tile->getMapData(O_FLOOR)->getSpecialType() == stt);
}

/**
 * Gets the unit height taking into account kneeling/standing.
 * @return Unit's height.
 */
int BattleUnit::getHeight() const
{
	return isKneeled()?getKneelHeight():getStandHeight();
}

/**
 * Adds one to the reaction exp counter.
 */
void BattleUnit::addReactionExp()
{
	_expReactions++;
}

/**
 * Adds one to the firing exp counter.
 */
void BattleUnit::addFiringExp()
{
	_expFiring++;
}

/**
 * Adds one to the throwing exp counter.
 */
void BattleUnit::addThrowingExp()
{
	_expThrowing++;
}

/**
 * Adds one to the psi skill exp counter.
 */
void BattleUnit::addPsiSkillExp()
{
	_expPsiSkill++;
}

/**
 * Adds one to the psi strength exp counter.
 */
void BattleUnit::addPsiStrengthExp()
{
	_expPsiStrength++;
}

/**
 * Adds one to the melee exp counter.
 */
void BattleUnit::addMeleeExp()
{
	_expMelee++;
}

void BattleUnit::updateGeoscapeStats(Soldier *soldier) const
{
	soldier->addMissionCount();
	soldier->addKillCount(_kills);
}

/**
 * Check if unit eligible for squaddie promotion. If yes, promote the unit.
 * Increase the mission counter. Calculate the experience increases.
 * @param geoscape Pointer to geoscape save.
 * @param statsDiff (out) The passed UnitStats struct will be filled with the stats differences.
 * @return True if the soldier was eligible for squaddie promotion.
 */
bool BattleUnit::postMissionProcedures(SavedGame *geoscape, UnitStats &statsDiff)
{
	Soldier *s = geoscape->getSoldier(_id);
	if (s == 0)
	{
		return false;
	}

	updateGeoscapeStats(s);

	UnitStats *stats = s->getCurrentStats();
	statsDiff -= *stats;        // subtract old stats
	const UnitStats caps = s->getRules()->getStatCaps();
	int healthLoss = _stats.health - _health;

	s->setWoundRecovery(RNG::generate((healthLoss*0.5),(healthLoss*1.5)));

	if (_expBravery && stats->bravery < caps.bravery)
	{
		if (_expBravery > RNG::generate(0,10)) stats->bravery += 10;
	}
	if (_expReactions && stats->reactions < caps.reactions)
	{
		stats->reactions += improveStat(_expReactions);
	}
	if (_expFiring && stats->firing < caps.firing)
	{
		stats->firing += improveStat(_expFiring);
	}
	if (_expMelee && stats->melee < caps.melee)
	{
		stats->melee += improveStat(_expMelee);
	}
	if (_expThrowing && stats->throwing < caps.throwing)
	{
		stats->throwing += improveStat(_expThrowing);
	}
	if (_expPsiSkill && stats->psiSkill < caps.psiSkill)
	{
		stats->psiSkill += improveStat(_expPsiSkill);
	}
	if (_expPsiStrength && stats->psiStrength < caps.psiStrength)
	{
		stats->psiStrength += improveStat(_expPsiStrength);
	}

	bool hasImproved = false;
	if (_expBravery || _expReactions || _expFiring || _expPsiSkill || _expPsiStrength || _expMelee)
	{
		hasImproved = true;
		if (s->getRank() == RANK_ROOKIE)
			s->promoteRank();
		int v;
		v = caps.tu - stats->tu;
		if (v > 0) stats->tu += RNG::generate(0, v/10 + 2);
		v = caps.health - stats->health;
		if (v > 0) stats->health += RNG::generate(0, v/10 + 2);
		v = caps.strength - stats->strength;
		if (v > 0) stats->strength += RNG::generate(0, v/10 + 2);
		v = caps.stamina - stats->stamina;
		if (v > 0) stats->stamina += RNG::generate(0, v/15 + 2);
	}

	statsDiff += *stats; // add new stats

	return hasImproved;
	}

/**
 * Converts the number of experience to the stat increase.
 * @param Experience counter.
 * @return Stat increase.
 */
int BattleUnit::improveStat(int exp) const
{
	if      (exp > 10) return RNG::generate(2, 6);
	else if (exp > 5)  return RNG::generate(1, 4);
	else if (exp > 2)  return RNG::generate(1, 3);
	else if (exp > 0)  return RNG::generate(0, 1);
	else               return 0;
}

/**
 * Get the unit's minimap sprite index. Used to display the unit on the minimap
 * @return the unit minimap index
 */
int BattleUnit::getMiniMapSpriteIndex() const
{
	//minimap sprite index:
	// * 0-2   : Xcom soldier
	// * 3-5   : Alien
	// * 6-8   : Civilian
	// * 9-11  : Item
	// * 12-23 : Xcom HWP
	// * 24-35 : Alien big terror unit(cyberdisk, ...)
	if (isOut())
	{
		return 9;
	}
	switch (getFaction())
	{
	case FACTION_HOSTILE:
		if (_armor->getSize() == 1)
			return 3;
		else
			return 24;
	case FACTION_NEUTRAL:
		if (_armor->getSize() == 1)
			return 6;
		else
			return 12;
	default:
		if (_armor->getSize() == 1)
			return 0;
		else
			return 12;
	}
}

/**
  * Set the turret type. -1 is no turret.
  * @param turretType
  */
void BattleUnit::setTurretType(int turretType)
{
	_turretType = turretType;
}

/**
  * Get the turret type. -1 is no turret.
  * @return type
  */
int BattleUnit::getTurretType() const
{
	return _turretType;
}

/**
 * Get the amount of fatal wound for a body part
 * @param part The body part (in the range 0-5)
 * @return The amount of fatal wound of a body part
 */
int BattleUnit::getFatalWound(int part) const
{
	if (part < 0 || part > 5)
		return 0;
	return _fatalWounds[part];
}

/**
 * Heal a fatal wound of the soldier
 * @param part the body part to heal
 * @param woundAmount the amount of fatal wound healed
 * @param healthAmount The amount of health to add to soldier health
 */
void BattleUnit::heal(int part, int woundAmount, int healthAmount)
{
	if (part < 0 || part > 5 || !_fatalWounds[part])
	{
		return;
	}

	_fatalWounds[part] -= woundAmount;
	if (_fatalWounds[part] < 0)
	{
		_fatalWounds[part] = 0;
	}

	_health += healthAmount;
	if (_health > getBaseStats()->health)
	{
		_health = getBaseStats()->health;
	}
}

/**
 * Restore soldier morale
 */
void BattleUnit::painKillers()
{
	int lostHealth = getBaseStats()->health - _health;
	if (lostHealth > _moraleRestored)
	{
		_morale = std::min(100, (lostHealth - _moraleRestored + _morale));
		_moraleRestored = lostHealth;
	}
}

/**
 * Restore soldier energy and reduce stun level
 * @param energy The amount of energy to add
 * @param s The amount of stun level to reduce
 */
void BattleUnit::stimulant (int energy, int s)
{
	_energy += energy;
	if (_energy > getBaseStats()->stamina)
		_energy = getBaseStats()->stamina;
	healStun (s);
}


/**
 * Get motion points for the motion scanner. More points
 * is a larger blip on the scanner.
 * @return points.
 */
int BattleUnit::getMotionPoints() const
{
	return _motionPoints;
}


/**
 * Gets the unit's armor.
 * @return Pointer to armor.
 */
Armor *BattleUnit::getArmor() const
{
	return _armor;
}

/**
 * Get unit's name.
 * An aliens name is the translation of it's race and rank.
 * hence the language pointer needed.
 * @param lang Pointer to language.
 * @param debugAppendId Append unit ID to name for debug purposes.
 * @return name Widecharstring of the unit's name.
 */
std::string BattleUnit::getName(Language *lang, bool debugAppendId) const
{
	if (_type != "SOLDIER" && lang != 0)
	{
		std::string ret;

		if (_type.find("STR_") != std::string::npos)
			ret = lang->getString(_type);
		else
			ret = lang->getString(_race);

		if (debugAppendId)
		{
			std::ostringstream ss;
			ss << ret << " " << _id;
			ret = ss.str();
		}
		return ret;
	}

	return _name;
}

/**
  * Gets pointer to the unit's stats.
  * @return stats Pointer to the unit's stats.
  */
UnitStats *BattleUnit::getBaseStats()
{
	return &_stats;
}

/**
  * Get the unit's stand height.
  * @return The unit's height in voxels, when standing up.
  */
int BattleUnit::getStandHeight() const
{
	return _standHeight;
}

/**
  * Get the unit's kneel height.
  * @return The unit's height in voxels, when kneeling.
  */
int BattleUnit::getKneelHeight() const
{
	return _kneelHeight;
}

/**
  * Get the unit's floating elevation.
  * @return The unit's elevation over the ground in voxels, when flying.
  */
int BattleUnit::getFloatHeight() const
{
	return _floatHeight;
}

/**
  * Get the unit's loft ID, one per unit tile.
  * Each tile only has one loft, as it is repeated over the entire height of the unit.
  * @param entry Unit tile
  * @return The unit's line of fire template ID.
  */
int BattleUnit::getLoftemps(int entry) const
{
	return _loftempsSet.at(entry);
}

/**
  * Get the unit's value. Used for score at debriefing.
  * @return value score
  */
int BattleUnit::getValue() const
{
	return _value;
}

/**
 * Get the unit's death sounds.
 * @return List of sound IDs.
 */
const std::vector<int> &BattleUnit::getDeathSounds() const
{
	if (_deathSound.empty() && _geoscapeSoldier != 0)
	{
		if (_gender == GENDER_MALE)
			return _geoscapeSoldier->getRules()->getMaleDeathSounds();
		else
			return _geoscapeSoldier->getRules()->getFemaleDeathSounds();
	}
	return _deathSound;
}

/**
 * Get the unit's move sound.
 * @return id.
 */
int BattleUnit::getMoveSound() const
{
	return _moveSound;
}


/**
 * Get whether the unit is affected by fatal wounds.
 * Normally only soldiers are affected by fatal wounds.
 * @return Is the unit affected by wounds?
 */
bool BattleUnit::isWoundable() const
{
	return (_type=="SOLDIER" || (Options::alienBleeding && _originalFaction != FACTION_PLAYER && _armor->getSize() == 1));
}

/**
 * Get whether the unit is affected by morale loss.
 * Normally only small units are affected by morale loss.
 * @return Is the unit affected by morale?
 */
bool BattleUnit::isFearable() const
{
	return (_armor->getSize() == 1);
}

/**
 * Get the number of turns an AI unit remembers a soldier's position.
 * @return intelligence.
 */
int BattleUnit::getIntelligence() const
{
	return _intelligence;
}

/**
 * Get the unit's aggression.
 * @return aggression.
 */
int BattleUnit::getAggression() const
{
	return _aggression;
}

/**
 * Returns the unit's special ability.
 * @return special ability.
 */
int BattleUnit::getSpecialAbility() const
{
	return _specab;
}

/**
 * Sets this unit to respawn (or not).
 * @param respawn whether it should respawn.
 */
void BattleUnit::setRespawn(bool respawn)
{
	_respawn = respawn;
}

/**
 * Gets this unit's respawn flag.
 */
bool BattleUnit::getRespawn() const
{
	return _respawn;
}

/**
 * Get the unit that is spawned when this one dies.
 * @return unit.
 */
std::string BattleUnit::getSpawnUnit() const
{
	return _spawnUnit;
}

/**
 * Set the unit that is spawned when this one dies.
 * @param spawnUnit unit.
 */
void BattleUnit::setSpawnUnit(const std::string &spawnUnit)
{
	_spawnUnit = spawnUnit;
}

/**
 * Get the units's rank string.
 * @return rank.
 */
std::string BattleUnit::getRankString() const
{
	return _rank;
}

/**
 * Get the geoscape-soldier object.
 * @return soldier.
 */
Soldier *BattleUnit::getGeoscapeSoldier() const
{
	return _geoscapeSoldier;
}

/**
 * Add a kill to the counter.
 */
void BattleUnit::addKillCount()
{
	_kills++;
}

/**
 * Get unit type.
 * @return unit type.
 */
std::string BattleUnit::getType() const
{
	return _type;
}

/**
 * Set unit's active hand.
 * @param hand active hand.
 */
void BattleUnit::setActiveHand(const std::string &hand)
{
	if (_activeHand != hand) _cacheInvalid = true;
	_activeHand = hand;
}

/**
 * Get unit's active hand.
 * @return active hand.
 */
std::string BattleUnit::getActiveHand() const
{
	if (getItem(_activeHand)) return _activeHand;
	if (getItem("STR_LEFT_HAND")) return "STR_LEFT_HAND";
	return "STR_RIGHT_HAND";
}

/**
 * Converts unit to another faction (original faction is still stored).
 * @param f faction.
 */
void BattleUnit::convertToFaction(UnitFaction f)
{
	_faction = f;
}

/**
* Set health to 0 - used when getting killed unconscious.
*/
void BattleUnit::kill()
{
	_health = 0;
}

/**
 * Set health to 0 and set status dead - used when getting zombified.
 */
void BattleUnit::instaKill()
{
	_health = 0;
	_status = STATUS_DEAD;
}

/**
 * Get sound to play when unit aggros.
 * @return sound
 */
int BattleUnit::getAggroSound() const
{
	return _aggroSound;
}

/**
 * Get the faction the unit was killed by.
 * @return faction
 */
UnitFaction BattleUnit::killedBy() const
{
	return _killedBy;
}

/**
 * Set the faction the unit was killed by.
 * @param f faction
 */
void BattleUnit::killedBy(UnitFaction f)
{
	_killedBy = f;
}

/**
 * Set the units we are charging towards.
 * @param chargeTarget Charge Target
 */
void BattleUnit::setCharging(BattleUnit *chargeTarget)
{
	_charging = chargeTarget;
}

/**
 * Get the units we are charging towards.
 * @return Charge Target
 */
BattleUnit *BattleUnit::getCharging()
{
	return _charging;
}

/**
 * Get the units carried weight in strength units.
 * @param draggingItem item to ignore
 * @return weight
 */
int BattleUnit::getCarriedWeight(BattleItem *draggingItem) const
{
	int weight = _armor->getWeight();
	for (std::vector<BattleItem*>::const_iterator i = _inventory.begin(); i != _inventory.end(); ++i)
	{
		if ((*i) == draggingItem) continue;
		weight += (*i)->getRules()->getWeight();
		if ((*i)->getAmmoItem() != (*i) && (*i)->getAmmoItem()) weight += (*i)->getAmmoItem()->getRules()->getWeight();
	}
	return std::max(0,weight);
}

/**
 * Set how long since this unit was last exposed.
 * @param turns number of turns
 */
void BattleUnit::setTurnsSinceSpotted (int turns)
{
	_turnsSinceSpotted = turns;
}

/**
 * Get how long since this unit was exposed.
 * @return number of turns
 */
int BattleUnit::getTurnsSinceSpotted() const
{
	return _turnsSinceSpotted;
}

/**
 * Get this unit's original Faction.
 * @return original faction
 */
UnitFaction BattleUnit::getOriginalFaction() const
{
	return _originalFaction;
}

/**
 * invalidate cache; call after copying object :(
 */
void BattleUnit::invalidateCache()
{
	for (int i = 0; i < 5; ++i) { _cache[i] = 0; }
	_cacheInvalid = true;
}

/**
 * Get the list of units spotted this turn.
 * @return List of units.
 */
std::vector<BattleUnit *> &BattleUnit::getUnitsSpottedThisTurn()
{
	return _unitsSpottedThisTurn;
}

/**
 * Change the numeric version of the unit's rank.
 * @param rank unit rank, 0 = lowest
 */
void BattleUnit::setRankInt(int rank)
{
	_rankInt = rank;
}

/**
 * Return the numeric version of the unit's rank.
 * @return unit rank, 0 = lowest
 */
int BattleUnit::getRankInt() const
{
	return _rankInt;
}

/**
 * Derive the numeric unit rank from the string rank
 * (for soldier units).
 */
void BattleUnit::deriveRank()
{
	if (_geoscapeSoldier)
	{
		switch (_geoscapeSoldier->getRank())
		{
		case RANK_ROOKIE:    _rankInt = 0; break;
		case RANK_SQUADDIE:  _rankInt = 1; break;
		case RANK_SERGEANT:  _rankInt = 2; break;
		case RANK_CAPTAIN:   _rankInt = 3; break;
		case RANK_COLONEL:   _rankInt = 4; break;
		case RANK_COMMANDER: _rankInt = 5; break;
		default:             _rankInt = 0; break;
		}
	}
}

/**
 * this function checks if a tile is visible, using maths.
 * @param pos the position to check against
 * @return what the maths decide
 */
bool BattleUnit::checkViewSector (Position pos) const
{
	int deltaX = pos.x - _pos.x;
	int deltaY = _pos.y - pos.y;

	switch (_direction)
	{
		case 0:
			if ( (deltaX + deltaY >= 0) && (deltaY - deltaX >= 0) )
				return true;
			break;
		case 1:
			if ( (deltaX >= 0) && (deltaY >= 0) )
				return true;
			break;
		case 2:
			if ( (deltaX + deltaY >= 0) && (deltaY - deltaX <= 0) )
				return true;
			break;
		case 3:
			if ( (deltaY <= 0) && (deltaX >= 0) )
				return true;
			break;
		case 4:
			if ( (deltaX + deltaY <= 0) && (deltaY - deltaX <= 0) )
				return true;
			break;
		case 5:
			if ( (deltaX <= 0) && (deltaY <= 0) )
				return true;
			break;
		case 6:
			if ( (deltaX + deltaY <= 0) && (deltaY - deltaX >= 0) )
				return true;
			break;
		case 7:
			if ( (deltaY >= 0) && (deltaX <= 0) )
				return true;
			break;
		default:
			return false;
	}
	return false;
}

/**
 * common function to adjust a unit's stats according to difficulty setting.
 * @param statAdjustment the stat adjustment variables coefficient value.
 */
void BattleUnit::adjustStats(const StatAdjustment &adjustment)
{
	_stats.tu += adjustment.statGrowth.tu * adjustment.growthMultiplier * _stats.tu / 100;
	_stats.stamina += adjustment.statGrowth.stamina * adjustment.growthMultiplier * _stats.stamina / 100;
	_stats.health += adjustment.statGrowth.health * adjustment.growthMultiplier * _stats.health / 100;
	_stats.bravery += adjustment.statGrowth.bravery * adjustment.growthMultiplier * _stats.bravery / 100;
	_stats.reactions += adjustment.statGrowth.reactions * adjustment.growthMultiplier * _stats.reactions / 100;
	_stats.firing += adjustment.statGrowth.firing * adjustment.growthMultiplier * _stats.firing / 100;
	_stats.throwing += adjustment.statGrowth.throwing * adjustment.growthMultiplier * _stats.throwing / 100;
	_stats.strength += adjustment.statGrowth.strength * adjustment.growthMultiplier * _stats.strength / 100;
	_stats.psiStrength += adjustment.statGrowth.psiStrength * adjustment.growthMultiplier * _stats.psiStrength / 100;
	_stats.psiSkill += adjustment.statGrowth.psiSkill * adjustment.growthMultiplier * _stats.psiSkill / 100;
	_stats.melee += adjustment.statGrowth.melee * adjustment.growthMultiplier * _stats.melee / 100;

	_stats.firing *= adjustment.aimAndArmorMultiplier;
	_maxArmor[0] *= adjustment.aimAndArmorMultiplier;
	_maxArmor[1] *= adjustment.aimAndArmorMultiplier;
	_maxArmor[2] *= adjustment.aimAndArmorMultiplier;
	_maxArmor[3] *= adjustment.aimAndArmorMultiplier;
	_maxArmor[4] *= adjustment.aimAndArmorMultiplier;
}

/**
 * did this unit already take fire damage this turn?
 * (used to avoid damaging large units multiple times.)
 * @return ow it burns
 */
bool BattleUnit::tookFireDamage() const
{
	return _hitByFire;
}

/**
 * toggle the state of the fire damage tracking boolean.
 */
void BattleUnit::toggleFireDamage()
{
	_hitByFire = !_hitByFire;
}

/**
 * Checks if this unit can be selected. Only alive units
 * belonging to the faction can be selected.
 * @param faction The faction to compare with.
 * @param checkReselect Check if the unit is reselectable.
 * @param checkInventory Check if the unit has an inventory.
 * @return True if the unit can be selected, false otherwise.
 */
bool BattleUnit::isSelectable(UnitFaction faction, bool checkReselect, bool checkInventory) const
{
	return (_faction == faction && !isOut() && (!checkReselect || reselectAllowed()) && (!checkInventory || hasInventory()));
}

/**
 * Checks if this unit has an inventory. Large units and/or
 * terror units generally don't have inventories.
 * @return True if an inventory is available, false otherwise.
 */
bool BattleUnit::hasInventory() const
{
	return (_armor->hasInventory());
}

/**
 * If this unit is breathing, what frame should be displayed?
 * @return frame number.
 */
int BattleUnit::getBreathFrame() const
{
	if (_floorAbove)
		return 0;
	return _breathFrame;
}

/**
 * Decides if we should start producing bubbles, and/or updates which bubble frame we are on.
 */
void BattleUnit::breathe()
{
	// _breathFrame of -1 means this unit doesn't produce bubbles
	if (_breathFrame < 0 || isOut())
	{
		_breathing = false;
		return;
	}

	if (!_breathing || _status == STATUS_WALKING)
	{
		// deviation from original: TFTD used a static 10% chance for every animation frame,
		// instead let's use 5%, but allow morale to affect it.
		_breathing = (_status != STATUS_WALKING && RNG::seedless(0, 99) < (105 - _morale));
		_breathFrame = 0;
	}

	if (_breathing)
	{
		// advance the bubble frame
		_breathFrame++;

		// we've reached the end of the cycle, get rid of the bubbles
		if (_breathFrame >= 17)
		{
			_breathFrame = 0;
			_breathing = false;
		}
	}
}

/**
 * Sets the flag for "this unit is under cover" meaning don't draw bubbles.
 * @param floor is there a floor.
 */
void BattleUnit::setFloorAbove(bool floor)
{
	_floorAbove = floor;
}

/**
 * Checks if the floor above flag has been set.
 * @return if we're under cover.
 */
bool BattleUnit::getFloorAbove() const
{
	return _floorAbove;
}

/**
 * Get the name of any melee weapon we may be carrying, or a built in one.
 * @return the name .
 */
BattleItem *BattleUnit::getMeleeWeapon()
{
	BattleItem *melee = getItem("STR_RIGHT_HAND");
	if (melee && melee->getRules()->getBattleType() == BT_MELEE)
	{
		return melee;
	}
	melee = getItem("STR_LEFT_HAND");
	if (melee && melee->getRules()->getBattleType() == BT_MELEE)
	{
		return melee;
	}
	melee = getSpecialWeapon(BT_MELEE);
	if (melee)
	{
		return melee;
	}
	return 0;
}

/**
 * use this instead of checking the rules of the armor.
 */
MovementType BattleUnit::getMovementType() const
{
	return _movementType;
}

/**
 * Elevates the unit to grand galactic inquisitor status,
 * meaning they will NOT take part in the current battle.
 */
void BattleUnit::goToTimeOut()
{
	_status = STATUS_IGNORE_ME;
}

/**
 * Helper function used by `BattleUnit::setSpecialWeapon`
 */
static inline BattleItem *createItem(SavedBattleGame *save, BattleUnit *unit, RuleItem *rule)
{
	BattleItem *item = new BattleItem(rule, save->getCurrentItemId());
	item->setOwner(unit);
	save->removeItem(item); //item outside inventory, deleted when game is shutdown.
	return item;
}

/**
 * Set special weapon that is handled outside inventory.
 * @param save
 */
void BattleUnit::setSpecialWeapon(SavedBattleGame *save, const Mod *mod)
{
	RuleItem *item = 0;
	int i = 0;

	if (getUnitRules())
	{
		item = mod->getItem(getUnitRules()->getMeleeWeapon());
		if (item)
		{
			_specWeapon[i++] = createItem(save, this, item);
		}
	}
	item = mod->getItem(getArmor()->getSpecialWeapon());
	if (item)
	{
		_specWeapon[i++] = createItem(save, this, item);
	}
	if (getBaseStats()->psiSkill > 0 && getOriginalFaction() == FACTION_HOSTILE)
	{
		item = mod->getItem(getUnitRules()->getPsiWeapon());
		if (item)
		{
			_specWeapon[i++] = createItem(save, this, item);
		}
	}
}

/**
 * Get special weapon.
 */
BattleItem *BattleUnit::getSpecialWeapon(BattleType type) const
{
	for (int i = 0; i < SPEC_WEAPON_MAX; ++i)
	{
		if (_specWeapon[i] && _specWeapon[i]->getRules()->getBattleType() == type)
		{
			return _specWeapon[i];
		}
	}
	return 0;
}

/**
 * Recovers a unit's TUs and energy, taking a number of factors into consideration.
 */
void BattleUnit::recoverTimeUnits()
{
	// recover TUs
	int TURecovery = getBaseStats()->tu;
	float encumbrance = (float)getBaseStats()->strength / (float)getCarriedWeight();
	if (encumbrance < 1)
	{
		TURecovery = int(encumbrance * TURecovery);
	}
	// Each fatal wound to the left or right leg reduces the soldier's TUs by 10%.
	TURecovery -= (TURecovery * ((_fatalWounds[BODYPART_LEFTLEG]+_fatalWounds[BODYPART_RIGHTLEG]) * 10))/100;
	setTimeUnits(TURecovery);

	// recover energy
	if (!isOut())
	{
		int ENRecovery;
		if (_geoscapeSoldier != 0)
		{
			ENRecovery = _geoscapeSoldier->getInitStats()->tu / 3;
		}
		else
		{
			ENRecovery = _unitRules->getEnergyRecovery();
		}
		// Each fatal wound to the body reduces the soldier's energy recovery by 10%.
		ENRecovery -= (_energy * (_fatalWounds[BODYPART_TORSO] * 10))/100;
		_energy = std::max(0, std::min(getBaseStats()->stamina, _energy + ENRecovery));
	}
}

/**
 * Get the unit's statistics.
 * @return BattleUnitStatistics statistics.
 */
BattleUnitStatistics* BattleUnit::getStatistics()
{
	return _statistics;
}

/**
 * Sets the unit murderer's id.
 * @param int murderer id.
 */
void BattleUnit::setMurdererId(int id)
{
	_murdererId = id;
}

/**
 * Gets the unit murderer's id.
 * @return int murderer id.
 */
int BattleUnit::getMurdererId() const
{
	return _murdererId;
}

/**
 * Set information on the unit's fatal blow.
 * @param UnitSide unit's side that was shot.
 * @param UnitBodyPart unit's body part that was shot.
 */
void BattleUnit::setFatalShotInfo(UnitSide side, UnitBodyPart bodypart)
{
	_fatalShotSide = side;
	_fatalShotBodyPart = bodypart;
}

/**
 * Get information on the unit's fatal shot's side.
 * @return UnitSide fatal shot's side.
 */
UnitSide BattleUnit::getFatalShotSide() const
{
	return _fatalShotSide;
}

/**
 * Get information on the unit's fatal shot's body part.
 * @return UnitBodyPart fatal shot's body part.
 */
UnitBodyPart BattleUnit::getFatalShotBodyPart() const
{
	return _fatalShotBodyPart;
}

/**
 * Gets the unit murderer's weapon.
 * @return int murderer weapon.
 */
std::string BattleUnit::getMurdererWeapon() const
{
	return _murdererWeapon;
}

/**
 * Set the murderer's weapon.
 * @param string murderer's weapon.
 */
void BattleUnit::setMurdererWeapon(const std::string& weapon)
{
	_murdererWeapon = weapon;
}

/**
 * Gets the unit murderer's weapon's ammo.
 * @return int murderer weapon ammo.
 */
std::string BattleUnit::getMurdererWeaponAmmo() const
{
	return _murdererWeaponAmmo;
}

/**
 * Set the murderer's weapon's ammo.
 * @param string murderer weapon ammo.
 */
void BattleUnit::setMurdererWeaponAmmo(const std::string& weaponAmmo)
{
	_murdererWeaponAmmo = weaponAmmo;
}

/**
 * Sets the unit mind controller's id.
 * @param int mind controller id.
 */
void BattleUnit::setMindControllerId(int id)
{
	_mindControllerID = id;
}

/**
 * Gets the unit mind controller's id.
 * @return int mind controller id.
 */
int BattleUnit::getMindControllerId() const
{
	return _mindControllerID;
}

/**
 * Get the unit's total firing xp for this mission.
 */
int BattleUnit::getFiringXP() const
{
	return _expFiring;
}

/**
 * Artificially alter a unit's firing xp. (used for shotguns)
 */
void BattleUnit::nerfFiringXP(int newXP)
{
	_expFiring = newXP;
}

/**
 * Was this unit just hit?
 */
bool BattleUnit::getHitState()
{
	return _hitByAnything;
}

/**
 * reset the unit hit state.
 */
void BattleUnit::resetHitState()
{
	_hitByAnything = false;
}

/**
 * Gets whether this unit can be captured alive (applies to aliens).
 */
bool BattleUnit::getCapturable() const
{
	return _capturable;
}

void BattleUnit::freePatrolTarget()
{
	if (_currentAIState)
	{
		_currentAIState->freePatrolTarget();
	}
}
}
