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
#include "BattleUnit.h"
#include "BattleItem.h"
#include <cmath>
#include <sstream>
#include <typeinfo>
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Engine/Language.h"
#include "../Engine/Logger.h"
#include "../Battlescape/Pathfinding.h"
#include "../Battlescape/BattleAIState.h"
#include "../Battlescape/AggroBAIState.h"
#include "Soldier.h"
#include "../Ruleset/Armor.h"
#include "../Ruleset/Unit.h"
#include "../Engine/RNG.h"
#include "../Ruleset/RuleInventory.h"
#include "../Ruleset/RuleSoldier.h"
#include "Tile.h"
#include "SavedGame.h"

namespace OpenXcom
{

/**
 * Initializes a BattleUnit from a Soldier
 * @param soldier Pointer to the Soldier.
 * @param faction Which faction the units belongs to.
 */
BattleUnit::BattleUnit(Soldier *soldier, UnitFaction faction) : _faction(faction), _originalFaction(faction), _killedBy(faction), _id(0), _pos(Position()), _tile(0), _lastPos(Position()), _direction(0), _directionTurret(0), _toDirectionTurret(0),  _verticalDirection(0), _status(STATUS_STANDING), _walkPhase(0), _fallPhase(0), _kneeled(false), _floating(false), _dontReselect(false), _fire(0), _currentAIState(0), _visible(false), _cacheInvalid(true), _expBravery(0), _expReactions(0), _expFiring(0), _expThrowing(0), _expPsiSkill(0), _expMelee(0), _turretType(-1), _motionPoints(0), _kills(0), _geoscapeSoldier(soldier), _charging(0), _turnsExposed(0), _hidingForTurn(false), _unitRules(0)
{
	_name = soldier->getName();
	_id = soldier->getId();
	_type = "SOLDIER";
	_rank = soldier->getRankString();
	_stats = *soldier->getCurrentStats();
	_standHeight = soldier->getRules()->getStandHeight();
	_kneelHeight = soldier->getRules()->getKneelHeight();
	_floatHeight = soldier->getRules()->getFloatHeight();
	_loftempsSet = soldier->getRules()->getLoftempsSet();
	_deathSound = 0; // this one is hardcoded
	_aggroSound = 0;
	_moveSound = -1;  // this one is hardcoded
	_intelligence = 2;
	_aggression = 1;
	_specab = SPECAB_NONE;
	_armor = soldier->getArmor();
	_gender = soldier->getGender();
	_faceDirection = -1;

	int rankbonus = 0;

	switch (soldier->getRank())
	{
	case RANK_SERGEANT:  rankbonus =  1; break;
	case RANK_CAPTAIN:   rankbonus =  3; break;
	case RANK_COLONEL:   rankbonus =  6; break;
	case RANK_COMMANDER: rankbonus = 10; break;
	default:             rankbonus =  0; break;
	}

	_value = 20 + soldier->getMissions() + rankbonus;

	_tu = _stats.tu;
	_energy = _stats.stamina;
	_health = _stats.health;
	_morale = 100;
	_stunlevel = 0;
	_currentArmor[SIDE_FRONT] = _armor->getFrontArmor();
	_currentArmor[SIDE_LEFT] = _armor->getSideArmor();
	_currentArmor[SIDE_RIGHT] = _armor->getSideArmor();
	_currentArmor[SIDE_REAR] = _armor->getRearArmor();
	_currentArmor[SIDE_UNDER] = _armor->getUnderArmor();
	for (int i = 0; i < 6; ++i)
		_fatalWounds[i] = 0;
	for (int i = 0; i < 5; ++i)
		_cache[i] = 0;

	_activeHand = "STR_RIGHT_HAND";

	lastCover = Position(-1, -1, -1);
}

/**
 * Initializes a BattleUnit from a Unit object.
 * @param unit Pointer to Unit object.
 * @param faction Which faction the units belongs to.
 */
BattleUnit::BattleUnit(Unit *unit, UnitFaction faction, int id, Armor *armor) : _faction(faction), _originalFaction(faction), _killedBy(faction), _id(id), _pos(Position()), _tile(0), _lastPos(Position()), _direction(0), _directionTurret(0), _toDirectionTurret(0),  _verticalDirection(0), _status(STATUS_STANDING), _walkPhase(0), _fallPhase(0), _kneeled(false), _floating(false), _dontReselect(false), _fire(0), _currentAIState(0), _visible(false), _cacheInvalid(true), _expBravery(0), _expReactions(0), _expFiring(0), _expThrowing(0), _expPsiSkill(0), _expMelee(0), _turretType(-1), _motionPoints(0), _kills(0), _armor(armor), _geoscapeSoldier(0), _charging(0), _turnsExposed(0),_hidingForTurn(false), _unitRules(unit)
{
	_type = unit->getType();
	_rank = unit->getRank();
	_race = unit->getRace();
	_stats = *unit->getStats();
	_standHeight = unit->getStandHeight();
	_kneelHeight = unit->getKneelHeight();
	_floatHeight = unit->getFloatHeight();
	_loftempsSet = unit->getLoftempsSet();
	_deathSound = unit->getDeathSound();
	_aggroSound = unit->getAggroSound();
	_moveSound = unit->getMoveSound();
	_intelligence = unit->getIntelligence();
	_aggression = unit->getAggression();
	_specab = (SpecialAbility) unit->getSpecialAbility();
	_zombieUnit = unit->getZombieUnit();
	_spawnUnit = unit->getSpawnUnit();
	_value = unit->getValue();
	_gender = GENDER_MALE;
	_faceDirection = -1;

	_tu = _stats.tu;
	_energy = _stats.stamina;
	_health = _stats.health;
	_morale = 100;
	_stunlevel = 0;
	_currentArmor[SIDE_FRONT] = _armor->getFrontArmor();
	_currentArmor[SIDE_LEFT] = _armor->getSideArmor();
	_currentArmor[SIDE_RIGHT] = _armor->getSideArmor();
	_currentArmor[SIDE_REAR] = _armor->getRearArmor();
	_currentArmor[SIDE_UNDER] = _armor->getUnderArmor();
	for (int i = 0; i < 6; ++i)
		_fatalWounds[i] = 0;
	for (int i = 0; i < 5; ++i)
		_cache[i] = 0;

	_activeHand = "STR_RIGHT_HAND";
	
	lastCover = Position(-1, -1, -1);
	
}

/// tedious copy constructor because we can't copy _cache by the default method
BattleUnit::BattleUnit(BattleUnit &b) : 
	_faction(b._faction), _originalFaction(b._originalFaction),
	_killedBy(b._killedBy),
	_id(b._id),
	_pos(b._pos),
	_tile(b._tile),
	_lastPos(b._lastPos),
	_direction(b._direction), _toDirection(b._toDirection),
	_directionTurret(b._directionTurret), _toDirectionTurret(b._toDirectionTurret),
	_verticalDirection(b._verticalDirection),
	_destination(b._destination),
	_status(b._status),
	_walkPhase(b._walkPhase), _fallPhase(b._fallPhase),
	_visibleUnits(b._visibleUnits),
	_visibleTiles(b._visibleTiles),
	_tu(b._tu), _energy(b._energy), _health(b._health), _morale(b._morale), _stunlevel(b._stunlevel),
	_kneeled(b._kneeled), _floating(b._floating), _dontReselect(b._dontReselect),
	//int _currentArmor[5];
	//int _fatalWounds[6];
	_fire(b._fire),
	_inventory(b._inventory),
	_currentAIState(b._currentAIState),
	_visible(b._visible),
	//Surface *_cache[5];
	_cacheInvalid(b._cacheInvalid),
	_expBravery(b._expBravery), _expReactions(b._expReactions), _expFiring(b._expFiring), _expThrowing(b._expThrowing), _expPsiSkill(b._expPsiSkill), _expMelee(b._expMelee),
	_turretType(b._expMelee),
	_needPainKiller(b._needPainKiller),
	_motionPoints(b._motionPoints),
	_kills(b._kills),
	_faceDirection(b._faceDirection),
	_type(b._type),
	_rank(b._rank),
	_race(b._race),
	_name(b._name),
	_stats(b._stats),
	_standHeight(b._standHeight), _kneelHeight(b._kneelHeight), _floatHeight(b._floatHeight),
	_value(b._value), _deathSound(b._deathSound), _aggroSound(b._aggroSound), _moveSound(b._moveSound),
	_intelligence(b._intelligence), _aggression(b._aggression),
	_specab(b._specab),
	_zombieUnit(b._zombieUnit), _spawnUnit(b._spawnUnit),
	_armor(b._armor),
	_gender(b._gender),
	_activeHand(b._activeHand),
	_geoscapeSoldier(b._geoscapeSoldier),
	_charging(b._charging),
	_turnsExposed(b._turnsExposed),
	_loftempsSet(b._loftempsSet),
	_unitRules(b._unitRules),
	_hidingForTurn(b._hidingForTurn),
	lastCover(b.lastCover)
{
	invalidateCache();
	for (int i = 0; i < 5; ++i)
	{
		_currentArmor[i] = b._currentArmor[i];
	}
	
	for (int i = 0; i < 6; ++i)
	{
		_fatalWounds[i] = b._fatalWounds[i];
	}
}



/**
 *
 */
BattleUnit::~BattleUnit()
{
	for (int i = 0; i < 5; ++i)
		if (_cache[i]) delete _cache[i];
}

/**
 * Loads the unit from a YAML file.
 * @param node YAML node.
 */
void BattleUnit::load(const YAML::Node &node)
{
	int a = 0;

	node["id"] >> _id;
	node["faction"] >> a;
	_faction = (UnitFaction)a;
	node["status"] >> a;
	_status = (UnitStatus)a;
	node["position"] >> _pos;
	node["direction"] >> _direction;
	node["directionTurret"] >> _directionTurret;
	node["tu"] >> _tu;
	node["health"] >> _health;
	node["stunlevel"] >> _stunlevel;
	node["energy"] >> _energy;
	node["morale"] >> _morale;
	node["kneeled"] >> _kneeled;
	node["floating"] >> _floating;
	for (int i=0; i < 5; i++)
		node["armor"][i] >> _currentArmor[i];
	for (int i=0; i < 6; i++)
		node["fatalWounds"][i] >> _fatalWounds[i];
	node["expBravery"] >> _expBravery;
	node["expReactions"] >> _expReactions;
	node["expFiring"] >> _expFiring;
	node["expThrowing"] >> _expThrowing;
	node["expPsiSkill"] >> _expPsiSkill;
	node["expMelee"] >> _expMelee;
	node["turretType"] >> _turretType;
	node["visible"] >> _visible;
	node["turnsExposed"] >> _turnsExposed;
	node["killedBy"] >> a;
	_killedBy = (UnitFaction)a;
	if (const YAML::Node *pName = node.FindValue("originalFaction"))
	{
		(*pName) >> a;
		_originalFaction = (UnitFaction)a;
	}
	else
	{
		_originalFaction = _faction;
	}
	_charging = 0;


}

/**
 * Saves the soldier to a YAML file.
 * @param out YAML emitter.
 */
void BattleUnit::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;

