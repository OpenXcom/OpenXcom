/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include "../Engine/Game.h"
#include "../Engine/Timer.h"
#include "../Interface/Text.h"
#include "../Interface/Frame.h"
#include "../Engine/Action.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"

namespace OpenXcom
{

/**
 * Initializes all the elements.
 * @param game Pointer to the core game.
 * @param msg Message string.
 */
InfoboxState::InfoboxState(const std::wstring &msg)
{
	_screen = false;

	// Create objects
	_frame = new Frame(261, 122, 34, 10);
	_text = new Text(251, 112, 39, 15);

	// Set palette
	_game->getSavedGame()->getSavedBattle()->setPaletteByDepth(this);

	add(_frame, "infoBox", "battlescape");
	add(_text, "infoBox", "battlescape");

	centerAllSurfaces();

	_frame->setHighContrast(true);
	_frame->setThickness(9);

	_text->setAlign(ALIGN_CENTER);
	_text->setVerticalAlign(ALIGN_MIDDLE);
	_text->setBig();
	_text->setWordWrap(true);
	_text->setText(msg);
	_text->setHighContrast(true);

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
 * Closes the window.
 */
void InfoboxState::close()
{
	_game->popState();
}

}
