/*
 * Copyright 2010-2011 OpenXcom Developers.
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
#include "RuleItem.h"
#include "RuleInventory.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Surface.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain type of item.
 * @param type String defining the type.
 */
RuleItem::RuleItem(const std::string &type) : _type(type), _name(type), _size(0.0), _costBuy(0), _costSell(0), _transferTime(24), _weight(999), _bigSprite(0), _floorSprite(-1), _handSprite(120), _bulletSprite(-1),
											_fireSound(-1), _hitSound(-1), _hitAnimation(0), _power(0), _priority(0), _compatibleAmmo(), _damageType(DT_NONE),
											_accuracyAuto(0), _accuracySnap(0), _accuracyAimed(0), _tuAuto(0), _tuSnap(0), _tuAimed(0), _clipSize(0), _accuracyMelee(0), _tuMelee(0),
											_battleType(BT_NONE), _twoHanded(false), _waypoint(false), _fixedWeapon(false), _invWidth(1), _invHeight(1),
											_painKiller(0), _heal(0), _stimulant(0), _healAmount(0), _healthAmount(0), _stun(0), _energy(0), _tuUse(0), _recoveryPoints(0), _armor(20), _turretType(-1),
											_recover(true), _liveAlien(false), _blastRadius(-1), _flatRate(false), _arcingShot(false)
{
}

/**
 *
 */
RuleItem::~RuleItem()
{
}

/**
 * Loads the item from a YAML file.
 * @param node YAML node.
 */
void RuleItem::load(const YAML::Node &node)
{
	int a = 0;
	for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
	{
		std::string key;
		i.first() >> key;
		if (key == "type")
		{
			i.second() >> _type;
		}
		else if (key == "name")
		{
			i.second() >> _name;
		}
		else if (key == "requires")
		{
			i.second() >> _requires;
		}
		else if (key == "size")
		{
			i.second() >> _size;
		}
		else if (key == "costBuy")
		{
			i.second() >> _costBuy;
		}
		else if (key == "costSell")
		{
			i.second() >> _costSell;
		}
		else if (key == "transferTime")
		{
			i.second() >> _transferTime;
		}
		else if (key == "weight")
		{
			i.second() >> _weight;
		}
		else if (key == "bigSprite")
		{
			i.second() >> _bigSprite;
		}
		else if (key == "floorSprite")
		{
			i.second() >> _floorSprite;
		}
		else if (key == "handSprite")
		{
			i.second() >> _handSprite;
		}
		else if (key == "bulletSprite")
		{
			i.second() >> _bulletSprite;
		}
		else if (key == "fireSound")
		{
			i.second() >> _fireSound;
		}
		else if (key == "hitSound")
		{
			i.second() >> _hitSound;
		}
		else if (key == "hitAnimation")
		{
			i.second() >> _hitAnimation;
		}
		else if (key == "power")
		{
			i.second() >> _power;
		}
		else if (key == "priority")
		{
			i.second() >> _priority;
		}
		else if (key == "compatibleAmmo")
		{
			i.second() >> _compatibleAmmo;
		}
		else if (key == "damageType")
		{
			i.second() >> a;
			_damageType = (ItemDamageType)a;
		}
		else if (key == "accuracyAuto")
		{
			i.second() >> _accuracyAuto;
		}
		else if (key == "accuracySnap")
		{
			i.second() >> _accuracySnap;
		}
		else if (key == "accuracyAimed")
		{
			i.second() >> _accuracyAimed;
		}
		else if (key == "tuAuto")
		{
			i.second() >> _tuAuto;
		}
		else if (key == "tuSnap")
		{
			i.second() >> _tuSnap;
		}
		else if (key == "tuAimed")
		{
			i.second() >> _tuAimed;
		}
		else if (key == "clipSize")
		{
			i.second() >> _clipSize;
		}
		else if (key == "accuracyMeee")
		{
			i.second() >> _accuracyMelee;
		}
		else if (key == "tuMelee")
		{
			i.second() >> _tuMelee;
		}
		else if (key == "battleType")
		{
			i.second() >> a;
			_battleType = (BattleType)a;
		}
		else if (key == "twoHanded")
		{
			i.second() >> _twoHanded;
		}
		else if (key == "waypoint")
		{
			i.second() >> _waypoint;
		}
		else if (key == "fixedWeapon")
		{
			i.second() >> _fixedWeapon;
		}
		else if (key == "invWidth")
		{
			i.second() >> _invWidth;
		}
		else if (key == "invHeight")
		{
			i.second() >> _invHeight;
		}
		else if (key == "painKiller")
		{
			i.second() >> _painKiller;
		}
		else if (key == "heal")
		{
			i.second() >> _heal;
		}
		else if (key == "stimulant")
		{
			i.second() >> _stimulant;
		}
		else if (key == "healAmount")
		{
			i.second() >> _healAmount;
		}
		else if (key == "healthAmount")
		{
			i.second() >> _healthAmount;
		}
		else if (key == "stun")
		{
			i.second() >> _stun;
		}
		else if (key == "energy")
		{
			i.second() >> _energy;
		}
		else if (key == "tuUse")
		{
			i.second() >> _tuUse;
		}
		else if (key == "recoveryPoints")
		{
			i.second() >> _recoveryPoints;
		}
		else if (key == "armor")
		{
			i.second() >> _armor;
		}
		else if (key == "recover")
		{
			i.second() >> _recover;
		}
		else if (key == "turretType")
		{
			i.second() >> _turretType;
		}
		else if (key == "liveAlien")
		{
			i.second() >> _liveAlien;
		}
		else if (key == "blastRadius")
		{
			i.second() >> _blastRadius;
		}
		else if (key == "flatRate")
		{
			i.second() >> _flatRate;
		}
		else if (key == "arcingShot")
		{
			i.second() >> _arcingShot;
		}
	}
}