	out << YAML::Key << "id" << YAML::Value << _id;
	out << YAML::Key << "faction" << YAML::Value << _faction;
	out << YAML::Key << "soldierId" << YAML::Value << _id;
	out << YAML::Key << "genUnitType" << YAML::Value << _type;
	out << YAML::Key << "genUnitArmor" << YAML::Value << _armor->getType();
	out << YAML::Key << "name" << YAML::Value << Language::wstrToUtf8(getName(0));
	out << YAML::Key << "status" << YAML::Value << _status;
	out << YAML::Key << "position" << YAML::Value << _pos;
	out << YAML::Key << "direction" << YAML::Value << _direction;
	out << YAML::Key << "directionTurret" << YAML::Value << _directionTurret;
	out << YAML::Key << "tu" << YAML::Value << _tu;
	out << YAML::Key << "health" << YAML::Value << _health;
	out << YAML::Key << "stunlevel" << YAML::Value << _stunlevel;
	out << YAML::Key << "energy" << YAML::Value << _energy;
	out << YAML::Key << "morale" << YAML::Value << _morale;
	out << YAML::Key << "kneeled" << YAML::Value << _kneeled;
	out << YAML::Key << "floating" << YAML::Value << _floating;
	out << YAML::Key << "armor" << YAML::Value;
	out << YAML::Flow << YAML::BeginSeq;
	for (int i=0; i < 5; i++) out << _currentArmor[i];
	out << YAML::EndSeq;
	out << YAML::Key << "fatalWounds" << YAML::Value;
	out << YAML::Flow << YAML::BeginSeq;
	for (int i=0; i < 6; i++) out << _fatalWounds[i];
	out << YAML::EndSeq;
	out << YAML::Key << "fire" << YAML::Value << _fire;
	out << YAML::Key << "expBravery" << YAML::Value << _expBravery;
	out << YAML::Key << "expReactions" << YAML::Value << _expReactions;
	out << YAML::Key << "expFiring" << YAML::Value << _expFiring;
	out << YAML::Key << "expThrowing" << YAML::Value << _expThrowing;
	out << YAML::Key << "expPsiSkill" << YAML::Value << _expPsiSkill;
	out << YAML::Key << "expMelee" << YAML::Value << _expMelee;
	out << YAML::Key << "turretType" << YAML::Value << _turretType;
	out << YAML::Key << "visible" << YAML::Value << _visible;
	out << YAML::Key << "turnsExposed" << YAML::Value << _turnsExposed;

