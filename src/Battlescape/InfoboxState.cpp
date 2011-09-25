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
#include "InfoboxState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Timer.h"
#include "../Interface/Text.h"
#include "../Interface/Window.h"

namespace OpenXcom
{

/**
 * Initializes all the elements.
 * @param game Pointer to the core game.
 * @param msg Pointer to  the message.
 */
InfoboxState::InfoboxState(Game *game, const std::wstring &msg) : State(game)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 261, 122, 34, 10);
	_text = new Text(175, 40, 46, 40);

	add(_window);

	_text->setAlign(ALIGN_CENTER);
	_text->setFonts(_game->getResourcePack()->getFont("Big.fnt"), 0);
	_text->setBig();
	_text->setText(msg);
	_text->setColor(Palette::blockOffset(0));
	_text->setHighContrast(true);
	_text->setPalette(_window->getPalette());

	_timer = new Timer(INFOBOX_DELAY);
	_timer->onTimer((StateHandler)&InfoboxState::close);
	_timer->start();

	draw();
}

/**
 *
 */
InfoboxState::~InfoboxState()
{

}


/**
 * Draws the bordered box.
 */
void InfoboxState::draw()
{
	SDL_Rect square;
	Uint8 color = 11;

	_window->clear();

	square.x = 0;
	square.w = _window->getWidth();

	square.y = 0;
	square.h = _window->getHeight();

	for (int i = 0; i < 9; ++i)
	{
		if (i == 8)
			color -= 4;
		_window->drawRect(&square, color);
		if (i < 3)
			color-=2;
		else
			color+=2;
		square.x++;
		square.y++;
		if (square.w >= 2)
			square.w -= 2;
		else
			square.w = 1;

		if (square.h >= 2)
			square.h -= 2;
		else
			square.h = 1;
	}

	_text->draw();
	_text->blit(_window);
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
