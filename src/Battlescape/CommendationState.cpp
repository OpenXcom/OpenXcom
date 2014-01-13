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
	_txtName = new Text(114, 9, 16, 32);
	_txtMedal = new Text(90, 9, 130, 32);
	_txtDecoration = new Text(80, 9, 220, 32);
	_lstSoldiers = new TextList(288, 128, 8, 40);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);
	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtName);
	add(_txtMedal);
	add(_txtDecoration);
	add(_lstSoldiers);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(15)-1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnOk->setColor(Palette::blockOffset(15)-1);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&CommendationState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&CommendationState::btnOkClick, (SDLKey)Options::getInt("keyOk"));
	_btnOk->onKeyboardPress((ActionHandler)&CommendationState::btnOkClick, (SDLKey)Options::getInt("keyCancel"));

	_txtTitle->setColor(Palette::blockOffset(8)+5);
	_txtTitle->setText(tr("STR_MEDALS"));
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();

	_txtName->setColor(Palette::blockOffset(15)-1);
	_txtName->setText(tr("STR_NAME"));

	_txtMedal->setColor(Palette::blockOffset(15)-1);
	_txtMedal->setText(tr("STR_NEW_MEDAL"));

	_txtDecoration->setColor(Palette::blockOffset(15)-1);
	_txtDecoration->setText(tr("STR_MEDAL_AWARD_LEVEL"));

	_lstSoldiers->setColor(Palette::blockOffset(8)+10);
	_lstSoldiers->setColumns(3, 114, 90, 84);
	_lstSoldiers->setSelectable(true);
	_lstSoldiers->setBackground(_window);
	_lstSoldiers->setMargin(8);

	for (std::vector<Soldier*>::iterator i = soldiersMedalled.begin() ; i != soldiersMedalled.end(); ++i)
	{
		for (std::vector<SoldierCommendations*>::iterator j = (*i)->getDiary()->getSoldierCommendations()->begin() ; j != (*i)->getDiary()->getSoldierCommendations()->end() ; ++j)
		{
			if ((*j)->isNew())
			{
				if ((*j)->getNoun() == "")
					_lstSoldiers->addRow(3, (*i)->getName().c_str(), tr((*j)->getCommendationName()).c_str(), tr((*j)->getDecorationLevelName()).c_str());
				else
					_lstSoldiers->addRow(3, (*i)->getName().c_str(), tr((*j)->getCommendationName()).arg(tr((*j)->getNoun())).c_str(), tr((*j)->getDecorationLevelName()).c_str());
				(*j)->makeOld();
			}
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