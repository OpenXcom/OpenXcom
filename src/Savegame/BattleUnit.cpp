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
#include "BattleUnit.h"
#include "BattleItem.h"
#include <cmath>
#include "../Engine/Palette.h"
#include "../Engine/Language.h"
#include "../Battlescape/Pathfinding.h"
#include "Alien.h"
#include "Soldier.h"
#include "../Ruleset/RuleArmor.h"
#include "../Engine/RNG.h"

namespace OpenXcom
{

/**
 * Initializes a BattleUnit.
 * @param rules Pointer to RuleUnit object.
 * @param faction Which faction the units belongs to.
 */
BattleUnit::BattleUnit(Unit *unit, UnitFaction faction) : _unit(unit), _faction(faction), _id(0), _pos(Position()), _lastPos(Position()), _direction(0), _status(STATUS_STANDING), _walkPhase(0), _fallPhase(0), _cached(false), _kneeled(false), _dontReselect(false), _fire(0)
{
	_tu = unit->getTimeUnits();
	_energy = unit->getStamina();
	_health = unit->getHealth();
	_morale = 100;
	_stunlevel = 0;
	_armor[SIDE_FRONT] = unit->getArmor()->getFrontArmor();
	_armor[SIDE_LEFT] = unit->getArmor()->getSideArmor();
	_armor[SIDE_RIGHT] = unit->getArmor()->getSideArmor();
	_armor[SIDE_REAR] = unit->getArmor()->getRearArmor();
	_armor[SIDE_UNDER] = unit->getArmor()->getUnderArmor();
	for (int i = 0; i < 6; i++)
		_fatalWounds[i] = 0;
}

/**
 *
 */
BattleUnit::~BattleUnit()
{

}

/**
 * Loads the unit from a YAML file.
 * @param node YAML node.
 */
void BattleUnit::load(const YAML::Node &node)
{
	int a = 0;

	node["id"] >> _id;
	std::string name;
	node["faction"] >> a;
	_faction = (UnitFaction)a;
	node["status"] >> a;
	_status = (UnitStatus)a;

	node["X"] >> _pos.x;
	node["Y"] >> _pos.y;
	node["Z"] >> _pos.z;
	node["direction"] >> _direction;

	node["tu"] >> _tu;
	node["health"] >> _health;
	node["energy"] >> _energy;
	node["morale"] >> _morale;

	node["kneeled"] >> _kneeled;
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
	out << YAML::Key << "status" << YAML::Value << _status;

	out << YAML::Key << "X" << YAML::Value << _pos.x;
	out << YAML::Key << "Y" << YAML::Value << _pos.y;
	out << YAML::Key << "Z" << YAML::Value << _pos.z;
	out << YAML::Key << "direction" << YAML::Value << _direction;

	out << YAML::Key << "tu" << YAML::Value << _tu;
	out << YAML::Key << "health" << YAML::Value << _health;
	out << YAML::Key << "energy" << YAML::Value << _energy;
	out << YAML::Key << "morale" << YAML::Value << _morale;

	out << YAML::Key << "kneeled" << YAML::Value << _kneeled;

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
 * Changes the BattleUnit's unique ID.
 * @param id Unique ID.
 */
void BattleUnit::setId(int id)
{
	_id = id;
}

/**
 * Returns the ruleset for the unit's type.
 * @return Pointer to ruleset.
 */
Unit *const BattleUnit::getUnit() const
{
	return _unit;
}

/**
 * Changes the BattleUnit's position.
 * @param pos position
 */
void BattleUnit::setPosition(const Position& pos)
{
	_lastPos = _pos;
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
 * Changes the BattleUnit's direction.
 * @param direction
 */
void BattleUnit::setDirection(int direction)
{
	_direction = direction;
	_toDirection = direction;
}

/**
 * Changes the BattleUnit's direction.
 * @return direction
 */
int BattleUnit::getDirection() const
{
	return _direction;
}

/**
 * Gets the unit's status.
 * @return the unit's status
 */
UnitStatus BattleUnit::getStatus()
{
	return _status;
}

/**
 * Initialises variables to start walking.
 * @param direction Which way to walk.
 * @param destination The position we should end up on.
 */
void BattleUnit::startWalking(int direction, const Position &destination)
{
	_direction = direction;
	_status = STATUS_WALKING;
	_walkPhase = 0;
	_destination = destination;
	_lastPos = _pos;
	_cached = false;
	_kneeled = false;
}

/**
 * This will increment the walking phase.
 */
void BattleUnit::keepWalking()
{
	int middle, end;
	// diagonal walking takes double the steps
	middle = 4 + 4 * (_direction % 2);
	end = 8 + 8 * (_direction % 2);

	_walkPhase++;

	if (_walkPhase == middle)
	{
		// we assume we reached our destination tile
		// this is actually a drawing hack, so soldiers are not overlapped by floortiles
		_pos = _destination;
	}

	if (_walkPhase == end)
	{
		// we officially reached our destination tile
		_status = STATUS_STANDING;
		_walkPhase = 0;
	}

	_cached = false;
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
 * @param point.
 */
void BattleUnit::lookAt(const Position &point)
{
	double ox = point.x - _pos.x;
	double oy = point.y - _pos.y;
	double angle = atan2(ox, oy);
	if (angle < 0) angle += (M_PI*2); // convert to a range from 0 to M_PI*2
	_toDirection = (int)((angle/(M_PI_4))+M_PI_4/2.0); // convert to 8 directions, somewhat rounded
	if (_toDirection > 7) _toDirection = 7;

	if (_toDirection != _direction)
	{
		_status = STATUS_TURNING;
	}
}

/**
 * Look at a direction.
 * @param direction.
 */
void BattleUnit::lookAt(int direction)
{
	_toDirection = direction;
	_status = STATUS_TURNING;
}

/**
 * Advances the turning towards the target direction.
 */
void BattleUnit::turn()
{
    int a = _toDirection - _direction;
    if (a != 0) {
        if (a > 0) {
            if (a <= 4) {
                _direction++;
            } else {
                _direction--;
            }
        } else {
            if (a > -4) {
                _direction--;
            } else {
                _direction++;
            }
        }
        if (_direction < 0) _direction = 7;
        if (_direction > 7) _direction = 0;
		_cached = false;
    }

	if (_toDirection == _direction)
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
 * @param cached
 */
void BattleUnit::setCached(bool cached)
{
	_cached = cached;
}

/**
 * Check if the unit is still cached in the Map cache.
 * When the unit changes it needs to be re-cached.
 * @return bool
 */
bool BattleUnit::isCached() const
{
	return _cached;
}

/**
 * Kneel down.
 * @param to kneel or to stand up
 */
void BattleUnit::kneel(bool kneeled)
{
	_kneeled = kneeled;
	setCached(false);
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
 * Aim. (shows the right hand sprite and weapon holding)
 * @param aiming
 */
void BattleUnit::aim(bool aiming)
{
	if (aiming)
		_status = STATUS_AIMING;
	else
		_status = STATUS_STANDING;

	setCached(false);
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
 */
void BattleUnit::damage(Position position, int power)
{
	int damage;
	UnitSide side;
	int impactheight, x=8, y=8;
	UnitBodyPart bodypart;

	if (power <= 0)
	{
		return;
	}

	if (position == Position(0, 0, 0))
	{
		side = SIDE_UNDER;
	}
	else
	{
		// normalize x and y
		switch(_direction)
		{
		case 0: // heading north, all is the same
			x = position.x;
			y = position.y;
			break;
		case 1: // somewhere in between 0 and 2
			x = (position.x + (15 - position.y))/2;
			y = (position.y + position.x)/2;
			break;
		case 2: // heading east
			x = 15 - position.y;
			y = position.x;
			break;
		case 3:
			x = ((15 - position.y) + (15 - position.x))/2;
			y = (position.x + (15 - position.y))/2;
			break;
		case 4: // heading south, both axis inversed
			x = 15 - position.x;
			y = 15 - position.y;
			break;
		case 5:
			x = ((15 - position.x) + position.y)/2;
			y = ((15 - position.y) + (15 - position.x))/2;
			break;
		case 6: // heading west
			x = position.y;
			y = 15 - position.x;
			break;
		case 7:
			x = (position.y + position.x)/2;
			y = ((15 - position.x) + position.y)/2;
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

	impactheight = 10*position.z/(isKneeled()?_unit->getKneelHeight():_unit->getStandHeight());

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

	damage = (power - getArmor(side));

	if (damage > 0)
	{
		// fatal wounds
		if (RNG::generate(0,damage) > 2)
			_fatalWounds[bodypart] += RNG::generate(1,3);

		if (_fatalWounds[bodypart])
			moraleChange(-_fatalWounds[bodypart]);

		// armor damage
		setArmor(getArmor(side) - (damage+5)/10, side);
		// health damage
		_health -= damage;
		if (_health < 0)
			_health = 0;
	}
}

/**
 * Do an amount of stun. Can be negative amount = stun recovery.
 * @param power
 */
void BattleUnit::stun(int power)
{
	_stunlevel += power;
	// recover from unconscious
	if (_status == STATUS_UNCONSCIOUS && _stunlevel < _health && _health > 0)
	{
		_status = STATUS_STANDING;
	}
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
	_status = STATUS_FALLING;
	_fallPhase = 0;
	setCached(false);
}

/**
 * Advances the phase of falling sequence.
 */
void BattleUnit::keepFalling()
{
	_fallPhase++;
	if (_fallPhase == 3)
	{
		_fallPhase = 2;
		if (_health == 0)
			_status = STATUS_DEAD;
		else
			_status = STATUS_UNCONSCIOUS;
	}
	setCached(false);
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
 * @param action
 * @param item
 * @return TUs
 */
int BattleUnit::getActionTUs(BattleActionType actionType, BattleItem *item)
{
	switch (actionType)
	{
		case BA_PRIME:
			return (int)floor(getUnit()->getTimeUnits() * 0.50);
		case BA_THROW:
			return (int)floor(getUnit()->getTimeUnits() * 0.25);
		case BA_AUTOSHOT:
			return (int)(getUnit()->getTimeUnits() * item->getRules()->getTUAuto() / 100);
		case BA_SNAPSHOT:
			return (int)(getUnit()->getTimeUnits() * item->getRules()->getTUSnap() / 100);
		case BA_AIMEDSHOT:
			return (int)(getUnit()->getTimeUnits() * item->getRules()->getTUAimed() / 100);
	}

	return 0;
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
 * @param debugmode If this is true, the function actually does noting.
 * @return flag if it could spend the time units or not.
 */
bool BattleUnit::spendEnergy(int tu, bool debugmode)
{
	if (debugmode) return true;
	int eu = tu / 3;

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
 * Calculate firing accuracy.
 * Formula = accuracyStat * weaponAccuracy * kneelingbonus(1.15) * one-handPenalty(0.8) * woundsPenalty(% health) * critWoundsPenalty (-10%/wound)
 * @param weaponAccuracy
 * @return firing Accuracy
 */
double BattleUnit::getFiringAccuracy(int weaponAccuracy)
{
	double result = (double)(_unit->getFiringAccuracy()/100.0);

	result *= (double)(weaponAccuracy/100.0);

	if (_kneeled)
		result *= 1.15;

	result *= ((double)_health/(double)_unit->getHealth());

	return result;
}

/**
 * Calculate throwing accuracy.
 * Formula = accuracyStat * woundsPenalty(% health) * critWoundsPenalty (-10%/wound)
 * @param weaponAccuracy
 * @return firing Accuracy
 */
double BattleUnit::getThrowingAccuracy()
{
	double result = (double)(_unit->getFiringAccuracy()/100.0);

	result *= ((double)_health/(double)_unit->getHealth());

	return result;
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
	_armor[side] = armor;
}

/**
 * Get the armor value of a certain armor side.
 * @param side The side of the armor.
 * @return Amount of armor.
 */
int BattleUnit::getArmor(UnitSide side)
{
	return _armor[side];
}

/**
 * Get total amount of fatal wounds this unit has.
 * @return Number of fatal wounds.
 */
int BattleUnit::getFatalWounds()
{
	int sum = 0;
	for (int i = 0; i < 6; i++)
		sum += _fatalWounds[i];
	return sum;
}


/**
 * Little formula to calculate reaction score.
 * @return Reaction score.
 */
double BattleUnit::getReactionScore() const
{
	//(Reactions Stat) × (Current Time Units / Max TUs) 
	double score = ((double)_unit->getReactions() * (double)getTimeUnits()) / (double)_unit->getTimeUnits();
	return score;
}


/**
 * Prepare for a new turn.
 */
void BattleUnit::prepareNewTurn()
{
	// recover TUs
	int TURecovery = _unit->getTimeUnits();
	// Each fatal wound to the left or right leg reduces the soldier's TUs by 10%.
	TURecovery -= (TURecovery * (_fatalWounds[BODYPART_LEFTLEG]+_fatalWounds[BODYPART_RIGHTLEG] * 10))/100;
	setTimeUnits(TURecovery);

	// recover energy
	int ENRecovery = _unit->getTimeUnits() / 3;
	// Each fatal wound to the body reduces the soldier's energy by 10%.
	ENRecovery -= (_energy * (_fatalWounds[BODYPART_TORSO] * 10))/100;
	_energy += ENRecovery;
	if (_energy > _unit->getStamina())
		_energy = _unit->getStamina();

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

	// recover stun 1pt/turn
	if (_stunlevel > 0)
		stun(-1);

	_dontReselect = false;
}


/**
 * Morale change with bounds check.
 * @param change can be positive or negative
 */
void BattleUnit::moraleChange(int change)
{
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
bool BattleUnit::reselectAllowed()
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
int BattleUnit::getFire()
{
	return _fire;
}

}
