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
#include "GraphsState.h"

GraphsState::GraphsState(Game *game) : State(game)
{
	// Create objects
	_bg = new Surface(320, 200, 0, 0);
	_btnGeoscape = new InteractiveSurface(32, 24, 288, 0);
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_2")->getColors());

	add(_bg);
	add(_btnGeoscape);

	// Set up objects
	_game->getResourcePack()->getSurface("GRAPHS.SPK")->blit(_bg);

	_btnGeoscape->onMouseClick((EventHandler)&GraphsState::btnGeoscapeClick);
}

GraphsState::~GraphsState()
{
	
}

void GraphsState::btnGeoscapeClick(SDL_Event *ev, int scale)
{
	_game->setState(new GeoscapeState(_game));
}