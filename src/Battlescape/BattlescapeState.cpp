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
#include <cmath>
#include "ActionMenuItem.h"
#include "Map.h"
#include "BattlescapeState.h"
#include "BattleState.h"
#include "UnitTurnBState.h"
#include "UnitWalkBState.h"
#include "ProjectileFlyBState.h"
#include "TerrainModifier.h"
#include "../Engine/Game.h"
#include "../Engine/Music.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Screen.h"
#include "../Engine/SoundSet.h"
#include "../Engine/Sound.h"
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
#include "../Interface/FpsCounter.h"


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
	_btnEndTurn = new InteractiveSurface(32, 16, 240, 144);
	_btnMapUp = new InteractiveSurface(32, 16, 80, 144);
	_btnMapDown = new InteractiveSurface(32, 16, 80, 160);
	_btnNextSoldier = new InteractiveSurface(32, 16, 176, 144);
	_btnCenter = new InteractiveSurface(32, 16, 145, 160);
	_btnReserveNone = new ImageButton(28, 11, 49, 177);
	_btnReserveSnap = new ImageButton(28, 11, 78, 177);
	_btnReserveAimed = new ImageButton(28, 11, 49, 189);
	_btnReserveAuto = new ImageButton(28, 11, 78, 189);
	_btnLeftHandItem = new InteractiveSurface(32, 48, 8, 149);
	_numAmmoLeft = new NumberText(3, 5, 8, 149);
	_btnRightHandItem = new InteractiveSurface(32, 48, 280, 149);
	_numAmmoRight = new NumberText(3, 5, 280, 149);
	_btnKneel = new InteractiveSurface(32, 16, 113, 160);
	for (int i = 0; i < 10; i++)
	{
		_btnVisibleUnit[i] = new InteractiveSurface(15, 12, 300, 128 - (i * 13));
		_numVisibleUnit[i] = new NumberText(15, 12, 306, 132 - (i * 13));
	}
	_numVisibleUnit[9]->setX(304); // center number 10
	_warningMessageBackground = new Surface(224, 24, 48, 176);
	_txtWarningMessage = new Text(224, 24, 48, 184);
	for (int i = 0; i < 5; i++)
	{
		_actionMenu[i] = new ActionMenuItem(this, i, _game->getResourcePack()->getFont("BIGLETS.DAT"));
	}

	// Create soldier stats summary
	_txtName = new Text(120, 10, 135, 176);

	_numTimeUnits = new NumberText(15, 5, 136, 186);
	_barTimeUnits = new Bar(102, 3, 170, 185);

	_numEnergy = new NumberText(15, 5, 154, 186);
	_barEnergy = new Bar(102, 3, 170, 189);

	_numHealth = new NumberText(15, 5, 136, 194);
	_barHealth= new Bar(102, 3, 170, 193);

	_numMorale = new NumberText(15, 5, 154, 194);
	_barMorale = new Bar(102, 3, 170, 197);

	_txtDebug = new Text(300, 10, 20, 0);

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

	// Fix the FpsCounter color.
	_game->getFpsCounter()->setColor(Palette::blockOffset(9));

	add(_map);
	add(_icons);
	add(_numLayers);
	add(_rank);
	add(_btnAbort);
	add(_btnEndTurn);
	add(_btnMapUp);
	add(_btnMapDown);
	add(_btnNextSoldier);
	add(_btnCenter);
	add(_btnKneel);
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
	add(_numAmmoLeft);
	add(_btnRightHandItem);
	add(_numAmmoRight);
	for (int i = 0; i < 10; i++)
	{
		add(_btnVisibleUnit[i]);
		add(_numVisibleUnit[i]);
	}
	add(_warningMessageBackground);
	add(_txtWarningMessage);
	for (int i = 0; i < 5; i++)
	{
		add(_actionMenu[i]);
	}

	add(_txtDebug);
	// Set up objects
	_game->getResourcePack()->getSurface("ICONS.PCK")->blit(_icons);

	_battleGame = _game->getSavedGame()->getBattleGame();
	_map->setResourcePack(_game->getResourcePack());
	_map->setSavedGame(_battleGame, _game);
	_map->init();
	_map->onMouseClick((ActionHandler)&BattlescapeState::mapClick);

	_numLayers->setColor(Palette::blockOffset(1)-2);
	_numLayers->setValue(1);

	_numAmmoLeft->setColor(2);
	_numAmmoLeft->setValue(999);

	_numAmmoRight->setColor(2);
	_numAmmoRight->setValue(999);

	_btnAbort->onMouseClick((ActionHandler)&BattlescapeState::btnAbortClick);
	_btnEndTurn->onMouseClick((ActionHandler)&BattlescapeState::btnEndTurnClick);
	_btnMapUp->onMouseClick((ActionHandler)&BattlescapeState::btnMapUpClick);
	_btnMapDown->onMouseClick((ActionHandler)&BattlescapeState::btnMapDownClick);
	_btnNextSoldier->onMouseClick((ActionHandler)&BattlescapeState::btnNextSoldierClick);
	_btnCenter->onMouseClick((ActionHandler)&BattlescapeState::btnCenterClick);
	_btnKneel->onMouseClick((ActionHandler)&BattlescapeState::btnKneelClick);
	_btnLeftHandItem->onMouseClick((ActionHandler)&BattlescapeState::btnLeftHandItemClick);
	_btnRightHandItem->onMouseClick((ActionHandler)&BattlescapeState::btnRightHandItemClick);
	for (int i = 0; i < 10; i++)
	{
		_btnVisibleUnit[i]->onMouseClick((ActionHandler)&BattlescapeState::btnVisibleUnitClick);
		_numVisibleUnit[i]->setColor(16);
		_numVisibleUnit[i]->setValue(i+1);
	}
	_txtWarningMessage->setColor(Palette::blockOffset(2)-1);
	_txtWarningMessage->setHighContrast(true);
	_txtWarningMessage->setAlign(ALIGN_CENTER);
	_warningMessageBackground->setVisible(false);
	for (int i = 0; i < 5; i++)
	{
		_actionMenu[i]->setVisible(false);
		_actionMenu[i]->onMouseClick((ActionHandler)&BattlescapeState::btnActionMenuItemClick);
	}

	_txtName->setColor(Palette::blockOffset(8)-1);
	_txtName->setHighContrast(true);
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

	_txtDebug->setColor(Palette::blockOffset(8)-1);
	_txtDebug->setHighContrast(true);

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

	_selectedAction = BA_NONE;
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
	_popup = false;
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
  * Shift the red colors of the visible unit buttons backgrounds.
  */
