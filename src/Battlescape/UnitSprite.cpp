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
#define _USE_MATH_DEFINES
#include <cmath>
#include "UnitSprite.h"
#include "../Engine/SurfaceSet.h"
#include "../Battlescape/Position.h"
#include "../Resource/ResourcePack.h"
#include "../Ruleset/RuleUnitSprite.h"
#include "../SaveGame/BattleUnit.h"
#include "../SaveGame/BattleSoldier.h"
#include "../SaveGame/Soldier.h"

/**
 * Sets up a UnitSprite with the specified size and position.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
UnitSprite::UnitSprite(int width, int height, int x, int y) : Surface(width, height, x, y)
{


}

/**
 * Deletes the UnitSprite.
 */
UnitSprite::~UnitSprite()
{

}

/**
 * Changes the pack for the UnitSprite to get resources for rendering.
 * @param res Pointer to the resource pack.
 */
void UnitSprite::setResourcePack(ResourcePack *res)
{
	_res = res;
}

/**
 * Changes the battleunit for the UnitSprite to get the data for rendering.
 * @param res Pointer to the battle unit pack.
 */
void UnitSprite::setBattleUnit(BattleUnit *unit)
{
	_unit = unit;
}

/**
 * Draws a unit, using the drawing rules of the unit.
 * This function is called by Map, for each unit on the screen.
 */
void UnitSprite::draw()
{
	RuleUnitSprite *rules = _unit->getRenderRules();
	std::string sheet = rules->getSpriteSheet();
	Surface *sprite;
	clear();
	
	// Render the torso
	if (((BattleSoldier*)_unit)->getSoldier()->getGender() == GENDER_FEMALE)
	{
		sprite = _res->getSurfaceSet(sheet)->getFrame(rules->getFemaleTorso() + _unit->getDirection());
	}
	else
	{
		sprite = _res->getSurfaceSet(sheet)->getFrame(rules->getTorso() + _unit->getDirection());
	}


	sprite->blit(this);
}

/**
 * Blits the unit onto another surface. 
 * @param surface Pointer to another surface.
 */
void UnitSprite::blit(Surface *surface)
{
	Surface::blit(surface);
}
