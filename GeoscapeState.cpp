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
#include "GeoscapeState.h"

GeoscapeState::GeoscapeState(Game *game) : State(game)
{
	// Create objects
	_bg = _game->getSurface("GEOBORD.SCR");
	_globe = new Globe(130, 100, 256, 200, 0, 0);

	if (_game->getLanguageName() != "ENGLISH.DAT")
	{
		if (_game->getLanguageName() == "GERMAN.DAT")
			_sidebar = _game->getSurface("LANG1.DAT");
		else if (_game->getLanguageName() == "FRENCH.DAT")
			_sidebar = _game->getSurface("LANG2.DAT");
		_sidebar->setX(320 - _sidebar->getWidth());
		_sidebar->setY(0);
	}
	else
	{
		_sidebar = NULL;
	}
	
	// Set palette
	_globe->setPalette(_game->getScreen()->getPalette());

	// Set up objects
	_globe->loadDat("../UFO/GEODATA/WORLD.DAT");
	_globe->setTexture(game->getSurfaceSet("TEXTURE.DAT"));
}

GeoscapeState::~GeoscapeState()
{
	delete _bg;
	delete _sidebar;
	delete _globe;
	State::~State();
}

void GeoscapeState::handle(SDL_Event *ev, int scale)
{
}

void GeoscapeState::think()
{
}

void GeoscapeState::blit()
{
	_bg->blit(_game->getScreen()->getSurface());
	if (_sidebar != NULL)
		_sidebar->blit(_game->getScreen()->getSurface());
	_globe->blit(_game->getScreen()->getSurface());
}