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
											_fireSound(-1), _hitSound(-1), _hitAnimation(0), _power(0), _compatibleAmmo(), _damageType(DT_NONE),
											_accuracyAuto(0), _accuracySnap(0), _accuracyAimed(0), _tuAuto(0), _tuSnap(0), _tuAimed(0), _clipSize(0), _accuracyMelee(0), _tuMelee(0),
											_battleType(BT_NONE), _twoHanded(false), _waypoint(false), _fixedWeapon(false), _invWidth(1), _invHeight(1),
											_painKiller(0), _heal(0), _stimulant(0), _woundRecovery(0), _healthRecovery(0), _stunRecovery(0), _energyRecovery(0), _tuUse(0), _recoveryPoints(0), _armor(20), _turretType(-1),
											_recover(true), _liveAlien(false), _blastRadius(-1), _attraction(0), _flatRate(false), _arcingShot(false), _listOrder(0), _range(0), _bulletSpeed(0), _autoShots(3)
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
 * @param modIndex Offsets the sounds and sprite values to avoid conflicts.
 * @param listOrder The list weight for this item.
 */
void RuleItem::load(const YAML::Node &node, int modIndex, int listOrder)
{
	_type = node["type"].as<std::string>(_type);
	_name = node["name"].as<std::string>(_name);
	_requires = node["requires"].as< std::vector<std::string> >(_requires);
	_size = node["size"].as<float>(_size);
	_costBuy = node["costBuy"].as<int>(_costBuy);
	_costSell = node["costSell"].as<int>(_costSell);
	_transferTime = node["transferTime"].as<int>(_transferTime);
	_weight = node["weight"].as<int>(_weight);
	_bigSprite = node["bigSprite"].as<int>(_bigSprite);
	// BIGOBS.PCK: 57 entries
	if (_bigSprite > 56)
		_bigSprite += modIndex;
	_floorSprite = node["floorSprite"].as<int>(_floorSprite);
	// FLOOROB.PCK: 73 entries
	if (_floorSprite > 72)
		_floorSprite += modIndex;
	_handSprite = node["handSprite"].as<int>(_handSprite);
	// HANDOBS.PCK: 128 entries
	if (_handSprite > 127)
		_handSprite += modIndex;
	if (node["bulletSprite"])
	{
		// Projectiles: 385 entries ((105*33) / (3*3)) (35 sprites per projectile(0-34), 11 projectiles (0-10))
		_bulletSprite = node["bulletSprite"].as<int>(_bulletSprite) * 35;
		if (_bulletSprite >= 385)
			_bulletSprite += modIndex;
	}
	_fireSound = node["fireSound"].as<int>(_fireSound);
	// BATTLE.CAT: 55 entries
	if (_fireSound > 54)
		_fireSound += modIndex;
	_hitSound = node["hitSound"].as<int>(_hitSound);
	// BATTLE.CAT: 55 entries
	if (_hitSound > 54)
		_hitSound += modIndex;
	_hitAnimation = node["hitAnimation"].as<int>(_hitAnimation);
	// SMOKE.PCK: 56 entries
	if (_hitAnimation > 55)
		_hitAnimation += modIndex;
	_power = node["power"].as<int>(_power);
	_compatibleAmmo = node["compatibleAmmo"].as< std::vector<std::string> >(_compatibleAmmo);
	_damageType = (ItemDamageType)node["damageType"].as<int>(_damageType);
	_accuracyAuto = node["accuracyAuto"].as<int>(_accuracyAuto);
	_accuracySnap = node["accuracySnap"].as<int>(_accuracySnap);
	_accuracyAimed = node["accuracyAimed"].as<int>(_accuracyAimed);
	_tuAuto = node["tuAuto"].as<int>(_tuAuto);
	_tuSnap = node["tuSnap"].as<int>(_tuSnap);
	_tuAimed = node["tuAimed"].as<int>(_tuAimed);
	_clipSize = node["clipSize"].as<int>(_clipSize);
	_accuracyMelee = node["accuracyMelee"].as<int>(_accuracyMelee);
	_tuMelee = node["tuMelee"].as<int>(_tuMelee);
	_battleType = (BattleType)node["battleType"].as<int>(_battleType);
	_twoHanded = node["twoHanded"].as<bool>(_twoHanded);
	_waypoint = node["waypoint"].as<bool>(_waypoint);
	_fixedWeapon = node["fixedWeapon"].as<bool>(_fixedWeapon);
	_invWidth = node["invWidth"].as<int>(_invWidth);
	_invHeight = node["invHeight"].as<int>(_invHeight);
	_painKiller = node["painKiller"].as<int>(_painKiller);
	_heal = node["heal"].as<int>(_heal);
	_stimulant = node["stimulant"].as<int>(_stimulant);
	_woundRecovery = node["woundRecovery"].as<int>(_woundRecovery);
	_healthRecovery = node["healthRecovery"].as<int>(_healthRecovery);
	_stunRecovery = node["stunRecovery"].as<int>(_stunRecovery);
	_energyRecovery = node["energyRecovery"].as<int>(_energyRecovery);
	_tuUse = node["tuUse"].as<int>(_tuUse);
	_recoveryPoints = node["recoveryPoints"].as<int>(_recoveryPoints);
	_armor = node["armor"].as<int>(_armor);
	_turretType = node["turretType"].as<int>(_turretType);
	_recover = node["recover"].as<bool>(_recover);
	_liveAlien = node["liveAlien"].as<bool>(_liveAlien);
	_blastRadius = node["blastRadius"].as<int>(_blastRadius);
	_attraction = node["attraction"].as<int>(_attraction);
	_flatRate = node["flatRate"].as<bool>(_flatRate);
	_arcingShot = node["arcingShot"].as<bool>(_arcingShot);
	_listOrder = node["listOrder"].as<int>(_listOrder);
	_range = node["maxRange"].as<int>(_range);
	_bulletSpeed = node["bulletSpeed"].as<int>(_bulletSpeed);
	_autoShots = node["autoShots"].as<int>(_autoShots);
	if (!_listOrder)
	{
		_listOrder = listOrder;
	}
}