/**
 * Saves the item to a YAML file.
 * @param out YAML emitter.
 */
void RuleItem::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "type" << YAML::Value << _type;
	out << YAML::Key << "name" << YAML::Value << _name;
	out << YAML::Key << "requires" << YAML::Value << _requires;
	out << YAML::Key << "size" << YAML::Value << _size;
	out << YAML::Key << "costBuy" << YAML::Value << _costBuy;
	out << YAML::Key << "costSell" << YAML::Value << _costSell;
	out << YAML::Key << "transferTime" << YAML::Value << _transferTime;
	out << YAML::Key << "weight" << YAML::Value << _weight;
	out << YAML::Key << "bigSprite" << YAML::Value << _bigSprite;
	out << YAML::Key << "floorSprite" << YAML::Value << _floorSprite;
	out << YAML::Key << "handSprite" << YAML::Value << _handSprite;
	out << YAML::Key << "bulletSprite" << YAML::Value << _bulletSprite;
	out << YAML::Key << "fireSound" << YAML::Value << _fireSound;
	out << YAML::Key << "hitSound" << YAML::Value << _hitSound;
	out << YAML::Key << "hitAnimation" << YAML::Value << _hitAnimation;
	out << YAML::Key << "power" << YAML::Value << _power;
	out << YAML::Key << "priority" << YAML::Value << _priority;
	out << YAML::Key << "compatibleAmmo" << YAML::Value << _compatibleAmmo;
	out << YAML::Key << "damageType" << YAML::Value << _damageType;
	out << YAML::Key << "accuracyAuto" << YAML::Value << _accuracyAuto;
	out << YAML::Key << "accuracySnap" << YAML::Value << _accuracySnap;
	out << YAML::Key << "accuracyAimed" << YAML::Value << _accuracyAimed;
	out << YAML::Key << "tuAuto" << YAML::Value << _tuAuto;
	out << YAML::Key << "tuSnap" << YAML::Value << _tuSnap;
	out << YAML::Key << "tuAimed" << YAML::Value << _tuAimed;
	out << YAML::Key << "clipSize" << YAML::Value << _clipSize;
	out << YAML::Key << "accuracyMelee" << YAML::Value << _accuracyMelee;
	out << YAML::Key << "tuMelee" << YAML::Value << _tuMelee;
	out << YAML::Key << "battleType" << YAML::Value << _battleType;
	out << YAML::Key << "twoHanded" << YAML::Value << _twoHanded;
	out << YAML::Key << "waypoint" << YAML::Value << _waypoint;
	out << YAML::Key << "fixedWeapon" << YAML::Value << _fixedWeapon;
	out << YAML::Key << "invWidth" << YAML::Value << _invWidth;
	out << YAML::Key << "invHeight" << YAML::Value << _invHeight;
	out << YAML::Key << "painKiller" << YAML::Value << _painKiller;
	out << YAML::Key << "heal" << YAML::Value << _heal;
	out << YAML::Key << "stimulant" << YAML::Value << _stimulant;
	out << YAML::Key << "healAmount" << YAML::Value << _healAmount;
	out << YAML::Key << "healthAmount" << YAML::Value << _healthAmount;
	out << YAML::Key << "stun" << YAML::Value << _stun;
	out << YAML::Key << "energy" << YAML::Value << _energy;
	out << YAML::Key << "tuUse" << YAML::Value << _tuUse;
	out << YAML::Key << "recoveryPoints" << YAML::Value << _recoveryPoints;
	out << YAML::Key << "armor" << YAML::Value << _armor;
	out << YAML::Key << "recover" << YAML::Value << _recover;
	out << YAML::Key << "turretType" << YAML::Value << _turretType;
	out << YAML::Key << "liveAlien" << YAML::Value << _liveAlien;
	out << YAML::Key << "blastRadius" << YAML::Value << _blastRadius;
	out << YAML::Key << "flatRate" << YAML::Value << _flatRate;
	out << YAML::Key << "arcingShot" << YAML::Value << _arcingShot;
	out << YAML::EndMap;
}