void BattlescapeState::blinkVisibleUnitButtons()
{
	static int delta = 1, color = 32;

	SDL_Rect square1;
	square1.x = 0;
	square1.y = 0;
	square1.w = 15;
	square1.h = 12;
	SDL_Rect square2;
	square2.x = 1;
	square2.y = 1;
	square2.w = 13;
	square2.h = 10;

	for (int i = 0; i < 10;  i++)
	{
		if (_btnVisibleUnit[i]->getVisible() == true)
		{
			_btnVisibleUnit[i]->drawRect(&square1, 15);
			_btnVisibleUnit[i]->drawRect(&square2, color);
		}
	}

	if (color == 44) delta = -2;
	if (color == 32) delta = 1;

	color += delta;
}


/**
  * Shift the red color of the warning message.
  */
void BattlescapeState::blinkWarningMessage()
{
	static int color = 32, delay = 12;

	if (_warningMessageBackground->getVisible() == false)
		return;

	SDL_Rect square1;
	square1.x = 0;
	square1.y = 0;
	square1.w = 224;
	square1.h = 48;
	_warningMessageBackground->drawRect(&square1, color);

	if (color >= 44)
	{
		delay--;
	}
	else
	{
		color++;
	}

	if (delay == 0)
	{
		_warningMessageBackground->setVisible(false);
		_txtWarningMessage->setVisible(false);
		color = 32;
		delay = 12;
	}

}

/**
  * Show warning message.
  * @param message untranslated
  */