	if (getCurrentAIState())
	{
		out << YAML::Key << "AI" << YAML::Value;
		getCurrentAIState()->save(out);
	}
	out << YAML::Key << "killedBy" << YAML::Value << _killedBy;
	if (_originalFaction != _faction)
		out << YAML::Key << "originalFaction" << YAML::Value << _originalFaction;

	out << YAML::EndMap;
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
 */
void BattleUnit::setPosition(const Position& pos, bool updateLastPos)
{
	if (updateLastPos) { _lastPos = _pos; }
	_pos = pos;
}

/**
 * Gets the BattleUnit's position.
 * @return position
 */
const Position& BattleUnit::getPosition() const
{
	return _pos;
}

/**
 * Gets the BattleUnit's position.
 * @return position
 */
const Position& BattleUnit::getLastPosition() const
{
	return _lastPos;
}

/**
 * Gets the BattleUnit's destination.
 * @return destination
 */
const Position& BattleUnit::getDestination() const
{
	return _destination;
}

/**
 * Changes the BattleUnit's direction. Only used for initial unit placement.
 * @param direction
 */
void BattleUnit::setDirection(int direction)
{
	_direction = direction;
	_toDirection = direction;
	_directionTurret = direction;
}

/**
 * Changes the facedirection. Only used for strafing moves.
 * @param direction
 */
void BattleUnit::setFaceDirection(int direction)
{
	_faceDirection = direction;
}

/**
 * Gets the BattleUnit's (horizontal) direction.
 * @return direction
 */
int BattleUnit::getDirection() const
{
	return _direction;
}

/**
 * Gets the BattleUnit's (horizontal) face direction. Used only during strafing moves
 * @return direction
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
 */
void BattleUnit::startWalking(int direction, const Position &destination, Tile *destinationTile, Tile *tileBelowMe, Tile *TileBelowDestination, bool cache)
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
}

