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
#include "../Ruleset/RuleItem.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/Soldier.h"

namespace OpenXcom
{

/**
 * Sets up a UnitSprite with the specified size and position.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
UnitSprite::UnitSprite(int width, int height, int x, int y) : Surface(width, height, x, y), _res(0), _unit(0), _item(0)
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
 * Links this sprite to a BattleItem to get the data for rendering.
 * @param unit Pointer to the BattleItem.
 */
void UnitSprite::setBattleItem(BattleItem *item)
{
	_item = item;
}

/**
 * Draws a unit, using the drawing rules of the unit.
 * This function is called by Map, for each unit on the screen.
 */
void UnitSprite::draw()
{
	RuleUnitSprite *rules = _unit->getRenderRules();
	std::string sheet = rules->getSpriteSheet();
	Surface *torso = 0, *legs = 0, *leftArm = 0, *rightArm = 0, *item = 0;
	clear();
	
	if (_unit->getGender() == GENDER_FEMALE)
	{
		torso = _res->getSurfaceSet(sheet)->getFrame(rules->getFemaleTorso() + _unit->getDirection());
	}
	else
	{
		torso = _res->getSurfaceSet(sheet)->getFrame(rules->getTorso() + _unit->getDirection());
	}

	if (_unit->getStatus() == STATUS_WALKING)
	{
		torso->setY(rules->getWalkTorsoOffset(_unit->getWalkingPhase()));
	}

	if (_unit->getStatus() == STATUS_WALKING)
	{
		legs = _res->getSurfaceSet(sheet)->getFrame(rules->getLegsWalk(_unit->getDirection()) + _unit->getWalkingPhase());
		leftArm = _res->getSurfaceSet(sheet)->getFrame(rules->getLeftArmWalk(_unit->getDirection()) + _unit->getWalkingPhase());
		rightArm = _res->getSurfaceSet(sheet)->getFrame(rules->getRightArmWalk(_unit->getDirection()) + _unit->getWalkingPhase());
	}
	else
	{
		if (_unit->isKneeled())
		{
			legs = _res->getSurfaceSet(sheet)->getFrame(rules->getLegsKneel() + _unit->getDirection());
		}
		else
		{
			legs = _res->getSurfaceSet(sheet)->getFrame(rules->getLegsStand() + _unit->getDirection());
		}
		leftArm = _res->getSurfaceSet(sheet)->getFrame(rules->getLeftArmStand() + _unit->getDirection());
		rightArm = _res->getSurfaceSet(sheet)->getFrame(rules->getRightArmStand() + _unit->getDirection());
	}

	if (_item)
	{
		// draw handob item
		if (_unit->getStatus() == STATUS_AIMING)
		{
			int dir = _unit->getDirection() + 2;
			if (dir > 7) dir -= 8;
			item = _res->getSurfaceSet("HANDOB.PCK")->getFrame(_item->getRules()->getHandSprite() + dir);
			int offX[8] = { 8, 10, 7, 4, -9, -11, -7, -3 };
			int offY[8] = { -6, -3, 0, 2, 0, -4, -7, -9 };

			item->setX(offX[_unit->getDirection()]);
			item->setY(offY[_unit->getDirection()]);
		}
		else
		{
			item = _res->getSurfaceSet("HANDOB.PCK")->getFrame(_item->getRules()->getHandSprite() + _unit->getDirection());
			item->setX(0);
			item->setY(0);
		}

		// draw arms holding the item
		if (_item->getRules()->getTwoHanded())
		{
			leftArm = _res->getSurfaceSet(sheet)->getFrame(rules->getLeftArm2HWeapon() + _unit->getDirection());
			if (_unit->getStatus() == STATUS_AIMING)
			{
				rightArm = _res->getSurfaceSet(sheet)->getFrame(rules->getRightArm2HShoot() + _unit->getDirection());
			}
			else
			{
				rightArm = _res->getSurfaceSet(sheet)->getFrame(rules->getRightArm2HWeapon() + _unit->getDirection());
			}
		}
		else
		{
			rightArm = _res->getSurfaceSet(sheet)->getFrame(rules->getRightArm1HWeapon() + _unit->getDirection());
		}

		// bob arm(s) & item up/down when walking
		if (_unit->getStatus() == STATUS_WALKING)
		{
			item->setY(rules->getWalkTorsoOffset(_unit->getWalkingPhase()));
			rightArm->setY(rules->getWalkTorsoOffset(_unit->getWalkingPhase()));
			if (_item->getRules()->getTwoHanded())
				leftArm->setY(rules->getWalkTorsoOffset(_unit->getWalkingPhase()));
		}
	}

	if (_unit->isKneeled())
	{
		leftArm->setY(4);
		rightArm->setY(4);
		torso->setY(4);
		item?item->setY(4):void();
	}
	else if (_unit->getStatus() != STATUS_WALKING)
	{
		leftArm->setY(0);
		rightArm->setY(0);
		torso->setY(0);
		item?item->setY(0):void();
	}
	
	switch (_unit->getDirection())
	{
	case 0: leftArm->blit(this); legs->blit(this); item?item->blit(this):void(); torso->blit(this); rightArm->blit(this); break;
	case 1: leftArm->blit(this); legs->blit(this); torso->blit(this); item?item->blit(this):void(); rightArm->blit(this); break;
	case 2: leftArm->blit(this); legs->blit(this); torso->blit(this); item?item->blit(this):void(); rightArm->blit(this); break;
	case 3: legs->blit(this); torso->blit(this); leftArm->blit(this); rightArm->blit(this); item?item->blit(this):void(); break;
	case 4: rightArm->blit(this); legs->blit(this); torso->blit(this); leftArm->blit(this); item?item->blit(this):void(); break;
	case 5: rightArm->blit(this); legs->blit(this); item?item->blit(this):void(); torso->blit(this); leftArm->blit(this); break;
	case 6: rightArm->blit(this); legs->blit(this); item?item->blit(this):void(); torso->blit(this); leftArm->blit(this); break;
	case 7: item?item->blit(this):void(); leftArm->blit(this); rightArm->blit(this); legs->blit(this); torso->blit(this); break;
	}
}

/**
 * Blits the unit onto another surface. 
 * @param surface Pointer to another surface.
 */
void UnitSprite::blit(Surface *surface)
{
	Surface::blit(surface);
}

}
