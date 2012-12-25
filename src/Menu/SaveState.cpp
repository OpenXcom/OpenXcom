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
#include "SaveState.h"
#include <yaml-cpp/yaml.h>
#include "../Engine/Logger.h"
#include "../Engine/CrossPlatform.h"
#include "../Savegame/SavedGame.h"
#include "../Engine/Game.h"
#include "../Engine/Action.h"
#include "../Engine/Exception.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Interface/TextEdit.h"
#include "ErrorMessageState.h"
#include "DeleteGameState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Save Game screen.
 * @param game Pointer to the core game.
 * @param geo True to use Geoscape palette, false to use Battlescape palette.
 */
SaveState::SaveState(Game *game, bool geo) : SavedGameState(game, geo), _selected(""), _previousSelectedRow(-1), _selectedRow(-1)
{
	// Create objects
	
	_edtSave = new TextEdit(168, 9, 0, 0);

	add(_edtSave);

	// Set up objects
	if (_geo)
	{
		_edtSave->setColor(Palette::blockOffset(8)+10);
	}
	else
	{
		_edtSave->setColor(Palette::blockOffset(0));
		_edtSave->setHighContrast(true);
	}

	_txtTitle->setText(_game->getLanguage()->getString("STR_SELECT_SAVE_POSITION"));

	_lstSaves->onMousePress((ActionHandler)&SaveState::lstSavesPress);

	_edtSave->setVisible(false);
	_edtSave->onKeyboardPress((ActionHandler)&SaveState::edtSaveKeyPress);
}

/**
 *
 */
SaveState::~SaveState()
{

}

/**
 * Updates the save game list with a current list
 * of available savegames.
 */
void SaveState::updateList()
{
	_lstSaves->clearList();
	_lstSaves->addRow(1, _game->getLanguage()->getString("STR_NEW_SAVED_GAME").c_str());
	SavedGame::getList(_lstSaves, _game->getLanguage());
}

/**
 * Names the selected save.
 * @param action Pointer to an action.
 */
void SaveState::lstSavesPress(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_previousSelectedRow = _selectedRow;
		_selectedRow = _lstSaves->getSelectedRow();

		switch (_previousSelectedRow)
		{
			case -1:	// first click on the savegame list
				break;
			case 0:
				_lstSaves->setCellText(_previousSelectedRow	, 0, _game->getLanguage()->getString("STR_NEW_SAVED_GAME"));
				break;
			default:
				_lstSaves->setCellText(_previousSelectedRow	, 0, Language::utf8ToWstr(_selected));
		}

		_selected = Language::wstrToUtf8(_lstSaves->getCellText(_lstSaves->getSelectedRow(), 0));
		_lstSaves->setCellText(_lstSaves->getSelectedRow(), 0, L"");
		if (_lstSaves->getSelectedRow() == 0)
		{
			_edtSave->setText(L"");
			_selected = "";
		}
		else
		{
			_edtSave->setText(Language::utf8ToWstr(_selected));
		}
		_edtSave->setX(_lstSaves->getColumnX(0));
		_edtSave->setY(_lstSaves->getRowY(_selectedRow));
		_edtSave->setVisible(true);
		_edtSave->focus();
		_lstSaves->setScrolling(false);
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT && _selectedRow)
	{
		if(_geo)
			_game->pushState(new DeleteGameState(_game, _lstSaves->getCellText(_lstSaves->getSelectedRow(),0), Palette::blockOffset(8)+10, "BACK01.SCR", 6, this));
		else
			_game->pushState(new DeleteGameState(_game, _lstSaves->getCellText(_lstSaves->getSelectedRow(),0), Palette::blockOffset(0), "TAC00.SCR", -1, this));
	}
}

/**
 * Saves the selected save.
 * @param action Pointer to an action.
 */
void SaveState::edtSaveKeyPress(Action *action)
{
	if (action->getDetails()->key.keysym.sym == SDLK_RETURN ||
		action->getDetails()->key.keysym.sym == SDLK_KP_ENTER)
	{
		updateStatus("STR_SAVING_GAME");
		try
		{
			_game->getSavedGame()->save(Language::wstrToUtf8(_edtSave->getText()));
			std::string oldName = Options::getUserFolder() + _selected + ".sav";
			std::string newName = Options::getUserFolder() + Language::wstrToUtf8(_edtSave->getText()) + ".sav";
			if (_selectedRow > 0 && oldName != newName)
			{
				if (!CrossPlatform::deleteFile(oldName))
				{
					throw Exception("Failed to overwrite save");
				}
			}
			_game->popState();
			_game->popState();
		}
		catch (Exception &e)
		{
			_edtSave->setVisible(false);
			Log(LOG_ERROR) << e.what();
			std::wstring error = _game->getLanguage()->getString("STR_SAVE_UNSUCCESSFUL") + L'\x02' + Language::utf8ToWstr(e.what());
			if (_geo)
				_game->pushState(new ErrorMessageState(_game, error, Palette::blockOffset(8)+10, "BACK01.SCR", 6));
			else
				_game->pushState(new ErrorMessageState(_game, error, Palette::blockOffset(0), "TAC00.SCR", -1));
		}
		catch (YAML::Exception &e)
		{
			_edtSave->setVisible(false);
			Log(LOG_ERROR) << e.what();
			std::wstring error = _game->getLanguage()->getString("STR_SAVE_UNSUCCESSFUL") + L'\x02' + Language::utf8ToWstr(e.what());
			if (_geo)
				_game->pushState(new ErrorMessageState(_game, error, Palette::blockOffset(8)+10, "BACK01.SCR", 6));
			else
				_game->pushState(new ErrorMessageState(_game, error, Palette::blockOffset(0), "TAC00.SCR", -1));
		}
	}
}

}