/**
 * This will increment the walking phase.
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
			if (_direction < 1 || _direction > 4)
				middle = end;
			else
				middle = 1;
		}
	}

	_walkPhase++;
	
	if (!cache)
	{
		_walkPhase = 1;
		middle = 1;
		end = 1;
	}

	if (_walkPhase == middle)
	{
		// we assume we reached our destination tile
		// this is actually a drawing hack, so soldiers are not overlapped by floortiles
		_pos = _destination;
	}

	if (_walkPhase == end)
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

/*
 * Gets the walking phase for animation and sound.
 * return phase will always go from 0-7
 */
int BattleUnit::getWalkingPhase() const
{
	return _walkPhase % 8;
}

/*
 * Gets the walking phase for diagonal walking.
 * return phase this will be 0 or 8
 */
int BattleUnit::getDiagonalWalkingPhase() const
{
	return (_walkPhase / 8) * 8;
}

/**
 * Look at a point.
 * @param point
 */
void BattleUnit::lookAt(const Position &point, bool turret)
{
	int dir = getDirectionTo (point);

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
		if (_toDirection != _direction)
		{
			_status = STATUS_TURNING;
		}
	}
}

/**
 * Look at a direction.
 * @param direction
 */
void BattleUnit::lookAt(int direction, bool force)
{
	if (!force)
	{
		_toDirection = direction;
		_status = STATUS_TURNING;
	}
	else
	{
		_toDirection = direction;
		_direction = direction;
	}
}

/**
 * Advances the turning towards the target direction.
 */
