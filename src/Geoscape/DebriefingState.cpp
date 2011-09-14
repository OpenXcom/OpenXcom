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
#include "DebriefingState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Engine/Music.h"
#include "../Savegame/SavedGame.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Debriefing screen.
 * @param game Pointer to the core game.
 */
DebriefingState::DebriefingState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(40, 12, 16, 180);
	_txtTitle = new Text(280, 16, 16, 8);
	_txtItem = new Text(180, 9, 16, 24);
	_txtQuantity = new Text(60, 9, 200, 24);
	_txtScore = new Text(50, 9, 260, 24);
	_txtUfoRecovery = new Text(180, 9, 16, 60);
	_txtRating = new Text(100, 9, 64, 180);
	_lstStats = new TextList(280, 80, 16, 32);
	_lstUfoRecovery = new TextList(280, 80, 16, 32);
	_lstTotal = new TextList(280, 9, 16, 12);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_0")->getColors());
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtItem);
	add(_txtQuantity);
	add(_txtScore);
	add(_txtUfoRecovery);
	add(_txtRating);
	add(_lstStats);
	add(_lstUfoRecovery);
	add(_lstTotal);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+2);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnOk->setColor(Palette::blockOffset(15)+2);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&DebriefingState::btnOkClick);

	_txtTitle->setColor(Palette::blockOffset(8)+5);
	_txtTitle->setBig();
	_txtTitle->setText(_game->getLanguage()->getString("STR_UFO_IS_RECOVERED"));

	_txtItem->setColor(Palette::blockOffset(8)+5);
	_txtItem->setText(_game->getLanguage()->getString("STR_ITEM"));

	_txtQuantity->setColor(Palette::blockOffset(8)+5);
	_txtQuantity->setText(_game->getLanguage()->getString("STR_QUANTITY_UC"));

	_txtScore->setColor(Palette::blockOffset(8)+5);
	_txtScore->setText(_game->getLanguage()->getString("STR_SCORE"));

	_txtUfoRecovery->setColor(Palette::blockOffset(8)+5);
	_txtUfoRecovery->setText(_game->getLanguage()->getString("STR_UFO_RECOVERY"));

	_txtRating->setColor(Palette::blockOffset(8)+5);
	_txtRating->setText(_game->getLanguage()->getString("STR_RATING"));

	_lstStats->setColor(Palette::blockOffset(15)-1);
	_lstStats->setSecondaryColor(Palette::blockOffset(8)+10);
	_lstStats->setColumns(3, 184, 60, 64);
	_lstStats->setDot(true);

	_lstUfoRecovery->setColor(Palette::blockOffset(15)-1);
	_lstUfoRecovery->setSecondaryColor(Palette::blockOffset(8)+10);
	_lstUfoRecovery->setColumns(3, 184, 60, 64);
	_lstUfoRecovery->setDot(true);

	_lstTotal->setColor(Palette::blockOffset(8)+5);
	_lstTotal->setColumns(2, 224, 64);
	_lstTotal->setDot(true);

	// Set music
	_game->getResourcePack()->getMusic("GMMARS")->play();
}

/**
 *
 */
DebriefingState::~DebriefingState()
{

}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void DebriefingState::btnOkClick(Action *action)
{
	_game->popState();
}

}
