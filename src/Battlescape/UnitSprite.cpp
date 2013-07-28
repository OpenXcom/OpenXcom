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
#include "../Ruleset/RuleInventory.h"
#include "../Ruleset/Ruleset.h"
#include "../Engine/ShaderDraw.h"
#include "../Engine/ShaderMove.h"

namespace OpenXcom
{

/**
 * Sets up a UnitSprite with the specified size and position.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
UnitSprite::UnitSprite(int width, int height, int x, int y) : Surface(width, height, x, y), _unit(0), _itemA(0), _itemB(0), _unitSurface(0), _itemSurfaceA(0), _itemSurfaceB(0), _part(0), _animationFrame(0)
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
void UnitSprite::setSurfaces(SurfaceSet *unitSurface, SurfaceSet *itemSurfaceA, SurfaceSet *itemSurfaceB)
{
	_unitSurface = unitSurface;
	_itemSurfaceA = itemSurfaceA;
	_itemSurfaceB = itemSurfaceB;
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
	if(item)
	{
		if(item->getSlot()->getId() == "STR_RIGHT_HAND")
			_itemA = item;
		if(item->getSlot()->getId() == "STR_LEFT_HAND")
			_itemB = item;
	}
	_redraw = true;
}


namespace
{

struct ColorFace
{
	static const Uint8 ColorGroup = 15<<4;
	static const Uint8 ColorShade = 15;

	static const Uint8 Hair = 9 << 4;
	static const Uint8 Face = 6 << 4;
	static inline void func(Uint8& src, const Uint8& hair_color, const Uint8& face_color, int, int)
	{
		if((src & ColorGroup) == Hair)
		{
			src = hair_color + (src & ColorShade);
		}
		else if((src & ColorGroup) == Face)
		{
			src = face_color + (src & ColorShade);
		}
	}
};


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
	_drawingRoutine = _unit->getArmor()->getDrawingRoutine();
	switch (_drawingRoutine)
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
	case 6:
		drawRoutine6();
		break;
	case 7:
		drawRoutine7();
		break;
	case 8:
		drawRoutine8();
		break;
	case 9:
		drawRoutine9();
		break;
	case 10: // muton
		drawRoutine0();
		break;
	}

}

/**
 * Drawing routine for xcom soldiers in overalls and Sectoids and Mutons (routine 10).
 */
void UnitSprite::drawRoutine0()
{

	Surface *torso = 0, *legs = 0, *leftArm = 0, *rightArm = 0, *itemA = 0, *itemB = 0;
	// magic numbers
	const int maleTorso = 32, femaleTorso = 267, legsStand = 16, legsKneel = 24, die = 264, legsFloat = 275;
	const int larmStand = 0, rarmStand = 8, rarm1H = 232, larm2H = 240, rarm2H = 248, rarmShoot = 256;
	const int legsWalk[8] = { 56, 56+24, 56+24*2, 56+24*3, 56+24*4, 56+24*5, 56+24*6, 56+24*7 };
	const int larmWalk[8] = { 40, 40+24, 40+24*2, 40+24*3, 40+24*4, 40+24*5, 40+24*6, 40+24*7 };
	const int rarmWalk[8] = { 48, 48+24, 48+24*2, 48+24*3, 48+24*4, 48+24*5, 48+24*6, 48+24*7 };
	const int yoffWalk[8] = {1, 0, -1, 0, 1, 0, -1, 0}; // bobbing up and down
	const int alternateyoffWalk[8] = {1, 1, 0, 0, 1, 1, 0, 0}; // bobbing up and down (muton)
	const int offX[8] = { 8, 10, 7, 4, -9, -11, -7, -3 }; // for the weapons
	const int offY[8] = { -6, -3, 0, 2, 0, -4, -7, -9 }; // for the weapons
	const int offX2[8] = { -8, 3, 5, 12, 6, -1, -5, -13 }; // for the left handed weapons
	const int offY2[8] = { 1, -4, -2, 0, 3, 3, 5, 0 }; // for the left handed weapons
	const int offX3[8] = { 0, 0, 2, 2, 0, 0, 0, 0 }; // for the weapons (muton)
	const int offY3[8] = { -3, -3, -1, -1, -1, -3, -3, -2 }; // for the weapons (muton)
	const int offX4[8] = { -8, 2, 7, 14, 7, -2, -4, -8 }; // for the left handed weapons
	const int offY4[8] = { -3, -3, -1, 0, 3, 3, 0, 1 }; // for the left handed weapons
	const int offX5[8] = { -1, 1, 1, 2, 0, -1, 0, 0 }; // for the weapons (muton)
	const int offY5[8] = { 1, -1, -1, -1, -1, -2, -3, 0 }; // for the weapons (muton)
	const int offX6[8] = { 0, 6, 6, 12, -4, -5, -5, -13 }; // for the left handed rifles
	const int offY6[8] = { -4, -4, -1, 0, 5, 0, 1, 0 }; // for the left handed rifles
	const int offX7[8] = { 0, 6, 8, 12, 2, -5, -5, -13 }; // for the left handed rifles (muton)
	const int offY7[8] = { -4, -6, -1, 0, 3, 0, 1, 0 }; // for the left handed rifles (muton)
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
		if(_unit->getGeoscapeSoldier())
		{
			SoldierLook look = _unit->getGeoscapeSoldier()->getLook();

			if(look)
			{
				Uint8 face_color = ColorFace::Face;
				Uint8 hair_color = ColorFace::Hair;
				switch(look)
				{
					case LOOK_BLONDE:
						break;
					case LOOK_BROWNHAIR:
						hair_color = (10<<4) + 4;
						break;
					case LOOK_ORIENTAL:
						face_color = 10<<4;
						hair_color = (15<<4) + 5;
						break;
					case LOOK_AFRICAN:
						face_color = (10<<4) + 3;
						hair_color = (10<<4) + 6;
						break;
				}
				lock();
				ShaderDraw<ColorFace>(ShaderSurface(this), ShaderScalar(hair_color), ShaderScalar(face_color));
				unlock();
			}
		}
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
		if(_drawingRoutine == 10)
			torso->setY(alternateyoffWalk[_unit->getWalkingPhase()]);
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
		else if (_unit->isFloating() && _unit->getArmor()->getMovementType() == MT_FLY)
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

	sortRifles();

	// holding an item
	if (_itemA)
	{
		// draw handob item
		if (_unit->getStatus() == STATUS_AIMING && _itemA->getRules()->isTwoHanded())
		{
			int dir = (_unit->getDirection() + 2)%8;
			itemA = _itemSurfaceA->getFrame(_itemA->getRules()->getHandSprite() + dir);
			itemA->setX(offX[_unit->getDirection()]);
			itemA->setY(offY[_unit->getDirection()]);
		}
		else
		{
			itemA = _itemSurfaceA->getFrame(_itemA->getRules()->getHandSprite() + _unit->getDirection());
			if(_drawingRoutine == 10)
			{
				if(_itemA->getRules()->isTwoHanded())
				{
					itemA->setX(offX3[_unit->getDirection()]);
					itemA->setY(offY3[_unit->getDirection()]);
				}
				else
				{
					itemA->setX(offX5[_unit->getDirection()]);
					itemA->setY(offY5[_unit->getDirection()]);
				}
			}
			else
			{
				itemA->setX(0);
				itemA->setY(0);
			}
		}

		// draw arms holding the item
		if (_itemA->getRules()->isTwoHanded())
		{
			leftArm = _unitSurface->getFrame(larm2H + _unit->getDirection());
			if (_unit->getStatus() == STATUS_AIMING)
			{
				rightArm = _unitSurface->getFrame(rarmShoot + _unit->getDirection());
			}
			else
			{
				rightArm = _unitSurface->getFrame(rarm2H + _unit->getDirection());
			}
		}
		else
		{
			if(_drawingRoutine == 10)
				rightArm = _unitSurface->getFrame(rarm2H + _unit->getDirection()); // missing/wrong arms on muton here, investigate spriteset
			else
				rightArm = _unitSurface->getFrame(rarm1H + _unit->getDirection());
		}
		

		// the fixed arm(s) have to be animated up/down when walking
		if (_unit->getStatus() == STATUS_WALKING)
		{
			if(_drawingRoutine == 10)
			{
				itemA->setY(itemA->getY() + alternateyoffWalk[_unit->getWalkingPhase()]);
				rightArm->setY(alternateyoffWalk[_unit->getWalkingPhase()]);
				if (_itemA->getRules()->isTwoHanded())
					leftArm->setY(alternateyoffWalk[_unit->getWalkingPhase()]);
			}
			else
			{
				itemA->setY(itemA->getY() + yoffWalk[_unit->getWalkingPhase()]);
				rightArm->setY(yoffWalk[_unit->getWalkingPhase()]);
				if (_itemA->getRules()->isTwoHanded())
					leftArm->setY(yoffWalk[_unit->getWalkingPhase()]);
			}
		}
	}
	//if we are left handed or dual wielding...
	if(_itemB)
	{
		leftArm = _unitSurface->getFrame(larm2H + _unit->getDirection());
		itemB = _itemSurfaceB->getFrame(_itemB->getRules()->getHandSprite() + _unit->getDirection());
		if (!_itemB->getRules()->isTwoHanded())
		{
			if(_drawingRoutine == 10)
			{
				itemB->setX(offX4[_unit->getDirection()]);
				itemB->setY(offY4[_unit->getDirection()]);
			}
			else
			{
				itemB->setX(offX2[_unit->getDirection()]);
				itemB->setY(offY2[_unit->getDirection()]);
			}
		}
		else
		{
			itemB->setX(0);
			itemB->setY(0);
			rightArm = _unitSurface->getFrame(rarm2H + _unit->getDirection());
		}

		if (_unit->getStatus() == STATUS_AIMING && _itemB->getRules()->isTwoHanded())
		{
			int dir = (_unit->getDirection() + 2)%8;
			itemB = _itemSurfaceB->getFrame(_itemB->getRules()->getHandSprite() + dir);
			if(_drawingRoutine == 10)
			{
				itemB->setX(offX7[_unit->getDirection()]);
				itemB->setY(offY7[_unit->getDirection()]);
			}
			else
			{
				itemB->setX(offX6[_unit->getDirection()]);
				itemB->setY(offY6[_unit->getDirection()]);
			}
			rightArm = _unitSurface->getFrame(rarmShoot + _unit->getDirection());
		}

		if (_unit->getStatus() == STATUS_WALKING)
		{
			if(_drawingRoutine == 10)
			{
				leftArm->setY(alternateyoffWalk[_unit->getWalkingPhase()]);
				itemB->setY(itemB->getY() + alternateyoffWalk[_unit->getWalkingPhase()]);
				if (_itemB->getRules()->isTwoHanded())
					rightArm->setY(alternateyoffWalk[_unit->getWalkingPhase()]);
			}
			else
			{
				leftArm->setY(yoffWalk[_unit->getWalkingPhase()]);
				itemB->setY(itemB->getY() + yoffWalk[_unit->getWalkingPhase()]);
				if (_itemB->getRules()->isTwoHanded())
					rightArm->setY(yoffWalk[_unit->getWalkingPhase()]);
			}
		}
	}
	// offset everything but legs when kneeled
	if (_unit->isKneeled())
	{
		leftArm->setY(offYKneel);
		rightArm->setY(offYKneel);
		torso->setY(offYKneel);
		itemA?itemA->setY(itemA->getY() + offYKneel):void();
		itemB?itemB->setY(itemB->getY() + offYKneel):void();
	}
	else if (_unit->getStatus() != STATUS_WALKING)
	{
		leftArm->setY(0);
		rightArm->setY(0);
		torso->setY(0);
	}

	// items are calculated for soldier height (22) - some aliens are smaller, so item is drawn lower.
	if (itemA)
	{
		itemA->setY(itemA->getY() + (22 - _unit->getStandHeight()));
	}
	if (itemB)
	{
		itemB->setY(itemB->getY() + (22 - _unit->getStandHeight()));
	}
	
	Surface *newTorso = new Surface(*torso);
	Surface *newLegs = new Surface(*legs);
	Surface *newLeftArm = new Surface(*leftArm);
	Surface *newRightArm = new Surface(*rightArm);
	if(_unit->getGeoscapeSoldier())
	{
		SoldierLook look = _unit->getGeoscapeSoldier()->getLook();
		
		if(look)
		{
			Uint8 face_color = ColorFace::Face;
			Uint8 hair_color = ColorFace::Hair;
			switch(look)
			{
				case LOOK_BLONDE:
					break;
				case LOOK_BROWNHAIR:
					hair_color = (10<<4) + 4;
					break;
				case LOOK_ORIENTAL:
					face_color = 10<<4;
					hair_color = (15<<4) + 5;
					break;
				case LOOK_AFRICAN:
					face_color = (10<<4) + 3;
					hair_color = (10<<4) + 6;
					break;
			}
			lock();
			ShaderDraw<ColorFace>(ShaderSurface(newLeftArm), ShaderScalar(hair_color), ShaderScalar(face_color));
			ShaderDraw<ColorFace>(ShaderSurface(newRightArm), ShaderScalar(hair_color), ShaderScalar(face_color));
			ShaderDraw<ColorFace>(ShaderSurface(newTorso), ShaderScalar(hair_color), ShaderScalar(face_color));
			ShaderDraw<ColorFace>(ShaderSurface(newLegs), ShaderScalar(hair_color), ShaderScalar(face_color));
			unlock();
			torso = newTorso;
			legs = newLegs;
			leftArm = newLeftArm;
			rightArm = newRightArm;
		}
	}

	// blit order depends on unit direction, and whether we are holding a 2 handed weapon.
	switch (_unit->getDirection())
	{
	case 0: itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void(); leftArm->blit(this); legs->blit(this); torso->blit(this); rightArm->blit(this); break;
	case 1: leftArm->blit(this); legs->blit(this); itemB?itemB->blit(this):void(); torso->blit(this); itemA?itemA->blit(this):void(); rightArm->blit(this); break;
	case 2: leftArm->blit(this); legs->blit(this); itemB?itemB->blit(this):void(); torso->blit(this); itemA?itemA->blit(this):void(); rightArm->blit(this); break;
	case 3:
		if (_unit->getStatus() != STATUS_AIMING  && ((_itemA && _itemA->getRules()->isTwoHanded()) || (_itemB && _itemB->getRules()->isTwoHanded())))
		{
			legs->blit(this); torso->blit(this); leftArm->blit(this); itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void(); rightArm->blit(this);
		}
		else
		{
			legs->blit(this); torso->blit(this); leftArm->blit(this); rightArm->blit(this); itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void();
		}
		break;
	case 4:	legs->blit(this); rightArm->blit(this); torso->blit(this); leftArm->blit(this); itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void();	break;
	case 5:
		if (_unit->getStatus() != STATUS_AIMING  && ((_itemA && _itemA->getRules()->isTwoHanded()) || (_itemB && _itemB->getRules()->isTwoHanded())))
		{
			rightArm->blit(this); legs->blit(this); torso->blit(this); leftArm->blit(this); itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void();
		}
		else
		{
			rightArm->blit(this); legs->blit(this); itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void(); torso->blit(this); leftArm->blit(this);
		}
		break;
	case 6: rightArm->blit(this); legs->blit(this); itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void(); torso->blit(this); leftArm->blit(this); break;
	case 7:
		if (_unit->getStatus() != STATUS_AIMING  && ((_itemA && _itemA->getRules()->isTwoHanded()) || (_itemB && _itemB->getRules()->isTwoHanded())))
		{
			rightArm->blit(this); itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void(); leftArm->blit(this); legs->blit(this); torso->blit(this);
		}
		else
		{
			itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void(); leftArm->blit(this); rightArm->blit(this); legs->blit(this); torso->blit(this);
		}
		break;
	}
	delete(newTorso);
	delete(newLegs);
	delete(newLeftArm);
	delete(newRightArm);
}


/**
 * Drawing routine for floaters.
 */
void UnitSprite::drawRoutine1()
{

	Surface *torso = 0, *leftArm = 0, *rightArm = 0, *itemA = 0, *itemB = 0;
	// magic numbers
	const int stand = 16, walk = 24, die = 64;
	const int larm = 8, rarm = 0, larm2H = 67, rarm2H = 75, rarmShoot = 83, rarm1H= 91; // note that arms are switched vs "normal" sheets
	const int yoffWalk[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // bobbing up and down
	const int offX[8] = { 8, 10, 7, 4, -9, -11, -7, -3 }; // for the weapons
	const int offY[8] = { -6, -3, 0, 2, 0, -4, -7, -9 }; // for the weapons
	const int offX2[8] = { -8, 3, 7, 13, 6, -3, -5, -13 }; // for the weapons
	const int offY2[8] = { 1, -4, -1, 0, 3, 3, 5, 0 }; // for the weapons
	const int offX3[8] = { 0, 6, 6, 12, -4, -5, -5, -13 }; // for the left handed rifles
	const int offY3[8] = { -4, -4, -1, 0, 5, 0, 1, 0 }; // for the left handed rifles
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

	sortRifles();

	// holding an item
	if (_itemA)
	{
		// draw handob item
		if (_unit->getStatus() == STATUS_AIMING && _itemA->getRules()->isTwoHanded())
		{
			int dir = (_unit->getDirection() + 2)%8;
			itemA = _itemSurfaceA->getFrame(_itemA->getRules()->getHandSprite() + dir);
			itemA->setX(offX[_unit->getDirection()]);
			itemA->setY(offY[_unit->getDirection()]);
		}
		else
		{
			itemA = _itemSurfaceA->getFrame(_itemA->getRules()->getHandSprite() + _unit->getDirection());
			itemA->setX(0);
			itemA->setY(0);
		}
		// draw arms holding the item
		if (_itemA->getRules()->isTwoHanded())
		{
			leftArm = _unitSurface->getFrame(larm2H + _unit->getDirection());
			if (_unit->getStatus() == STATUS_AIMING)
			{
				rightArm = _unitSurface->getFrame(rarmShoot + _unit->getDirection());
			}
			else
			{
				rightArm = _unitSurface->getFrame(rarm2H + _unit->getDirection());
			}
		}
		else
		{
			rightArm = _unitSurface->getFrame(rarm1H + _unit->getDirection());
		}
	}

	//if we are left handed or dual wielding...
	if(_itemB)
	{
		leftArm = _unitSurface->getFrame(larm2H + _unit->getDirection());
		itemB = _itemSurfaceB->getFrame(_itemB->getRules()->getHandSprite() + _unit->getDirection());
		if (!_itemB->getRules()->isTwoHanded())
		{
			itemB->setX(offX2[_unit->getDirection()]);
			itemB->setY(offY2[_unit->getDirection()]);
		}
		else
		{
			itemB->setX(0);
			itemB->setY(0);
			rightArm = _unitSurface->getFrame(rarm2H + _unit->getDirection());
		}

		if (_unit->getStatus() == STATUS_AIMING && _itemB->getRules()->isTwoHanded())
		{
			int dir = (_unit->getDirection() + 2)%8;
			itemB = _itemSurfaceB->getFrame(_itemB->getRules()->getHandSprite() + dir);
			itemB->setX(offX3[_unit->getDirection()]);
			itemB->setY(offY3[_unit->getDirection()]);
			rightArm = _unitSurface->getFrame(rarmShoot + _unit->getDirection());
		}

		if (_unit->getStatus() == STATUS_WALKING)
		{
			leftArm->setY(yoffWalk[_unit->getWalkingPhase()]);
			itemB->setY(itemB->getY() + yoffWalk[_unit->getWalkingPhase()]);
			if (_itemB->getRules()->isTwoHanded())
				rightArm->setY(yoffWalk[_unit->getWalkingPhase()]);
		}
	}

	if (_unit->getStatus() != STATUS_WALKING)
	{
		leftArm->setY(0);
		rightArm->setY(0);
		torso->setY(0);
	}

	// blit order depends on unit direction.
	switch (_unit->getDirection())
	{
	case 0: itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void(); leftArm->blit(this); torso->blit(this); rightArm->blit(this); break;
	case 1: leftArm->blit(this); torso->blit(this); rightArm->blit(this); itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void(); break;
	case 2: leftArm->blit(this); torso->blit(this); rightArm->blit(this); itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void();  break;
	case 3:	torso->blit(this); leftArm->blit(this); rightArm->blit(this); itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void(); break;
	case 4:	torso->blit(this); leftArm->blit(this); rightArm->blit(this); itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void(); break;
	case 5:	rightArm->blit(this); torso->blit(this); leftArm->blit(this); itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void(); break;
	case 6: rightArm->blit(this); itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void(); torso->blit(this); leftArm->blit(this); break;
	case 7:	rightArm->blit(this); itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void(); leftArm->blit(this); torso->blit(this); break;
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
	const int offX[8] = { -2, -7, -5, 0, 5, 7, 2, 0 }; // hovertank offsets
	const int offy[8] = { -1, -3, -4, -5, -4, -3, -1, -1 }; // hovertank offsets

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
		int turretOffsetX = 0;
		int turretOffsetY = -4;
		if(hoverTank)
		{
			turretOffsetX += offX[_unit->getDirection()];
			turretOffsetY += offy[_unit->getDirection()];
		}
		s->setX(turretOffsetX);
		s->setY(turretOffsetY);
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
 * Drawing routine for civilians and ethereals.
 * Very easy: first 8 is standing positions, then 8 walking sequences of 8, finally death sequence of 3
 */
void UnitSprite::drawRoutine4()
{
	if (_unit->isOut())
	{
		// unit is drawn as an item
		return;
	}

	Surface *s = 0, *itemA = 0, *itemB = 0;
	const int stand = 0, walk = 8, die = 72;
	const int offX[8] = { 8, 10, 7, 4, -9, -11, -7, -3 }; // for the weapons
	const int offY[8] = { -6, -3, 0, 2, 0, -4, -7, -9 }; // for the weapons
	const int offX2[8] = { -8, 3, 5, 12, 6, -1, -5, -13 }; // for the weapons
	const int offY2[8] = { 1, -4, -2, 0, 3, 3, 5, 0 }; // for the weapons
	const int offX3[8] = { 0, 6, 6, 12, -4, -5, -5, -13 }; // for the left handed rifles
	const int offY3[8] = { -4, -4, -1, 0, 5, 0, 1, 0 }; // for the left handed rifles

	if (_unit->isOut())
	{
		// unit is drawn as an item
		return;
	}

	if (_unit->getStatus() == STATUS_COLLAPSING)
	{
		s = _unitSurface->getFrame(die + _unit->getFallingPhase());
		s->blit(this);
		return;
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

	sortRifles();

	if(_itemA && !_itemA->getRules()->isFixed())
	{
		// draw handob item
		if (_unit->getStatus() == STATUS_AIMING && _itemA->getRules()->isTwoHanded())
		{
			int dir = (_unit->getDirection() + 2)%8;
			itemA = _itemSurfaceA->getFrame(_itemA->getRules()->getHandSprite() + dir);
			itemA->setX(offX[_unit->getDirection()]);
			itemA->setY(offY[_unit->getDirection()]);
		}
		else
		{
			if(_itemA->getSlot()->getId() == "STR_RIGHT_HAND")
			{
			itemA = _itemSurfaceA->getFrame(_itemA->getRules()->getHandSprite() + _unit->getDirection());
			itemA->setX(0);
			itemA->setY(0);
			}
			else
			{
			itemA = _itemSurfaceA->getFrame(_itemA->getRules()->getHandSprite() + _unit->getDirection());
			itemA->setX(offX2[_unit->getDirection()]);
			itemA->setY(offY2[_unit->getDirection()]);
			}
		}
	}
		
	//if we are dual wielding...
	if(_itemB && !_itemB->getRules()->isFixed())
	{
		itemB = _itemSurfaceB->getFrame(_itemB->getRules()->getHandSprite() + _unit->getDirection());
		if (!_itemB->getRules()->isTwoHanded())
		{
			itemB->setX(offX2[_unit->getDirection()]);
			itemB->setY(offY2[_unit->getDirection()]);
		}
		else
		{
			itemB->setX(0);
			itemB->setY(0);
		}

		if (_unit->getStatus() == STATUS_AIMING && _itemB->getRules()->isTwoHanded())
		{
			int dir = (_unit->getDirection() + 2)%8;
			itemB = _itemSurfaceB->getFrame(_itemB->getRules()->getHandSprite() + dir);
			itemB->setX(offX3[_unit->getDirection()]);
			itemB->setY(offY3[_unit->getDirection()]);
		}
	}
		
	switch (_unit->getDirection())
	{
	case 0: itemB?itemB->blit(this):void(); itemA?itemA->blit(this):void(); s->blit(this); break;
	case 1: itemB?itemB->blit(this):void(); s->blit(this); itemA?itemA->blit(this):void(); break;
	case 2: s->blit(this); itemB?itemB->blit(this):void(); itemA?itemA->blit(this):void(); break;
	case 3: s->blit(this); itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void(); break;
	case 4: s->blit(this); itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void(); break;
	case 5: itemA?itemA->blit(this):void(); s->blit(this); itemB?itemB->blit(this):void(); break;
	case 6: itemA?itemA->blit(this):void(); s->blit(this); itemB?itemB->blit(this):void(); break;
	case 7: itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void(); s->blit(this); break;
	}
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
		s = _unitSurface->getFrame( 32 + (_unit->getDirection() * 16) + (_part * 4) + ((_unit->getWalkingPhase() / 2) % 4));
	}
	else
	{
		s = _unitSurface->getFrame((_part * 8) + _unit->getDirection());
	}

	s->blit(this);
}

/**
 * Drawing routine for snakemen
 */
void UnitSprite::drawRoutine6()
{
	Surface *torso = 0, *legs = 0, *leftArm = 0, *rightArm = 0, *itemA = 0, *itemB = 0;
	// magic numbers
	const int Torso = 24, legsStand = 16, die = 96;
	const int larmStand = 0, rarmStand = 8, rarm1H = 99, larm2H = 107, rarm2H = 115, rarmShoot = 123;
	const int legsWalk[8] = { 32, 40, 48, 56, 64, 72, 80, 88 };
	const int yoffWalk[8] = {3, 3, 2, 1, 0, 0, 1, 2}; // bobbing up and down
	const int xoffWalka[8] = {0, 0, 1, 2, 3, 3, 2, 1};
	const int xoffWalkb[8] = {0, 0, -1, -2, -3, -3, -2, -1};
	const int offX[8] = { 8, 10, 8, 5, -8, -10, -5, -2 }; // for the weapons
	const int offY[8] = { -6, -3, 0, -3, 0, -4, -7, -9 }; // for the weapons
	const int offX2[8] = { -8, 2, 7, 13, 7, 0, -3, -15 }; // for the weapons
	const int offY2[8] = { 1, -4, -2, 0, 3, 3, 5, 0 }; // for the weapons
	const int offX3[8] = { 0, 6, 6, 12, -4, -5, -5, -13 }; // for the left handed rifles
	const int offY3[8] = { -4, -4, -1, 0, 5, 0, 1, 0 }; // for the left handed rifles

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

		torso = _unitSurface->getFrame(Torso + _unit->getDirection());
		leftArm = _unitSurface->getFrame(larmStand + _unit->getDirection());
		rightArm = _unitSurface->getFrame(rarmStand + _unit->getDirection());
	

	// when walking, torso(fixed sprite) has to be animated up/down
	if (_unit->getStatus() == STATUS_WALKING)
	{
		int xoffWalk = 0;
		if(_unit->getDirection() < 3)
			xoffWalk = xoffWalka[_unit->getWalkingPhase()];
		if(_unit->getDirection() < 7 && _unit->getDirection() > 3)
			xoffWalk = xoffWalkb[_unit->getWalkingPhase()];
		torso->setY(yoffWalk[_unit->getWalkingPhase()]);
		torso->setX(xoffWalk);
		legs = _unitSurface->getFrame(legsWalk[_unit->getDirection()] + _unit->getWalkingPhase());
		rightArm->setY(yoffWalk[_unit->getWalkingPhase()]);
		leftArm->setY(yoffWalk[_unit->getWalkingPhase()]);
		rightArm->setX(xoffWalk);
		leftArm->setX(xoffWalk);
	}
	else
	{
		legs = _unitSurface->getFrame(legsStand + _unit->getDirection());
	}

	sortRifles();

	// holding an item
	if (_itemA)
	{
		// draw handob item
		if (_unit->getStatus() == STATUS_AIMING && _itemA->getRules()->isTwoHanded())
		{
			int dir = (_unit->getDirection() + 2)%8;
			itemA = _itemSurfaceA->getFrame(_itemA->getRules()->getHandSprite() + dir);
			itemA->setX(offX[_unit->getDirection()]);
			itemA->setY(offY[_unit->getDirection()]);
		}
		else
		{
			itemA = _itemSurfaceB->getFrame(_itemA->getRules()->getHandSprite() + _unit->getDirection());
			itemA->setX(0);
			itemA->setY(0);
		}


		// draw arms holding the item
		if (_itemA->getRules()->isTwoHanded())
		{
			leftArm = _unitSurface->getFrame(larm2H + _unit->getDirection());
			if (_unit->getStatus() == STATUS_AIMING)
			{
				rightArm = _unitSurface->getFrame(rarmShoot + _unit->getDirection());
			}
			else
			{
				rightArm = _unitSurface->getFrame(rarm2H + _unit->getDirection());
			}
		}
		else
		{
			rightArm = _unitSurface->getFrame(rarm1H + _unit->getDirection());
		}
		

		// the fixed arm(s) have to be animated up/down when walking
		if (_unit->getStatus() == STATUS_WALKING)
		{
			itemA->setY(yoffWalk[_unit->getWalkingPhase()]);
			rightArm->setY(yoffWalk[_unit->getWalkingPhase()]);
			if (_itemA->getRules()->isTwoHanded())
				leftArm->setY(yoffWalk[_unit->getWalkingPhase()]);
		}
	}
	//if we are left handed or dual wielding...
	if(_itemB)
	{
		leftArm = _unitSurface->getFrame(larm2H + _unit->getDirection());
		itemB = _itemSurfaceB->getFrame(_itemB->getRules()->getHandSprite() + _unit->getDirection());
		if (!_itemB->getRules()->isTwoHanded())
		{
			itemB->setX(offX2[_unit->getDirection()]);
			itemB->setY(offY2[_unit->getDirection()]);
		}
		else
		{
			itemB->setX(0);
			itemB->setY(0);
			rightArm = _unitSurface->getFrame(rarm2H + _unit->getDirection());
		}

		if (_unit->getStatus() == STATUS_AIMING && _itemB->getRules()->isTwoHanded())
		{
			int dir = (_unit->getDirection() + 2)%8;
			itemB = _itemSurfaceB->getFrame(_itemB->getRules()->getHandSprite() + dir);
			itemB->setX(offX3[_unit->getDirection()]);
			itemB->setY(offY3[_unit->getDirection()]);
			rightArm = _unitSurface->getFrame(rarmShoot + _unit->getDirection());
		}

		if (_unit->getStatus() == STATUS_WALKING)
		{
			leftArm->setY(yoffWalk[_unit->getWalkingPhase()]);
			itemB->setY(offY2[_unit->getDirection()] + yoffWalk[_unit->getWalkingPhase()]);
			if (_itemB->getRules()->isTwoHanded())
				rightArm->setY(yoffWalk[_unit->getWalkingPhase()]);
		}
	}
	// offset everything but legs when kneeled
	if (_unit->getStatus() != STATUS_WALKING)
	{
		leftArm->setY(0);
		rightArm->setY(0);
		torso->setY(0);
	}
	
	// blit order depends on unit direction.
	switch (_unit->getDirection())
	{
	case 0: itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void(); leftArm->blit(this); legs->blit(this); torso->blit(this); rightArm->blit(this); break;
	case 1: leftArm->blit(this); legs->blit(this); itemB?itemB->blit(this):void(); torso->blit(this); itemA?itemA->blit(this):void(); rightArm->blit(this); break;
	case 2: leftArm->blit(this); legs->blit(this); torso->blit(this); rightArm->blit(this); itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void(); break;
	case 3: legs->blit(this); torso->blit(this); leftArm->blit(this); rightArm->blit(this); itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void();	break;
	case 4:	rightArm->blit(this); legs->blit(this); torso->blit(this); leftArm->blit(this); itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void(); break;
	case 5:	rightArm->blit(this); legs->blit(this); torso->blit(this); leftArm->blit(this); itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void(); break;
	case 6: rightArm->blit(this); legs->blit(this); itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void(); torso->blit(this); leftArm->blit(this); break;
	case 7:	itemA?itemA->blit(this):void(); itemB?itemB->blit(this):void(); leftArm->blit(this); rightArm->blit(this); legs->blit(this); torso->blit(this); break;
	}
}

/**
 * Drawing routine for chryssalid
 */
void UnitSprite::drawRoutine7()
{

	Surface *torso = 0, *legs = 0, *leftArm = 0, *rightArm = 0;
	// magic numbers
	const int Torso = 24, legsStand = 16, die = 224;
	const int larmStand = 0, rarmStand = 8;
	const int legsWalk[8] = { 48, 48+24, 48+24*2, 48+24*3, 48+24*4, 48+24*5, 48+24*6, 48+24*7 };
	const int larmWalk[8] = { 32, 32+24, 32+24*2, 32+24*3, 32+24*4, 32+24*5, 32+24*6, 32+24*7 };
	const int rarmWalk[8] = { 40, 40+24, 40+24*2, 40+24*3, 40+24*4, 40+24*5, 40+24*6, 40+24*7 };
	const int yoffWalk[8] = {1, 0, -1, 0, 1, 0, -1, 0}; // bobbing up and down

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


	torso = _unitSurface->getFrame(Torso + _unit->getDirection());


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
		
		legs = _unitSurface->getFrame(legsStand + _unit->getDirection());
		leftArm = _unitSurface->getFrame(larmStand + _unit->getDirection());
		rightArm = _unitSurface->getFrame(rarmStand + _unit->getDirection());
		leftArm->setY(0);
		rightArm->setY(0);
		torso->setY(0);
	}

	// blit order depends on unit direction
	switch (_unit->getDirection())
	{
	case 0: leftArm->blit(this); legs->blit(this); torso->blit(this); rightArm->blit(this); break;
	case 1: leftArm->blit(this); legs->blit(this); torso->blit(this); rightArm->blit(this); break;
	case 2: leftArm->blit(this); legs->blit(this); torso->blit(this); rightArm->blit(this); break;
	case 3: legs->blit(this); torso->blit(this); leftArm->blit(this); rightArm->blit(this); break;
	case 4: rightArm->blit(this); legs->blit(this); torso->blit(this); leftArm->blit(this); break;
	case 5: rightArm->blit(this); legs->blit(this); torso->blit(this); leftArm->blit(this); break;
	case 6: rightArm->blit(this); legs->blit(this); torso->blit(this); leftArm->blit(this); break;
	case 7: leftArm->blit(this); rightArm->blit(this); legs->blit(this); torso->blit(this); break;
	}
}
//silacoids
void UnitSprite::drawRoutine8()
{
	Surface *legs = 0;
	// magic numbers
	const int Body = 0, aim = 5, die = 6;
	const int Pulsate[8] = { 0, 1, 2, 3, 4, 3, 2, 1 };
	
	if (_unit->isOut())
	{
		// unit is drawn as an item
		return;
	}

	legs = _unitSurface->getFrame(Body + Pulsate[_animationFrame]);
	_redraw = true;

	if (_unit->getStatus() == STATUS_COLLAPSING)
		legs = _unitSurface->getFrame(die + _unit->getFallingPhase());
	
	if (_unit->getStatus() == STATUS_AIMING)
		legs = _unitSurface->getFrame(aim);

	legs->blit(this);
}
//celatids
void UnitSprite::drawRoutine9()
{
	Surface *torso = 0;
	// magic numbers
	const int Body = 0, die = 25;
	
	if (_unit->isOut())
	{
		// unit is drawn as an item
		return;
	}

	torso = _unitSurface->getFrame(Body + _animationFrame);
	_redraw = true;

	if (_unit->getStatus() == STATUS_COLLAPSING)
		torso = _unitSurface->getFrame(die + _unit->getFallingPhase());

	torso->blit(this);
}

/*
 * in case of 2-handed weapons, determine which ones to display.
 */
void UnitSprite::sortRifles()
{
	if (_itemA && _itemA->getRules()->isTwoHanded())
	{
		if (_itemB && _itemB->getRules()->isTwoHanded())
		{
			if (_unit->getActiveHand() == "STR_LEFT_HAND")
			{
				_itemA = _itemB;
			}
			_itemB = 0;
		}
		else if (_unit->getStatus() != STATUS_AIMING)
		{
			_itemB = 0;
		}
	}
	else if (_itemB && _itemB->getRules()->isTwoHanded())
	{
		if (_unit->getStatus() != STATUS_AIMING)
		{
			_itemA = 0;
		}
	}
}

}