void BattleUnit::turn(bool turret)
{
	int a = 0;

	if (turret)
		a = _toDirectionTurret - _directionTurret;
	else
		a = _toDirection - _direction;

	if (a != 0) {
		if (a > 0) {
			if (a <= 4) {
				if (!turret) _direction++;
				else _directionTurret++;
			} else {
				if (!turret) _direction--;
				else _directionTurret--;
			}
		} else {
			if (a > -4) {
				if (!turret) _direction--;
				else _directionTurret--;
			} else {
				if (!turret) _direction++;
				else _directionTurret++;
			}
		}
		if (_direction < 0) _direction = 7;
		if (_direction > 7) _direction = 0;
		if (_directionTurret < 0) _directionTurret = 7;
		if (_directionTurret > 7) _directionTurret = 0;
		if (_visible || _faction == FACTION_PLAYER)
			_cacheInvalid = true;
	}

	if (turret && _toDirectionTurret == _directionTurret)
	{
		// we officially reached our destination
		_status = STATUS_STANDING;
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
 * Set to true when the unit has to be redrawn from scratch.
 * @param cache
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
 * Check if the unit is still cached in the Map cache.
 * When the unit changes it needs to be re-cached.
 * @param invalid
 * @return cache
 */
Surface *BattleUnit::getCache(bool *invalid, int part) const
{
	if (part < 0) part = 0;
	*invalid = _cacheInvalid;
	return _cache[part];
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
 * @param aiming
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
 * Returns the soldier's amount of time units.
 * @return Time units.
 */
int BattleUnit::getDirectionTo(const Position &point) const
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
 * @param position The position defines which part of armor and/or bodypart is hit.
 * @param power
 * @param type
 */
void BattleUnit::damage(Position position, int power, ItemDamageType type, bool ignoreArmor)
{
	UnitSide side = SIDE_FRONT;
	int impactheight;
	UnitBodyPart bodypart = BODYPART_TORSO;

	if (power <= 0)
	{
		return;
	}

	power = (int)floor(power * _armor->getDamageModifier(type));

	if (type == DT_SMOKE) type = DT_STUN; // smoke doesn't do real damage, but stun damage

	if (!ignoreArmor)
	{
		if (position == Position(0, 0, 0))
		{
			side = SIDE_UNDER;
		}
		else
		{
			// normalize x and y towards north
			int x = 8, y = 8;
			switch(_direction)
			{
			case 0: // heading north
				x = position.x;
				y = 15 - position.y;
				break;
			case 1: // somewhere in between 0 and 2
				x = (position.x + position.y)/2;
				y = ((15 - position.y) + position.x)/2;
				break;
			case 2: // heading east
				x = 15 - position.y;
				y = 15 - position.x;
				break;
			case 3:
				x = (position.y + (15 - position.x))/2;
				y = (position.x + position.y)/2;
				break;
			case 4: // heading south
				x = 15 - position.x;
				y = position.y;
				break;
			case 5:
				x = ((15 - position.x) + (15 - position.y))/2;
				y = (position.y + (15 - position.x))/2;
				break;
			case 6: // heading west
				x = 15 - position.y;
				y = 15 - position.x;
				break;
			case 7:
				x = ((15 - position.y) + position.x)/2;
				y = ((15 - position.x) + (15 - position.y))/2;
				break;
			}
			// determine side
			if (y > 9)
				side = SIDE_FRONT;
			else if (y < 6)
				side = SIDE_REAR;
			else if (x < 6)
				side = SIDE_LEFT;
			else if (x > 9)
				side = SIDE_RIGHT;
			else
				side = SIDE_FRONT;
		}

		impactheight = 10*position.z/getHeight();

		if (impactheight > 4 && impactheight < 7) // torso
		{
			if (side == SIDE_LEFT)
			{
				bodypart = BODYPART_LEFTARM;
			}else if (side == SIDE_RIGHT)
			{
				bodypart = BODYPART_RIGHTARM;
			}else
			{
				bodypart = BODYPART_TORSO;
			}
		}else if (impactheight >= 7) //head
		{
			bodypart = BODYPART_HEAD;
		}else if (impactheight <=4) //legs
		{
			if (side == SIDE_LEFT || side == SIDE_FRONT)
			{
				bodypart = BODYPART_LEFTLEG;
			}else
			{
				bodypart = BODYPART_RIGHTLEG;
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
				_health = 0;

			if (type != DT_IN)
			{
				// fatal wounds
				if (isWoundable())
				{
					if (RNG::generate(0,power) > 2)
						_fatalWounds[bodypart] += RNG::generate(1,3);

					if (_fatalWounds[bodypart])
						moraleChange(-_fatalWounds[bodypart]);
				}
				// armor damage
				setArmor(getArmor(side) - (power/10) - 1, side);
			}
			_needPainKiller = true;
		}
	}
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
 * Intialises the falling sequence. Occurs after death or stunned.
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
	int endFrame = 3;
	if (_spawnUnit != "" && _specab != SPECAB_RESPAWN)
	{
		endFrame = 18;
	}
	if (_fallPhase == endFrame)
	{
		_fallPhase--;
		if (_health == 0)
			_status = STATUS_DEAD;
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
	return _status == STATUS_DEAD || _status == STATUS_UNCONSCIOUS;
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

	switch (actionType)
	{
		case BA_PRIME:
			return (int)floor(getStats()->tu * 0.50);
		case BA_THROW:
			return (int)floor(getStats()->tu * 0.25);
		case BA_AUTOSHOT:
			return (int)(getStats()->tu * item->getRules()->getTUAuto() / 100);
		case BA_SNAPSHOT:
			return (int)(getStats()->tu * item->getRules()->getTUSnap() / 100);
		case BA_HIT:
			return (int)(getStats()->tu * item->getRules()->getTUMelee() / 100);
		case BA_LAUNCH:
		case BA_AIMEDSHOT:
			return (int)(getStats()->tu * item->getRules()->getTUAimed() / 100);
		case BA_USE:
		case BA_MINDCONTROL:
		case BA_PANIC:
			if (item->getRules()->getFlatRate())
				return item->getRules()->getTUUse();
			else
				return (int)(getStats()->tu * item->getRules()->getTUUse() / 100);
		default:
			return 0;
	}
}


/**
 * Spend time units if it can. Return false if it can't.
 * @param tu
 * @param debugmode If this is true, the function actually does noting.
 * @return flag if it could spend the time units or not.
 */
bool BattleUnit::spendTimeUnits(int tu, bool debugmode)
{
	if (debugmode) return true;

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
	_tu = tu;
}

/**
 * Add this unit to the list of visible units. Returns true if this is a new one.
 * @param unit
 * @return
 */
bool BattleUnit::addToVisibleUnits(BattleUnit *unit)
{
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
double BattleUnit::getFiringAccuracy(BattleActionType actionType, BattleItem *item)
{
	double result = (double)(getStats()->firing / 100.0);

	double weaponAcc = item->getRules()->getAccuracySnap();
	if (actionType == BA_AIMEDSHOT || actionType == BA_LAUNCH)
		weaponAcc = item->getRules()->getAccuracyAimed();
	else if (actionType == BA_AUTOSHOT)
		weaponAcc = item->getRules()->getAccuracyAuto();
	else if (actionType == BA_HIT)
		weaponAcc = item->getRules()->getAccuracyMelee();

	result *= (double)(weaponAcc/100.0);

	if (_kneeled)
		result *= 1.15;

	if (item->getRules()->isTwoHanded())
	{
		// two handed weapon, means one hand should be empty
		if (getItem("STR_RIGHT_HAND") != 0 && getItem("STR_LEFT_HAND") != 0)
		{
			result *= 0.80;
		}
	}

	return result * getAccuracyModifier();
}

/**
 * To calculate firing accuracy. Takes health and fatal wounds into account.
 * Formula = accuracyStat * woundsPenalty(% health) * critWoundsPenalty (-10%/wound)
 * @return modifier
 */
double BattleUnit::getAccuracyModifier()
{
	double result = ((double)_health/(double)getStats()->health);

	int wounds = _fatalWounds[BODYPART_HEAD] + _fatalWounds[BODYPART_RIGHTARM];
	if (wounds > 9)
		wounds = 9;

	result *= 1 + (-0.1*wounds);

	return result;
}

/**
 * Calculate throwing accuracy.
 * @return throwing Accuracy
 */
double BattleUnit::getThrowingAccuracy()
{
	return (double)(getStats()->throwing/100.0) * getAccuracyModifier();
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
	double score = ((double)getStats()->reactions * (double)getTimeUnits()) / (double)getStats()->tu;
	return score;
}


/**
 * Prepare for a new turn.
 */
void BattleUnit::prepareNewTurn()
{
	// revert to original faction
	_faction = _originalFaction;

	// recover TUs
	int TURecovery = getStats()->tu;
	float encumbrance = (float)getStats()->strength / (float)getCarriedWeight();
	if (encumbrance < 1)
	{
	  TURecovery = int(encumbrance * TURecovery);
	}
	// Each fatal wound to the left or right leg reduces the soldier's TUs by 10%.
	TURecovery -= (TURecovery * (_fatalWounds[BODYPART_LEFTLEG]+_fatalWounds[BODYPART_RIGHTLEG] * 10))/100;
	setTimeUnits(TURecovery);

	// recover energy
	if (!isOut())
	{
		int ENRecovery = getStats()->tu / 3;
		// Each fatal wound to the body reduces the soldier's energy recovery by 10%.
		ENRecovery -= (_energy * (_fatalWounds[BODYPART_TORSO] * 10))/100;
		_energy += ENRecovery;
		if (_energy > getStats()->stamina)
			_energy = getStats()->stamina;
	}

	// suffer from fatal wounds
	_health -= getFatalWounds();

	// suffer from fire
	if (_fire > 0)
	{
		_health -= RNG::generate(5, 10);
		_fire--;
	}

	if (_health < 0)
		_health = 0;

	// if unit is dead, AI state should be gone
	if (_health == 0 && _currentAIState)
	{
		_currentAIState->exit();
		delete _currentAIState;
		_currentAIState = 0;
	}

	// recover stun 1pt/turn
	if (_stunlevel > 0)
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

	_dontReselect = false;
	_motionPoints = 0;
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
 * Mark this unit is not reselectable.
 */
void BattleUnit::dontReselect()
{
	_dontReselect = true;
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
void BattleUnit::setAIState(BattleAIState *aiState)
{
	if (_currentAIState)
	{
		if (dynamic_cast<AggroBAIState*>(aiState) != 0 && dynamic_cast<AggroBAIState*>(_currentAIState) != 0)
		{
			return; // try not to overwrite an existing aggro AI state
			// I tried using typeid but it does not produce the expected results :(
		}
		
		_currentAIState->exit();
		delete _currentAIState;
	}
	_currentAIState = aiState;
	_currentAIState->enter();
}

/**
 * Returns the current AI state.
 * @return Pointer to AI state.
 */
BattleAIState *BattleUnit::getCurrentAIState() const
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
 * @param tile
 */
void BattleUnit::setTile(Tile *tile, Tile *tileBelow)
{
	_tile = tile;
	if (!_tile)
		return;
	// unit could have changed from flying to walking or vice versa
	if (_status == STATUS_WALKING && _tile->hasNoFloor(tileBelow) && _armor->getMovementType() == MT_FLY)
	{
		_status = STATUS_FLYING;
		_floating = true;
	}
	else if (_status == STATUS_FLYING && !_tile->hasNoFloor(tileBelow) && _verticalDirection == 0)
	{
		_status = STATUS_WALKING;
		_floating = false;
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
* @param quickest Wether to get the quickest weapon, default true
* @return Pointer to item.
*/
BattleItem *BattleUnit::getMainHandWeapon(bool quickest) const
{
	BattleItem *weaponRightHand = getItem("STR_RIGHT_HAND");
	BattleItem *weaponLeftHand = getItem("STR_LEFT_HAND");

	// if there is only one weapon, or only one weapon loaded (rules out grenades) it's easy:
	if (!weaponRightHand || !weaponRightHand->getAmmoItem() || !weaponRightHand->getAmmoItem()->getAmmoQuantity())
		return weaponLeftHand;
	if (!weaponLeftHand || !weaponLeftHand->getAmmoItem() || !weaponLeftHand->getAmmoItem()->getAmmoQuantity())
		return weaponRightHand;

	// otherwise pick the one with the least snapshot TUs
	int tuRightHand = weaponRightHand->getRules()->getTUSnap();
	int tuLeftHand = weaponLeftHand->getRules()->getTUSnap();
	if (tuLeftHand >= tuRightHand)
	{
		return quickest?weaponRightHand:weaponLeftHand;
	}
	else
	{
		return quickest?weaponLeftHand:weaponRightHand;
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
 * Check if we have ammo and reload if needed (used for AI)
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

	spendTimeUnits(15,false);
	weapon->setAmmoItem(ammo);
	ammo->moveToOwner(0);

	return true;
}

/**
* Check if this unit is in the exit area.
* @return Is in the exit area?
*/
bool BattleUnit::isInExitArea(SpecialTileType stt) const
{
	return _tile && _tile->getMapData(MapData::O_FLOOR) && (_tile->getMapData(MapData::O_FLOOR)->getSpecialType() == stt);
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
* Adds one to the firing exp counter.
*/
void BattleUnit::addThrowingExp()
{
	_expThrowing++;
}

/**
* Adds one to the firing exp counter.
*/
void BattleUnit::addPsiExp()
{
	_expPsiSkill++;
}

/**
* Adds one to the firing exp counter.
*/
void BattleUnit::addMeleeExp()
{
	_expMelee++;
}

/**
* Check if unit eligible for squaddie promotion. If yes, promote the unit.
* Increase the mission counter. Calculate the experience increases.
* @return True if the soldier was eligible for squaddie promotion.
*/
bool BattleUnit::postMissionProcedures(SavedGame *geoscape)
{
	Soldier *s = geoscape->getSoldier(_id);
	if (s == 0)
	{
		return false;
	}

	s->addMissionCount();
	s->addKillCount(_kills);

	UnitStats *stats = s->getCurrentStats();
	int healthLoss = stats->health - _health;

	s->setWoundRecovery(RNG::generate((healthLoss*0.5),(healthLoss*1.5)));

	if (_expBravery && stats->bravery < 100)
	{
		if (_expBravery > RNG::generate(0,10)) stats->bravery += 10;
	}
	if (_expReactions && stats->reactions < 100)
	{
		stats->reactions += improveStat(_expReactions);
	}
	if (_expFiring && stats->firing < 120)
	{
		stats->firing += improveStat(_expFiring);
	}
	if (_expMelee && stats->melee < 120)
	{
		stats->melee += improveStat(_expMelee);
	}
	if (_expThrowing && stats->throwing < 120)
	{
		stats->throwing += improveStat(_expThrowing);
	}
	if (_expPsiSkill && stats->psiSkill < 100)
	{
		stats->psiSkill += improveStat(_expPsiSkill);
	}

	if (_expBravery || _expReactions || _expFiring || _expPsiSkill || _expMelee)
	{
		if (s->getRank() == RANK_ROOKIE)
			s->promoteRank();
		int v;
		v = 80 - stats->tu;
		if (v > 0) stats->tu += RNG::generate(0, v/10 + 2);
		v = 60 - stats->health;
		if (v > 0) stats->health += RNG::generate(0, v/10 + 2);
		v = 70 - stats->strength;
		if (v > 0) stats->strength += RNG::generate(0, v/10 + 2);
		v = 100 - stats->stamina;
		if (v > 0) stats->stamina += RNG::generate(0, v/10 + 2);
		return true;
	}
	else
	{
		return false;
	}
}

/**
* Converts the number of experience to the stat increase.
* @param Experience counter.
* @return Stat increase.
*/
int BattleUnit::improveStat(int exp)
{
	double v = 4;
	if (exp < 3) v = 1;
	if (exp < 6) v = 2;
	if (exp < 10) v = 3;
	return (int)(v/2.0 + RNG::generate(0.0, v));
}

/*
 * Get the unit's minimap sprite index. Used to display the unit on the minimap
 * @return the unit minimap index
 */
int BattleUnit::getMiniMapSpriteIndex () const
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
		return 6;
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
 * @param healAmount the amount of fatal wound healed
 * @param healthAmount The amount of health to add to soldier health
 */
void BattleUnit::heal(int part, int healAmount, int healthAmount)
{
	if (part < 0 || part > 5)
		return;
	if(!_fatalWounds[part])
		return;
	_fatalWounds[part] -= healAmount;
	_health += healthAmount;
	if (_health > getStats()->health)
		_health = getStats()->health;
}

/**
 * Restore soldier morale
 */
void BattleUnit::painKillers ()
{
	if (!getFatalWounds() || !_needPainKiller)
	{
		return ;
	}
	_needPainKiller = false;
	int lostHealth = getStats()->health - _health;
	_morale += lostHealth;
	if (_morale > 100) _morale = 100;
}

/**
 * Restore soldier energy and reduce stun level
 * @param energy The amount of energy to add
 * @param s The amount of stun level to reduce
 */
void BattleUnit::stimulant (int energy, int s)
{
	_energy += energy;
	if (_energy > getStats()->stamina)
		_energy = getStats()->stamina;
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
 * @return name Widecharstring of the unit's name.
 */
std::wstring BattleUnit::getName(Language *lang, bool debugAppendId) const
{
	if (_type != "SOLDIER" && lang != 0)
	{
		std::wstring ret;

		if (_type.find("STR_") != std::string::npos)
			ret = lang->getString(_type);
		else
			ret = lang->getString(_race);

		if (debugAppendId)
		{
			std::wstringstream ss;
			ss << ret << L" " << _id;
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
UnitStats *BattleUnit::getStats()
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
  * Get the unit's loft ID. This is only one, as it is repeated over the entire height of the unit.
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
 * Get the unit's death sound.
 * @return id.
 */
int BattleUnit::getDeathSound() const
{
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
  * @return true or false
  */
bool BattleUnit::isWoundable() const
{
	return (_type=="SOLDIER");
}
/**
  * Get whether the unit is affected by morale loss.
  * Normally only small units are affected by morale loss.
  * @return true or false
  */
bool BattleUnit::isFearable() const
{
	return (_armor->getSize() == 1);
}

/**
  * Get the unit's intelligence. Is the number of turns AI remembers a soldiers position.
  * @return intelligence 
  */
int BattleUnit::getIntelligence() const
{
	return _intelligence;
}

/**
/// Get the unit's aggression.
 */
int BattleUnit::getAggression() const
{
	return _aggression;
}

/**
/// Get the units's special ability.
 */
int BattleUnit::getSpecialAbility() const
{
	return _specab;
}

/**
/// Get the units's special ability.
 */
void BattleUnit::setSpecialAbility(SpecialAbility specab)
{
	_specab = specab;
}

/**
 * Get the unit that the victim is morphed into when attacked.
 * @return unit.
 */
std::string BattleUnit::getZombieUnit() const
{
	return _zombieUnit;
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
 * @return unit.
 */
void BattleUnit::setSpawnUnit(std::string spawnUnit)
{
	_spawnUnit = spawnUnit;
}

/**
/// Get the units's rank string.
 */
std::string BattleUnit::getRankString() const
{
	return _rank;
}

/**
/// Get the geoscape-soldier object.
 */
Soldier *BattleUnit::getGeoscapeSoldier() const
{
	return _geoscapeSoldier;
}

/**
/// Add a kill to the counter.
 */
void BattleUnit::addKillCount()
{
	_kills++;
}

/**
/// Get unit type.
 */
std::string BattleUnit::getType() const
{
	return _type;
}

/**
/// Set unit's active hand.
 */
void BattleUnit::setActiveHand(const std::string &hand)
{
	if (_activeHand != hand) _cacheInvalid = true;
	_activeHand = hand;
}
/**
 * Get unit's active hand.
 */
std::string BattleUnit::getActiveHand() const
{
	if (getItem(_activeHand)) return _activeHand;
	if (getItem("STR_LEFT_HAND")) return "STR_LEFT_HAND";
	return "STR_RIGHT_HAND";
}

/**
 * Converts unit to another faction (original faction is still stored).
 */
void BattleUnit::convertToFaction(UnitFaction f)
{
	_faction = f;
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
 * Set health to 0 and set status dead - used when getting zombified.
 */
int BattleUnit::getAggroSound() const
{
	return _aggroSound;
}
/**
 * Set a specific number of timeunits.
 * @param tu
 */
void BattleUnit::setEnergy(int energy)
{
	_energy = energy;
}

/**
 * Halve this unit's armor values (for beginner mode)
 */
void BattleUnit::halveArmor()
{
	_currentArmor[0] /= 2;
	_currentArmor[1] /= 2;
	_currentArmor[2] /= 2;
	_currentArmor[3] /= 2;
	_currentArmor[4] /= 2;
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
 * @return weight
 */
int BattleUnit::getCarriedWeight(BattleItem *draggingItem) const
{
	int weight = 6;
	for (std::vector<BattleItem*>::const_iterator i = _inventory.begin(); i != _inventory.end(); ++i)
	{
		if ((*i) == draggingItem) continue;
		weight += (*i)->getRules()->getWeight();
		if ((*i)->getAmmoItem() && (*i)->getAmmoItem()->getRules()->getClipSize() != -1) weight += (*i)->getAmmoItem()->getRules()->getWeight();
	}
	return weight;
}

/**
 * Set how long this unit will be exposed for.
 * @param turns
 */
void BattleUnit::setTurnsExposed (int turns)
{
	_turnsExposed = turns;
}

/**
 * Get how long this unit will be exposed for.
 * @return turns
 */
int BattleUnit::getTurnsExposed () const
{
	return _turnsExposed;
}

/**
 * Get This unit's original Faction.
 * @return original faction
 */
UnitFaction BattleUnit::getOriginalFaction() const
{
	return _originalFaction;
}

/// invalidate cache; call after copying object :(
void BattleUnit::invalidateCache()
{
	for (int i = 0; i < 5; ++i) { _cache[i] = 0; }
	_cacheInvalid = true;
}

}
