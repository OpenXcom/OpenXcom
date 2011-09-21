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
#include "../Battlescape/BattlescapeState.h"
#include "Soldier.h"

namespace OpenXcom
{

class Tile;
class BattleItem;
class Unit;
class RuleUnit;
class BattleAIState;
class BattlescapeState;
class Node;
class Surface;
class RuleInventory;

enum UnitStatus {STATUS_STANDING, STATUS_WALKING, STATUS_TURNING, STATUS_AIMING, STATUS_FALLING, STATUS_DEAD, STATUS_UNCONSCIOUS};
enum UnitFaction {FACTION_PLAYER, FACTION_HOSTILE, FACTION_NEUTRAL};
enum UnitSide {SIDE_FRONT, SIDE_LEFT, SIDE_RIGHT, SIDE_REAR, SIDE_UNDER};
enum UnitBodyPart {BODYPART_HEAD, BODYPART_TORSO, BODYPART_RIGHTARM, BODYPART_LEFTARM, BODYPART_RIGHTLEG, BODYPART_LEFTLEG};

/**
 * Represents a moving unit in the battlescape, player controlled or AI controlled
 * it holds info about it's position, items carrying, stats, etc
 */
class BattleUnit
{
private:
	Unit *_unit;
	UnitFaction _faction;
	int _id;
	Position _pos;
	Tile *_tile;
	Position _lastPos;
	int _direction, _toDirection;
	Position _destination;
	UnitStatus _status;
	int _walkPhase, _fallPhase;
	std::vector<BattleUnit *> _visibleUnits;
	std::vector<Tile *> _visibleTiles;
	int _tu, _energy, _health, _morale, _stunlevel;
	bool _kneeled, _dontReselect;
	int _armor[5];
	int _fatalWounds[6];
	int _fire;
	std::vector<BattleItem*> _inventory;
	BattleAIState *_currentAIState;
	bool _visible;
	Surface *_cache;
	bool _cacheInvalid;
	int _expBravery, _expReactions, _expFiring, _expThrowing, _expPsiSkill, _expMelee;
public:
	/// Creates a BattleUnit.
	BattleUnit(Unit *_unit, UnitFaction faction);
	/// Cleans up the BattleUnit.
	virtual ~BattleUnit();
	/// Loads the unit from YAML.
	void load(const YAML::Node& node);
	/// Saves the unit to YAML.
	void save(YAML::Emitter& out) const;
	/// Gets the BattleUnit's ID.
	int getId() const;
	/// Sets the BattleUnit's ID.
	void setId(int id);
	/// Gets the unit's soldier data.
	Unit *const getUnit() const;
	/// Sets the unit's position
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
	UnitStatus getStatus() const;
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
	void turn();
	/// Abort turning.
	void abortTurn();
	/// Gets the soldier's gender.
	SoldierGender getGender() const;
	/// Gets the unit's faction.
	UnitFaction getFaction() const;
	/// Set the cached flag.
	void setCache(Surface *cache);
	/// If this unit is cached on the battlescape.
	Surface *getCache(bool *invalid) const;
	/// Kneel down.
	void kneel(bool kneeled);
	/// Is kneeled?
	bool isKneeled() const;
	/// Aim.
	void aim(bool aiming);
	/// Gets the unit's time units.
	int getTimeUnits() const;
	/// Gets the unit's stamina.
	int getEnergy() const;
	/// Gets the unit's health.
	int getHealth() const;
	/// Gets the unit's bravery.
	int getMorale() const;
	/// Do damage to the unit.
	void damage(Position position, int power);
	/// Do stun to the unit.
	void stun(int power);
	/// Gets the unit's stun level.
	int getStunlevel() const;
	/// Start falling sequence.
	void startFalling();
	/// Increment the falling sequence.
	void keepFalling();
	/// Get falling sequence.
	int getFallingPhase() const;
	/// The unit is out - either dead or unconscious.
	bool isOut() const;
	/// Get the number of time units a certain action takes.
	int getActionTUs(BattleActionType actionType, BattleItem *item) const;
	/// Spend time units if it can.
	bool spendTimeUnits(int tu, bool debugmode);
	/// Spend energy if it can.
	bool spendEnergy(int tu, bool debugmode);
	/// Set time units.
	void setTimeUnits(int tu);
	/// Add unit to visible units.
	bool addToVisibleUnits(BattleUnit *unit);
	/// Get the list of visible units.
	std::vector<BattleUnit*> *const getVisibleUnits();
	/// Clear visible units.
	void clearVisibleUnits();
	/// Calculate firing accuracy.
	double getFiringAccuracy(int baseAccuracy) const;
	/// Calculate throwing accuracy.
	double getThrowingAccuracy() const;
	/// Set armor value.
	void setArmor(int armor, UnitSide side);
	/// Get armor value.
	int getArmor(UnitSide side) const;
	/// Get total number of fatal wounds.
	int getFatalWounds() const;
	/// Get the current reaction score.
	double getReactionScore() const;
	/// Prepare for a new turn.
	void prepareNewTurn();
	/// Morale change
	void moraleChange(int change);
	/// Don't reselect this unit
	void dontReselect();
	/// Check whether reselecting this unit is allowed.
	bool reselectAllowed() const;
	/// Set fire.
	void setFire(int fire);
	/// Get fire.
	int getFire() const;
	/// Get the list of items in the inventory.
	std::vector<BattleItem*> *const getInventory();
	/// Let AI do their thing.
	void think(BattleAction *action);
	/// Get current AI state.
	BattleAIState *getCurrentAIState() const;
	/// Set next AI State
	void setAIState(BattleAIState *aiState);
	/// Set whether this unit is visible
	void setVisible(bool flag);
	/// Get whether this unit is visible
	bool getVisible() const;
	/// Sets the unit's tile it's standing on
	void setTile(Tile *tile);
	/// Gets the unit's tile.
	Tile *getTile() const;
	/// Gets the item in the specified slot.
	BattleItem *getItem(RuleInventory *slot, int x = 0, int y = 0) const;
	/// Gets the item in the specified slot.
	BattleItem *getItem(const std::string &slot, int x = 0, int y = 0) const;
	/// Gets the item in the main hand.
	BattleItem *getMainHandWeapon() const;
	/// Check if this unit is in the exit area
	bool isInExitArea() const;
	/// Gets the unit height taking into account kneeling/standing.
	int getHeight() const;
	/// Get the sprite index for the minimap
	int getMiniMapSpriteIndex () const;
};

}

#endif
