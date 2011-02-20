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
#ifndef OPENXCOM_BATTLEUNIT_H
#define OPENXCOM_BATTLEUNIT_H

#include <vector>
#include <string>
#include "../Battlescape/Position.h"
#include "Soldier.h"

namespace OpenXcom
{

class RuleUnitSprite;
class Tile;
class BattleItem;
class Soldier;

enum UnitStatus {STATUS_STANDING, STATUS_SITTING, STATUS_WALKING, STATUS_TURNING, STATUS_FALLING, STATUS_DEAD};
enum UnitFaction {FACTION_PLAYER, FACTION_HOSTILE, FACTION_NEUTRAL, FACTION_FRIENDLY};

/**
 * Represents a moving unit in the battlescape, player controlled or AI controlled
 * it holds info about it's position, items carrying, stats, etc
 */
class BattleUnit
{
private:
	RuleUnitSprite *_renderRules;
	UnitFaction _faction;
	int _id;
	Position _pos;
	Position _lastPos;
	int _direction, _toDirection;
	Position _destination;
	UnitStatus _status;
	int _walkPhase;
	std::vector<Tile *> _visibleTiles;
	int _tu, _energy, _health, _morale;
	Soldier *_soldier;
	//BattleAI *_ai;
	//RuleNPC *_NPCRule;
	std::wstring _name;
	bool _cached;
	BattleItem *_rightHandItem, *_leftHandItem;
public:
	/// Creates a BattleUnit.
	BattleUnit(RuleUnitSprite *renderRules, UnitFaction faction);
	/// Cleans up the BattleUnit.
	virtual ~BattleUnit();
	/// Loads the unit from YAML.
	void load(const YAML::Node& node);
	/// Saves the unit to YAML.
	void save(YAML::Emitter& out) const;
	/// Assigns a geoscape soldier.
	void setSoldier(Soldier *soldier);
	/// Assigns an NPC rule and Battle AI.
	//void setNPC(RuleNPC *NPCRule, BattleAI *ai);
	/// Gets the BattleUnit's ID.
	int getId() const;
	/// Sets the BattleUnit's ID.
	void setId(int id);
	/// Gets the unit's rendering ruleset.
	RuleUnitSprite *const getRenderRules() const;
	/// Sets the unit's position X, Y, Z
	void setPosition(const Position& pos);
	/// Gets the unit's position.
	const Position& getPosition() const;
	/// Gets the unit's position.
	const Position& getLastPosition() const;
	/// Sets the unit's direction 0-7.
	void setDirection(int direction);
	/// Gets the unit's direction.
	int getDirection() const;
	/// Gets the unit's status.
	UnitStatus getStatus();
	/// Start the walkingPhase
	void startWalking(int direction, const Position &destination);
	/// Increase the walkingPhase
	void keepWalking();
	/// Gets the walking phase for animation and sound
	int getWalkingPhase() const;
	/// Gets the walking phase for diagonal walking
	int getDiagonalWalkingPhase() const;
	/// Gets the unit's destination when walking
	const Position &getDestination() const;
	/// Look at a certain point.
	void lookAt(const Position &point);
	/// Look at a certain direction.
	void lookAt(int direction);
	/// Turn to the destination direction.
	void turn(bool spendTU);
	/// Gets the unit's maximum time units.
	int getMaxTimeUnits() const;
	/// Gets the unit's maximum stamina.
	int getMaxStamina() const;
	/// Gets the unit's maximum health.
	int getMaxHealth() const;
	/// Gets the soldier's name.
	std::wstring getName() const;
	/// Gets the soldier's gender.
	SoldierGender getGender() const;
	/// Gets the unit's faction.
	UnitFaction getFaction() const;
	/// Sets the unit's name. Only for aliens.
	void setName(const std::wstring &name);
	/// Gets a sprite version of the soldier's rank.
	int getRankSprite() const;
	/// Set the cached flag.
	void setCached(bool cached);
	/// If this unit is cached on the battlescape.
	bool isCached() const;

};

}

#endif
