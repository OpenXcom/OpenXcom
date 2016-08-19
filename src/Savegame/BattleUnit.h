#pragma once
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
#include <vector>
#include <string>
#include <yaml-cpp/yaml.h>
#include "../Battlescape/Position.h"
#include "../Battlescape/BattlescapeGame.h"
#include "../Mod/RuleItem.h"
#include "../Mod/Unit.h"
#include "../Mod/MapData.h"
#include "Soldier.h"
#include "BattleItem.h"

namespace OpenXcom
{

class Tile;
class BattleItem;
class Unit;
class BattleAIState;
class SavedBattleGame;
class Node;
class Surface;
class RuleInventory;
class Soldier;
class Armor;
class SavedGame;
class Language;
class AIModule;
struct BattleUnitStatistics;
struct StatAdjustment;

enum UnitStatus {STATUS_STANDING, STATUS_WALKING, STATUS_FLYING, STATUS_TURNING, STATUS_AIMING, STATUS_COLLAPSING, STATUS_DEAD, STATUS_UNCONSCIOUS, STATUS_PANICKING, STATUS_BERSERK, STATUS_IGNORE_ME};
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
	static const int SPEC_WEAPON_MAX = 3;

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
	std::vector<BattleUnit *> _visibleUnits, _unitsSpottedThisTurn;
	std::vector<Tile *> _visibleTiles;
	int _tu, _energy, _health, _morale, _stunlevel;
	bool _kneeled, _floating, _dontReselect;
	int _currentArmor[5], _maxArmor[5];
	int _fatalWounds[6];
	int _fire;
	std::vector<BattleItem*> _inventory;
	BattleItem* _specWeapon[SPEC_WEAPON_MAX];
	AIModule *_currentAIState;
	bool _visible;
	Surface *_cache[5];
	bool _cacheInvalid;
	int _expBravery, _expReactions, _expFiring, _expThrowing, _expPsiSkill, _expPsiStrength, _expMelee;
	int improveStat(int exp) const;
	int _motionPoints;
	int _kills;
	int _faceDirection; // used only during strafeing moves
	bool _hitByFire;
	int _moraleRestored;
	int _coverReserve;
	BattleUnit *_charging;
	int _turnsSinceSpotted;
	std::string _spawnUnit;
	std::string _activeHand;
	BattleUnitStatistics* _statistics;
	int _murdererId;	// used to credit the murderer with the kills that this unit got by blowing up on death
	int _mindControllerID;	// used to credit the mind controller with the kills of the mind controllee
	UnitSide _fatalShotSide;
	UnitBodyPart _fatalShotBodyPart;
	std::string _murdererWeapon, _murdererWeaponAmmo;

	// static data
	std::string _type;
	std::string _rank;
	std::string _race;
	std::wstring _name;
	UnitStats _stats;
	int _standHeight, _kneelHeight, _floatHeight;
	std::vector<int> _deathSound;
	int _value, _aggroSound, _moveSound;
	int _intelligence, _aggression;
	SpecialAbility _specab;
	Armor *_armor;
	SoldierGender _gender;
	Soldier *_geoscapeSoldier;
	std::vector<int> _loftempsSet;
	Unit *_unitRules;
	int _rankInt;
	int _turretType;
	int _breathFrame;
	bool _breathing;
	bool _hidingForTurn, _floorAbove, _respawn;
	MovementType _movementType;
	std::vector<std::pair<Uint8, Uint8> > _recolor;