/**
 * Gets the item type. Each item has a unique type.
 * @return The item's type.
 */
std::string RuleItem::getType() const
{
	return _type;
}

/**
 * Gets the language string that names
 * this item. This is not necessarily unique.
 * @return  The item's name.
 */
std::string RuleItem::getName() const
{
	return _name;
}

/**
 * Gets the list of research required to
 * use this item.
 * @return The list of research IDs.
 */
const std::vector<std::string> &RuleItem::getRequirements() const
{
	return _requires;
}

/**
 * Gets the amount of space this item
 * takes up in a storage facility.
 * @return The storage size.
 */
float RuleItem::getSize() const
{
	return _size;
}

/**
 * Gets the amount of money this item
 * costs to purchase (0 if not purchasable).
 * @return The buy cost.
 */
int RuleItem::getBuyCost() const
{
	return _costBuy;
}

/**
 * Gets the amount of money this item
 * is worth to sell.
 * @return The sell cost.
 */
int RuleItem::getSellCost() const
{
	return _costSell;
}

/**
 * Gets the amount of time this item
 * takes to arrive at a base.
 * @return The time in hours.
 */
int RuleItem::getTransferTime() const
{
	return _transferTime;
}

/**
 * Gets the weight of the item.
 * @return The weight in strength units.
 */
int RuleItem::getWeight() const
{
	return _weight;
}

/**
 * Gets the reference in BIGOBS.PCK for use in inventory.
 * @return The sprite reference.
 */
int RuleItem::getBigSprite() const
{
	return _bigSprite;
}

/**
 * Gets the reference in FLOOROB.PCK for use in inventory.
 * @return The sprite reference.
 */
int RuleItem::getFloorSprite() const
{
	return _floorSprite;
}

/**
 * Gets the reference in HANDOB.PCK for use in inventory.
 * @return The sprite reference.
 */
int RuleItem::getHandSprite() const
{
	return _handSprite;
}

/**
 * Returns whether this item is held with two hands.
 * @return True if it is two-handed.
 */
bool RuleItem::isTwoHanded() const
{
	return _twoHanded;
}

/**
 * Returns whether this item uses waypoints.
 * @return True if it uses waypoints.
 */
