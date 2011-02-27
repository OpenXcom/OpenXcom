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
#include "BattleState.h"
#include "UnitTurnBState.h"
#include "UnitWalkBState.h"
#include "ProjectileFlyBState.h"
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
#include "../Interface/Window.h"
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

	_stateTimer = new Timer(DEFAULT_ANIM_SPEED);
	_stateTimer->onTimer((StateHandler)&BattlescapeState::handleState);
	_stateTimer->start();

	_animTimer = new Timer(DEFAULT_ANIM_SPEED);
	_animTimer->onTimer((StateHandler)&BattlescapeState::animate);
	_animTimer->start();

}

/**
 *
 */
BattlescapeState::~BattlescapeState()
{
	delete _stateTimer;
	delete _animTimer;
}


void BattlescapeState::init()
{
	_map->focus();
	_map->cacheUnits();
	_map->draw(true);
	_targeting = false;
}

/**
  * think
  */
void BattlescapeState::think()
{
	_stateTimer->think(this, 0);
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
	// right-click aborts walking state
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		if (!_states.empty())
		{
			_states.front()->cancel();
			return;
		}
	}

	// don't handle mouseclicks below 140, because they are in the buttons area (it overlaps with map surface)
	if (action->getYMouse() / action->getYScale() > BUTTONS_AREA) return;

	// don't accept leftclicks if there is no cursor or there is an action busy
	if (_map->getCursorType() == CT_NONE || !_states.empty()) return;

	Position pos;
	_map->getSelectorPosition(&pos);

	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		if (_targeting)
		{
			//  -= fire weapon =-
			_targeting = false;
			_target = pos;
			handlePlayerAction(new ProjectileFlyBState(this));
		}
		else
		{
			BattleUnit *unit = _battleGame->selectUnit(pos);
			if (unit)
			{
			//  -= select unit =-
				_battleGame->setSelectedUnit(unit);
				updateSoldierInfo(unit);
			}
			else
			{
			//  -= start walking =-
				_selectedAction = BA_WALK;
				_target = pos;
				handlePlayerAction(new UnitWalkBState(this));
			}
		}
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		//  -= turn to or open door =-
		_selectedAction = BA_TURN;
		_target = pos;
		handlePlayerAction(new UnitTurnBState(this));
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
 * Some actions result in a change of gamestate.
 * @param item Item the user clicked on (righthand/lefthand)
 */
void BattlescapeState::handleItemClick(BattleItem *item)
{
	// make sure there is an item, and the battlescape is in an idle state
	if (item && _states.empty())
	{
		// TODO popup menu	

		// TODO other gamestates: scanner/medikit
		// this should be returned by the popup menu, but is now hardcoded to test without popup menu
		_selectedAction = BA_SNAPSHOT;
		_selectedItem = item;
		_map->setCursorType(CT_AIM);
		_targeting = true;	
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
 * Give time slice to the front state.
 */
void BattlescapeState::handleState()
{
	if (!_states.empty())
	{
		if (!_states.front()->think())
		{
			// the state is dead - clean it up
			_states.pop_front();
			// if all states are empty - give the mouse back to the player
			if (_states.empty())
			{
				_map->setCursorType(CT_NORMAL);
				_game->getCursor()->setVisible(true);
			}
			else
			{
				// init the next state in queue
				_states.front()->init();
			}
		}
		_map->draw(true); // redraw map
	}
}

void BattlescapeState::handlePlayerAction(BattleState* state)
{
	if (state->init())
	{
		_map->setCursorType(CT_NONE);
		_game->getCursor()->setVisible(false);
		statePushFront(state);
	}
	else
	{
		if (_states.empty())
		{
			_map->setCursorType(CT_NORMAL);
			_game->getCursor()->setVisible(true);
		}
	}
}

/**
 * Animate other stuff on the map.
 */
void BattlescapeState::animate()
{
	_map->animate();
}

/// Get target.
Position BattlescapeState::getTarget() const
{
	return _target;
}

/// Get game.
Game *BattlescapeState::getGame() const
{
	return _game;
}

/// Get map.
Map *BattlescapeState::getMap() const
{
	return _map;
}

void BattlescapeState::statePushFront(BattleState *bs)
{
	_states.push_front(bs);
	bs->init();
}

void BattlescapeState::statePushNext(BattleState *bs)
{
	_states.insert(++_states.begin(), bs);
}


void BattlescapeState::statePushBack(BattleState *bs)
{
	_states.push_back(bs);
}

void BattlescapeState::setStateInterval(Uint32 interval)
{
	_stateTimer->setInterval(interval);
}

BattleItem *BattlescapeState::getSelectedItem() const
{
	return _selectedItem;
}

}
