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
#include "CommendationState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/SoldierDiary.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Medals screen.
 * @param game Pointer to the core game.
 */
CommendationState::CommendationState(Game *game, std::vector<Soldier*> soldiersMedalled) : State(game)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(288, 16, 16, 176);
	_txtTitle = new Text(300, 16, 10, 8);
	_lstSoldiers = new TextList(288, 128, 8, 32);

	// Set palette
	setPalette("PAL_GEOSCAPE", 0);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_lstSoldiers);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(15)-1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnOk->setColor(Palette::blockOffset(15)-1);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&CommendationState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&CommendationState::btnOkClick, Options::keyOk);
	_btnOk->onKeyboardPress((ActionHandler)&CommendationState::btnOkClick, Options::keyCancel);

	_txtTitle->setColor(Palette::blockOffset(8)+5);
	_txtTitle->setText(tr("STR_MEDALS"));
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();

	_lstSoldiers->setColor(Palette::blockOffset(8)+10);
	_lstSoldiers->setColumns(2, 204, 84);
	_lstSoldiers->setSelectable(true);
	_lstSoldiers->setBackground(_window);
	_lstSoldiers->setMargin(8);

	int row = 0;
	int titleRow = 0;
	std::wstringstream ss, ss2;
	bool decrementIterator = false;

	for (std::map<std::string, RuleCommendations *>::const_iterator commList = _game->getRuleset()->getCommendation().begin(); commList != _game->getRuleset()->getCommendation().end(); ++commList)
	{
		if (decrementIterator)
		{
			--commList;
		}
		decrementIterator = false;
		ss.clear();
		ss2.clear();
		titleRow = row;

		for (std::vector<Soldier*>::iterator s = soldiersMedalled.begin() ; s != soldiersMedalled.end(); ++s)
		{
			for (std::vector<SoldierCommendations*>::const_iterator soldierComm = (*s)->getDiary()->getSoldierCommendations()->begin(); soldierComm != (*s)->getDiary()->getSoldierCommendations()->end(); ++soldierComm)
			{
				if ((*soldierComm)->getType() == (*commList).first && (*soldierComm)->isNew())
				{
					(*soldierComm)->makeOld();
					row++;

					// Soldier name
					ss2 << "   ";
					ss2 << (*s)->getName().c_str();
					_lstSoldiers->addRow(2, ss2, tr((*soldierComm)->getDecorationLevelName()).c_str());
					_lstSoldiers->setRowColor(row, Palette::blockOffset(8)+10);

					// Decide medal noun ...
					if ((*soldierComm)->getNoun() == "noNoun")
					{
						ss << tr((*soldierComm)->getType()).c_str();
					}
					else
					{
						ss << tr((*soldierComm)->getType()).arg(tr((*soldierComm)->getNoun())).c_str();
						decrementIterator = true; // In case we have more than one modular medal
					}
				}
			}
		}
        
        if (titleRow != row)
		{
			// Medal name
			_lstSoldiers->addRow(2, ss, L"");
			_lstSoldiers->setRowColor(titleRow, Palette::blockOffset(15)-1);
		}
	}	
}

/**
 *
 */
CommendationState::~CommendationState()
{
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void CommendationState::btnOkClick(Action *)
{
	_game->popState();
}

}