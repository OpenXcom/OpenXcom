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
#include "Armor.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain
 * type of armor.
 * @param type String defining the type.
 * @param spriteSheet Spritesheet used to render the unit.
 * @param drawingRoutine The drawing routine for this armor.
 * @param movementType The movement type for this armor (walk, fly or slide).
 * @param size The size of the armor. Normally this is 1 (small) or 2 (big).
 */
Armor::Armor(const std::string &type, std::string spriteSheet, int drawingRoutine, MovementType movementType, int size) : _type(type), _spriteSheet(spriteSheet), _spriteInv(""), _corpseItem(""), _storeItem(""), _frontArmor(0), _sideArmor(0), _rearArmor(0), _underArmor(0), _drawingRoutine(drawingRoutine),
	_tuBonusAbs(0), _tuBonusPercent(0), _staminaBonusAbs(0), _staminaBonusPercent(0), _strengthBonusAbs(0), _strengthBonusPercent(0), _movementType(movementType), _size(size)
{
	for (int i=0; i < DAMAGE_TYPES; i++)
		_damageModifier[i] = 1.0;
}

/**
 *
 */
Armor::~Armor()
{

}

/**
 * Loads the armor from a YAML file.
 * @param node YAML node.
 */
void Armor::load(const YAML::Node &node)
{
	_type = node["type"].as<std::string>(_type);
	_spriteSheet = node["spriteSheet"].as<std::string>(_spriteSheet);
	_spriteInv = node["spriteInv"].as<std::string>(_spriteInv);
	_corpseItem = node["corpseItem"].as<std::string>(_corpseItem);
	_storeItem = node["storeItem"].as<std::string>(_storeItem);
	_frontArmor = node["frontArmor"].as<int>(_frontArmor);
	_sideArmor = node["sideArmor"].as<int>(_sideArmor);
	_rearArmor = node["rearArmor"].as<int>(_rearArmor);
	_underArmor = node["underArmor"].as<int>(_underArmor);
	_drawingRoutine = node["drawingRoutine"].as<int>(_drawingRoutine);
	_movementType = (MovementType)node["movementType"].as<int>(_movementType);
	_size = node["size"].as<int>(_size);
	if (const YAML::Node &dmg = node["damageModifier"])
	{
		for (size_t i = 0; i < dmg.size() && i < DAMAGE_TYPES; ++i)
		{
			_damageModifier[i] = dmg[i].as<float>();
		}
	}
	_loftempsSet = node["loftempsSet"].as< std::vector<int> >(_loftempsSet);
	_tuBonusAbs = node["tuBonusAbs"].as<int>(_tuBonusAbs);
	_tuBonusPercent = node["tuBonusPercent"].as<int>(_tuBonusPercent);
	_staminaBonusAbs = node["staminaBonusAbs"].as<int>(_staminaBonusAbs);
	_staminaBonusPercent = node["staminaBonusPercent"].as<int>(_staminaBonusPercent);
	_strengthBonusAbs = node["strengthBonusAbs"].as<int>(_strengthBonusAbs);
	_strengthBonusPercent = node["strengthBonusPercent"].as<int>(_strengthBonusPercent);
}

/**
 * Returns the language string that names
 * this armor. Each armor has a unique name. Coveralls, Power Suit,...
 * @return The armor name.
 */
std::string Armor::getType() const
{
	return _type;
}

/**
 * Gets the unit's sprite sheet.
 * @return The sprite sheet name.
 */
std::string Armor::getSpriteSheet() const
{
	return _spriteSheet;
}

/**
 * Gets the unit's inventory sprite.
 * @return The inventory sprite name.
 */
std::string Armor::getSpriteInventory() const
{
	return _spriteInv;
}

/**
 * Gets the front armor level.
 * @return The front armor level.
 */
int Armor::getFrontArmor() const
{
	return _frontArmor;
}

/**
 * Gets the side armor level.
 * @return The side armor level.
 */
int Armor::getSideArmor() const
{
	return _sideArmor;
}

/**
 * Gets the rear armor level.
 * @return The rear armor level.
 */
int Armor::getRearArmor() const
{
	return _rearArmor;
}

/**
 * Gets the under armor level.
 * @return The under armor level.
 */
int Armor::getUnderArmor() const
{
	return _underArmor;
}


/**
 * Gets the corpse item.
 * @return The name of the corpse item.
 */
std::string Armor::getCorpseItem() const
{
	return _corpseItem;
}

/**
 * Gets the storage item needed to equip this.
 * @return The name of the store item.
 */
std::string Armor::getStoreItem() const
{
	return _storeItem;
}

/**
 * Gets the drawing routine ID.
 * @return The drawing routine ID.
 */
int Armor::getDrawingRoutine() const
{
	return _drawingRoutine;
}

/**
 * Gets the movement type of this armor.
 * Useful for determining whether the armor can fly.
 * @return The movement type.
 */
MovementType Armor::getMovementType() const
{
	return _movementType;
}

/**
 * Gets the size of the unit. Normally this is 1 (small) or 2 (big).
 * @return The unit's size.
 */
int Armor::getSize() const
{
	return _size;
}

/**
 * Gets the damage modifier for a certain damage type.
 * @param dt The damageType.
 * @return The damage modifier 0->1.
 */
float Armor::getDamageModifier(ItemDamageType dt)
{
	return _damageModifier[(int)dt];
}

/** Gets the loftempSet.
 * @return The loftempsSet.
 */
std::vector<int> Armor::getLoftempsSet() const
{
	return _loftempsSet;
}

/**
 * Gets the armor TU bonus relative to a certain TU's amount.
 * @param tu The base TU.
 * @return The TU bonus.
 */
int Armor::getTUBonus(int tu)
{
	if(Options::getBool("advancedArmorBonuses")==true)
		return tu*_tuBonusPercent/100+_tuBonusAbs;
	else
		return 0;
}

/**
 * Gets the armor stamina bonus relative to a certain stamina amount.
 * @param stamina The base stamina.
 * @return The stamina bonus.
 */
int Armor::getStaminaBonus(int stamina)
{
	if(Options::getBool("advancedArmorBonuses")==true)
		return stamina*_staminaBonusPercent/100+_staminaBonusAbs;
	else
		return 0;
}

/**
 * Gets the strength bonus relative to a certain strength amount.
 * @param strength The base strength.
 * @return The strength bonus.
 */
int Armor::getStrengthBonus(int strength)
{
	if(Options::getBool("advancedArmorBonuses")==true)
		return strength*_strengthBonusPercent/100+_strengthBonusAbs;
	else
		return 0;
}

}
