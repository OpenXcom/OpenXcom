/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include "../Ruleset/RuleSoldier.h"
#include "../Ruleset/Unit.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/Armor.h"
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
UnitSprite::UnitSprite(int width, int height, int x, int y) : Surface(width, height, x, y), _unit(0), _item(0), _unitSurface(0), _itemSurface(0), _part(0), _animationFrame(0)
{


}

/**
 * Deletes the UnitSprite.
 */
UnitSprite::~UnitSprite()
{

}

/**
 * Changes the surface sets for the UnitSprite to get resources for rendering.
 * @param unitSurface Pointer to the unit surface set.
 * @param itemSurface Pointer to the item surface set.
 */
void UnitSprite::setSurfaces(SurfaceSet *unitSurface, SurfaceSet *itemSurface)
{
	_unitSurface = unitSurface;
	_itemSurface = itemSurface;
	_redraw = true;
}

/**
 * Links this sprite to a BattleUnit to get the data for rendering.
 * @param unit Pointer to the BattleUnit.
 * @param part The part number for large units.
 */
void UnitSprite::setBattleUnit(BattleUnit *unit, int part)
{
	_unit = unit;
	_redraw = true;
	_part = part;
}

/**
 * Links this sprite to a BattleItem to get the data for rendering.
 * @param item Pointer to the BattleItem.
 */
void UnitSprite::setBattleItem(BattleItem *item)
{
	_item = item;
	_redraw = true;
}

/**
 * Sets the animation frame for animated units.
 * @param frame Frame number.
 */
void UnitSprite::setAnimationFrame(int frame)
{
	_animationFrame = frame;
}
/**
 * Draws a unit, using the drawing rules of the unit.
 * This function is called by Map, for each unit on the screen.
 */
void UnitSprite::draw()
{
	Surface::draw();

	switch (_unit->getArmor()->getDrawingRoutine())
	{
	case 0:
		drawRoutine0();
		break;
	case 1:
		drawRoutine1();
		break;
	case 2:
		drawRoutine2();
		break;
	case 3:
		drawRoutine3();
		break;
	case 4:
		drawRoutine4();
		break;
	case 5:
		drawRoutine5();
		break;
	}

}

/**
 * Drawing routine for xcom soldiers in overalls and Sectoids.
 */