/**
 * Returns the item type. Each item has a unique type.
 * @return Item name.
 */
std::string RuleItem::getType() const
{
	return _type;
}

/**
 * Returns the language string that names
 * this item. This is not necessarily unique.
 * @return Item name.
 */
std::string RuleItem::getName() const
{
	return _name;
}

/**
 * Returns the list of research required to
 * use this item.
 * @return List of research IDs.
 */
const std::vector<std::string> &RuleItem::getRequirements() const
{
	return _requires;
}

/**
 * Returns the amount of space this item
 * takes up in a storage facility.
 * @return Storage size.
 */
float RuleItem::getSize() const
{
	return _size;
}

/**
 * Returns the amount of money this item
 * costs to purchase (0 if not purchasable).
 * @return Cost.
 */
int RuleItem::getBuyCost() const
{
	return _costBuy;
}

/**
 * Returns the amount of money this item
 * is worth to sell.
 * @return Cost.
 */
int RuleItem::getSellCost() const
{
	return _costSell;
}

/**
 * Returns the amount of time this item
 * takes to arrive at a base.
 * @return Time in hours.
 */
int RuleItem::getTransferTime() const
{
	return _transferTime;
}

/**
 * Returns the weight of the item.
 * @return Weight in strength units.
 */
int RuleItem::getWeight() const
{
	return _weight;
}

/**
 * Returns the reference in BIGOBS.PCK for use in inventory.
 * @return Sprite reference.
 */
int RuleItem::getBigSprite() const
{
	return _bigSprite;
}

/**
 * Returns the reference in FLOOROB.PCK for use in inventory.
 * @return Sprite reference.
 */
int RuleItem::getFloorSprite() const
{
	return _floorSprite;
}

