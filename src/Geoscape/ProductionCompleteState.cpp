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
#include "ProductionCompleteState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in a Production Complete window.
 * @param game Pointer to the core game.
 * @param item Item that finished producing.
 * @param base Base the item belongs to.
 * @param endType What ended the production.
 */
ProductionCompleteState::ProductionCompleteState(Game *game, const std::wstring &item, const std::wstring &base, productionProgress_e endType) : State(game)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 256, 160, 32, 20, POPUP_BOTH);
	_btnOk = new TextButton(120, 18, 100, 154);
	_txtMessage = new Text(246, 110, 37, 35);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtMessage);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)-1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK17.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&ProductionCompleteState::btnOkClick);

	_txtMessage->setColor(Palette::blockOffset(15)-1);
	_txtMessage->setAlign(ALIGN_CENTER);
	_txtMessage->setVerticalAlign(ALIGN_MIDDLE);
	_txtMessage->setBig();
	_txtMessage->setWordWrap(true);
	std::wstring s;
	switch(endType)
	{
	case PROGRESS_COMPLETE:
		s = _game->getLanguage()->getString("STR_PRODUCTION_OF");
		break;
	case PROGRESS_NOT_ENOUGH_MONEY:
		s = _game->getLanguage()->getString("STR_NOT_ENOUGH_MONEY_TO_PRODUCE");
		break;
	case PROGRESS_NOT_ENOUGH_MATERIALS:
		s = _game->getLanguage()->getString("STR_NOT_ENOUGH_SPECIAL_MATERIALS_TO_PRODUCE");
		break;
	default:
		assert(false);
	}
	s += item;
	s += _game->getLanguage()->getString("STR__AT__");
	s += base;
	if(endType == PROGRESS_COMPLETE)
	{
		s += _game->getLanguage()->getString("STR_IS_COMPLETE");
	}
	_txtMessage->setText(s);
}

/**
 *
 */
ProductionCompleteState::~ProductionCompleteState()
{

}

/**
 * Resets the palette.
 */
void ProductionCompleteState::init()
{
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);
}

/**
 * Closes the window.
 * @param action Pointer to an action.
 */
void ProductionCompleteState::btnOkClick(Action *action)
{
	_game->popState();
}

}