void BattlescapeState::showWarningMessage(std::string message)
{
	std::wstring messageText = _game->getLanguage()->getString(message);
	_warningMessageBackground->setVisible(true);
	_txtWarningMessage->setVisible(true);
	_txtWarningMessage->setText(messageText);
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
		if (_popup)
		{
			hidePopup();
			return;
		}
		if (_states.empty())
		{
			if (_targeting)
			{
				_targeting = false;
				_map->setCursorType(CT_NORMAL);
				_game->getCursor()->setVisible(true);
				_selectedAction = BA_NONE;
				return;
			}
		}
		else
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
		if (_targeting && _battleGame->getSelectedUnit())
		{
			//  -= fire weapon =-
			_target = pos;
			_map->setCursorType(CT_NONE);
			_game->getCursor()->setVisible(false);
			statePushBack(new UnitTurnBState(this));
			statePushBack(new ProjectileFlyBState(this));
		}
		else
		{
			BattleUnit *unit = _battleGame->selectUnit(pos);
			if (unit && !unit->isOut())
			{
			//  -= select unit =-
				if (unit->getFaction() == _battleGame->getSide())
				{
					_battleGame->setSelectedUnit(unit);
					updateSoldierInfo(unit);
				}
			}
			else if (_battleGame->getSelectedUnit())
			{
			//  -= start walking =-
				_target = pos;
				_map->setCursorType(CT_NONE);
				_game->getCursor()->setVisible(false);
				statePushBack(new UnitWalkBState(this));
			}
		}
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT && _battleGame->getSelectedUnit())
	{
		//  -= turn to or open door =-
		_target = pos;
		statePushBack(new UnitTurnBState(this));
	}

}

/**
 * Move unit up.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnUnitUpClick(Action *action)
{
	if (_popup) return;
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
	if (_popup) return;
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
	if (_popup) return;
	_map->up();
}

/**
 * Show previous map layer.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnMapDownClick(Action *action)
{
	if (_popup) return;
	_map->down();
}

/**
 * Show minimap.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnShowMapClick(Action *action)
{
	if (_popup) return;
}

/**
 * Kneel/Standup.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnKneelClick(Action *action)
{
	if (_popup) return;
	// TODO: check for timeunits... check for FOV...
	BattleUnit *bu = _battleGame->getSelectedUnit();
	if (bu)
	{
		if (bu->spendTimeUnits(bu->isKneeled()?8:4, _battleGame->getDebugMode()))
		{
			bu->kneel(!bu->isKneeled());
			_map->cacheUnits();
			updateSoldierInfo(bu);
		}
	}
}

/**
 * Go to soldier info screen.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnSoldierClick(Action *action)
{
	if (_popup) return;
}

/**
 * Center on currently selected soldier.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnCenterClick(Action *action)
{
	if (_popup) return;
	if (_battleGame->getSelectedUnit())
	{
		_map->centerOnPosition(_battleGame->getSelectedUnit()->getPosition());
	}
}

/**
 * Select next soldier.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnNextSoldierClick(Action *action)
{
	if (_popup) return;
	BattleUnit *unit = _battleGame->selectNextPlayerUnit();
	updateSoldierInfo(unit);
	if (unit) _map->centerOnPosition(unit->getPosition());
}

/**
 * Don't select current soldier and select next soldier.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnNextStopClick(Action *action)
{
	if (_popup) return;
}

/**
 * Show/hide all map layers.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnShowLayersClick(Action *action)
{
	if (_popup) return;
}

/**
 * Show options.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnHelpClick(Action *action)
{
	if (_popup) return;
}

/**
 * End turn.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnEndTurnClick(Action *action)
{
	if (_popup) return;

	if (_battleGame->getTerrainModifier()->closeUfoDoors())
	{
		_game->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(21)->play(); // ufo door closed
	}

	_battleGame->endTurn();
	updateSoldierInfo(_battleGame->getSelectedUnit());
	if (_battleGame->getSelectedUnit())
	{
		_map->centerOnPosition(_battleGame->getSelectedUnit()->getPosition());
	}

	if (_battleGame->getSide() == FACTION_HOSTILE)
	{
		// do AI stuff

		if (!_battleGame->getDebugMode())
		{
			btnEndTurnClick(action);
		}
	}

}

/**
 * Abort game.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnAbortClick(Action *action)
{
	if (_popup) return;
	_game->getSavedGame()->endBattle();
	_game->getCursor()->setColor(Palette::blockOffset(15)+12);
	_game->getFpsCounter()->setColor(Palette::blockOffset(15)+12);
	_game->popState();
}

/**
 * Shows action popup menu. When clicked, create the action.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnLeftHandItemClick(Action *action)
{
	if (_popup)
	{
		hidePopup();
		return;
	}
	if (_selectedAction != BA_NONE) return;
	if (_battleGame->getSelectedUnit())
	{
		BattleItem *leftHandItem = _battleGame->getItemFromUnit(_battleGame->getSelectedUnit(), LEFT_HAND);
		handleItemClick(leftHandItem);
	}
}

/**
 * Shows action popup menu. When clicked, create the action.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnRightHandItemClick(Action *action)
{
	if (_popup)
	{
		hidePopup();
		return;
	}
	if (_selectedAction != BA_NONE) return;
	if (_battleGame->getSelectedUnit())
	{
		BattleItem *rightHandItem = _battleGame->getItemFromUnit(_battleGame->getSelectedUnit(), RIGHT_HAND);
		handleItemClick(rightHandItem);
	}
}

/**
 * Center on the unit corresponding with this button.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnVisibleUnitClick(Action *action)
{
	if (_popup) return;

	int btnID = -1;

	popState();

	// got to find out which button was pressed
	for (int i = 0; i < 10 && btnID == -1; i++)
	{
		if (action->getSender() == _btnVisibleUnit[i])
		{
			btnID = i;
		}
	}

	if (btnID != -1)
	{
		_map->centerOnPosition(_visibleUnit[btnID]->getPosition());
	}
}

/**
 * Execute the action corresponding with this action menu item.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnActionMenuItemClick(Action *action)
{
	int btnID = -1;

	// got to find out which button was pressed
	for (int i = 0; i < 10 && btnID == -1; i++)
	{
		if (action->getSender() == _actionMenu[i])
		{
			btnID = i;
		}
	}

	if (btnID != -1)
	{
		_selectedAction = _actionMenu[btnID]->getAction();
		if (_selectedAction == BA_AUTOSHOT)
		{
			_autoShot = 1;
		}
		_map->setCursorType(CT_AIM);
		_targeting = true;
		hidePopup();
	}
}

/**
 * Hide the popup action menu.
 */
