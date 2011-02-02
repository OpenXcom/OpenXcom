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
#ifndef OPENXCOM_RULEUNITSPRITE_H
#define OPENXCOM_RULEUNITSPRITE_H

#include <string>

namespace OpenXcom
{

/**
 * Represents all variables needed to render a certain type of unit on screen.
 * @sa BattleUnit
 */
class RuleUnitSprite
{
private:
	std::string _spriteSheet;

	// sprite ID's of all different body parts and positions
	int _leftArmStand;
	int _rightArmStand;
	int _legsStand;
	int _torso;
	int _leftArmWalk[8];
	int _rightArmWalk[8];
	int _legsWalk[8];
	int _leftArm2HWeapon;
	int _rightArm2HWeapon;
	int _leftArm1HWeapon;
	int _rightArm1HWeapon;
	int _die;
	int _femaleTorso;

	// torso bobs up and down when walking
	int _walkTorsoYOffset[8];

	// offset parts when sitting
	int _sitTorsoYOffset;
	int _sitArmsYOffset;
	int _sitLegsYOffset;

	// offsets in Y pixels to draw the weapon
	int _standWeaponYOffset;
	int _sitWeaponYOffset;

public:
	RuleUnitSprite();
	~RuleUnitSprite();
	/// Sets the spritesheet name.
	void setSpriteSheet(std::string spriteSheet);
	/// Gets the sprite sheet.
	std::string getSpriteSheet() const;
	/// Sets the torso sprite start frame.
	void setTorso(int torso);
	/// Gets the torso sprite start frame.
	int getTorso() const;
	/// Sets the female torso start frame.
	void setFemaleTorso(int torso);
	/// Gets the female torso start frame.
	int getFemaleTorso() const;
	/// Sets the legs stand start frame.
	void setLegsStand(int id);
	/// Gets the legs stand start frame.
	int getLegsStand() const;
	/// Sets the legs walk start frame.
	void setLegsWalk(int id, int dir);
	/// Gets the legs walk start frame.
	int getLegsWalk(int dir) const;
	/// Sets the legs walk offset.
	void setLegsWalkOffset(int offset, int phase);
	/// Gets the legs walk offset.
	int getLegsWalkOffset(int phase);
    int getDie() const;
    int getLeftArm1HWeapon() const;
    int getLeftArm2HWeapon() const;
    int getLeftArmStand() const;
    int getLeftArmWalk(int phase) const;
    int getRightArm1HWeapon() const;
    int getRightArm2HWeapon() const;
    int getRightArmStand() const;
    int getRightArmWalk(int phase) const;
    int getSitArmsYOffset() const;
    int getSitLegsYOffset() const;
    int getSitTorsoYOffset() const;
    int getSitWeaponYOffset() const;
    int getStandWeaponYOffset() const;
    int getWalkArmsOffset(int phase) const;
    int getWalkTorsoOffset(int phase) const;
    void setDie(int die);
    void setLeftArm1HWeapon(int leftArm1HWeapon);
    void setLeftArm2HWeapon(int leftArm2HWeapon);
    void setLeftArmStand(int leftArmStand);
    void setLeftArmWalk(int leftArmWalk, int phase);
    void setRightArm1HWeapon(int rightArm1HWeapon);
    void setRightArm2HWeapon(int rightArm2HWeapon);
    void setRightArmStand(int rightArmStand);
    void setRightArmWalk(int rightArmWalk, int phase);
    void setSitArmsYOffset(int sitArmsYOffset);
    void setSitLegsYOffset(int sitLegsYOffset);
    void setSitTorsoYOffset(int sitTorsoYOffset);
    void setSitWeaponYOffset(int sitWeaponYOffset);
    void setStandWeaponYOffset(int standWeaponYOffset);
    void setWalkArmsYOffset(int walkArmsYOffset, int phase);
    void setWalkTorsoYOffset(int walkArmsYOffset, int phase);
};

}

#endif