bool RuleItem::isWaypoint() const
{
	return _waypoint;
}

/**
 * Returns whether this item is a fixed weapon.
 * You can't move/throw/drop fixed weapons - e.g. HWP turrets.
 * @return True if it is a fixed weapon.
 */
bool RuleItem::isFixed() const
{
	return _fixedWeapon;
}

/**
 * Gets the item's bullet sprite reference.
 * @return The sprite reference.
 */
int RuleItem::getBulletSprite() const
{
	return _bulletSprite;
}

/**
 * Gets the item's fire sound.
 * @return The fire sound id.
 */
int RuleItem::getFireSound() const
{
	return _fireSound;
}

/**
 * Gets the item's hit sound.
 * @return The hit sound id.
 */
int RuleItem::getHitSound() const
{
	return _hitSound;
}

/**
 * Gets the item's hit animation.
 * @return The hit animation id.
 */
int RuleItem::getHitAnimation() const
{
	return _hitAnimation;
}

/**
 * Gets the item's power.
 * @return The power.
 */
int RuleItem::getPower() const
{
	return _power;
}

/**
 * Gets the item's accuracy for snapshots.
 * @return The snapshot accuracy.
 */
int RuleItem::getAccuracySnap() const
{
	return _accuracySnap;
}

/**
 * Gets the item's accuracy for autoshots.
 * @return The autoshot accuracy.
 */
int RuleItem::getAccuracyAuto() const
{
	return _accuracyAuto;
}

/**
 * Gets the item's accuracy for aimed shots.
 * @return The aimed accuracy.
 */
int RuleItem::getAccuracyAimed() const
{
	return _accuracyAimed;
}

/**
 * Gets the item's accuracy for melee attacks.
 * @return The melee accuracy.
 */
int RuleItem::getAccuracyMelee() const
{
	return _accuracyMelee;
}

/**
 * Gets the item's time unit percentage for snapshots.
 * @return The snapshot TU percentage.
 */
int RuleItem::getTUSnap() const
{
	return _tuSnap;
}

/**
 * Gets the item's time unit percentage for autoshots.
 * @return The autoshot TU percentage.
 */
int RuleItem::getTUAuto() const
{
	return _tuAuto;
}

/**
 * Gets the item's time unit percentage for aimed shots.
 * @return The aimed shot TU percentage.
 */
int RuleItem::getTUAimed() const
{
	return _tuAimed;
}

/**
 * Gets the item's time unit percentage for melee attacks.
 * @return The melee TU percentage.
 */
int RuleItem::getTUMelee() const
{
	return _tuMelee;
}

/**
 * Gets a list of compatible ammo.
 * @return Pointer to a list of compatible ammo.
 */
std::vector<std::string> *RuleItem::getCompatibleAmmo()
{
	return &_compatibleAmmo;
}

/**
 * Gets the item's damage type.
 * @return The damage type.
 */
ItemDamageType RuleItem::getDamageType() const
{
	return _damageType;
}

/**
 * Gets the item's battlye type.
 * @return The battle type.
 */
BattleType RuleItem::getBattleType() const
{
	return _battleType;
}

/**
 * Gets the item's width in a soldier's inventory.
 * @return The width.
 */
int RuleItem::getInventoryWidth() const
{
	return _invWidth;
}

/**
 * Gets the item's height in a soldier's inventory.
 * @return The height.
 */
int RuleItem::getInventoryHeight() const
{
	return _invHeight;
}

/**
 * Gets the item's ammo clip size.
 * @return The ammo clip size.
 */
int RuleItem::getClipSize() const
{
	return _clipSize;
}

/**
 * Draws and centers the hand sprite on a surface
 * according to its dimensions.
 * @param texture Pointer to the surface set to get the sprite from.
 * @param surface Pointer to the surface to draw to.
 */
void RuleItem::drawHandSprite(SurfaceSet *texture, Surface *surface) const
{
	Surface *frame = texture->getFrame(this->getBigSprite());
	frame->setX((RuleInventory::HAND_W - this->getInventoryWidth()) * RuleInventory::SLOT_W/2);
	frame->setY((RuleInventory::HAND_H - this->getInventoryHeight()) * RuleInventory::SLOT_H/2);
	texture->getFrame(this->getBigSprite())->blit(surface);
}

