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
#include "OptionsControlsState.h"
#include <SDL.h>
#include "../Engine/Game.h"
#include "../Engine/Options.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Engine/Action.h"
#include "../Engine/Logger.h"

namespace OpenXcom
{

// Welp time for some ugly static code becuase C++
// is too much of a derp to initialize class arrays.
KeyOption OptionsControlsState::_controlsGeneral[] =
	{{"keyOk", "STR_OK", SDLK_UNKNOWN},
	{"keyCancel", "STR_CANCEL", SDLK_UNKNOWN},
	{"keyScreenshot", "STR_SCREENSHOT", SDLK_UNKNOWN},
	{"keyFps", "STR_FPS_COUNTER", SDLK_UNKNOWN}};

KeyOption OptionsControlsState::_controlsGeo[] =
	{{"keyGeoLeft", "STR_ROTATE_LEFT", SDLK_UNKNOWN},
	{"keyGeoRight", "STR_ROTATE_RIGHT", SDLK_UNKNOWN},
	{"keyGeoUp", "STR_ROTATE_UP", SDLK_UNKNOWN},
	{"keyGeoDown", "STR_ROTATE_DOWN", SDLK_UNKNOWN},
	{"keyGeoZoomIn", "STR_ZOOM_IN", SDLK_UNKNOWN},
	{"keyGeoZoomOut", "STR_ZOOM_OUT", SDLK_UNKNOWN},
	{"keyGeoSpeed1", "STR_5_SECS", SDLK_UNKNOWN},
	{"keyGeoSpeed2", "STR_1_MIN", SDLK_UNKNOWN},
	{"keyGeoSpeed3", "STR_5_MINS", SDLK_UNKNOWN},
	{"keyGeoSpeed4", "STR_30_MINS", SDLK_UNKNOWN},
	{"keyGeoSpeed5", "STR_1_HOUR", SDLK_UNKNOWN},
	{"keyGeoSpeed6", "STR_1_DAY", SDLK_UNKNOWN},
	{"keyGeoIntercept", "STR_INTERCEPT", SDLK_UNKNOWN},
	{"keyGeoBases", "STR_BASES", SDLK_UNKNOWN},
	{"keyGeoGraphs", "STR_GRAPHS", SDLK_UNKNOWN},
	{"keyGeoUfopedia", "STR_UFOPAEDIA_UC", SDLK_UNKNOWN},
	{"keyGeoOptions", "STR_OPTIONS_UC", SDLK_UNKNOWN},
	{"keyGeoFunding", "STR_FUNDING_UC", SDLK_UNKNOWN},
	{"keyGeoToggleDetail", "STR_TOGGLE_COUNTRY_DETAIL", SDLK_UNKNOWN},
	{"keyGeoToggleRadar", "STR_TOGGLE_RADAR_RANGES", SDLK_UNKNOWN},
	{"keyQuickSave", "STR_QUICK_SAVE", SDLK_UNKNOWN},
	{"keyQuickLoad", "STR_QUICK_LOAD", SDLK_UNKNOWN}};

KeyOption OptionsControlsState::_controlsBattle[] =
	{{"keyBattleLeft", "STR_SCROLL_LEFT", SDLK_UNKNOWN},
	{"keyBattleRight", "STR_SCROLL_RIGHT", SDLK_UNKNOWN},
	{"keyBattleUp", "STR_SCROLL_UP", SDLK_UNKNOWN},
	{"keyBattleDown", "STR_SCROLL_DOWN", SDLK_UNKNOWN},
	{"keyBattleLevelUp", "STR_VIEW_LEVEL_ABOVE", SDLK_UNKNOWN},
	{"keyBattleLevelDown", "STR_VIEW_LEVEL_BELOW", SDLK_UNKNOWN},
	{"keyBattleCenterUnit", "STR_CENTER_SELECTED_UNIT", SDLK_UNKNOWN},
	{"keyBattlePrevUnit", "STR_PREVIOUS_UNIT", SDLK_UNKNOWN},
	{"keyBattleNextUnit", "STR_NEXT_UNIT", SDLK_UNKNOWN},
	{"keyBattleDeselectUnit", "STR_DESELECT_UNIT", SDLK_UNKNOWN},
	{"keyBattleInventory", "STR_INVENTORY", SDLK_UNKNOWN},
	{"keyBattleMap", "STR_MINIMAP", SDLK_UNKNOWN},
	{"keyBattleOptions", "STR_OPTIONS", SDLK_UNKNOWN},
	{"keyBattleEndTurn", "STR_END_TURN", SDLK_UNKNOWN},
	{"keyBattleAbort", "STR_ABORT_MISSION", SDLK_UNKNOWN},
	{"keyBattleStats", "STR_UNIT_STATS", SDLK_UNKNOWN},
	{"keyBattleKneel", "STR_KNEEL", SDLK_UNKNOWN},
	{"keyBattleReload", "STR_RELOAD", SDLK_UNKNOWN},
	{"keyBattlePersonalLighting", "STR_TOGGLE_PERSONAL_LIGHTING", SDLK_UNKNOWN},
	{"keyBattleReserveNone", "STR_DONT_RESERVE_TUS", SDLK_UNKNOWN},
	{"keyBattleReserveSnap", "STR_RESERVE_TUS_FOR_SNAP_SHOT", SDLK_UNKNOWN},
	{"keyBattleReserveAimed", "STR_RESERVE_TUS_FOR_AIMED_SHOT", SDLK_UNKNOWN},
	{"keyBattleReserveAuto", "STR_RESERVE_TUS_FOR_AUTO_SHOT", SDLK_UNKNOWN},
	{"keyBattleCenterEnemy1", "STR_CENTER_ON_ENEMY_1", SDLK_UNKNOWN},
	{"keyBattleCenterEnemy2", "STR_CENTER_ON_ENEMY_2", SDLK_UNKNOWN},
	{"keyBattleCenterEnemy3", "STR_CENTER_ON_ENEMY_3", SDLK_UNKNOWN},
	{"keyBattleCenterEnemy4", "STR_CENTER_ON_ENEMY_4", SDLK_UNKNOWN},
	{"keyBattleCenterEnemy5", "STR_CENTER_ON_ENEMY_5", SDLK_UNKNOWN},
	{"keyBattleCenterEnemy6", "STR_CENTER_ON_ENEMY_6", SDLK_UNKNOWN},
	{"keyBattleCenterEnemy7", "STR_CENTER_ON_ENEMY_7", SDLK_UNKNOWN},
	{"keyBattleCenterEnemy8", "STR_CENTER_ON_ENEMY_8", SDLK_UNKNOWN},
	{"keyBattleCenterEnemy9", "STR_CENTER_ON_ENEMY_9", SDLK_UNKNOWN},
	{"keyBattleCenterEnemy10", "STR_CENTER_ON_ENEMY_10", SDLK_UNKNOWN},
	{"keyBattleVoxelView", "STR_SAVE_VOXEL_VIEW", SDLK_UNKNOWN}};

/**
 * Initializes all the elements in the Controls screen.
 * @param game Pointer to the core game.
 */
OptionsControlsState::OptionsControlsState(Game *game) : State(game), _selected(-1), _selKey(0)
{
	_countGeneral = 4;
	_countGeo = 20;
	_countBattle = 34;
	if (Options::getInt("autosave") == 1)
		_countGeo += 2;	// You can tune quick save/load hotkeys only if you choose autosave in the advanced options.

	// Create objects
	_window = new Window(this, 320, 200, 0, 0, POPUP_BOTH);
	_btnOk = new TextButton(148, 16, 8, 176);
	_btnCancel = new TextButton(148, 16, 164, 176);
	_txtTitle = new Text(310, 16, 5, 8);
	_lstControls = new TextList(288, 136, 8, 30);

	add(_window);
	add(_btnOk);
	add(_btnCancel);
	add(_txtTitle);
	add(_lstControls);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&OptionsControlsState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&OptionsControlsState::btnOkClick, (SDLKey)Options::getInt("keyOk"));

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&OptionsControlsState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&OptionsControlsState::btnCancelClick, (SDLKey)Options::getInt("keyCancel"));

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
	_lstControls->onMouseClick((ActionHandler)&OptionsControlsState::lstControlsClick, 0);
	_lstControls->onKeyboardPress((ActionHandler)&OptionsControlsState::lstControlsKeyPress);
	_lstControls->focus();

	_lstControls->addRow(2, _game->getLanguage()->getString("STR_GENERAL").c_str(), L"");
	_lstControls->setCellColor(0, 0, Palette::blockOffset(15)-1);
	addControls(_controlsGeneral, _countGeneral);
	_lstControls->addRow(2, L"", L"");
	_lstControls->addRow(2, _game->getLanguage()->getString("STR_GEOSCAPE").c_str(), L"");
	_lstControls->setCellColor(_countGeneral + 2, 0, Palette::blockOffset(15)-1);
	addControls(_controlsGeo, _countGeo);
	_lstControls->addRow(2, L"", L"");
	_lstControls->addRow(2, _game->getLanguage()->getString("STR_BATTLESCAPE").c_str(), L"");
	_lstControls->setCellColor(_countGeneral + 2 + _countGeo + 2, 0, Palette::blockOffset(15)-1);
	addControls(_controlsBattle, _countBattle);
}

