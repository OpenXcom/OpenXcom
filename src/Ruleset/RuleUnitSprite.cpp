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
std::string RuleUnitSprite::getSpriteSheet()
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
int RuleUnitSprite::getTorso()
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
int RuleUnitSprite::getFemaleTorso()
{
	return _femaleTorso;
}