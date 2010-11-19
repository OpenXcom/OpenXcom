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
#include "../Savegame/BattleUnit.h"
#include "../Savegame/BattleSoldier.h"
#include "../Savegame/Soldier.h"

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
 * Links this sprite to a BattleUnit to get the data for rendering.
 * @param unit Pointer to the BattleUnit.
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
	Surface *torso = 0, *legs = 0, *bottomArm = 0, *topArm = 0;
	clear();
	BattleSoldier *soldier = dynamic_cast<BattleSoldier*>(_unit);
	
	if (rules->getTorso() > -1)
	{
		if (soldier != 0 && soldier->getSoldier()->getGender() == GENDER_FEMALE)
		{
			torso = _res->getSurfaceSet(sheet)->getFrame(rules->getFemaleTorso() + _unit->getDirection());
		}
		else
		{
			torso = _res->getSurfaceSet(sheet)->getFrame(rules->getTorso() + _unit->getDirection());
		}
	}

	if (_unit->getStatus() == STATUS_STANDING)
	{
		legs = _res->getSurfaceSet(sheet)->getFrame(rules->getLegsStand() + _unit->getDirection());
		if (_unit->getDirection() < 4)
		{
			bottomArm = _res->getSurfaceSet(sheet)->getFrame(rules->getLeftArmStand() + _unit->getDirection());
			topArm = _res->getSurfaceSet(sheet)->getFrame(rules->getRightArmStand() + _unit->getDirection());
		}
		else
		{
			topArm = _res->getSurfaceSet(sheet)->getFrame(rules->getLeftArmStand() + _unit->getDirection());
			bottomArm = _res->getSurfaceSet(sheet)->getFrame(rules->getRightArmStand() + _unit->getDirection());
		}
	}
	else
	{
		legs = _res->getSurfaceSet(sheet)->getFrame(rules->getLegsWalk(_unit->getDirection()) + _unit->getWalkingPhase());
		legs->setY(rules->getLegsWalkOffset(_unit->getWalkingPhase()));
		if (_unit->getDirection() < 4)
		{
			bottomArm = _res->getSurfaceSet(sheet)->getFrame(rules->getLeftArmWalk(_unit->getDirection()) + _unit->getWalkingPhase());
			topArm = _res->getSurfaceSet(sheet)->getFrame(rules->getRightArmWalk(_unit->getDirection()) + _unit->getWalkingPhase());
		}
		else
		{
			topArm = _res->getSurfaceSet(sheet)->getFrame(rules->getLeftArmWalk(_unit->getDirection()) + _unit->getWalkingPhase());
			bottomArm = _res->getSurfaceSet(sheet)->getFrame(rules->getRightArmWalk(_unit->getDirection()) + _unit->getWalkingPhase());
		}
		topArm->setY(rules->getWalkArmsOffset(_unit->getWalkingPhase()));
		bottomArm->setY(rules->getWalkArmsOffset(_unit->getWalkingPhase()));		
	}


	if (bottomArm) bottomArm->blit(this);
	if (torso) torso->blit(this);
	if (legs) legs->blit(this);
	if (topArm) topArm->blit(this);

}

/**
 * Blits the unit onto another surface. 
 * @param surface Pointer to another surface.
 */
void UnitSprite::blit(Surface *surface)
{
	Surface::blit(surface);
}
