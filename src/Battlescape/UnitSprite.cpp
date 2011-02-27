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
	Surface *torso = 0, *legs = 0, *bottomArm = 0, *topArm = 0, *item = 0;
	clear();
	
	if (rules->getTorso() > -1)
	{
		if (_unit != 0 && _unit->getGender() == GENDER_FEMALE)
		{
			torso = _res->getSurfaceSet(sheet)->getFrame(rules->getFemaleTorso() + _unit->getDirection());
		}
		else
		{
			torso = _res->getSurfaceSet(sheet)->getFrame(rules->getTorso() + _unit->getDirection());
		}
	}

	if (torso && _unit->getStatus() == STATUS_WALKING)
	{
		torso->setY(rules->getWalkTorsoOffset(_unit->getWalkingPhase()));
	}

	if (_unit->getStatus() == STATUS_STANDING || _unit->getStatus() == STATUS_TURNING || _unit->getStatus() == STATUS_AIMING)
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
	if (_unit->getStatus() == STATUS_WALKING)
	{
		legs = _res->getSurfaceSet(sheet)->getFrame(rules->getLegsWalk(_unit->getDirection()) + _unit->getWalkingPhase());
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
	}

	if (_item)
	{
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

		if (_item->getRules()->getTwoHanded())
		{
			if (_unit->getDirection() < 4)
			{
				bottomArm = _res->getSurfaceSet(sheet)->getFrame(rules->getLeftArm2HWeapon() + _unit->getDirection());
				if (_unit->getStatus() == STATUS_AIMING)
				{
					topArm = _res->getSurfaceSet(sheet)->getFrame(rules->getRightArm2HShoot() + _unit->getDirection());
				}
				else
				{
					topArm = _res->getSurfaceSet(sheet)->getFrame(rules->getRightArm2HWeapon() + _unit->getDirection());
				}
			}
			else
			{
				topArm = _res->getSurfaceSet(sheet)->getFrame(rules->getLeftArm2HWeapon() + _unit->getDirection());
				if (_unit->getStatus() == STATUS_AIMING)
				{
					bottomArm = _res->getSurfaceSet(sheet)->getFrame(rules->getRightArm2HShoot() + _unit->getDirection());
				}
				else
				{
					bottomArm = _res->getSurfaceSet(sheet)->getFrame(rules->getRightArm2HWeapon() + _unit->getDirection());
				}
			}
			if (_unit->getStatus() == STATUS_WALKING)
			{
				topArm->setY(rules->getWalkTorsoOffset(_unit->getWalkingPhase()));
				bottomArm->setY(rules->getWalkTorsoOffset(_unit->getWalkingPhase()));
			}
		}
		else
		{
			if (_unit->getDirection() < 4)
			{
				topArm = _res->getSurfaceSet(sheet)->getFrame(rules->getRightArm1HWeapon() + _unit->getDirection());
				if (_unit->getStatus() == STATUS_WALKING)
				{
					topArm->setY(rules->getWalkTorsoOffset(_unit->getWalkingPhase()));
				}
			}
			else
			{
				bottomArm = _res->getSurfaceSet(sheet)->getFrame(rules->getRightArm1HWeapon() + _unit->getDirection());
				if (_unit->getStatus() == STATUS_WALKING)
				{
					bottomArm->setY(rules->getWalkTorsoOffset(_unit->getWalkingPhase()));
				}
			}
		}
		if (_unit->getStatus() == STATUS_WALKING)
		{
			item->setY(rules->getWalkTorsoOffset(_unit->getWalkingPhase()));
		}
	}

	// todo: kneeling +4 pixels
	// to check
/*
[00:01] <Dexus> dir0_seq: [leftarm, legs, handob, torso, rightarm]
[00:01] <Dexus> dir1_seq: [leftarm, legs, torso, handob, rightarm]
[00:01] <Dexus> dir2_seq: [leftarm, legs, torso, handob, rightarm]
[00:01] <Dexus> dir3_seq: [legs, torso, leftarm, rightarm, handob]
[00:01] <Dexus> dir4_seq: [rightarm, legs, torso, leftarm, handob]
[00:01] <Dexus> dir5_seq: [rightarm, legs, handob, torso, leftarm]
[00:01] <Dexus> dir6_seq: [rightarm, legs, handob, torso, leftarm]
[00:01] <Dexus> dir7_seq: [handob, leftarm, rightarm, legs, torso]
*/

	if (_unit->getDirection() < 1 || _unit->getDirection() > 5)
		if (item) item->blit(this);
	if (bottomArm) bottomArm->blit(this);
	if (torso) torso->blit(this);
	if (legs) legs->blit(this);
	if (_unit->getDirection() >= 4)	
		if (topArm) topArm->blit(this);
	if (_unit->getDirection() > 0 && _unit->getDirection() < 6)
		if (item) item->blit(this);	
	if (_unit->getDirection() < 4)	
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

}