/**
 * Gets the heal quantity of the item.
 * @return The new heal quantity.
 */
int RuleItem::getHealQuantity () const
{
	return _heal;
}

/**
 * Gets the pain killer quantity of the item.
 * @return The new pain killer quantity.
 */
int RuleItem::getPainKillerQuantity () const
{
	return _painKiller;
}

/**
 * Gets the stimulant quantity of the item.
 * @return The new stimulant quantity.
 */
int RuleItem::getStimulantQuantity () const
{
	return _stimulant;
}

/**
 * Gets the amount of fatal wound healed per usage.
 * @return The amount of fatal wound healed.
 */
int RuleItem::getWoundRecovery () const
{
	return _woundRecovery;
}

/**
 * Gets the amount of health added to a wounded soldier's health.
 * @return The amount of health to add.
 */
int RuleItem::getHealthRecovery () const
{
	return _healthRecovery;
}

/**
 * Gets the amount of energy added to a soldier's energy.
 * @return The amount of energy to add.
 */
int RuleItem::getEnergyRecovery () const
{
	return _energyRecovery;
}

/**
 * Gets the amount of stun removed from a soldier's stun level.
 * @return The amount of stun removed.
 */
int RuleItem::getStunRecovery () const
{
	return _stunRecovery;
}

/**
 * Gets the number of Time Units needed to use this item.
 * @return The number of Time Units needed to use this item.
 */
int RuleItem::getTUUse() const
{
	return _tuUse;
}

/**
 * Returns the item's max explosion radius. Small explosions don't have a restriction.
 * Larger explosions are restricted using a formula, with a maximum of radius 10 no matter how large the explosion is.
 * @return The radius.
 */
int RuleItem::getExplosionRadius() const
{
	int radius = 0;
	if (_blastRadius == -1)
	{
		if (_damageType == DT_IN)
		{
			radius = (_power / 30) + 1;
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

	if (radius > 11)
	{
		radius = 11;
	}

	return radius;
}

/**
 * Returns the item's recovery points.
 * This is used during the battlescape debriefing score calculation.
 * @return The recovery points.
 */
int RuleItem::getRecoveryPoints() const
{
	return _recoveryPoints;
}

/**
 * Returns the item's armor.
 * The item is destroyed when an explosion power bigger than its armor hits it.
 * @return The armor.
 */
int RuleItem::getArmor() const
{
	return _armor;
}

/**
 * Returns if the item should be recoverable
 * from the battlescape.
 * @return True if it is recoverable.
 */
bool RuleItem::isRecoverable() const
{
	return _recover;
}


/**
 * Returns the item's Turret Type.
 * @return The turret index (-1 for no turret).
 */
int RuleItem::getTurretType() const
{
	return _turretType;
}

/**
 * Returns if this is a live alien.
 * @return True if this is a live alien.
 */
bool RuleItem::getAlien() const
{
	return _liveAlien;
}

/**
 * Returns whether this item charges a flat TU rate.
 * @return True if this item charges a flat TU rate.
 */
bool RuleItem::getFlatRate() const
{
	return _flatRate;
}

/**
 * Returns if this weapon should arc its shots.
 * @return True if this weapon should arc its shots.
 */
bool RuleItem::getArcingShot() const
{
	return _arcingShot;
}

/**
 * Gets the attraction value for this item (for AI).
 * @return The attraction value.
 */
int RuleItem::getAttraction() const
{
	return _attraction;
}

/**
 * Gets the list weight for this research item
 * @return The list weight.
 */
int RuleItem::getListOrder() const
{
	 return _listOrder;
}

/**
 * Gets the maximim range of this weapon (0 = unlimited)
 * @return The maximum range.
 */
int RuleItem::getRange() const
{
	return _range;
}

/**
 * Gets the speed at which this bullet travels.
 * @return The speed.
 */
int RuleItem::getBulletSpeed() const
{
	return _bulletSpeed;
}

/**
* Gets the amount of auto shots fired by this weapon.
* @return The shots.
*/
int RuleItem::getAutoShots() const
{
	return _autoShots;
}

}