	/// Helper function initing recolor vector.
	void setRecolor(int basicLook, int utileLook, int rankLook);
public:
	static const int MAX_SOLDIER_ID = 1000000;
	/// Creates a BattleUnit from solder.
	BattleUnit(Soldier *soldier, int depth);
	/// Creates a BattleUnit from unit.
	BattleUnit(Unit *unit, UnitFaction faction, int id, Armor *armor, StatAdjustment *adjustment, int depth);
	/// Cleans up the BattleUnit.
	~BattleUnit();
	/// Loads the unit from YAML.
	void load(const YAML::Node& node);
	/// Saves the unit to YAML.
	YAML::Node save() const;
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
	void startWalking(int direction, const Position &destination, Tile *tileBelowMe, bool cache);
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
	/// Gets unit sprite recolors values.
	const std::vector<std::pair<Uint8, Uint8> > &getRecolor() const;
	/// Kneel down.
	void kneel(bool kneeled);
	/// Is kneeled?
	bool isKneeled() const;
	/// Is floating?
	bool isFloating() const;
	/// Aim.
	void aim(bool aiming);
	/// Get direction to a certain point
	int directionTo(const Position &point) const;
	/// Gets the unit's time units.
	int getTimeUnits() const;
	/// Gets the unit's stamina.
	int getEnergy() const;
	/// Gets the unit's health.
	int getHealth() const;
	/// Gets the unit's bravery.
	int getMorale() const;
	/// Do damage to the unit.
	int damage(const Position &relative, int power, ItemDamageType type, bool ignoreArmor = false);
	/// Heal stun level of the unit.
	void healStun(int power);
	/// Gets the unit's stun level.
	int getStunlevel() const;
	/// Knocks the unit out instantly.
	void knockOut(BattlescapeGame *battle);
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
	int getActionTUs(BattleActionType actionType, RuleItem *item);
	/// Spend time units if it can.
	bool spendTimeUnits(int tu);
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
	int getFiringAccuracy(BattleActionType actionType, BattleItem *item);
	/// Calculate accuracy modifier.
	int getAccuracyModifier(BattleItem *item = 0);
	/// Calculate throwing accuracy.
	double getThrowingAccuracy();
	/// Set armor value.
	void setArmor(int armor, UnitSide side);
	/// Get armor value.
	int getArmor(UnitSide side) const;
	/// Get max armor value.
	int getMaxArmor(UnitSide side) const;
	/// Get total number of fatal wounds.
	int getFatalWounds() const;
	/// Get the current reaction score.
	double getReactionScore();
	/// Prepare for a new turn.
	void prepareNewTurn(bool fullProcess = true);
	/// Morale change
	void moraleChange(int change);
	/// Don't reselect this unit
	void dontReselect();
	/// Reselect this unit
	void allowReselect();
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
	/// Get AI Module.
	AIModule *getAIModule() const;
	/// Set AI Module.
	void setAIModule(AIModule *ai);
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
	/// Adds one to the psi skill exp counter.
	void addPsiSkillExp();
	/// Adds one to the psi strength exp counter.
	void addPsiStrengthExp();
	/// Adds one to the melee exp counter.
	void addMeleeExp();
	/// Updates the stats of a Geoscape soldier.
	void updateGeoscapeStats(Soldier *soldier) const;
	/// Check if unit eligible for squaddie promotion.
	bool postMissionProcedures(SavedGame *geoscape);
	/// Get the sprite index for the minimap
	int getMiniMapSpriteIndex() const;
	/// Set the turret type. -1 is no turret.
	void setTurretType(int turretType);
	/// Get the turret type. -1 is no turret.
	int getTurretType() const;
	/// Get fatal wound amount of a body part
	int getFatalWound(int part) const;
	/// Heal one fatal wound
	void heal(int part, int woundAmount, int healthAmount);
	/// Give pain killers to this unit
	void painKillers();
	/// Give stimulant to this unit
	void stimulant (int energy, int stun);
	/// Get motion points for the motion scanner.
	int getMotionPoints() const;
	/// Gets the unit's armor.
	Armor *getArmor() const;
	/// Gets the unit's name.
	std::wstring getName(Language *lang, bool debugAppendId = false) const;
	/// Gets the unit's stats.
	UnitStats *getBaseStats();
	/// Get the unit's stand height.
	int getStandHeight() const;
	/// Get the unit's kneel height.
	int getKneelHeight() const;
	/// Get the unit's loft ID.
	int getLoftemps(int entry = 0) const;
	/// Get the unit's value.
	int getValue() const;
	/// Get the unit's death sounds.
	const std::vector<int> &getDeathSounds() const;
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
	/// Set the units's respawn flag.
	void setRespawn(bool respawn);
	/// Get the units's respawn flag.
	bool getRespawn() const;
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
	/// Set health to 0
	void kill();
	/// Set health to 0 and set status dead
	void instaKill();
	/// Gets the unit's spawn unit.
	std::string getSpawnUnit() const;
	/// Sets the unit's spawn unit.
	void setSpawnUnit(const std::string &spawnUnit);
	/// Gets the unit's aggro sound.
	int getAggroSound() const;
	/// Sets the unit's energy level.
	void setEnergy(int energy);
	/// Get the faction that killed this unit.
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
	void setTurnsSinceSpotted (int turns);
	/// Set how many turns this unit will be exposed for.
	int getTurnsSinceSpotted() const;
	/// Get this unit's original faction
	UnitFaction getOriginalFaction() const;
	/// call this after the default copy constructor deletes the cache?
	void invalidateCache();

