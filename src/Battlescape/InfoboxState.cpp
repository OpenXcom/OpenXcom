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
#include "InfoboxState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Timer.h"
#include "../Interface/Text.h"
#include "../Interface/Window.h"
#include "../Engine/Action.h"

namespace OpenXcom
{

/**
 * Initializes all the elements.
 * @param game Pointer to the core game.
 * @param msg Message string.
 */
InfoboxState::InfoboxState(Game *game, const std::wstring &msg) : State(game)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 261, 122, 34, 10);
	_text = new Text(251, 112, 39, 15);

	add(_window);
	add(_text);

	centerAllSurfaces();

	_window->setColor(Palette::blockOffset(0));
	_window->setHighContrast(true);

	_text->setAlign(ALIGN_CENTER);
	_text->setVerticalAlign(ALIGN_MIDDLE);
	_text->setBig();
	_text->setWordWrap(true);
	_text->setText(msg);
	_text->setColor(Palette::blockOffset(0));
	_text->setHighContrast(true);
	_text->setPalette(_window->getPalette());

	_timer = new Timer(INFOBOX_DELAY);
	_timer->onTimer((StateHandler)&InfoboxState::close);
	_timer->start();
}

/**
 *
 */
InfoboxState::~InfoboxState()
{
	delete _timer;
}

/**
 * Closes the window.
 * @param action Pointer to an action.
 */
void InfoboxState::handle(Action *action)
{
	State::handle(action);

	if (action->getDetails()->type == SDL_KEYDOWN || action->getDetails()->type == SDL_MOUSEBUTTONDOWN)
	{
		close();
	}
}


/**
 * Keeps the animation timers running.
 */
void InfoboxState::think()
{
	_timer->think(this, 0);
}

/**
 * closes the window.
 */
void InfoboxState::close()
{
	_game->popState();
}



}
