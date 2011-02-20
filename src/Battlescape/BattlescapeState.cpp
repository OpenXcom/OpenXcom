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

#include <sstream>
#include <string>
#include "Map.h"
#include "BattlescapeState.h"
#include "BattleAction.h"
#include "../Engine/Game.h"
#include "../Engine/Music.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Screen.h"
#include "../Engine/Action.h"
#include "../Resource/ResourcePack.h"
#include "../Interface/Cursor.h"
#include "../Interface/Text.h"
#include "../Interface/Bar.h"
#include "../Interface/ImageButton.h"
#include "../Interface/NumberText.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/BattleItem.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleItem.h"
#include "../Engine/Timer.h"

namespace OpenXcom
{

#define DEFAULT_WALK_SPEED 40
#define DEFAULT_BULLET_SPEED 20
#define DEFAULT_ANIM_SPEED 100

/**
 * Initializes all the elements in the Battlescape screen.
 * @param game Pointer to the core game.
 */
BattlescapeState::BattlescapeState(Game *game) : State(game)
{
	// Create the battlemap view
	_map = new Map(320, 200, 0, 0);

	// Create buttonbar
	_icons = new Surface(320, 200, 0, 0);
	_numLayers = new NumberText(3, 5, 232, 150);
	_rank = new Surface(26,23,107,177);

	// Create buttons
	_btnAbort = new InteractiveSurface(32, 16, 240, 160);
	_btnMapUp = new InteractiveSurface(32, 16, 80, 144);
	_btnMapDown = new InteractiveSurface(32, 16, 80, 160);
	_btnNextSoldier = new InteractiveSurface(32, 16, 176, 144);
	_btnCenter = new InteractiveSurface(32, 16, 145, 160);
	_btnReserveNone = new ImageButton(28, 11, 49, 177);
	_btnReserveSnap = new ImageButton(28, 11, 78, 177);
	_btnReserveAimed = new ImageButton(28, 11, 49, 189);
	_btnReserveAuto = new ImageButton(28, 11, 78, 189);
	_btnLeftHandItem = new ImageButton(32, 48, 8, 149);
	_btnRightHandItem = new ImageButton(32, 48, 280, 149);

	// Create soldier stats summary
	_txtName = new Text(120, 10, 135, 176);

	_numTimeUnits = new NumberText(15, 5, 136, 186);
	_barTimeUnits = new Bar(102, 3, 170, 185);

	_numEnergy = new NumberText(15, 5, 153, 186);
	_barEnergy = new Bar(102, 3, 170, 189);

	_numHealth = new NumberText(15, 5, 136, 194);
	_barHealth= new Bar(102, 3, 170, 193);

	_numMorale = new NumberText(15, 5, 154, 194);
	_barMorale = new Bar(102, 3, 170, 197);

	_reserve = _btnReserveNone;

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_4")->getColors());

	// Last 16 colors are a greyish gradient
	SDL_Color color[] = {{140, 152, 148},
						 {132, 136, 140},
						 {116, 124, 132},
						 {108, 116, 124},
						 {92, 104, 108},
						 {84, 92, 100},
						 {76, 80, 92},
						 {56, 68, 84},
						 {48, 56, 68},
						 {40, 48, 56},
						 {32, 36, 48},
						 {24, 28, 32},
						 {16, 20, 24},
						 {8, 12, 16},
						 {0, 4, 8},
						 {0, 0, 0}};
	_game->setPalette(color, Palette::backPos+16, 16);

	// Fix cursor
	_game->getCursor()->setColor(Palette::blockOffset(9));

	add(_map);
	add(_icons);
	add(_numLayers);
	add(_rank);
	add(_btnAbort);
	add(_btnMapUp);
	add(_btnMapDown);
	add(_btnNextSoldier);
	add(_btnCenter);
	add(_txtName);
	add(_numTimeUnits);
	add(_numEnergy);
	add(_numHealth);
	add(_numMorale);
	add(_barTimeUnits);
	add(_barEnergy);
	add(_barHealth);
	add(_barMorale);
	add(_btnReserveNone);
	add(_btnReserveSnap);
	add(_btnReserveAimed);
	add(_btnReserveAuto);
	add(_btnLeftHandItem);
	add(_btnRightHandItem);

	// Set up objects
	_game->getResourcePack()->getSurface("ICONS.PCK")->blit(_icons);
	
	_battleGame = _game->getSavedGame()->getBattleGame();
	_map->setResourcePack(_game->getResourcePack());
	_map->setSavedGame(_battleGame, _game);
	_map->init();
	_map->onMouseClick((ActionHandler)&BattlescapeState::mapClick);

	_numLayers->setColor(Palette::blockOffset(1)-2);
	_numLayers->setValue(1);

	_btnAbort->onMouseClick((ActionHandler)&BattlescapeState::btnAbortClick);
	_btnMapUp->onMouseClick((ActionHandler)&BattlescapeState::btnMapUpClick);
	_btnMapDown->onMouseClick((ActionHandler)&BattlescapeState::btnMapDownClick);
	_btnNextSoldier->onMouseClick((ActionHandler)&BattlescapeState::btnNextSoldierClick);
	_btnCenter->onMouseClick((ActionHandler)&BattlescapeState::btnCenterClick);
	_btnLeftHandItem->onMouseClick((ActionHandler)&BattlescapeState::btnLeftHandItemClick);
	_btnRightHandItem->onMouseClick((ActionHandler)&BattlescapeState::btnRightHandItemClick);
	
	_txtName->setColor(Palette::blockOffset(13));
	_numTimeUnits->setColor(Palette::blockOffset(4));
	_numEnergy->setColor(Palette::blockOffset(1));
	_numHealth->setColor(Palette::blockOffset(2));
	_numMorale->setColor(Palette::blockOffset(12));
	_barTimeUnits->setColor(Palette::blockOffset(4));
	_barTimeUnits->setScale(1.0);
	_barEnergy->setColor(Palette::blockOffset(1));
	_barEnergy->setScale(1.0);
	_barHealth->setColor(Palette::blockOffset(2));
	_barHealth->setScale(1.0);
	_barMorale->setColor(Palette::blockOffset(12));
	_barMorale->setScale(1.0);

	updateSoldierInfo(_battleGame->getSelectedUnit());
	_map->centerOnPosition(_battleGame->getSelectedUnit()->getPosition());

	_btnReserveNone->copy(_icons);
	_btnReserveNone->setColor(Palette::blockOffset(4)+6);
	_btnReserveNone->setGroup(&_reserve);

	_btnReserveSnap->copy(_icons);
	_btnReserveSnap->setColor(Palette::blockOffset(2)+6);
	_btnReserveSnap->setGroup(&_reserve);

	_btnReserveAimed->copy(_icons);
	_btnReserveAimed->setColor(Palette::blockOffset(2)+6);
	_btnReserveAimed->setGroup(&_reserve);

	_btnReserveAuto->copy(_icons);
	_btnReserveAuto->setColor(Palette::blockOffset(2)+6);
	_btnReserveAuto->setGroup(&_reserve);

	// Set music
	_game->getResourcePack()->getMusic("GMTACTIC")->play();

	_walkingTimer = new Timer(DEFAULT_WALK_SPEED);
	_walkingTimer->onTimer((StateHandler)&BattlescapeState::moveUnit);
	_walkingTimer->start();

	_bulletTimer = new Timer(DEFAULT_BULLET_SPEED);
	_bulletTimer->onTimer((StateHandler)&BattlescapeState::moveBullet);
	_bulletTimer->start();

	_animTimer = new Timer(DEFAULT_ANIM_SPEED);
	_animTimer->onTimer((StateHandler)&BattlescapeState::animate);
	_animTimer->start();

	_action = 0;
}

/**
 *
 */
BattlescapeState::~BattlescapeState()
{
	delete _walkingTimer;
	delete _bulletTimer;
	delete _animTimer;
	delete _action;
}


void BattlescapeState::init()
{
	_map->focus();
	_map->cacheUnits();
	_map->draw();
}

/**
  * think
  */
void BattlescapeState::think()
{
	_walkingTimer->think(this, 0);
	_bulletTimer->think(this, 0);
	_animTimer->think(this, 0);
	_map->think();
}

/**
 * Processes any clicks on the map to
 * command units.
 * @param action Pointer to an action.
 */
void BattlescapeState::mapClick(Action *action)
{
	// right-click abort 
	
	// right-click abort walking or in progress action
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		if (_action)
		{
			_action->cancel();
			checkActionFinished();
			return;
		}
	}

