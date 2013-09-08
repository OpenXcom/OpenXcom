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
#ifndef OPENXCOM_RULEITEM_H
#define OPENXCOM_RULEITEM_H

#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

enum ItemDamageType { DT_NONE, DT_AP, DT_IN, DT_HE, DT_LASER, DT_PLASMA, DT_STUN, DT_MELEE, DT_ACID, DT_SMOKE };
enum BattleType { BT_NONE, BT_FIREARM, BT_AMMO, BT_MELEE, BT_GRENADE, BT_PROXIMITYGRENADE, BT_MEDIKIT, BT_SCANNER, BT_MINDPROBE, BT_PSIAMP, BT_FLARE, BT_CORPSE };

namespace OpenXcom
{

class SurfaceSet;
class Surface;

/**
 * Represents a specific type of item.
 * Contains constant info about an item like
 * storage size, sell price, etc.
 * @sa Item
 */
class RuleItem
{
private:
	std::string _type, _name; // two types of objects can have the same name
	std::vector<std::string> _requires;
	float _size;
	int _costBuy, _costSell, _transferTime, _weight;
	int _bigSprite, _floorSprite, _handSprite, _bulletSprite;
	int _fireSound, _hitSound, _hitAnimation;
	int _power;
	std::vector<std::string> _compatibleAmmo;
	ItemDamageType _damageType;
	int _accuracyAuto, _accuracySnap, _accuracyAimed, _tuAuto, _tuSnap, _tuAimed;
	int _clipSize, _accuracyMelee, _tuMelee;
	BattleType _battleType;
	bool _twoHanded, _waypoint, _fixedWeapon;
	int _invWidth, _invHeight;
	int _painKiller, _heal, _stimulant;
	int _woundRecovery, _healthRecovery, _stunRecovery, _energyRecovery;
	int _tuUse;
	int _recoveryPoints;
	int _armor;
	int _turretType;
	bool _recover, _liveAlien;
	int _blastRadius, _attraction;
	bool _flatRate, _arcingShot;
	int _listOrder, _range, _bulletSpeed, _autoShots;
public:
	/// Creates a blank item ruleset.
	RuleItem(const std::string &type);
	/// Cleans up the item ruleset.
	~RuleItem();
	/// Loads item data from YAML.
	void load(const YAML::Node& node, int modIndex, int listIndex);
	/// Gets the item's type.
	std::string getType() const;
	/// Gets the item's name.
	std::string getName() const;
	/// Gets the item's requirements.
	const std::vector<std::string> &getRequirements() const;
	/// Gets the item's size.
	float getSize() const;
	/// Gets the item's purchase cost.
	int getBuyCost() const;
	/// Gets the item's sale cost.
	int getSellCost() const;
	/// Gets the item's transfer time.
	int getTransferTime() const;
	/// Gets the item's weight.
	int getWeight() const;
	/// Gets the item's reference in BIGOBS.PCK for use in inventory.
	int getBigSprite() const;
	/// Gets the item's reference in FLOOROB.PCK for use in inventory.
	int getFloorSprite() const;
	/// Gets the item's reference in HANDOB.PCK for use in inventory.
	int getHandSprite() const;
	/// Gets if the item is two-handed.
	bool isTwoHanded() const;
	/// Gets if the item is a launcher.
	bool isWaypoint() const;
	/// Gets if the item is fixed.
	bool isFixed() const;
	/// Gets the item's bullet sprite reference.
	int getBulletSprite() const;
	/// Gets the item's fire sound.
	int getFireSound() const;
	/// Gets the item's hit sound.
	int getHitSound() const;
	/// Gets the item's hit animation.
	int getHitAnimation() const;
	/// Gets the item's power.
	int getPower() const;
	/// Gets the item's snapshot accuracy.
	int getAccuracySnap() const;
	/// Gets the item's autoshot accuracy.
	int getAccuracyAuto() const;
	/// Gets the item's aimed shot accuracy.
	int getAccuracyAimed() const;
	/// Gets the item's melee accuracy.
	int getAccuracyMelee() const;
	/// Gets the item's snapshot TU cost.
	int getTUSnap() const;
	/// Gets the item's autoshot TU cost.
	int getTUAuto() const;
	/// Gets the item's aimed shot TU cost.
	int getTUAimed() const;
	/// Gets the item's melee TU cost.
	int getTUMelee() const;
	/// Gets list of compatible ammo.
	std::vector<std::string> *getCompatibleAmmo();
	/// Gets the item's damage type.
	ItemDamageType getDamageType() const;
	/// Gets the item's type.
	BattleType getBattleType() const;
	/// Gets the item's inventory width.
	int getInventoryWidth() const;
	/// Gets the item's inventory height.
	int getInventoryHeight() const;
	/// Gets the ammo amount.
	int getClipSize() const;
	/// Draws the item's hand sprite onto a surface.
	void drawHandSprite(SurfaceSet *texture, Surface *surface) const;
	/// Gets the medikit heal quantity.
	int getHealQuantity () const;
	/// Gets the medikit pain killer quantity.
	int getPainKillerQuantity () const;
	/// Gets the medikit stimulant quantity.
	int getStimulantQuantity () const;
	/// Gets the medikit wound healed per shot.
	int getWoundRecovery () const;
	/// Gets the medikit health recovered per shot.
	int getHealthRecovery () const;
	/// Gets the medikit energy recovered per shot.
	int getEnergyRecovery () const;
	/// Gets the medikit stun recovered per shot.
	int getStunRecovery () const;
	/// Gets the Time Unit use.
	int getTUUse() const;
	/// Gets the max explosion radius.
	int getExplosionRadius() const;
	/// Gets the recovery points score
	int getRecoveryPoints() const;
	/// Gets the item's armor.
	int getArmor() const;
	/// Gets the item's recoverability.
	bool isRecoverable() const;
	/// Gets the item's turret type.
	int getTurretType() const;
	/// Checks if this a live alien.
	bool getAlien() const;
	/// Should we charge a flat rate?
	bool getFlatRate() const;
	/// Should this weapon arc?
	bool getArcingShot() const;
	/// How much do aliens want this thing?
	int getAttraction() const;
	/// Get the list weight for this item.
	int getListOrder() const;
	/// Get the max range of this weapon.
	int getRange() const;
	/// How fast does a projectile fired from this weapon travel?
	int getBulletSpeed() const;
	/// How many auto shots does this weapon fire.
	int getAutoShots() const;
};

}

#endif