/**
 * Returns the reference in HANDOB.PCK for use in inventory.
 * @return Sprite reference.
 */
int RuleItem::getHandSprite() const
{
	return _handSprite;
}

/**
 * Returns whether this item is held with two hands.
 * @return Is it two-handed?
 */
bool RuleItem::isTwoHanded() const
{
	return _twoHanded;
}

/**
 * Returns whether this uses waypoints.
 * @return Uses waypoints?
 */
bool RuleItem::isWaypoint() const
{
	return _waypoint;
}

/**
 * Returns whether this item is a fixed weapon.
 * You can't move/throw/drop fixed weapons - ie. HWP turrets.
 * @return Is it fixed weapon?
 */
bool RuleItem::isFixed() const
{
	return _fixedWeapon;
}

/**
 * Returns the item's bullet sprite reference.
 * @return Sprite reference.
 */
int RuleItem::getBulletSprite() const
{
	return _bulletSprite;
}

/**
 * Returns the item's fire sound.
 * @return Sound id.
 */
int RuleItem::getFireSound() const
{
	return _fireSound;
}

/**
 * Returns the item's hit sound.
 * @return Sound id.
 */
int RuleItem::getHitSound() const
{
	return _hitSound;
}

/**
 * Returns the item's hit sound.
 * @return Sound id.
 */
int RuleItem::getHitAnimation() const
{
	return _hitAnimation;
}

/**
 * Returns the item's power.
 * @return Teh powah.
 */
int RuleItem::getPower() const
{
	return _power;
}

/**
 * Returns the item's accuracy for snapshots.
 * @return item's accuracy for snapshots.
 */
int RuleItem::getAccuracySnap() const
{
	return _accuracySnap;
}

/**
 * Returns the item's accuracy for autoshots.
 * @return item's accuracy for autoshots.
 */
int RuleItem::getAccuracyAuto() const
{
	return _accuracyAuto;
}

/**
 * Returns the item's accuracy for aimed shots.
 * @return item's accuracy for aimed shots.
 */
int RuleItem::getAccuracyAimed() const
{
	return _accuracyAimed;
}

/**
 * Returns the item's accuracy for melee.
 * @return item's accuracy for melee.
 */
int RuleItem::getAccuracyMelee() const
{
	return _accuracyMelee;
}

/**
 * Returns the item's time unit percentage for snapshots.
 * @return item's time unit percentage for snapshots.
 */
int RuleItem::getTUSnap() const
{
	return _tuSnap;
}

/**
 * Returns the item's time unit percentage for autoshots.
 * @return item's time unit percentage for autoshots.
 */
int RuleItem::getTUAuto() const
{
	return _tuAuto;
}

/**
 * Returns the item's time unit percentage for aimed shots.
 * @return item's time unit percentage for aimed shots.
 */
int RuleItem::getTUAimed() const
{
	return _tuAimed;
}

/**
 * Returns the item's time unit percentage for melee.
 * @return item's time unit percentage for melee.
 */
int RuleItem::getTUMelee() const
{
	return _tuMelee;
}

/**
 * Returns a list of compatible ammo.
 * @return pointer to a list of compatible ammo.
 */
std::vector<std::string> *RuleItem::getCompatibleAmmo()
{
	return &_compatibleAmmo;
}

/**
 * Returns the item's damage type.
 * @return damagetype the item's damage type.
 */
ItemDamageType RuleItem::getDamageType() const
{
	return _damageType;
}

/**
 * Returns the item's type.
 * @return type the item's type.
 */
BattleType RuleItem::getBattleType() const
{
	return _battleType;
}

/**
 * Returns the item's width in a soldier's inventory.
 * @return the item's width.
 */
int RuleItem::getInventoryWidth() const
{
	return _invWidth;
}

/**
 * Returns the item's height in a soldier's inventory.
 * @return the item's height.
 */
int RuleItem::getInventoryHeight() const
{
	return _invHeight;
}

