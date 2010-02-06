/*
 * Copyright 2010 Daniel Albano
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
#include "TestState.h"

TestState::TestState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(300, 180, 10, 10);
	_text = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 280, 160, 20, 50);
	_button = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 100, 20, 110, 150);
	_list = new TextList(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 300, 180, 10, 10);
	_set = game->getSurfaceSet("BASEBITS.PCK");
	_set->getFrame(1);
	
	// Set palette
	_game->setPalette(_game->getPalette("PALETTES.DAT_1")->getColors());
	_game->setPalette(_game->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(2)), Palette::backPos, 16);

	add(_window);
	add(_button);
	add(_text);
	add(_list);
	add(_set->getSurface());

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+4);
	_window->setBg(game->getSurface("BACK04.SCR"));

	_button->setColor(Palette::blockOffset(15)+4);
	_button->setText("LOLOLOL");
	
	_text->setColor(Palette::blockOffset(15)+1);
	_text->setBig();
	//_text->setWordWrap(false);
	_text->setAlign(ALIGN_CENTER);
	//_game->setLanguage("GERMAN.DAT");
	_text->setText(_game->getLanguage()->getString(251));

	_list->setColor(Palette::blockOffset(15)+1);
	_list->setColumns(3, 100, 50, 100);
	_list->addRow(2, "a", "b");
	_list->addRow(3, "lol", "welp", "yo");
	_list->addRow(1, "0123456789");
	
	_i = 0;
}

TestState::~TestState()
{
	State::~State();
}

void TestState::think()
{
	/*
	_text->setText(_game->getLanguage()->getString(_i));
	_i++;
	*/
}

SDL_Surface *TestState::testSurface()
{
	SDL_Surface *surface;

	// Create surface
	surface = SDL_CreateRGBSurface(SDL_HWSURFACE, 256, 25, 8, 0, 0, 0, 0);
	
	if (surface == NULL)
	{
		throw SDL_GetError();
	}

	// Lock the surface
	SDL_LockSurface(surface);

	Uint8 *index = (Uint8 *)surface->pixels;
	
	for (int j = 0; j < 25; j++)
		for (int i = 0; i < 256; i++, index++)
			*index = i;

	// Unlock the surface
	SDL_UnlockSurface(surface);

	return surface;
}