	// don't handle mouseclicks below 140, because they are in the buttons area (it overlaps with map surface)
	if (action->getDetails()->motion.y/action->getYScale() > BUTTONS_AREA) return;

	// don't accept leftclicks if there is no cursor
	if (_map->getCursorType() == CT_NONE) return;

	Position pos;
	_map->getSelectorPosition(&pos);

	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		// if we clicked a unit, we select this unit
		BattleUnit *unit = _battleGame->selectUnit(pos);
		if (unit)
		{
			_battleGame->setSelectedUnit(unit);
			updateSoldierInfo(unit);
			return;
		}
		// if we clicked somewhere else we start a walk action
		if (!_action)
		{
			_action = new BattleAction(_battleGame, _game->getResourcePack(), 0, WALK);
			_action->setTarget(pos);
			_action->start();
			_map->setCursorType(CT_NONE);
			checkActionFinished(); // could be the action goes into error immediatly
		}
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		if (!_action)
		{
			_action = new BattleAction(_battleGame, _game->getResourcePack(), 0, TURN);
			_action->setTarget(pos);
			_action->start();
			_map->cacheUnits();
			_map->draw();
			checkActionFinished(); // could be the action goes into error immediatly
		}
	}
	
}

/**
 * Move unit up.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnUnitUpClick(Action *action)
{
	/*Pathfinding *pf = _battleGame->getPathfinding();
	Position start = _battleGame->getSelectedUnit()->getPosition();
	Position end = start + Position(0, 0, +1);
	pf->calculate(_battleGame->getSelectedUnit(), end);*/
}