void BattlescapeState::hidePopup()
{
	for (int i=0; i < 5; i++)
		_actionMenu[i]->setVisible(false);
	_popup = false;
	if (_selectedAction == BA_NONE)
	{
		_map->setCursorType(CT_NORMAL);
	}
	else
	{
		_map->setCursorType(CT_AIM);
	}
}

/**
 * Updates soldier name/rank/tu/energy/health/morale.
 * @param battleUnit Pointer to current unit.
 */
void BattlescapeState::updateSoldierInfo(BattleUnit *battleUnit)
{

	for (int i = 0; i < 10; i++)
	{
		_btnVisibleUnit[i]->hide();
		_numVisibleUnit[i]->hide();
		_visibleUnit[i] = 0;
	}

	if (battleUnit == 0)
	{
		_txtName->setText(L"");
		_rank->clear();
		_numTimeUnits->clear();
		_barTimeUnits->clear();
		_barTimeUnits->clear();
		_numEnergy->clear();
		_barEnergy->clear();
		_barEnergy->clear();
		_numHealth->clear();
		_barHealth->clear();
		_barHealth->clear();
		_numMorale->clear();
		_barMorale->clear();
		_barMorale->clear();
		_btnLeftHandItem->clear();
		_btnRightHandItem->clear();
		return;
	}

	_txtName->setText(battleUnit->getUnit()->getName());
	Soldier *soldier = dynamic_cast<Soldier*>(battleUnit->getUnit());
	if (soldier != 0)
	{
		SurfaceSet *texture = _game->getResourcePack()->getSurfaceSet("BASEBITS.PCK");
		texture->getFrame(soldier->getRankSprite())->blit(_rank);
	}
	_numTimeUnits->setValue(battleUnit->getTimeUnits());
	_barTimeUnits->setMax(battleUnit->getUnit()->getTimeUnits());
	_barTimeUnits->setValue(battleUnit->getTimeUnits());
	_numEnergy->setValue(battleUnit->getEnergy());
	_barEnergy->setMax(battleUnit->getUnit()->getStamina());
	_barEnergy->setValue(battleUnit->getEnergy());
	_numHealth->setValue(battleUnit->getHealth());
	_barHealth->setMax(battleUnit->getUnit()->getHealth());
	_barHealth->setValue(battleUnit->getHealth());
	_numMorale->setValue(battleUnit->getMorale());
	_barMorale->setMax(100);
	_barMorale->setValue(battleUnit->getMorale());

	BattleItem *leftHandItem = _battleGame->getItemFromUnit(battleUnit, LEFT_HAND);
	_btnLeftHandItem->clear();
	_numAmmoLeft->clear();
	if (leftHandItem)
	{
		drawItemSprite(leftHandItem, _btnLeftHandItem);
		_numAmmoLeft->setValue(leftHandItem->getAmmoQuantity());
	}
	BattleItem *rightHandItem = _battleGame->getItemFromUnit(battleUnit, RIGHT_HAND);
	_btnRightHandItem->clear();
	_numAmmoRight->clear();
	if (rightHandItem)
	{
		drawItemSprite(rightHandItem, _btnRightHandItem);
		_numAmmoRight->setValue(rightHandItem->getAmmoQuantity());
	}

	_battleGame->getTerrainModifier()->calculateFOV(_battleGame->getSelectedUnit());
	int j = 0;
	for (std::vector<BattleUnit*>::iterator i = battleUnit->getVisibleUnits()->begin(); i != battleUnit->getVisibleUnits()->end(); i++)
	{
		_btnVisibleUnit[j]->show();
		_numVisibleUnit[j]->show();
		_visibleUnit[j] = (*i);
		j++;
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
		BattleUnit *bu = _battleGame->getSelectedUnit();
		// Build up the popup menu
		int id = 0;
		std::wstring strAcc = _game->getLanguage()->getString("STR_ACC");
		std::wstring strTU = _game->getLanguage()->getString("STR_TUS");
		std::wstringstream ss1, ss2;
		ss1 << strAcc.c_str() << (int)floor(bu->getThrowingAccuracy() * 100) << "%";
		ss2 << strTU.c_str() << (int)floor(bu->getUnit()->getTimeUnits() * 0.25);
		_actionMenu[id]->setAction(BA_THROW, _game->getLanguage()->getString("STR_THROW"), ss1.str(), ss2.str());
		_actionMenu[id]->setVisible(true);
		id++;
		ss1.str(L"");
		ss2.str(L"");
		if (item->getRules()->getAccuracyAuto() != 0)
		{
			ss1 << strAcc.c_str() << (int)floor(bu->getFiringAccuracy(item->getRules()->getAccuracyAuto()) * 100) << "%";
			ss2 << strTU.c_str() << 0;
			_actionMenu[id]->setAction(BA_AUTOSHOT, _game->getLanguage()->getString("STR_AUTO_SHOT"), ss1.str(), ss2.str());
			_actionMenu[id]->setVisible(true);
			id++;
			ss1.str(L"");
			ss2.str(L"");
		}
		if (item->getRules()->getAccuracySnap() != 0)
		{
			ss1 << strAcc.c_str() << (int)floor(bu->getFiringAccuracy(item->getRules()->getAccuracySnap()) * 100) << "%";
			ss2 << strTU.c_str() << 0;
			_actionMenu[id]->setAction(BA_SNAPSHOT, _game->getLanguage()->getString("STR_SNAP_SHOT"), ss1.str(), ss2.str());
			_actionMenu[id]->setVisible(true);
			id++;
			ss1.str(L"");
			ss2.str(L"");
		}
		if (item->getRules()->getAccuracyAimed() != 0)
		{
			ss1 << strAcc.c_str() << (int)floor(bu->getFiringAccuracy(item->getRules()->getAccuracyAimed()) * 100) << "%";
			ss2 << strTU.c_str() << 0;
			_actionMenu[id]->setAction(BA_AIMEDSHOT, _game->getLanguage()->getString("STR_AIMED_SHOT"), ss1.str(), ss2.str());
			_actionMenu[id]->setVisible(true);
			id++;
			ss1.str(L"");
			ss2.str(L"");
		}

		_map->setCursorType(CT_NONE);
		_popup = true;

		// TODO other gamestates: scanner/medikit
		// this should be returned by the popup menu, but is now hardcoded to test without popup menu
		_selectedItem = item;
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
	Surface *frame = texture->getFrame(item->getRules()->getBigSprite());
	frame->setX((2 - item->getRules()->getSizeX()) * 8);
	frame->setY((3 - item->getRules()->getSizeY()) * 8);
	texture->getFrame(item->getRules()->getBigSprite())->blit(surface);
}

/**
 * Give time slice to the front state.
 */
void BattlescapeState::handleState()
{
	if (!_states.empty())
	{
		_states.front()->think();
		_map->draw(true); // redraw map
	}
}

/**
 * Animate map objects on the map, also smoke,fire,....
 */
void BattlescapeState::animate()
{
	_animFrame++;
	if (_animFrame == 8) _animFrame = 0;
	_map->animate();

	blinkVisibleUnitButtons();
	blinkWarningMessage();
}

/**
 * Get the current position on the map that the user has targetted.
 * For example to shoot or throw towards.
 */
Position BattlescapeState::getTarget() const
{
	return _target;
}

/**
 * Gets the item the user has clicked to use (can be left or right-hand item).
 * @return item Left or right-hand item.
 */
BattleItem *BattlescapeState::getSelectedItem() const
{
	return _selectedItem;
}

/**
 * Gets the item the user has clicked to use (can be left or right-hand item).
 * @return item Left or right-hand item.
 */
BattleActionType BattlescapeState::getSelectedAction() const
{
	return _selectedAction;
}

/**
 * Get pointer to the game. Some states need this info.
 */
Game *BattlescapeState::getGame() const
{
	return _game;
}

/**
 * Get pointer to the map. Some states need this info.
 */
Map *BattlescapeState::getMap() const
{
	return _map;
}

/**
 * Push a state at the front of the queue and start it.
 * @param bs Battlestate.
 */
void BattlescapeState::statePushFront(BattleState *bs)
{
	_states.push_front(bs);
	bs->init();
}

/**
 * Push a state as the next state after the current one.
 * @param bs Battlestate.
 */
void BattlescapeState::statePushNext(BattleState *bs)
{
	if (_states.empty())
	{
		_states.push_front(bs);
		bs->init();
	}
	else
	{
		_states.insert(++_states.begin(), bs);
	}

}

/**
 * Push a state at the back.
 * @param bs Battlestate.
 */
void BattlescapeState::statePushBack(BattleState *bs)
{
	if (_states.empty())
	{
		_states.push_front(bs);
		bs->init();
	}
	else
	{
		_states.push_back(bs);
	}
}

/**
 * Pop the current state. Handle errors and mouse cursor appearing again.
 * States pop themselves when they are finished.
 */
void BattlescapeState::popState()
{
	if (_states.empty()) return;

	if (_states.front()->getResult().length() > 0)
	{
		showWarningMessage(_states.front()->getResult());
	}
	_states.pop_front();

	if (_states.empty() && _selectedAction == BA_AUTOSHOT)
	{
		_autoShot++;
		if (_autoShot <= 3)
		{
			// shoot another try
			statePushBack(new ProjectileFlyBState(this));
		}
		else
		{
			_autoShot = 1;
		}
	}

	// if all states are empty - give the mouse back to the player
	if (_states.empty())
	{
		_game->getCursor()->setVisible(true);
		if (_selectedAction == BA_NONE)
		{
			_map->setCursorType(CT_NORMAL);
		}
		else
		{
			_map->setCursorType(CT_AIM);
		}
	}
	else
	{
		// init the next state in queue
		_states.front()->init();
	}
	if (_battleGame->getSelectedUnit() == 0 || _battleGame->getSelectedUnit()->isOut())
	{
		_targeting = false;
		_selectedAction = BA_NONE;
		_map->setCursorType(CT_NORMAL);
		_game->getCursor()->setVisible(true);
		_battleGame->setSelectedUnit(0);
	}
	updateSoldierInfo(_battleGame->getSelectedUnit());
}

/**
 * Sets the timer interval for think() calls of the state.
 * @param interval An interval in ms.
 */
void BattlescapeState::setStateInterval(Uint32 interval)
{
	_stateTimer->setInterval(interval);
}

/**
 * Show a debug message in the topleft corner.
 * @param message Debug message.
 */
void BattlescapeState::debug(const std::wstring message)
{
	if (_battleGame->getDebugMode())
	{
		_txtDebug->setText(message);
	}
}

/**
 * Takes care of any events from the core game engine.
 * @param action Pointer to an action.
 */
void BattlescapeState::handle(Action *action)
{
	if (_game->getCursor()->getVisible())
	{
		State::handle(action);

		if (action->getDetails()->type == SDL_KEYDOWN)
		{
				// "d" - enable debug mode
			if (action->getDetails()->key.keysym.sym == SDLK_d)
			{
				_battleGame->setDebugMode();
				debug(L"Debug Mode");
			}
		}
	}

}

}