void UnitSprite::drawRoutine0()
{

	Surface *torso = 0, *legs = 0, *leftArm = 0, *rightArm = 0, *item = 0;
	// magic numbers
	const int maleTorso = 32, femaleTorso = 267, legsStand = 16, legsKneel = 24, die = 264, legsFloat = 275;
	const int larmStand = 0, rarmStand = 8, rarm1H = 232, larm2H = 240, ramr2H = 248, rarmShoot = 256;
	const int legsWalk[8] = { 56, 56+24, 56+24*2, 56+24*3, 56+24*4, 56+24*5, 56+24*6, 56+24*7 };
	const int larmWalk[8] = { 40, 40+24, 40+24*2, 40+24*3, 40+24*4, 40+24*5, 40+24*6, 40+24*7 };
	const int rarmWalk[8] = { 48, 48+24, 48+24*2, 48+24*3, 48+24*4, 48+24*5, 48+24*6, 48+24*7 };
	const int yoffWalk[8] = {1, 0, -1, 0, 1, 0, -1, 0}; // bobbing up and down
	const int offX[8] = { 8, 10, 7, 4, -9, -11, -7, -3 }; // for the weapons
	const int offY[8] = { -6, -3, 0, 2, 0, -4, -7, -9 }; // for the weapons
	const int offYKneel = 4;

	if (_unit->isOut())
	{
		// unit is drawn as an item
		return;
	}

	if (_unit->getStatus() == STATUS_COLLAPSING)
	{
		torso = _unitSurface->getFrame(die + _unit->getFallingPhase());
		torso->blit(this);
		return;
	}

	if (_unit->getArmor()->getType() == "STR_POWER_SUIT_UC")
	{
		torso = _unitSurface->getFrame(maleTorso + _unit->getDirection());
	}
	else if (_unit->getArmor()->getType() == "STR_FLYING_SUIT_UC")
	{
		torso = _unitSurface->getFrame(femaleTorso + _unit->getDirection());
	}
	else if (_unit->getGender() == GENDER_FEMALE)
	{
		torso = _unitSurface->getFrame(femaleTorso + _unit->getDirection());
	}
	else
	{
		torso = _unitSurface->getFrame(maleTorso + _unit->getDirection());
	}

	// when walking, torso(fixed sprite) has to be animated up/down
	if (_unit->getStatus() == STATUS_WALKING)
	{
		torso->setY(yoffWalk[_unit->getWalkingPhase()]);
		legs = _unitSurface->getFrame(legsWalk[_unit->getDirection()] + _unit->getWalkingPhase());
		leftArm = _unitSurface->getFrame(larmWalk[_unit->getDirection()] + _unit->getWalkingPhase());
		rightArm = _unitSurface->getFrame(rarmWalk[_unit->getDirection()] + _unit->getWalkingPhase());
	}
	else
	{
		if (_unit->isKneeled())
		{
			legs = _unitSurface->getFrame(legsKneel + _unit->getDirection());
		}
		else if (_unit->isFloating())
		{
			legs = _unitSurface->getFrame(legsFloat + _unit->getDirection());
		}
		else
		{
			legs = _unitSurface->getFrame(legsStand + _unit->getDirection());
		}
		leftArm = _unitSurface->getFrame(larmStand + _unit->getDirection());
		rightArm = _unitSurface->getFrame(rarmStand + _unit->getDirection());
	}

	// holding an item
	if (_item)
	{
		// draw handob item
		if (_unit->getStatus() == STATUS_AIMING && _item->getRules()->isTwoHanded())
		{
			int dir = (_unit->getDirection() + 2)%8;
			item = _itemSurface->getFrame(_item->getRules()->getHandSprite() + dir);
			item->setX(offX[_unit->getDirection()]);
			item->setY(offY[_unit->getDirection()]);
		}
		else
		{
			item = _itemSurface->getFrame(_item->getRules()->getHandSprite() + _unit->getDirection());
			item->setX(0);
			item->setY(0);
		}

		// draw arms holding the item
		if (_item->getRules()->isTwoHanded())
		{
			leftArm = _unitSurface->getFrame(larm2H + _unit->getDirection());
			if (_unit->getStatus() == STATUS_AIMING)
			{
				rightArm = _unitSurface->getFrame(rarmShoot + _unit->getDirection());
			}
			else
			{
				rightArm = _unitSurface->getFrame(ramr2H + _unit->getDirection());
			}
		}
		else
		{
			rightArm = _unitSurface->getFrame(rarm1H + _unit->getDirection());
		}

		// the fixed arm(s) have to be animated up/down when walking
		if (_unit->getStatus() == STATUS_WALKING)
		{
			item->setY(yoffWalk[_unit->getWalkingPhase()]);
			rightArm->setY(yoffWalk[_unit->getWalkingPhase()]);
			if (_item->getRules()->isTwoHanded())
				leftArm->setY(yoffWalk[_unit->getWalkingPhase()]);
		}
	}

	// offset everything but legs when kneeled
	if (_unit->isKneeled())
	{
		leftArm->setY(offYKneel);
		rightArm->setY(offYKneel);
		torso->setY(offYKneel);
		item?item->setY(item->getY() + offYKneel):void();
	}
	else if (_unit->getStatus() != STATUS_WALKING)
	{
		leftArm->setY(0);
		rightArm->setY(0);
		torso->setY(0);
	}

	// items are calculated for soldier height (22) - some aliens are smaller, so item is drawn lower.
	if (item)
	{
		item->setY(item->getY() + (22 - _unit->getStandHeight()));
	}

	// blit order depends on unit direction
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
 * Drawing routine for floaters.
 */
void UnitSprite::drawRoutine1()
{

	Surface *torso = 0, *leftArm = 0, *rightArm = 0, *item = 0;
	// magic numbers
	const int stand = 16, walk = 24, die = 64;
	const int larm = 8, rarm = 0, larm2H = 67, ramr2H = 75, rarmShoot = 83, rarm1H= 91; // note that arms are switched vs "normal" sheets
	const int yoffWalk[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // bobbing up and down
	const int offX[8] = { 8, 10, 7, 4, -9, -11, -7, -3 }; // for the weapons
	const int offY[8] = { -6, -3, 0, 2, 0, -4, -7, -9 }; // for the weapons

	if (_unit->isOut())
	{
		// unit is drawn as an item
		return;
	}

	if (_unit->getStatus() == STATUS_COLLAPSING)
	{
		torso = _unitSurface->getFrame(die + _unit->getFallingPhase());
		torso->blit(this);
		return;
	}


	leftArm = _unitSurface->getFrame(larm + _unit->getDirection());
	rightArm = _unitSurface->getFrame(rarm + _unit->getDirection());
	// when walking, torso(fixed sprite) has to be animated up/down
	if (_unit->getStatus() == STATUS_WALKING)
	{
		torso = _unitSurface->getFrame(walk + (5 * _unit->getDirection()) + (_unit->getWalkingPhase() / 1.6)); // floater only has 5 walk animations instead of 8
		torso->setY(yoffWalk[_unit->getWalkingPhase()]);
	}
	else
	{
		torso = _unitSurface->getFrame(stand + _unit->getDirection());
	}

	// holding an item
	if (_item)
	{
		// draw handob item
		if (_unit->getStatus() == STATUS_AIMING && _item->getRules()->isTwoHanded())
		{
			int dir = (_unit->getDirection() + 2)%8;
			item = _itemSurface->getFrame(_item->getRules()->getHandSprite() + dir);
			item->setX(offX[_unit->getDirection()]);
			item->setY(offY[_unit->getDirection()]);
		}
		else
		{
			item = _itemSurface->getFrame(_item->getRules()->getHandSprite() + _unit->getDirection());
			item->setX(0);
			item->setY(0);
		}

		// draw arms holding the item
		if (_item->getRules()->isTwoHanded())
		{
			leftArm = _unitSurface->getFrame(larm2H + _unit->getDirection());
			if (_unit->getStatus() == STATUS_AIMING)
			{
				rightArm = _unitSurface->getFrame(rarmShoot + _unit->getDirection());
			}
			else
			{
				rightArm = _unitSurface->getFrame(ramr2H + _unit->getDirection());
			}
		}
		else
		{
			rightArm = _unitSurface->getFrame(rarm1H + _unit->getDirection());
		}

		// the fixed arm(s) have to be animated together up/down when walking
		if (_unit->getStatus() == STATUS_WALKING)
		{
			item->setY(yoffWalk[_unit->getWalkingPhase()]);
			rightArm->setY(yoffWalk[_unit->getWalkingPhase()]);
			if (_item->getRules()->isTwoHanded())
				leftArm->setY(yoffWalk[_unit->getWalkingPhase()]);
		}
	}

	if (_unit->getStatus() != STATUS_WALKING)
	{
		leftArm->setY(0);
		rightArm->setY(0);
		torso->setY(0);
	}

	// items are calculated for soldier height (22) - some aliens are smaller, so item is drawn lower.
	/*if (item)
	{
		item->setY(item->getY() + (22 - _unit->getUnit()->getStandHeight()));
	}*/

	// blit order depends on unit direction
	switch (_unit->getDirection())
	{
	case 0: leftArm->blit(this); item?item->blit(this):void(); torso->blit(this); rightArm->blit(this); break;
	case 1: leftArm->blit(this); torso->blit(this); item?item->blit(this):void(); rightArm->blit(this); break;
	case 2: leftArm->blit(this); torso->blit(this); item?item->blit(this):void(); rightArm->blit(this); break;
	case 3: torso->blit(this); leftArm->blit(this); rightArm->blit(this); item?item->blit(this):void(); break;
	case 4: rightArm->blit(this); torso->blit(this); leftArm->blit(this); item?item->blit(this):void(); break;
	case 5: rightArm->blit(this); item?item->blit(this):void(); torso->blit(this); leftArm->blit(this); break;
	case 6: rightArm->blit(this); item?item->blit(this):void(); torso->blit(this); leftArm->blit(this); break;
	case 7: item?item->blit(this):void(); leftArm->blit(this); rightArm->blit(this); torso->blit(this); break;
	}
}

/**
 * Drawing routine for x-com tanks.
 */
void UnitSprite::drawRoutine2()
{
	if (_unit->isOut())
	{
		// unit is drawn as an item
		return;
	}

	int hoverTank = 0;
	if (_unit->getArmor()->getMovementType() == MT_FLY)
	{
		hoverTank = 32;
	}

	Surface *s = 0;
	int turret = _unit->getTurretType();

	// draw the animated propulsion below the hwp
	if (_part > 0 && hoverTank != 0)
	{
		s = _unitSurface->getFrame(104 + ((_part-1) * 8) + _animationFrame);
		s->blit(this);
	}

	// draw the tank itself
	s = _unitSurface->getFrame(hoverTank + (_part * 8) + _unit->getDirection());
	s->blit(this);

	// draw the turret, together with the last part
	if (_part == 3 && turret != -1)
	{
		s = _unitSurface->getFrame(64 + (turret * 8) + _unit->getTurretDirection());
		s->setX(0);
		s->setY(-4);
		s->blit(this);
	}

}

/**
 * Drawing routine for cyberdiscs.
 */
void UnitSprite::drawRoutine3()
{
	if (_unit->isOut())
	{
		// unit is drawn as an item
		return;
	}

	Surface *s = 0;
	
	// draw the animated propulsion below the hwp
	if (_part > 0)
	{
		s = _unitSurface->getFrame(32 + ((_part-1) * 8) + _animationFrame);
		s->blit(this);
	}

	s = _unitSurface->getFrame((_part * 8) + _unit->getDirection());

	s->blit(this);
}

/**
 * Drawing routine for civilians.
 * Very easy: first 8 is standing positions, then 8 walking sequences of 8, finally death sequence of 3
 */
void UnitSprite::drawRoutine4()
{
	if (_unit->isOut())
	{
		// unit is drawn as an item
		return;
	}

	Surface *s = 0;
	const int stand = 0, walk = 8, die = 72;

	
	if (_unit->isOut())
	{
		// unit is drawn as an item
		return;
	}

	if (_unit->getStatus() == STATUS_COLLAPSING)
	{
		s = _unitSurface->getFrame(die + _unit->getFallingPhase());
	}
	else
	if (_unit->getStatus() == STATUS_WALKING)
	{
		s = _unitSurface->getFrame(walk + (8 * _unit->getDirection()) + _unit->getWalkingPhase());
	}
	else
	{
		s = _unitSurface->getFrame(stand + _unit->getDirection());
	}

	s->blit(this);
}

/**
 * Drawing routine for sectopods and reapers.
 */
void UnitSprite::drawRoutine5()
{
	if (_unit->isOut())
	{
		// unit is drawn as an item
		return;
	}

	Surface *s = 0;

	if (_unit->getStatus() == STATUS_WALKING)
	{
		s = _unitSurface->getFrame( 32 + (_unit->getDirection() * 16) + (_part * 4) + (_unit->getWalkingPhase() % 4) );
	}
	else
	{
		s = _unitSurface->getFrame((_part * 8) + _unit->getDirection());
	}
	
	s->blit(this);
}

}
