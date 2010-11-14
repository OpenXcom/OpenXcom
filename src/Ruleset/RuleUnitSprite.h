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

/**
 * Represents all variables needed to render a certain type of unit on screen.
 * @sa RuleUnitSprite
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

	// some bodyparts wobble up and down when walking
	int _walkArmsYOffset[8];
	int _walkLegsYOffset[8];

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
	std::string getSpriteSheet();
	/// Sets the torso sprite start frame.
	void setTorso(int torso);
	/// Gets the torso sprite start frame.
	int getTorso();
	/// Sets the female torso start frame.
	void setFemaleTorso(int torso);
	/// Gets the female torso start frame.
	int getFemaleTorso();
	/// Sets the legs stand start frame.
	void setLegsStand(int id);
	/// Gets the legs stand start frame.
	int getLegsStand();
	/// Sets the legs walk start frame.
	void setLegsWalk(int id, int dir);
	/// Gets the legs walk start frame.
	int getLegsWalk(int dir);
	/// Sets the legs walk offset.
	void setLegsWalkOffset(int offset, int phase);
	/// Gets the legs walk offset.
	int getLegsWalkOffset(int phase);
};

#endif
