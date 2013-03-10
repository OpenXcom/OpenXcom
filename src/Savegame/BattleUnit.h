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
#ifndef OPENXCOM_BATTLEUNIT_H
#define OPENXCOM_BATTLEUNIT_H

#include <vector>
#include <string>
#include "../Battlescape/Position.h"
#include "../Battlescape/BattlescapeGame.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/Unit.h"
#include "../Ruleset/MapData.h"
#include "Soldier.h"

namespace OpenXcom
{

class Tile;
class BattleItem;
class Unit;
class BattleAIState;
class BattlescapeState;
class Node;
class Surface;
class RuleInventory;
class Soldier;
class Armor;
class SavedGame;
class Language;
class AggroBAIState;
class PatrolBAIState;

enum UnitStatus {STATUS_STANDING, STATUS_WALKING, STATUS_FLYING, STATUS_TURNING, STATUS_AIMING, STATUS_COLLAPSING, STATUS_DEAD, STATUS_UNCONSCIOUS, STATUS_PANICKING, STATUS_BERSERK};
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
	UnitFaction _faction, _originalFaction;
	UnitFaction _killedBy;
	int _id;
	Position _pos;
	Tile *_tile;
	Position _lastPos;
	int _direction, _toDirection;
	int _directionTurret, _toDirectionTurret;
	int _verticalDirection;
	Position _destination;
	UnitStatus _status;
	int _walkPhase, _fallPhase;
	std::vector<BattleUnit *> _visibleUnits;
	std::vector<Tile *> _visibleTiles;
	int _tu, _energy, _health, _morale, _stunlevel;
	bool _kneeled, _floating, _dontReselect;
	int _currentArmor[5];
	int _fatalWounds[6];
	int _fire;
	std::vector<BattleItem*> _inventory;
	BattleAIState *_currentAIState;
	bool _visible;
	Surface *_cache[5];
	bool _cacheInvalid;
	int _expBravery, _expReactions, _expFiring, _expThrowing, _expPsiSkill, _expMelee;
	int improveStat(int exp);
	int _turretType;
	bool _needPainKiller;
	int _motionPoints;
	int _kills;
	int _faceDirection; // used only during strafeing moves
	
