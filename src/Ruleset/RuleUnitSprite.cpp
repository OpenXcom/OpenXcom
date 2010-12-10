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
#include "RuleUnitSprite.h"

namespace OpenXcom
{

/**
* RuleUnitSprite construction.
*/
RuleUnitSprite::RuleUnitSprite()
{

}

/**
* RuleUnitSprite DESTRUCTION.
*/
RuleUnitSprite::~RuleUnitSprite()
{

}

/**
 * Sets the spritesheet name.
 * @param spriteSheet sheetname.
 */
void RuleUnitSprite::setSpriteSheet(std::string spriteSheet)
{
	_spriteSheet = spriteSheet;
}

/**
 * Gets the sprite sheet.
 * @return sprite sheet name.
 */
std::string RuleUnitSprite::getSpriteSheet() const
{
	return _spriteSheet;
}

/**
 * Sets the torso sprite start frame.
 * @param torso framenumber.
 */
void RuleUnitSprite::setTorso(int torso)
{
	_torso = torso;
}

/**
 * Gets the torso sprite start frame.
 * @return Start frame.
 */
int RuleUnitSprite::getTorso() const
{
	return _torso;
}

/**
 * Sets the female torso start frame.
 * @param torso framenumber.
 */
void RuleUnitSprite::setFemaleTorso(int torso)
{
	_femaleTorso = torso;
}

/**
 * Gets the female torso start frame.
 * @return Start frame.
 */
int RuleUnitSprite::getFemaleTorso() const
{
	return _femaleTorso;
}

/**
 * Sets the legs stand start frame.
 * @param id framenumber.
 */
void RuleUnitSprite::setLegsStand(int id)
{
	_legsStand = id;
}

/**
 * Gets the legs stand frame.
 * @return Start frame.
 */
int RuleUnitSprite::getLegsStand() const
{
	return _legsStand;
}

/**
 * Sets the legs walk start frame.
 * @param id framenumber.
 * @param dir direction.
 */
void RuleUnitSprite::setLegsWalk(int id, int dir)
{
	_legsWalk[dir] = id;
}

/**
 * Gets the legs stand frame.
 * @param dir
 * @return Start frame.
 */
int RuleUnitSprite::getLegsWalk(int dir) const
{
	return _legsWalk[dir];
}

/**
 * Sets the legs walk offset.
 * @param offset Y.
 * @param phase 
 */
void RuleUnitSprite::setLegsWalkOffset(int offset, int phase)
{
	_walkLegsYOffset[phase] = offset;
}

int RuleUnitSprite::getDie() const
{
    return _die;
}

int RuleUnitSprite::getLeftArm1HWeapon() const
{
    return _leftArm1HWeapon;
}

int RuleUnitSprite::getLeftArm2HWeapon() const
{
    return _leftArm2HWeapon;
}

int RuleUnitSprite::getLeftArmStand() const
{
    return _leftArmStand;
}

int RuleUnitSprite::getLeftArmWalk(int phase) const
{
    return _leftArmWalk[phase];
}

int RuleUnitSprite::getRightArm1HWeapon() const
{
    return _rightArm1HWeapon;
}

int RuleUnitSprite::getRightArm2HWeapon() const
{
    return _rightArm2HWeapon;
}

int RuleUnitSprite::getRightArmStand() const
{
    return _rightArmStand;
}

int RuleUnitSprite::getRightArmWalk(int phase) const
{
    return _rightArmWalk[phase];
}

int RuleUnitSprite::getSitArmsYOffset() const
{
    return _sitArmsYOffset;
}

int RuleUnitSprite::getSitLegsYOffset() const
{
    return _sitLegsYOffset;
}

int RuleUnitSprite::getSitTorsoYOffset() const
{
    return _sitTorsoYOffset;
}

int RuleUnitSprite::getSitWeaponYOffset() const
{
    return _sitWeaponYOffset;
}

int RuleUnitSprite::getStandWeaponYOffset() const
{
    return _standWeaponYOffset;
}

int RuleUnitSprite::getWalkArmsOffset(int phase) const
{
    return _walkArmsYOffset[phase];
}

void RuleUnitSprite::setDie(int die)
{
    this->_die = die;
}

void RuleUnitSprite::setLeftArm1HWeapon(int leftArm1HWeapon)
{
    this->_leftArm1HWeapon = leftArm1HWeapon;
}

void RuleUnitSprite::setLeftArm2HWeapon(int leftArm2HWeapon)
{
    this->_leftArm2HWeapon = leftArm2HWeapon;
}

void RuleUnitSprite::setLeftArmStand(int leftArmStand)
{
    this->_leftArmStand = leftArmStand;
}

void RuleUnitSprite::setLeftArmWalk(int leftArmWalk, int phase)
{
    this->_leftArmWalk[phase] = leftArmWalk;
}

void RuleUnitSprite::setRightArm1HWeapon(int rightArm1HWeapon)
{
    this->_rightArm1HWeapon = rightArm1HWeapon;
}

void RuleUnitSprite::setRightArm2HWeapon(int rightArm2HWeapon)
{
    this->_rightArm2HWeapon = rightArm2HWeapon;
}

void RuleUnitSprite::setRightArmStand(int rightArmStand)
{
    this->_rightArmStand = rightArmStand;
}

void RuleUnitSprite::setRightArmWalk(int rightArmWalk, int phase)
{
    this->_rightArmWalk[phase] = rightArmWalk;
}

void RuleUnitSprite::setSitArmsYOffset(int sitArmsYOffset)
{
    this->_sitArmsYOffset = sitArmsYOffset;
}

void RuleUnitSprite::setSitLegsYOffset(int sitLegsYOffset)
{
    this->_sitLegsYOffset = sitLegsYOffset;
}

void RuleUnitSprite::setSitTorsoYOffset(int sitTorsoYOffset)
{
    this->_sitTorsoYOffset = sitTorsoYOffset;
}

void RuleUnitSprite::setSitWeaponYOffset(int sitWeaponYOffset)
{
    this->_sitWeaponYOffset = sitWeaponYOffset;
}

void RuleUnitSprite::setStandWeaponYOffset(int standWeaponYOffset)
{
    this->_standWeaponYOffset = standWeaponYOffset;
}

void RuleUnitSprite::setWalkArmsYOffset(int walkArmsYOffset, int phase)
{
    this->_walkArmsYOffset[phase] = walkArmsYOffset;
}

/**
 * Gets the legs stand frame.
 * @param phase
 * @return Y offset.
 */
int RuleUnitSprite::getLegsWalkOffset(int phase)
{
	return _walkLegsYOffset[phase];
}

}
