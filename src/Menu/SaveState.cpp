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
#include "SaveState.h"
#include <cstdio>
#include "../Engine/Logger.h"
#include "../Engine/CrossPlatform.h"
#include "../Savegame/SavedGame.h"
#include "../Engine/Game.h"
#include "../Engine/Action.h"
#include "../Engine/Exception.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Interface/TextList.h"
#include "../Interface/TextEdit.h"
#include "../Interface/TextButton.h"
#include "ErrorMessageState.h"
#include "DeleteGameState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Save Game screen.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 */
SaveState::SaveState(Game *game, OptionsOrigin origin) : SavedGameState(game, origin, 1), _selected(L""), _previousSelectedRow(-1), _selectedRow(-1)
{
	// Create objects
	_edtSave = new TextEdit(this, 168, 9, 0, 0);
	_btnSaveGame = new TextButton(80, 16, 60, 172);

	add(_edtSave);
	add(_btnSaveGame);

	// Set up objects

	_txtTitle->setText(tr("STR_SELECT_SAVE_POSITION"));

	_lstSaves->onMousePress((ActionHandler)&SaveState::lstSavesPress);

	_btnCancel->setX(180);

	_btnSaveGame->setColor(Palette::blockOffset(8)+5);
	_btnSaveGame->setText(tr("STR_SAVE_GAME"));
	_btnSaveGame->onMouseClick((ActionHandler)&SaveState::btnSaveGameClick);

	_edtSave->setColor(Palette::blockOffset(8)+10);
	_edtSave->setVisible(false);
	_edtSave->onKeyboardPress((ActionHandler)&SaveState::edtSaveKeyPress);

	centerAllSurfaces();
}

/**
 * Creates the Quick Save Game state.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 * @param showMsg True if need to show messages like "Loading game" or "Saving game".
 */
SaveState::SaveState(Game *game, OptionsOrigin origin, bool showMsg) : SavedGameState(game, origin, 1, showMsg)
{
	game->getSavedGame()->setName(L"autosave");
	quickSave("autosave");
}

/**
 *
 */
SaveState::~SaveState()
{

}

/**
 * Updates the save game list with the current list
 * of available savegames.
 */
void SaveState::updateList()
{
	_lstSaves->addRow(1, tr("STR_NEW_SAVED_GAME").c_str());
	SavedGameState::updateList();
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
				_lstSaves->setCellText(_previousSelectedRow	, 0, tr("STR_NEW_SAVED_GAME"));
				break;
			default:
				_lstSaves->setCellText(_previousSelectedRow	, 0, _selected);
		}

		_selected = _lstSaves->getCellText(_lstSaves->getSelectedRow(), 0);
		_lstSaves->setCellText(_lstSaves->getSelectedRow(), 0, L"");
		if (_lstSaves->getSelectedRow() == 0)
		{
			_edtSave->setText(L"");
			_selected = L"";
		}
		else
		{
			_edtSave->setText(_selected);
		}
		_edtSave->setX(_lstSaves->getColumnX(0));
		_edtSave->setY(_lstSaves->getRowY(_selectedRow));
		_edtSave->setVisible(true);
		_edtSave->setFocus(true, false);
		_lstSaves->setScrolling(false);
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT && _lstSaves->getSelectedRow())
	{
		_game->pushState(new DeleteGameState(_game, _origin, _saves[_lstSaves->getSelectedRow() - 1].fileName));
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
		saveGame();
	}
}

/**
 * Saves the selected save.
 * @param action Pointer to an action.
 */
void SaveState::btnSaveGameClick(Action *)
{
	if (_selectedRow != -1)
	{
		saveGame();
	}
}

void SaveState::saveGame()
{
	updateStatus("STR_SAVING_GAME");
	_game->getSavedGame()->setName(_edtSave->getText());
	std::string oldFilename, newFilename;
	newFilename = CrossPlatform::sanitizeFilename(Language::wstrToFs(_edtSave->getText()));
	if (_selectedRow > 0)
	{
		oldFilename = _saves[_selectedRow-1].fileName;
	}
	else
	{
		while (CrossPlatform::fileExists(Options::getUserFolder() + newFilename + ".sav"))
		{
			newFilename += "_";
		}
		oldFilename = newFilename;
	}
	quickSave(oldFilename);
	if (oldFilename != newFilename)
	{
		while (CrossPlatform::fileExists(Options::getUserFolder() + newFilename + ".sav"))
		{
			newFilename += "_";
		}
		std::string oldPath = Options::getUserFolder() + oldFilename + ".sav";
		std::string newPath = Options::getUserFolder() + newFilename + ".sav";
		rename(oldPath.c_str(), newPath.c_str());
	}
	_game->popState();
	_game->popState();
}

/**
 * Quick save game.
 * @param filename name of file without ".sav"
 */
void SaveState::quickSave(const std::string &filename)
{
	if (_showMsg) updateStatus("STR_SAVING_GAME");

	std::string fullPath = Options::getUserFolder() + filename + ".sav";
	std::string bakPath = fullPath + ".bak";

	try
	{
		if (CrossPlatform::fileExists(fullPath))
		{
			if (CrossPlatform::fileExists(bakPath) && !CrossPlatform::deleteFile(bakPath))
			{
				throw Exception("Failed to delete " + filename + ".sav.bak");
			}
			if (rename(fullPath.c_str(), bakPath.c_str()))
			{
				throw Exception("Failed to rename " + filename + ".sav");
			}
		}

		_game->getSavedGame()->save(filename);
		CrossPlatform::deleteFile(bakPath);
	}
	catch (Exception &e)
	{
		Log(LOG_ERROR) << e.what();
		std::wostringstream error;
		error << tr("STR_SAVE_UNSUCCESSFUL") << L'\x02' << Language::fsToWstr(e.what());
		if (_origin != OPT_BATTLESCAPE)
			_game->pushState(new ErrorMessageState(_game, error.str(), _palette, Palette::blockOffset(8)+10, "BACK01.SCR", 6));
		else
			_game->pushState(new ErrorMessageState(_game, error.str(), _palette, Palette::blockOffset(0), "TAC00.SCR", -1));
	}
	catch (YAML::Exception &e)
	{
		Log(LOG_ERROR) << e.what();
		std::wostringstream error;
		error << tr("STR_SAVE_UNSUCCESSFUL") << L'\x02' << Language::fsToWstr(e.what());
		if (_origin != OPT_BATTLESCAPE)
			_game->pushState(new ErrorMessageState(_game, error.str(), _palette, Palette::blockOffset(8)+10, "BACK01.SCR", 6));
		else
			_game->pushState(new ErrorMessageState(_game, error.str(), _palette, Palette::blockOffset(0), "TAC00.SCR", -1));
	}
}

}