	// static data
	std::string _type;
	std::string _rank;
	std::string _race;
	std::wstring _name;
	UnitStats _stats;
	int _standHeight, _kneelHeight, _floatHeight;
	int _value, _deathSound, _aggroSound, _moveSound;
	int _intelligence, _aggression;
	SpecialAbility _specab;
	std::string _zombieUnit, _spawnUnit;
	Armor *_armor;
	SoldierGender _gender;
	std::string _activeHand;
	Soldier *_geoscapeSoldier;
	BattleUnit *_charging;
	int _turnsExposed;
	std::vector<int> _loftempsSet;
	Unit *_unitRules;
public:
	static const int MAX_SOLDIER_ID = 1000000;
	/// Creates a BattleUnit.
	BattleUnit(Soldier *soldier, UnitFaction faction);
	BattleUnit(Unit *unit, UnitFaction faction, int id, Armor *armor);
	BattleUnit(BattleUnit&);
	/// Cleans up the BattleUnit.
	~BattleUnit();
	/// Loads the unit from YAML.
	void load(const YAML::Node& node);
	/// Saves the unit to YAML.
	void save(YAML::Emitter& out) const;
	/// Gets the BattleUnit's ID.
	int getId() const;
	/// Sets the unit's position
	void setPosition(const Position& pos, bool updateLastPos = true);
	/// Gets the unit's position.
	const Position& getPosition() const;
	/// Gets the unit's position.
	const Position& getLastPosition() const;
	/// Sets the unit's direction 0-7.
	void setDirection(int direction);
	/// Sets the unit's face direction (only used by strafing moves)
	void setFaceDirection(int direction);
	/// Gets the unit's direction.
	int getDirection() const;
	/// Gets the unit's face direction (only used by strafing moves)
	int getFaceDirection() const;
	/// Gets the unit's turret direction.
	int getTurretDirection() const;
	/// Gets the unit's turret To direction.
	int getTurretToDirection() const;
	/// Gets the unit's vertical direction.
	int getVerticalDirection() const;
	/// Gets the unit's status.
	UnitStatus getStatus() const;
	/// Start the walkingPhase
	void startWalking(int direction, const Position &destination, Tile *destinationTile, Tile *tileBelowMe, Tile *TileBelowDestination, bool cache);
	/// Increase the walkingPhase
	void keepWalking(Tile *tileBelowMe, bool cache);
	/// Gets the walking phase for animation and sound
	int getWalkingPhase() const;
	/// Gets the walking phase for diagonal walking
	int getDiagonalWalkingPhase() const;
	/// Gets the unit's destination when walking
	const Position &getDestination() const;
	/// Look at a certain point.
	void lookAt(const Position &point, bool turret = false);
	/// Look at a certain direction.
	void lookAt(int direction, bool force = false);
	/// Turn to the destination direction.
	void turn(bool turret = false);
	/// Abort turning.
	void abortTurn();
	/// Gets the soldier's gender.
	SoldierGender getGender() const;
	/// Gets the unit's faction.
	UnitFaction getFaction() const;
	/// Set the cached flag.
	void setCache(Surface *cache, int part = 0);
	/// If this unit is cached on the battlescape.
	Surface *getCache(bool *invalid, int part = 0) const;
	/// Kneel down.
	void kneel(bool kneeled);
	/// Is kneeled?
	bool isKneeled() const;
	/// Is floating?
	bool isFloating() const;
	/// Aim.
	void aim(bool aiming);
	/// Get direction to a certain point
	int getDirectionTo(const Position &point) const;
	/// Gets the unit's time units.
	int getTimeUnits() const;
	/// Gets the unit's stamina.
	int getEnergy() const;
	/// Gets the unit's health.
	int getHealth() const;
	/// Gets the unit's bravery.
	int getMorale() const;
	/// Do damage to the unit.
	void damage(Position position, int power, ItemDamageType type, bool ignoreArmor = false);
	/// Heal stun level of the unit.
	void healStun(int power);
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
	int getActionTUs(BattleActionType actionType, BattleItem *item);
	/// Spend time units if it can.
	bool spendTimeUnits(int tu, bool debugmode = false);
	/// Spend energy if it can.
	bool spendEnergy(int tu);
	/// Set time units.
	void setTimeUnits(int tu);
	/// Add unit to visible units.
	bool addToVisibleUnits(BattleUnit *unit);
	/// Get the list of visible units.
	std::vector<BattleUnit*> *getVisibleUnits();
	/// Clear visible units.
	void clearVisibleUnits();
	/// Add unit to visible tiles.
	bool addToVisibleTiles(Tile *tile);
	/// Get the list of visible tiles.
	std::vector<Tile*> *getVisibleTiles();
	/// Clear visible tiles.
	void clearVisibleTiles();
	/// Calculate firing accuracy.
	double getFiringAccuracy(BattleActionType actionType, BattleItem *item);
	/// Calculate accuracy modifier.
	double getAccuracyModifier();
	/// Calculate throwing accuracy.
	double getThrowingAccuracy();
	/// Set armor value.
	void setArmor(int armor, UnitSide side);
	/// Get armor value.
	int getArmor(UnitSide side) const;
	/// Get total number of fatal wounds.
	int getFatalWounds() const;
	/// Get the current reaction score.
	double getReactionScore();
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
	std::vector<BattleItem*> *getInventory();
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
	void setTile(Tile *tile, Tile *tileBelow = 0);
	/// Gets the unit's tile.
	Tile *getTile() const;
	/// Gets the item in the specified slot.
	BattleItem *getItem(RuleInventory *slot, int x = 0, int y = 0) const;
	/// Gets the item in the specified slot.
	BattleItem *getItem(const std::string &slot, int x = 0, int y = 0) const;
	/// Gets the item in the main hand.
	BattleItem *getMainHandWeapon(bool quickest = true) const;
	/// Gets a grenade from the belt, if any.
	BattleItem *getGrenadeFromBelt() const;
	/// Reloads righthand weapon if needed.
	bool checkAmmo();
	/// Check if this unit is in the exit area
	bool isInExitArea(SpecialTileType stt = START_POINT) const;
	/// Gets the unit height taking into account kneeling/standing.
	int getHeight() const;
	/// Gets the unit floating elevation.
	int getFloatHeight() const;
	/// Adds one to the reaction exp counter.
	void addReactionExp();
	/// Adds one to the firing exp counter.
	void addFiringExp();
	/// Adds one to the throwing exp counter.
	void addThrowingExp();
	/// Adds one to the psi exp counter.
	void addPsiExp();
	/// Adds one to the melee exp counter.
	void addMeleeExp();
	/// Check if unit eligible for squaddie promotion.
	bool postMissionProcedures(SavedGame *geoscape);
	/// Get the sprite index for the minimap
	int getMiniMapSpriteIndex () const;
	/// Set the turret type. -1 is no turret.
	void setTurretType(int turretType);
	/// Get the turret type. -1 is no turret.
	int getTurretType() const;
	/// Get fatal wound amount of a body part
	int getFatalWound(int part) const;
	/// Heal one fatal wound
	void heal(int part, int healAmount, int healthAmount);
	/// Give pain killers to this unit
	void painKillers ();
	/// Give stimulant to this unit
	void stimulant (int energy, int stun);
	/// Get motion points for the motion scanner.
	int getMotionPoints() const;
	/// Gets the unit's armor.
	Armor *getArmor() const;
	/// Gets the unit's name.
	std::wstring getName(Language *lang, bool debugAppendId = false) const;
	/// Gets the unit's stats.
	UnitStats *getStats();
	/// Get the unit's stand height.
	int getStandHeight() const;
	/// Get the unit's kneel height.
	int getKneelHeight() const;
	/// Get the unit's loft ID.
	int getLoftemps(int entry = 0) const;
	/// Get the unit's value.
	int getValue() const;
	/// Get the unit's death sound.
	int getDeathSound() const;
	/// Get the unit's move sound.
	int getMoveSound() const;
	/// Get whether the unit is affected by fatal wounds.
	bool isWoundable() const;
	/// Get whether the unit is affected by fear.
	bool isFearable() const;
	/// Get the unit's intelligence.
	int getIntelligence() const;
	/// Get the unit's aggression.
	int getAggression() const;
	/// Get the units's special ability.
	int getSpecialAbility() const;
	/// Set the units's special ability.
	void setSpecialAbility(SpecialAbility specab);
	/// Get the units's rank string.
	std::string getRankString() const;
	/// Get the geoscape-soldier object.
	Soldier *getGeoscapeSoldier() const;
	/// Add a kill to the counter.
	void addKillCount();
	/// Get unit type.
	std::string getType() const;
	/// Set the hand this unit is using;
	void setActiveHand(const std::string &slot);
	/// Get unit's active hand.
	std::string getActiveHand() const;
	/// Convert's unit to a faction
	void convertToFaction(UnitFaction f);
	/// Set health to 0 and set status dead
	void instaKill();
	/// Gets the unit's zombie unit.
	std::string getZombieUnit() const;
	/// Gets the unit's spawn unit.
	std::string getSpawnUnit() const;
	/// Sets the unit's spawn unit.
	void setSpawnUnit(std::string spawnUnit);
	/// Gets the unit's aggro sound.
	int getAggroSound() const;
	/// Sets the unit's energy level.
	void setEnergy(int energy);
	/// Halve the unit's armor values.
	void halveArmor();
	/// Gets the unit's faction.
	UnitFaction killedBy() const;
	/// Set the faction that killed this unit.
	void killedBy(UnitFaction f);
	/// Set the units we are charging towards.
	void setCharging(BattleUnit *chargeTarget);
	/// Get the units we are charging towards.
	BattleUnit *getCharging();
	/// Get the carried weight in strength units.
	int getCarriedWeight(BattleItem *draggingItem = 0) const;
	/// Set how many turns this unit will be exposed for.
	void setTurnsExposed (int turns);
	/// Set how many turns this unit will be exposed for.
	int getTurnsExposed () const;
	/// Get this unit's original faction
	UnitFaction getOriginalFaction() const;
	/// call this after the default copy constructor deletes the cache?
	void invalidateCache();
	
	Unit *getUnitRules() const { return _unitRules; }

	/// scratch value for AI's left hand to tell its right hand what's up...
	bool _hidingForTurn; // don't zone out and start patrolling again
	Position lastCover;
};

}

#endif
