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
#include <sstream>
#include <iomanip>
#include <fstream>
#include "dirent.h"
#include "yaml.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/GameTime.h"
#include "../Engine/Game.h"
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
	_btnCancel->setText(_game->getResourcePack()->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&LoadGameState::btnCancelClick);

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getResourcePack()->getLanguage()->getString("STR_SELECT_GAME_TO_LOAD"));

	_txtName->setColor(Palette::blockOffset(15)-1);
	_txtName->setText(_game->getResourcePack()->getLanguage()->getString("STR_NAME"));

	_txtTime->setColor(Palette::blockOffset(15)-1);
	_txtTime->setText(_game->getResourcePack()->getLanguage()->getString("STR_TIME"));

	_txtDate->setColor(Palette::blockOffset(15)-1);
	_txtDate->setText(_game->getResourcePack()->getLanguage()->getString("STR_DATE"));
	
	_lstSaves->setColor(Palette::blockOffset(8)+10);
	_lstSaves->setArrowColor(Palette::blockOffset(8)+8);
	_lstSaves->setColumns(5, 168, 30, 30, 30, 30);
	_lstSaves->setSelectable(true);
	_lstSaves->setBackground(_window);
	_lstSaves->setMargin(8);
	_lstSaves->onMouseClick((ActionHandler)&LoadGameState::lstSavesClick);
	getSavesList("./USER/");
}

/**
 *
 */
LoadGameState::~LoadGameState()
{
	
}

/**
 * Gets all the saves found in a certain folder
 * and adds them to the Saves list.
 * @param dir Save directory.
 */
void LoadGameState::getSavesList(const std::string &dir)
{
	DIR *dp = opendir(dir.c_str());
    if (dp == 0)
	{
        throw "Failed to open saves directory";
    }
	
    struct dirent *dirp;
    while ((dirp = readdir(dp)) != 0)
	{
		std::string file = dirp->d_name;
		// Check if it's a valid save
		if (file.find(".sav") == std::string::npos)
		{
			continue;
		}
		std::string fullname = dir + file;
		std::ifstream fin(fullname.c_str());
		if (!fin)
		{
			throw "Failed to load savegame";
		}
		YAML::Parser parser(fin);
		YAML::Node doc;
		
		parser.GetNextDocument(doc);
		GameTime time = GameTime(6, 1, 1, 1999, 12, 0, 0);
		time.load(doc["time"]);
		std::stringstream saveTime, saveDay, saveMonth, saveYear;
		saveTime << time.getHour() << ":" << std::setfill('0') << std::setw(2) << time.getMinute();
		saveDay << time.getDay() << _game->getResourcePack()->getLanguage()->getString(time.getDayString());
		saveMonth << _game->getResourcePack()->getLanguage()->getString(time.getMonthString());
		saveYear << time.getYear();
		_lstSaves->addRow(5, file.substr(0, file.length()-4).c_str(), saveTime.str().c_str(), saveDay.str().c_str(), saveMonth.str().c_str(), saveYear.str().c_str());
		fin.close();
    }
    closedir(dp);
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
	_game->setRuleset(new XcomRuleset());
	SavedGame *s = new SavedGame(DIFF_BEGINNER);
	s->load(_lstSaves->getCell(_lstSaves->getSelectedRow(), 0)->getText(), _game->getRuleset());
	_game->setSavedGame(s);
	if (_game->getSavedGame()->getBattleGame() == 0)
	{
		_game->setState(new GeoscapeState(_game));
	}
	else
	{
		_game->setState(new BattlescapeState(_game));
	}
}

}
