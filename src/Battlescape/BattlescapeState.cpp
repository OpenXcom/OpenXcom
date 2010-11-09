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
#include "../Engine/Game.h"
#include "../Engine/Music.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Engine/Screen.h"
#include "../Resource/ResourcePack.h"
#include "../Resource/LangString.h"
#include "../Interface/Cursor.h"
#include "../Interface/Text.h"
#include "../Interface/Bar.h"
#include "../Interface/ImageButton.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/BattleSoldier.h"
#include "../Savegame/Soldier.h"
#include "../Ruleset/Ruleset.h"

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

	// Create buttons
	_btnAbort = new InteractiveSurface(32, 16, 240, 160);
	_btnMapUp = new InteractiveSurface(32, 16, 80, 144);
	_btnMapDown = new InteractiveSurface(32, 16, 80, 160);
	_btnNextSoldier = new InteractiveSurface(32, 16, 176, 144);
	_btnReserveNone = new ImageButton(28, 11, 49, 177);
	_btnReserveSnap = new ImageButton(28, 11, 78, 177);
	_btnReserveAimed = new ImageButton(28, 11, 49, 189);
	_btnReserveAuto = new ImageButton(28, 11, 78, 189);

	// Create soldier stats summary
	_txtName = new Text(_game->getResourcePack()->getFont("BIGLETS.DAT"), _game->getResourcePack()->getFont("SMALLSET.DAT"), 120, 10, 135, 176);

	_numTimeUnits = new Text(_game->getResourcePack()->getFont("BIGLETS.DAT"), _game->getResourcePack()->getFont("SMALLSET.DAT"), 15, 10, 135, 185);
	_barTimeUnits = new Bar(100, 3, 170, 185);

	_numEnergy = new Text(_game->getResourcePack()->getFont("BIGLETS.DAT"), _game->getResourcePack()->getFont("SMALLSET.DAT"), 15, 10, 153, 185);
	_barEnergy = new Bar(100, 3, 170, 189);

	_numHealth = new Text(_game->getResourcePack()->getFont("BIGLETS.DAT"), _game->getResourcePack()->getFont("SMALLSET.DAT"), 15, 10, 135, 193);
	_barHealth= new Bar(100, 3, 170, 193);

	_numMorale = new Text(_game->getResourcePack()->getFont("BIGLETS.DAT"), _game->getResourcePack()->getFont("SMALLSET.DAT"), 15, 10, 153, 193);
	_barMorale = new Bar(100, 3, 170, 197);

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
	add(_btnAbort);
	add(_btnMapUp);
	add(_btnMapDown);
	add(_btnNextSoldier);
	/*add(_txtName);
	add(_numTimeUnits);
	add(_numEnergy);
	add(_numHealth);
	add(_numMorale);
	add(_barTimeUnits);
	add(_barEnergy);
	add(_barHealth);
	add(_barMorale);*/
	add(_btnReserveNone);
	add(_btnReserveSnap);
	add(_btnReserveAimed);
	add(_btnReserveAuto);

	// Set up objects
	_game->getResourcePack()->getSurface("ICONS.PCK")->blit(_icons);
	
	_map->setSavedGame(_game->getSavedGame()->getBattleGame());
	_map->setResourcePack(_game->getResourcePack());
	_map->init();

	_btnAbort->onMouseClick((ActionHandler)&BattlescapeState::btnAbortClick);
	_btnMapUp->onMouseClick((ActionHandler)&BattlescapeState::btnMapUpClick);
	_btnMapDown->onMouseClick((ActionHandler)&BattlescapeState::btnMapDownClick);
	_btnNextSoldier->onMouseClick((ActionHandler)&BattlescapeState::btnNextSoldierClick);

	_txtName->setColor(Palette::blockOffset(8)+2);
	_numTimeUnits->setSmall();
	_numEnergy->setSmall();
	_numHealth->setSmall();
	_numMorale->setSmall();

	updateSoldierInfo(_game->getSavedGame()->getBattleGame()->getSelectedSoldier());

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
{}

/**
 * Move unit up.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnUnitUpClick(Action *action)
{}

/**
 * Move unit down.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnUnitDownClick(Action *action)
{}

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
{}

/**
 * Select next soldier.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnNextSoldierClick(Action *action)
{
	updateSoldierInfo(_game->getSavedGame()->getBattleGame()->selectNextSoldier());
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
	_numTimeUnits->setText("65");
	_barTimeUnits->setMax(65);
	_barTimeUnits->setValue(65);
	_barTimeUnits->setScale(0.9);
	_numEnergy->setText("65");
	_barEnergy->setMax(165);
	_barEnergy->setValue(65);
	_barTimeUnits->setScale(1.0);
	_numHealth->setText("65");
	_barHealth->setMax(65);
	_barHealth->setValue(65);
	_barTimeUnits->setScale(1.0);
	_numMorale->setText("100");
	_barMorale->setMax(100);
	_barMorale->setValue(100);
	_barTimeUnits->setScale(1.0);
}