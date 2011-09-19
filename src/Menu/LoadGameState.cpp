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
#include "LoadGameState.h"
#include <iostream>
#include "yaml.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Engine/Game.h"
#include "../Engine/Exception.h"
#include "../Resource/ResourcePack.h"
#include "../Ruleset/XcomRuleset.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Geoscape/GeoscapeState.h"
#include "../Geoscape/GeoscapeErrorState.h"
#include "../Battlescape/BattlescapeState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Load Game screen.
 * @param game Pointer to the core game.
 */
LoadGameState::LoadGameState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0, POPUP_BOTH);
	_btnCancel = new TextButton(80, 16, 120, 172);
	_txtTitle = new Text(310, 16, 5, 8);
	_txtName = new Text(150, 9, 16, 24);
	_txtTime = new Text(30, 9, 184, 24);
	_txtDate = new Text(30, 9, 214, 24);
	_lstSaves = new TextList(288, 128, 8, 32);
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);

	add(_window);
	add(_btnCancel);
	add(_txtTitle);
	add(_txtName);
	add(_txtTime);
	add(_txtDate);
	add(_lstSaves);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+8);
	_window->setBackground(game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnCancel->setColor(Palette::blockOffset(8)+8);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&LoadGameState::btnCancelClick);

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getLanguage()->getString("STR_SELECT_GAME_TO_LOAD"));

	_txtName->setColor(Palette::blockOffset(15)-1);
	_txtName->setText(_game->getLanguage()->getString("STR_NAME"));

	_txtTime->setColor(Palette::blockOffset(15)-1);
	_txtTime->setText(_game->getLanguage()->getString("STR_TIME"));

	_txtDate->setColor(Palette::blockOffset(15)-1);
	_txtDate->setText(_game->getLanguage()->getString("STR_DATE"));
	
	_lstSaves->setColor(Palette::blockOffset(8)+10);
	_lstSaves->setArrowColor(Palette::blockOffset(8)+8);
	_lstSaves->setColumns(5, 168, 30, 30, 30, 30);
	_lstSaves->setSelectable(true);
	_lstSaves->setBackground(_window);
	_lstSaves->setMargin(8);
	_lstSaves->onMouseClick((ActionHandler)&LoadGameState::lstSavesClick);
	SavedGame::getList(_lstSaves, _game->getLanguage());
}

/**
 *
 */
LoadGameState::~LoadGameState()
{
	
}

/**
 * Resets the palette since it's bound to change on other screens.
 */
void LoadGameState::init()
{
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void LoadGameState::btnCancelClick(Action *action)
{
	_game->popState();
}

/**
 * Loads the selected save.
 * @param action Pointer to an action.
 */
void LoadGameState::lstSavesClick(Action *action)
{
	Ruleset *r = new XcomRuleset();
	SavedGame *s = new SavedGame(DIFF_BEGINNER);
	try
	{
		s->load(Language::wstrToUtf8(_lstSaves->getCellText(_lstSaves->getSelectedRow(), 0)), r);
		_game->setSavedGame(s);
		_game->setRuleset(r);
		if (_game->getSavedGame()->getBattleGame() == 0)
		{
			_game->setState(new GeoscapeState(_game));
		}
		else
		{
			_game->getSavedGame()->getBattleGame()->loadMapResources(_game->getResourcePack());
			_game->setState(new BattlescapeState(_game));
		}
	}
	catch (Exception &e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
		_game->pushState(new GeoscapeErrorState(_game, "STR_LOAD_UNSUCCESSFUL"));
		delete r;
		delete s;
	}
	catch (YAML::Exception &e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
		_game->pushState(new GeoscapeErrorState(_game, "STR_LOAD_UNSUCCESSFUL"));
		delete r;
		delete s;
	}
}

}
