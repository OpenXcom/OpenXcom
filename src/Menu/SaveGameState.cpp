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
#include "SaveGameState.h"
#include <iostream>
#include "yaml.h"
#include "../Savegame/SavedGame.h"
#include "../Engine/Game.h"
#include "../Engine/Action.h"
#include "../Engine/Exception.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Interface/TextEdit.h"
#include "../Geoscape/GeoscapeErrorState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Save Game screen.
 * @param game Pointer to the core game.
 * @param geo True to use Geoscape palette, false to use Battlescape palette.
 */
SaveGameState::SaveGameState(Game *game, bool geo) : State(game), _selected(""), _geo(geo)
{
	// Create objects
	WindowPopup p = POPUP_BOTH;
	if (!geo)
		p = POPUP_NONE;
	_window = new Window(this, 320, 200, 0, 0, p);
	_btnCancel = new TextButton(80, 16, 120, 172);
	_txtTitle = new Text(310, 16, 5, 8);
	_txtName = new Text(150, 9, 16, 24);
	_txtTime = new Text(30, 9, 184, 24);
	_txtDate = new Text(30, 9, 214, 24);
	_lstSaves = new TextList(288, 128, 8, 32);
	_edtSave = new TextEdit(168, 9, 0, 0);
	
	// Set palette
	if (_geo)
	{
		_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);
	}
        
	_previousSelectedRow = -1;
	_selectedRow = -1;

	add(_window);
	add(_btnCancel);
	add(_txtTitle);
	add(_txtName);
	add(_txtTime);
	add(_txtDate);
	add(_lstSaves);
	add(_edtSave);

	// Set up objects
	if (_geo)
	{
		_window->setColor(Palette::blockOffset(8)+5);
		_window->setBackground(game->getResourcePack()->getSurface("BACK01.SCR"));
		
		_btnCancel->setColor(Palette::blockOffset(8)+5);

		_txtTitle->setColor(Palette::blockOffset(15)-1);

		_txtName->setColor(Palette::blockOffset(15)-1);

		_txtTime->setColor(Palette::blockOffset(15)-1);

		_txtDate->setColor(Palette::blockOffset(15)-1);
		
		_lstSaves->setColor(Palette::blockOffset(8)+10);
		
		_edtSave->setColor(Palette::blockOffset(8)+10);
	}
	else
	{
        	_window->setColor(Palette::blockOffset(0));
		_window->setHighContrast(true);
		_window->setBackground(_game->getResourcePack()->getSurface("TAC00.SCR"));

		_btnCancel->setColor(Palette::blockOffset(0));
		_btnCancel->setHighContrast(true);

		_txtTitle->setColor(Palette::blockOffset(0));
		_txtTitle->setHighContrast(true);

		_txtName->setColor(Palette::blockOffset(0));
		_txtName->setHighContrast(true);

		_txtTime->setColor(Palette::blockOffset(0));
		_txtTime->setHighContrast(true);

		_txtDate->setColor(Palette::blockOffset(0));
		_txtDate->setHighContrast(true);
		
		_lstSaves->setColor(Palette::blockOffset(0));
		_lstSaves->setHighContrast(true);
		
		_edtSave->setColor(Palette::blockOffset(0));
		_edtSave->setHighContrast(true);
	}

	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&SaveGameState::btnCancelClick);

	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getLanguage()->getString("STR_SELECT_SAVE_POSITION"));

	_txtName->setText(_game->getLanguage()->getString("STR_NAME"));

	_txtTime->setText(_game->getLanguage()->getString("STR_TIME"));

	_txtDate->setText(_game->getLanguage()->getString("STR_DATE"));

	_lstSaves->setColumns(5, 168, 30, 30, 30, 30);
	_lstSaves->setSelectable(true);
	_lstSaves->setBackground(_window);
	_lstSaves->setMargin(8);
	_lstSaves->onMouseClick((ActionHandler)&SaveGameState::lstSavesClick);
	_lstSaves->addRow(1, L"<NEW SAVED GAME>");
	SavedGame::getList(_lstSaves, _game->getLanguage());

	_edtSave->setVisible(false);
	_edtSave->onKeyboardPress((ActionHandler)&SaveGameState::edtSaveKeyPress);
}

/**
 *
 */
SaveGameState::~SaveGameState()
{
	
}

/**
 * Resets the palette since it's bound to change on other screens.
 */
void SaveGameState::init()
{
	if (_geo)
	{
		_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);
	}
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SaveGameState::btnCancelClick(Action *action)
{
	_game->popState();
}

/**
 * Names the selected save.
 * @param action Pointer to an action.
 */
void SaveGameState::lstSavesClick(Action *action)
{       
	_previousSelectedRow = _selectedRow;
	_selectedRow = _lstSaves->getSelectedRow();
	if (_previousSelectedRow != _selectedRow || _previousSelectedRow != -1)
	{
		_lstSaves->clearList();    
		_lstSaves->addRow(1, L"<NEW SAVED GAME>");
		SavedGame::getList(_lstSaves, _game->getLanguage());

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
		_edtSave->setX(_lstSaves->getX() + _lstSaves->getMargin());
		_edtSave->setY(_lstSaves->getY() + _lstSaves->getSelectedRow() * 8);
		_edtSave->setVisible(true);
		_edtSave->focus();
		_edtSave->caretAtEnd();
	}
}

/**
 * Saves the selected save.
 * @param action Pointer to an action.
 */
void SaveGameState::edtSaveKeyPress(Action *action)
{
	if (action->getDetails()->key.keysym.sym == SDLK_RETURN)
	{
		try
		{
			if (_selected != "")
			{
				std::string oldName = Options::getUserFolder() + _selected + ".sav";
				std::string newName = Options::getUserFolder() + Language::wstrToUtf8(_edtSave->getText()) + ".sav";
				if (rename(oldName.c_str(), newName.c_str()) != 0)
				{
					throw Exception("Failed to overwrite save");
				}
			}
			_game->getSavedGame()->save(Language::wstrToUtf8(_edtSave->getText()));
		}
		catch (Exception &e)
		{
			std::cerr << "ERROR: " << e.what() << std::endl;
			_game->pushState(new GeoscapeErrorState(_game, "STR_SAVE_UNSUCCESSFUL"));
		}
		catch (YAML::Exception &e)
		{
			std::cerr << "ERROR: " << e.what() << std::endl;
			_game->pushState(new GeoscapeErrorState(_game, "STR_SAVE_UNSUCCESSFUL"));
		}
		_game->popState();
	}
}

}