/**
 *
 */
OptionsControlsState::~OptionsControlsState()
{

}

/**
 * Uppercases all the words in a string.
 * @param str Source string.
 * @return Destination string.
 */
std::string OptionsControlsState::ucWords(std::string str)
{
	if (!str.empty())
	{
		str[0] = toupper(str[0]);
	}
	for (size_t i = str.find_first_of(' '); i != std::string::npos; i = str.find_first_of(' ', i+1))
	{
		if (str.length() > i+1)
			str[i+1] = toupper(str[i+1]);
		else
			break;
	}
	return str;
}

/**
 * Adds a bunch of controls to the list.
 * @param keys Array of controls.
 * @param count Number of controls.
 */
void OptionsControlsState::addControls(KeyOption keys[], int count)
{
	for (int i = 0; i < count; ++i)
	{
		keys[i].key = (SDLKey)Options::getInt(keys[i].option);
		std::wstring name = _game->getLanguage()->getString(keys[i].name);
		std::wstring key = Language::utf8ToWstr(ucWords(SDL_GetKeyName(keys[i].key)));
		if (keys[i].key == SDLK_UNKNOWN)
			key = L"";
		_lstControls->addRow(2, name.c_str(), key.c_str());
	}
}

/**
 * Saves the controls.
 * @param action Pointer to an action.
 */
