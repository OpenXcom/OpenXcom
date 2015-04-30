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
#ifndef OPENXCOM_ARMOR_H
#define OPENXCOM_ARMOR_H

#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>
#include "MapData.h"
#include "Unit.h"

namespace OpenXcom
{

enum ForcedTorso{ TORSO_USE_GENDER, TORSO_ALWAYS_MALE, TORSO_ALWAYS_FEMALE };
/**
 * Represents a specific type of armor.
 * Not only soldier armor, but also alien armor - some alien races wear Soldier Armor, Leader Armor or Commander Armor
 * depending on their rank.
 */
class Armor
{
public:
	static const int DAMAGE_TYPES = 10;
private:
	std::string _type, _spriteSheet, _spriteInv, _corpseGeo, _storeItem, _specWeapon;
	std::vector<std::string> _corpseBattle;
	int _frontArmor, _sideArmor, _rearArmor, _underArmor, _drawingRoutine;
	MovementType _movementType;
	int _size, _weight;
	float _damageModifier[DAMAGE_TYPES];
	std::vector<int> _loftempsSet;
	UnitStats _stats;
	int _deathFrames;
	bool _constantAnimation, _canHoldWeapon, _hasInventory;
	ForcedTorso _forcedTorso;
	int _faceColorGroup, _hairColorGroup, _utileColorGroup, _rankColorGroup;
	std::vector<int> _faceColor, _hairColor, _utileColor, _rankColor;
public:
	/// Creates a blank armor ruleset.
	Armor(const std::string &type);
	/// Cleans up the armor ruleset.
	~Armor();
	/// Loads the armor data from YAML.
	void load(const YAML::Node& node);
	/// Gets the armor's type.
	std::string getType() const;
	/// Gets the unit's sprite sheet.
	std::string getSpriteSheet() const;
	/// Gets the unit's inventory sprite.
	std::string getSpriteInventory() const;
	/// Gets the front armor level.
	int getFrontArmor() const;
	/// Gets the side armor level.
	int getSideArmor() const;
	/// Gets the rear armor level.
	int getRearArmor() const;
	/// Gets the under armor level.
	int getUnderArmor() const;
	/// Gets the Geoscape corpse item.
	std::string getCorpseGeoscape() const;
	/// Gets the Battlescape corpse item.
	const std::vector<std::string> &getCorpseBattlescape() const;
	/// Gets the stores item.
	std::string getStoreItem() const;
	/// Gets the special weapon type.
	std::string getSpecialWeapon() const;
	/// Gets the battlescape drawing routine ID.
	int getDrawingRoutine() const;
	/// DO NOT USE THIS FUNCTION OUTSIDE THE BATTLEUNIT CONSTRUCTOR OR I WILL HUNT YOU DOWN.
	MovementType getMovementType() const;
	/// Gets whether this is a normal or big unit.
	int getSize() const;
	/// Gets damage modifier.
	float getDamageModifier(ItemDamageType dt) const;
	/// Gets loftempSet
	const std::vector<int>& getLoftempsSet() const;
	/// Gets the armor's stats.
	const UnitStats *getStats() const;
	/// Gets the armor's weight.
	int getWeight() const;
	/// Gets number of death frames.
	int getDeathFrames() const;
	/// Gets if armor uses constant animation.
	bool getConstantAnimation() const;
	/// Gets if armor can hold weapon.
	bool getCanHoldWeapon() const;
	/// Checks if this armor ignores gender (power suit/flying suit).
	ForcedTorso getForcedTorso() const;
	/// Get face base color
	int getFaceColorGroup() const;
	/// Get hair base color
	int getHairColorGroup() const;
	/// Get utile base color
	int getUtileColorGroup() const;
	/// Get rank base color
	int getRankColorGroup() const;
	/// Get face base color
	int getFaceColor(int i) const;
	/// Get hair base color
	int getHairColor(int i) const;
	/// Get utile base color
	int getUtileColor(int i) const;
	/// Get rank base color
	int getRankColor(int i) const;
	/// can we access this unit's inventory?
	bool hasInventory() const;
};

}

#endif
