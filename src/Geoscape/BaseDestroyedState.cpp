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
#include "BaseDestroyedState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"

namespace OpenXcom
{

BaseDestroyedState::BaseDestroyedState(Game *game, Base *base) : State(game), _base(base)
{
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(7)), Palette::backPos, 16);
	_screen = false;
	// Create objects
	_window = new Window(this, 256, 160, 32, 20);
	_btnConfirm = new TextButton(100, 20, 110, 142);
	_txtMessage = new Text(224, 48, 48, 76);
	
	add(_window);
	add(_btnConfirm);
	add(_txtMessage);
	
	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK15.SCR"));

	_btnConfirm->setColor(Palette::blockOffset(8)+5);
	_btnConfirm->setText(_game->getLanguage()->getString("STR_OK"));
	_btnConfirm->onMouseClick((ActionHandler)&BaseDestroyedState::btnConfirmClick);
		
	_txtMessage->setAlign(ALIGN_CENTER);
	_txtMessage->setBig();
	_txtMessage->setWordWrap(true);
	_txtMessage->setColor(Palette::blockOffset(8)+5);
	std::wstringstream ss;
	ss << _game->getLanguage()->getString("STR_THE_ALIENS_HAVE_DESTROYED_THE_UNDEFENDED_BASE") << _base->getName();
	_txtMessage->setText(ss.str());
}

/**
 *
 */
BaseDestroyedState::~BaseDestroyedState()
{
}

/**
 *
 */
void BaseDestroyedState::init()
{
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void BaseDestroyedState::btnConfirmClick(Action *)
{
	_game->popState();
	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
		if ((*i) == _base)
		{
			delete (*i);
			_game->getSavedGame()->getBases()->erase(i);
			break;
		}
	}
}

}