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
#include "Map.h"
#include "BattlescapeState.h"
#include "Pathfinding.h"
#include "../Engine/Game.h"
#include "../Engine/Music.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Engine/Screen.h"
#include "../Engine/Action.h"
#include "../Resource/ResourcePack.h"
#include "../Resource/LangString.h"
#include "../Interface/Cursor.h"
#include "../Interface/Text.h"
#include "../Interface/Bar.h"
#include "../Interface/ImageButton.h"
#include "../Interface/NumberText.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/BattleSoldier.h"
#include "../Savegame/Soldier.h"
#include "../Ruleset/Ruleset.h"

/**
 * Initializes all the elements in the Battlescape screen.
 * @param game Pointer to the core game.
 */
#include <iostream>
BattlescapeState::BattlescapeState(Game *game) : State(game)
{
	// Create the battlemap view
	_map = new Map(320, 200, 0, 0);

	// Create buttonbar
	_icons = new Surface(320, 200, 0, 0);
	_numLayers = new NumberText(3, 5, 232, 150);

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

	// Create soldier stats summary
	_txtName = new Text(_game->getResourcePack()->getFont("BIGLETS.DAT"), _game->getResourcePack()->getFont("SMALLSET.DAT"), 120, 10, 135, 176);

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

	// Last 16 colors are a grey gradient
	SDL_Color color[16];
	for (int i = 0; i < 16; i++)
	{
		color[i].r = 128 - (i + 1) * 8;
		color[i].g = 128 - (i + 1) * 8;
		color[i].b = 128 - (i + 1) * 8;
	}
	_game->setPalette(color, Palette::backPos+16, 16);

	// Fix cursor
	_game->getCursor()->setColor(Palette::blockOffset(9));

	add(_map);
	add(_icons);
	add(_numLayers);
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

	// Set up objects
	_game->getResourcePack()->getSurface("ICONS.PCK")->blit(_icons);
	
	_map->setSavedGame(_game->getSavedGame()->getBattleGame(), _game);
	_map->setResourcePack(_game->getResourcePack());
	_map->init();

	_numLayers->setColor(Palette::blockOffset(1)-2);
	_numLayers->setValue(1);

	_btnAbort->onMouseClick((ActionHandler)&BattlescapeState::btnAbortClick);
	_btnMapUp->onMouseClick((ActionHandler)&BattlescapeState::btnMapUpClick);
	_btnMapDown->onMouseClick((ActionHandler)&BattlescapeState::btnMapDownClick);
	_btnNextSoldier->onMouseClick((ActionHandler)&BattlescapeState::btnNextSoldierClick);
	_btnCenter->onMouseClick((ActionHandler)&BattlescapeState::btnCenterClick);

	_map->onMouseClick((ActionHandler)&BattlescapeState::mapClick);

	_txtName->setColor(Palette::blockOffset(8));
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

	updateSoldierInfo(_game->getSavedGame()->getBattleGame()->getSelectedSoldier());
	_map->centerOnPosition(_game->getSavedGame()->getBattleGame()->getSelectedSoldier()->getPosition());

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
}

/**
 *
 */
BattlescapeState::~BattlescapeState()
{

}

/**
 * Processes any clicks on the map to
 * command units.
 * @param action Pointer to an action.
 */
void BattlescapeState::mapClick(Action *action)
{
	// don't handle mouseclicks below 140, because they are in the buttons area (it overlaps with map surface)
	if (action->getDetails()->motion.y/action->getYScale() > BUTTONS_AREA) return;

	Position pos;
	_map->getSelectorPosition(&pos);

	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		BattleUnit *unit = _game->getSavedGame()->getBattleGame()->selectUnit(pos);
		if (unit)
		{
			_game->getSavedGame()->getBattleGame()->setSelectedSoldier((BattleSoldier*)unit);
			updateSoldierInfo((BattleSoldier*)unit);
			return;
		}
		Pathfinding *pf = _game->getSavedGame()->getBattleGame()->getPathfinding();
		pf->calculate(_game->getSavedGame()->getBattleGame()->getSelectedSoldier(), pos);
	}
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		_game->getSavedGame()->getBattleGame()->getSelectedSoldier()->lookAt(pos);
	}

	
}

/**
 * Move unit up.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnUnitUpClick(Action *action)
{
	Pathfinding *pf = _game->getSavedGame()->getBattleGame()->getPathfinding();
	Position start = _game->getSavedGame()->getBattleGame()->getSelectedSoldier()->getPosition();
	Position end = start + Position(0, 0, +1);
	pf->calculate((BattleUnit*)_game->getSavedGame()->getBattleGame()->getSelectedSoldier(), end);
}

/**
 * Move unit down.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnUnitDownClick(Action *action)
{
	Pathfinding *pf = _game->getSavedGame()->getBattleGame()->getPathfinding();
	Position start = _game->getSavedGame()->getBattleGame()->getSelectedSoldier()->getPosition();
	Position end = start + Position(0, 0, -1);
	pf->calculate((BattleUnit*)_game->getSavedGame()->getBattleGame()->getSelectedSoldier(), end);
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
	_map->centerOnPosition(_game->getSavedGame()->getBattleGame()->getSelectedSoldier()->getPosition());
}

/**
 * Select next soldier.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnNextSoldierClick(Action *action)
{
	BattleSoldier *soldier = _game->getSavedGame()->getBattleGame()->selectNextSoldier();
	updateSoldierInfo(soldier);
	_map->centerOnPosition(soldier->getPosition());
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
	_game->getRuleset()->endBattle(_game->getSavedGame());
	_game->getCursor()->setColor(Palette::blockOffset(15) + 12);
	_game->popState();
}

/**
 * Updates soldier name/rank/tu/energy/health/morale.
 * @param soldier Pointer to current battlesoldier.
 */
void BattlescapeState::updateSoldierInfo(BattleSoldier *soldier)
{
	_txtName->setText(soldier->getSoldier()->getName());
	_numTimeUnits->setValue(soldier->getSoldier()->getTimeUnits());
	_barTimeUnits->setMax(soldier->getSoldier()->getTimeUnits());
	_barTimeUnits->setValue(soldier->getSoldier()->getTimeUnits());
	_numEnergy->setValue(soldier->getSoldier()->getStamina());
	_barEnergy->setMax(soldier->getSoldier()->getStamina());
	_barEnergy->setValue(soldier->getSoldier()->getStamina());
	_numHealth->setValue(soldier->getSoldier()->getHealth());
	_barHealth->setMax(soldier->getSoldier()->getHealth());
	_barHealth->setValue(soldier->getSoldier()->getHealth());
	_numMorale->setValue(100);
	_barMorale->setMax(100);
	_barMorale->setValue(100);
}