	Unit *getUnitRules() const { return _unitRules; }

	Position lastCover;
	/// get the vector of units we've seen this turn.
	std::vector<BattleUnit *> &getUnitsSpottedThisTurn();
	/// set the rank integer
	void setRankInt(int rank);
	/// get the rank integer
	int getRankInt() const;
	/// derive a rank integer based on rank string (for xcom soldiers ONLY)
	void deriveRank();
	/// this function checks if a tile is visible, using maths.
	bool checkViewSector(Position pos) const;
	/// adjust this unit's stats according to difficulty.
	void adjustStats(const StatAdjustment &adjustment);
	/// did this unit already take fire damage this turn? (used to avoid damaging large units multiple times.)
	bool tookFireDamage() const;
	/// switch the state of the fire damage tracker.
	void toggleFireDamage();
	void setCoverReserve(int reserve);
	int getCoverReserve() const;
	/// Is this unit selectable?
	bool isSelectable(UnitFaction faction, bool checkReselect, bool checkInventory) const;
	/// Does this unit have an inventory?
	bool hasInventory() const;
	/// Is this unit breathing and if so what frame?
	int getBreathFrame() const;
	/// Start breathing and/or update the breathing frame.
	void breathe();
	/// Set the flag for "floor above me" meaning stop rendering bubbles.
	void setFloorAbove(bool floor);
	/// Get the flag for "floor above me".
	bool getFloorAbove() const;
	/// Get any melee weapon we may be carrying, or a built in one.
	BattleItem *getMeleeWeapon();
	/// Use this function to check the unit's movement type.
	MovementType getMovementType() const;
	/// Checks if this unit is in hiding for a turn.
	bool isHiding() const {return _hidingForTurn; };
	/// Sets this unit is in hiding for a turn (or not).
	void setHiding(bool hiding) { _hidingForTurn = hiding; };
	/// Puts the unit in the corner to think about what he's done.
	void goToTimeOut();
	/// Create special weapon for unit.
	void setSpecialWeapon(SavedBattleGame *save, const Mod *mod);
	/// Get special weapon.
	BattleItem *getSpecialWeapon(BattleType type) const;
	/// Recovers the unit's time units and energy.
	void recoverTimeUnits();
	/// Get the unit's mission statistics.
	BattleUnitStatistics* getStatistics();
	/// Set the unit murderer's id.
	void setMurdererId(int id);
	/// Get the unit murderer's id.
	int getMurdererId() const;
	/// Set information on the unit's fatal shot.
	void setFatalShotInfo(UnitSide side, UnitBodyPart bodypart);
	/// Get information on the unit's fatal shot's side.
	UnitSide getFatalShotSide() const;
	/// Get information on the unit's fatal shot's body part.
	UnitBodyPart getFatalShotBodyPart() const;
	/// Get the unit murderer's weapon.
	std::string getMurdererWeapon() const;
	/// Set the unit murderer's weapon.
	void setMurdererWeapon(std::string weapon);
	/// Get the unit murderer's weapon's ammo.
	std::string getMurdererWeaponAmmo() const;
	/// Set the unit murderer's weapon's ammo.
	void setMurdererWeaponAmmo(std::string weaponAmmo);
	/// Set the unit mind controller's id.
	void setMindControllerId(int id);
	/// Get the unit mind controller's id.
	int getMindControllerId() const;
	
};

}