/**
 * Move unit down.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnUnitDownClick(Action *action)
{
	/*Pathfinding *pf = _battleGame->getPathfinding();
	Position start = _battleGame->getSelectedUnit()->getPosition();
	Position end = start + Position(0, 0, -1);
	pf->calculate((BattleUnit*)_battleGame->getSelectedUnit(), end);*/
}

/**
 * Show next map layer.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnMapUpClick(Action *action)
{
	_map->up();
}

/**
 * Show previous map layer.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnMapDownClick(Action *action)
{
	_map->down();
}

/**
 * Show minimap.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnShowMapClick(Action *action)
{}

/**
 * Kneel/Standup.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnKneelClick(Action *action)
{}

/**
 * Go to soldier info screen.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnSoldierClick(Action *action)
{}

/**
 * Center on currently selected soldier.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnCenterClick(Action *action)
{
	_map->centerOnPosition(_battleGame->getSelectedUnit()->getPosition());
}

/**
 * Select next soldier.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnNextSoldierClick(Action *action)
{
	BattleUnit *unit = _battleGame->selectNextPlayerUnit();
	updateSoldierInfo(unit);
	_map->centerOnPosition(unit->getPosition());
}

/**
 * Don't select current soldier and select next soldier.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnNextStopClick(Action *action)
{}

/**
 * Show/hide all map layers.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnShowLayersClick(Action *action)
{}

/**
 * Show options.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnHelpClick(Action *action)
{}

/**
 * End turn.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnEndTurnClick(Action *action)
{}

/**
 * Abort game.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnAbortClick(Action *action)
{
	_game->getSavedGame()->endBattle();
	_game->getCursor()->setColor(Palette::blockOffset(15) + 12);
	_game->popState();
}

/**
 * Shows action popup menu. When clicked, create the action.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnLeftHandItemClick(Action *action)
{
	BattleItem *leftHandItem = _battleGame->getItemFromUnit(_battleGame->getSelectedUnit(), LEFT_HAND);
	handleItemClick(leftHandItem);
}

/**
 * Shows action popup menu. When clicked, create the action.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnRightHandItemClick(Action *action)
{
	BattleItem *rightHandItem = _battleGame->getItemFromUnit(_battleGame->getSelectedUnit(), RIGHT_HAND);
	handleItemClick(rightHandItem);	
}

/**
 * Updates soldier name/rank/tu/energy/health/morale.
 * @param unit Pointer to current unit.
 */