/**
 * Returns the item's ammo clip size.
 * @return the item's ammo clip size.
 */
int RuleItem::getClipSize() const
{
	return _clipSize;
}

/**
 * Draws and centers the hand sprite on a surface
 * according to its dimensions.
 * @param texture Pointer to surface set to get the sprite from.
 * @param surface Pointer to surface to draw to.
 */
void RuleItem::drawHandSprite(SurfaceSet *texture, Surface *surface) const
{
	Surface *frame = texture->getFrame(this->getBigSprite());
	frame->setX((RuleInventory::HAND_W - this->getInventoryWidth()) * RuleInventory::SLOT_W/2);
	frame->setY((RuleInventory::HAND_H - this->getInventoryHeight()) * RuleInventory::SLOT_H/2);
	texture->getFrame(this->getBigSprite())->blit(surface);
}

/**
 * Get the heal quantity of item
 * @return The new heal quantity
 */
int RuleItem::getHealQuantity () const
{
	return _heal;
}

/**
 * Get the pain killer quantity of item
 * @return The new pain killer quantity
 */
int RuleItem::getPainKillerQuantity () const
{
	return _painKiller;
}

/**
 * Get the stimulant quantity of item
 * @return The new stimulant quantity
 */
int RuleItem::getStimulantQuantity () const
{
	return _stimulant;
}

/**
 * Get the amount of fatal wound healed per usage
 * @return The amount of fatal wound healed
 */
int RuleItem::getHealAmount () const
{
	return _healAmount;
}

/**
 * Get the amount of health added to wounded soldier health
 * @return The amount of health to add
 */
int RuleItem::getHealthAmount () const
{
	return _healthAmount;
}

/**
 * Get the amount of energy added to soldier energy
 * @return The amount of energy to add
 */
int RuleItem::getEnergy () const
{
	return _energy;
}

/**
 * Get the amount of stun removed to soldier stun level
 * @return The amount of stun to remove
 */
int RuleItem::getStun () const
{
	return _stun;
}

/**
 * Get the amount of Time Unit needed to use this item
 * @return The amount of Time Unit needed to use this item
 */
int RuleItem::getTUUse() const
{
	return _tuUse;
}

/**
 * Returns the item's max explosion radius. Small explosions don't have a restriction.
 * Larger explosions are restricted using a formula, with a maximum of radius 10 no matter how large the explosion is.
 * @return radius.
 */
int RuleItem::getExplosionRadius() const
{
	int radius = 5;
	if (_blastRadius == -1)
	{
		if (_damageType == DT_IN)
		{
			radius = _power / 30;
		}
		else if (_damageType == DT_HE || _damageType == DT_STUN)
		{
			radius = _power / 20;
		}
	}
	else
	{
		radius = _blastRadius;
	}

	if (radius > 10)
	{
		radius = 10;
	}

	return radius;
}

/**
 * Returns the item's recovery points.
 * This is used at battlescape debriefing score calculation.
 * @return points.
 */
int RuleItem::getRecoveryPoints() const
{
	return _recoveryPoints;
}

/**
 * Returns the item's armor.
 * Item is destroyed when an explosion power -bigger than it's armor- hits the item.
 * @return armor.
 */
int RuleItem::getArmor() const
{
	return _armor;
}

/**
 * Returns if the item should be recoverable
 * from the battlescape.
 * @return Is it recoverable.
 */
bool RuleItem::isRecoverable() const
{
	return _recover;
}


/**
 * Returns the item's Turret Type
 * @return turret index (-1 for no turret)
 */
int RuleItem::getTurretType() const
{
	return _turretType;
}

/**
 * Returns if this is a live alien.
 * @return alien
 */
bool RuleItem::getAlien() const
{
	return _liveAlien;
}

bool RuleItem::getFlatRate() const
{
	return _flatRate;
}

bool RuleItem::getArcingShot() const
{
	return _arcingShot;
}
}