void OptionsControlsState::btnOkClick(Action *)
{
	for (int i = 0; i < _countGeneral; ++i)
	{
		Options::setInt(_controlsGeneral[i].option, _controlsGeneral[i].key);
	}
	for (int i = 0; i < _countGeo; ++i)
	{
		Options::setInt(_controlsGeo[i].option, _controlsGeo[i].key);
	}
	for (int i = 0; i < _countBattle; ++i)
	{
		Options::setInt(_controlsBattle[i].option, _controlsBattle[i].key);
	}
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

/**
 * Select a control for setting.
 * @param action Pointer to an action.
 */
void OptionsControlsState::lstControlsClick(Action *action)
{
	if (_selected != -1)
	{
		int selected = _selected;
		_lstControls->setCellColor(_selected, 0, Palette::blockOffset(8)+10);
		_lstControls->setCellColor(_selected, 1, Palette::blockOffset(8)+10);
		_selected = -1;
		_selKey = 0;
		if (selected == _lstControls->getSelectedRow())
			return;
	}
	_selected = _lstControls->getSelectedRow();
	if (_selected > 0 && _selected <= _countGeneral)
	{
		_selKey = &_controlsGeneral[_selected - 1];
	}
	else if (_selected > _countGeneral + 2 && _selected <= _countGeneral + 2 + _countGeo)
	{
		_selKey = &_controlsGeo[_selected - 1 - _countGeneral - 2];
	}
	else if (_selected > _countGeneral + 2 + _countGeo + 2 && _selected <= _countGeneral + 2 + _countGeo + 2 + _countBattle)
	{
		_selKey = &_controlsBattle[_selected - 1 - _countGeneral - 2 - _countGeo - 2];
	}
	else
	{
		_selected = -1;
		_selKey = 0;
		return;
	}

	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_lstControls->setCellColor(_selected, 0, Palette::blockOffset(8)+5);
		_lstControls->setCellColor(_selected, 1, Palette::blockOffset(8)+5);
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		_lstControls->setCellText(_selected, 1, L"");
		_selKey->key = SDLK_UNKNOWN;
		_selected = -1;
		_selKey = 0;
	}
}

/**
 * Change selected control.
 * @param action Pointer to an action.
 */
void OptionsControlsState::lstControlsKeyPress(Action *action)
{
	if (_selected != -1)
	{
		SDLKey key = action->getDetails()->key.keysym.sym;
		if (key != 0)
		{
			_selKey->key = key;
			std::wstring name = Language::utf8ToWstr(ucWords(SDL_GetKeyName(_selKey->key)));
			_lstControls->setCellText(_selected, 1, name);
		}
		_lstControls->setCellColor(_selected, 0, Palette::blockOffset(8)+10);
		_lstControls->setCellColor(_selected, 1, Palette::blockOffset(8)+10);
		_selected = -1;
		_selKey = 0;
	}
}

}