void BattlescapeState::updateSoldierInfo(BattleUnit *unit)
{
	_txtName->setText(unit->getName());
	if (unit->getRankSprite())
	{
		SurfaceSet *texture = _game->getResourcePack()->getSurfaceSet("BASEBITS.PCK");
		texture->getFrame(unit->getRankSprite())->blit(_rank);
	}
	_numTimeUnits->setValue(unit->getMaxTimeUnits());
	_barTimeUnits->setMax(unit->getMaxTimeUnits());
	_barTimeUnits->setValue(unit->getMaxTimeUnits());
	_numEnergy->setValue(unit->getMaxStamina());
	_barEnergy->setMax(unit->getMaxStamina());
	_barEnergy->setValue(unit->getMaxStamina());
	_numHealth->setValue(unit->getMaxHealth());
	_barHealth->setMax(unit->getMaxHealth());
	_barHealth->setValue(unit->getMaxHealth());
	_numMorale->setValue(100);
	_barMorale->setMax(100);
	_barMorale->setValue(100);

	BattleItem *leftHandItem = _battleGame->getItemFromUnit(unit, LEFT_HAND);
	_btnLeftHandItem->clear();
	if (leftHandItem)
	{
		drawItemSprite(leftHandItem, _btnLeftHandItem);
	}
	BattleItem *rightHandItem = _battleGame->getItemFromUnit(unit, RIGHT_HAND);
	_btnRightHandItem->clear();
	if (rightHandItem)
	{
		drawItemSprite(rightHandItem, _btnRightHandItem);
	}
}

/*
 * This function popups a context sensitive list of actions the user can choose from.
 * Some actions result in a change of gamestate. Some result in a battlescape action.
 * @param item Item the user clicked on (righthand/lefthand)
 */
void BattlescapeState::handleItemClick(BattleItem *item)
{
	// TODO popup menu	

	// TODO other gamestates: scanner/medikit

	if (item && !_action)
	{
		_action = new BattleAction(_battleGame, _game->getResourcePack(), item, SNAP_SHOT);
		_map->setCursorType(CT_AIM);
		checkActionFinished(); // could be the action goes into error immediatly
	}
}

/**
 * This function checks if the current actions finished, displaying an error if any.
 */
void BattlescapeState::checkActionFinished()
{
	if (_action->getStatus() == FINISHED)
	{
		_map->setCursorType(CT_NORMAL);
		_game->getCursor()->setVisible(true);
		// TODO handle result

		delete _action;
		_action = 0;
	}
}

/**
 * Draws the item's sprite on a surface.
 * @param item the given item
 * @surface surface the given surface
 */
void BattlescapeState::drawItemSprite(BattleItem *item, Surface *surface)
{
	SurfaceSet *texture = _game->getResourcePack()->getSurfaceSet("BIGOBS.PCK");
	texture->getFrame(item->getRules()->getBigSprite())->blit(surface);
}

/**
 * Animate walking unit.
 */
void BattlescapeState::moveUnit()
{
	if (_action && (_action->getType() == WALK || _action->getType() == TURN))
	{
		_action->moveUnit();
		// if the unit changed level, camera changes level with
		_map->setViewHeight(_battleGame->getSelectedUnit()->getPosition().z);
		checkActionFinished();
		_map->cacheUnits();
		_map->draw();
	}
}


/**
 * Animate flying bullet.
 */
void BattlescapeState::moveBullet()
{
	if (_action)
	{
		_action->moveBullet();
		checkActionFinished();
	}
}

/**
 * Animate other stuff.
 */
void BattlescapeState::animate()
{
	if (_action)
	{
		_action->animate();
		checkActionFinished();
	}
	_map->animate();
}

}
