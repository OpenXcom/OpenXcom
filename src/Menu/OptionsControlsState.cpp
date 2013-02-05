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
#include "OptionsControlsState.h"
#include <SDL/SDL.h>
#include "../Engine/Game.h"
#include "../Engine/Options.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"

namespace OpenXcom
{

struct KeyOption {
	std::string option;
	std::string name;
	SDLKey key;
};

/**
 * Initializes all the elements in the Saved Game screen.
 * @param game Pointer to the core game.
 * @param geo True to use Geoscape palette, false to use Battlescape palette.
 */
OptionsControlsState::OptionsControlsState(Game *game) : State(game)
{
	KeyOption controls[] = {{"keyOk", "", SDLK_RETURN},
							{"keyCancel", "", SDLK_ESCAPE},
							{"keyScreenshot", "", SDLK_F12},
							{"keyFps", "", SDLK_F5},
							{"keyGeoLeft", "", SDLK_LEFT},
							{"keyGeoRight", "", SDLK_RIGHT},
							{"keyGeoUp", "", SDLK_UP},
							{"keyGeoDown", "", SDLK_DOWN},
							{"keyGeoZoomIn", "", SDLK_PLUS},
							{"keyGeoZoomOut", "", SDLK_MINUS},
							{"keyGeoSpeed1", "", SDLK_1},
							{"keyGeoSpeed2", "", SDLK_2},
							{"keyGeoSpeed3", "", SDLK_3},
							{"keyGeoSpeed4", "", SDLK_4},
							{"keyGeoSpeed5", "", SDLK_5},
							{"keyGeoSpeed6", "", SDLK_6},
							{"keyGeoIntercept", "", SDLK_i},
							{"keyGeoBases", "", SDLK_b},
							{"keyGeoGraphs", "", SDLK_g},
							{"keyGeoUfopaedia", "", SDLK_u},
							{"keyGeoOptions", "", SDLK_ESCAPE},
							{"keyGeoFunding", "", SDLK_f},
							{"keyGeoToggleDetail", "", SDLK_TAB},
							{"keyGeoToggleRadar", "", SDLK_r},
							{"keyBattleLeft", "", SDLK_LEFT},
							{"keyBattleRight", "", SDLK_RIGHT},
							{"keyBattleUp", "", SDLK_UP},
							{"keyBattleDown", "", SDLK_DOWN},
							{"keyBattleLevelUp", "", SDLK_PAGEUP},
							{"keyBattleLevelDown", "", SDLK_PAGEDOWN},
							{"keyBattleCenterUnit", "", SDLK_HOME},
							{"keyBattleNextUnit", "", SDLK_TAB},
							{"keyBattlePrevUnit", "", SDLK_LSHIFT},
							{"keyBattleOptions", "", SDLK_ESCAPE},
							{"keyBattleEndTurn", "", SDLK_BACKSPACE},
							{"keyBattleInventory", "", SDLK_i},
							{"keyBattleMap", "", SDLK_m},
							{"keyBattleCrouch", "", SDLK_c},
							{"keyBattleReload", "", SDLK_r},
							{"keyBattlePersonalLighting", "", SDLK_l},
							{"keyBattleAbort", "", SDLK_a},
							{"keyBattleStats", "", SDLK_F1},
							{"keyBattleCenterEnemy1", "", SDLK_1},
							{"keyBattleCenterEnemy2", "", SDLK_2},
							{"keyBattleCenterEnemy3", "", SDLK_3},
							{"keyBattleCenterEnemy4", "", SDLK_4},
							{"keyBattleCenterEnemy5", "", SDLK_5},
							{"keyBattleCenterEnemy6", "", SDLK_6},
							{"keyBattleCenterEnemy7", "", SDLK_7},
							{"keyBattleCenterEnemy8", "", SDLK_8},
							{"keyBattleCenterEnemy9", "", SDLK_9},
							{"keyBattleReserveNone", "", SDLK_F2},
							{"keyBattleReserveSnap", "", SDLK_F3},
							{"keyBattleReserveAimed", "", SDLK_F4},
							{"keyBattleReserveAuto", "", SDLK_F5}};

	// Create objects
	_window = new Window(this, 320, 200, 0, 0, POPUP_BOTH);
	_btnOk = new TextButton(148, 16, 8, 176);
	_btnCancel = new TextButton(148, 16, 164, 176);
	_txtTitle = new Text(310, 16, 5, 8);
	_lstControls = new TextList(288, 120, 8, 40);

	add(_window);
	add(_btnOk);
	add(_btnCancel);
	add(_txtTitle);
	add(_lstControls);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&OptionsControlsState::btnOkClick);

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&OptionsControlsState::btnOkClick);

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getLanguage()->getString("STR_CONTROLS"));

	_lstControls->setColor(Palette::blockOffset(8)+10);
	_lstControls->setArrowColor(Palette::blockOffset(8)+5);
	_lstControls->setColumns(2, 168, 120);
	_lstControls->setSelectable(true);
	_lstControls->setBackground(_window);
	_lstControls->setMargin(8);

	for (int i = 0; i < 55; i++)
	{
		std::wstring option = Language::utf8ToWstr(controls[i].option);
		std::wstring key = Language::utf8ToWstr(SDL_GetKeyName(controls[i].key));
		_lstControls->addRow(2, option.c_str(), key.c_str());
	}
}

/**
 *
 */
OptionsControlsState::~OptionsControlsState()
{

}

/**
 * Saves the controls.
 * @param action Pointer to an action.
 */
void OptionsControlsState::btnOkClick(Action *)
{
	Options::save();
	_game->popState();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void OptionsControlsState::btnCancelClick(Action *)
{
	_game->popState();
}

}
