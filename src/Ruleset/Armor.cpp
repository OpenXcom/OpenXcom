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
#include "Armor.h"
#include "../Savegame/Soldier.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain
 * type of armor.
 * @param type String defining the type.
 */
Armor::Armor(const std::string &type) :
	_type(type), _frontArmor(0), _sideArmor(0), _rearArmor(0), _underArmor(0),
	_drawingRoutine(0), _movementType(MT_WALK), _size(1), _weight(0),
	_deathFrames(3), _constantAnimation(false), _canHoldWeapon(false), _hasInventory(true),
	_forcedTorso(TORSO_USE_GENDER),
	_faceColorGroup(0), _hairColorGroup(0), _utileColorGroup(0), _rankColorGroup(0)
{
	for (int i=0; i < DAMAGE_TYPES; i++)
		_damageModifier[i] = 1.0f;
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
	_hasInventory = node["allowInv"].as<bool>(_hasInventory);
	if (node["corpseItem"])
	{
		_corpseBattle.clear();
		_corpseBattle.push_back(node["corpseItem"].as<std::string>());
		_corpseGeo = _corpseBattle[0];
	}
	else if (node["corpseBattle"])
	{
		_corpseBattle = node["corpseBattle"].as< std::vector<std::string> >();
		_corpseGeo = _corpseBattle[0];
	}
	_corpseGeo = node["corpseGeo"].as<std::string>(_corpseGeo);
	_storeItem = node["storeItem"].as<std::string>(_storeItem);
	_specWeapon = node["specialWeapon"].as<std::string>(_specWeapon);
	_frontArmor = node["frontArmor"].as<int>(_frontArmor);
	_sideArmor = node["sideArmor"].as<int>(_sideArmor);
	_rearArmor = node["rearArmor"].as<int>(_rearArmor);
	_underArmor = node["underArmor"].as<int>(_underArmor);
	_drawingRoutine = node["drawingRoutine"].as<int>(_drawingRoutine);
	_movementType = (MovementType)node["movementType"].as<int>(_movementType);
	_size = node["size"].as<int>(_size);
	_weight = node["weight"].as<int>(_weight);
	_stats.merge(node["stats"].as<UnitStats>(_stats));
	if (const YAML::Node &dmg = node["damageModifier"])
	{
		for (size_t i = 0; i < dmg.size() && i < (size_t)DAMAGE_TYPES; ++i)
		{
			_damageModifier[i] = dmg[i].as<float>();
		}
	}
	_loftempsSet = node["loftempsSet"].as< std::vector<int> >(_loftempsSet);
	if (node["loftemps"])
		_loftempsSet.push_back(node["loftemps"].as<int>());
	_deathFrames = node["deathFrames"].as<int>(_deathFrames);
	_constantAnimation = node["constantAnimation"].as<bool>(_constantAnimation);
	_forcedTorso = (ForcedTorso)node["forcedTorso"].as<int>(_forcedTorso);
	if (_drawingRoutine == 0 ||
		_drawingRoutine == 1 ||
		_drawingRoutine == 4 ||
		_drawingRoutine == 6 ||
		_drawingRoutine == 10 ||
		_drawingRoutine == 13 ||
		_drawingRoutine == 14 ||
		_drawingRoutine == 15 ||
		_drawingRoutine == 17 ||
		_drawingRoutine == 18)
	{
		_canHoldWeapon = true;
	}
	else
	{
		_canHoldWeapon = false;
	}

	_faceColorGroup = node["spriteFaceGroup"].as<int>(_faceColorGroup);
	_hairColorGroup = node["spriteHairGroup"].as<int>(_hairColorGroup);
	_rankColorGroup = node["spriteRankGroup"].as<int>(_rankColorGroup);
	_utileColorGroup = node["spriteUtileGroup"].as<int>(_utileColorGroup);
	_faceColor = node["spriteFaceColor"].as<std::vector<int> >(_faceColor);
	_hairColor = node["spriteHairColor"].as<std::vector<int> >(_hairColor);
	_rankColor = node["spriteRankColor"].as<std::vector<int> >(_rankColor);
	_utileColor = node["spriteUtileColor"].as<std::vector<int> >(_utileColor);
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
 * Gets the corpse item used in the Geoscape.
 * @return The name of the corpse item.
 */
std::string Armor::getCorpseGeoscape() const
{
	return _corpseGeo;
}

/**
 * Gets the list of corpse items dropped by the unit
 * in the Battlescape (one per unit tile).
 * @return The list of corpse items.
 */
const std::vector<std::string> &Armor::getCorpseBattlescape() const
{
	return _corpseBattle;
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
 * Gets the type of special weapon.
 * @return The name of the special weapon.
 */
std::string Armor::getSpecialWeapon() const
{
	return _specWeapon;
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
 * @important: do not use this function outside the BattleUnit constructor,
 * unless you are SURE you know what you are doing.
 * for more information, see the BattleUnit constructor.
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
float Armor::getDamageModifier(ItemDamageType dt) const
{
	return _damageModifier[(int)dt];
}

/** Gets the loftempSet.
 * @return The loftempsSet.
 */
const std::vector<int>& Armor::getLoftempsSet() const
{
	return _loftempsSet;
}

/**
  * Gets pointer to the armor's stats.
  * @return stats Pointer to the armor's stats.
  */
const UnitStats *Armor::getStats() const
{
	return &_stats;
}

/**
 * Gets the armor's weight.
 * @return the weight of the armor.
 */
int Armor::getWeight() const
{
	return _weight;
}

/**
 * Gets number of death frames.
 * @return number of death frames.
 */
int Armor::getDeathFrames() const
{
	return _deathFrames;
}

/*
 * Gets if armor uses constant animation.
 * @return if it uses constant animation
 */
bool Armor::getConstantAnimation() const
{
	return _constantAnimation;
}

/*
 * Gets if armor can hold weapon.
 * @return if it can hold weapon
 */
bool Armor::getCanHoldWeapon() const
{
	return _canHoldWeapon;
}

/**
 * Checks if this armor ignores gender (power suit/flying suit).
 * @return which torso to force on the sprite.
 */
ForcedTorso Armor::getForcedTorso() const
{
	return _forcedTorso;
}

/**
 * Gets hair base color group for replacement, if 0 then don't replace colors.
 * @return Color group or 0.
 */
int Armor::getFaceColorGroup() const
{
	return _faceColorGroup;
}

/**
 * Gets hair base color group for replacement, if 0 then don't replace colors.
 * @return Color group or 0.
 */
int Armor::getHairColorGroup() const
{
	return _hairColorGroup;
}

/**
 * Gets utile base color group for replacement, if 0 then don't replace colors.
 * @return Color group or 0.
 */
int Armor::getUtileColorGroup() const
{
	return _utileColorGroup;
}

/**
 * Gets rank base color group for replacement, if 0 then don't replace colors.
 * @return Color group or 0.
 */
int Armor::getRankColorGroup() const
{
	return _rankColorGroup;
}

/**
 * Gets new face colors for replacement, if 0 then don't replace colors.
 * @return Color index or 0.
 */
int Armor::getFaceColor(int i) const
{
	if ((size_t)i < _faceColor.size())
	{
		return _faceColor[i];
	}
	else
	{
		return 0;
	}
}

/**
 * Gets new hair colors for replacement, if 0 then don't replace colors.
 * @return Color index or 0.
 */
int Armor::getHairColor(int i) const
{
	if ((size_t)i < _hairColor.size())
	{
		return _hairColor[i];
	}
	else
	{
		return 0;
	}
}

/**
 * Gets new utile colors for replacement, if 0 then don't replace colors.
 * @return Color index or 0.
 */
int Armor::getUtileColor(int i) const
{
	if ((size_t)i < _utileColor.size())
	{
		return _utileColor[i];
	}
	else
	{
		return 0;
	}
}

/**
 * Gets new rank colors for replacement, if 0 then don't replace colors.
 * @return Color index or 0.
 */
int Armor::getRankColor(int i) const
{
	if ((size_t)i < _rankColor.size())
	{
		return _rankColor[i];
	}
	else
	{
		return 0;
	}
}

/**
 * Can this unit's inventory be accessed for any reason?
 * @return if we can access the inventory.
 */
bool Armor::hasInventory() const
{
	return _hasInventory;
}

}
