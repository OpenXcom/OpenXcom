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
#include "GraphsState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Engine/InteractiveSurface.h"
#include "../Savegame/Country.h"
#include "../Savegame/Region.h"
#include "../Ruleset/RuleCountry.h"
#include "../Ruleset/RuleRegion.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Graphs screen.
 * @param game Pointer to the core game.
 */
GraphsState::GraphsState(Game *game) : State(game)
{
	// Create objects
	_bg = new Surface(320, 200, 0, 0);
	_btnUfoRegion = new InteractiveSurface(32, 24, 90, 0);
	_btnUfoCountry = new InteractiveSurface(32, 24, 123, 0);
	_btnXcomRegion = new InteractiveSurface(32, 24, 156, 0);
	_btnXcomCountry = new InteractiveSurface(32, 24, 189, 0);
	_btnIncome = new InteractiveSurface(32, 24, 222, 0);
	_btnFinance = new InteractiveSurface(32, 24, 255, 0);
	_btnGeoscape = new InteractiveSurface(32, 24, 288, 0);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_2")->getColors());

	add(_bg);
	add(_btnUfoRegion);
	add(_btnUfoCountry);
	add(_btnXcomRegion);
	add(_btnXcomCountry);
	add(_btnIncome);
	add(_btnFinance);
	add(_btnGeoscape);

	// set up the grid
	SDL_Rect current;
	current.w = 188;
	current.h = 127;
	current.x = 125;
	current.y = 49;
	_bg->drawRect(&current, Palette::blockOffset(10));

	for(int grid = 0; grid !=5; ++grid)
	{
	current.w = 16 - (grid*2);
	current.h = 13 - (grid*2);
		for(int y = 50 + grid; y <= 163 + grid; y += 14)
		{
			current.y = y;
			for(int x = 126 + grid; x <= 297 + grid; x += 17)
			{
				current.x = x;
				Uint8 color = Palette::blockOffset(10)+grid+1;
				if(grid == 4)
				{
					color = 0;
				} 
				_bg->drawRect(&current, color);
			}
		}
	}


	// Set up objects
	_game->getResourcePack()->getSurface("GRAPHS.SPK")->blit(_bg);

	_btnUfoRegion->onMouseClick((ActionHandler)&GraphsState::btnUfoRegionClick);
	_btnUfoCountry->onMouseClick((ActionHandler)&GraphsState::btnUfoCountryClick);
	_btnXcomRegion->onMouseClick((ActionHandler)&GraphsState::btnXcomRegionClick);
	_btnXcomCountry->onMouseClick((ActionHandler)&GraphsState::btnXcomCountryClick);
	_btnIncome->onMouseClick((ActionHandler)&GraphsState::btnIncomeClick);
	_btnFinance->onMouseClick((ActionHandler)&GraphsState::btnFinanceClick);
	_btnGeoscape->onMouseClick((ActionHandler)&GraphsState::btnGeoscapeClick);
}

/**
 *
 */
GraphsState::~GraphsState()
{

}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void GraphsState::btnGeoscapeClick(Action *action)
{
	_game->popState();
}
void GraphsState::btnUfoRegionClick(Action *action)
{
	_game->popState();
}
void GraphsState::btnUfoCountryClick(Action *action)
{
	_game->popState();
}
void GraphsState::btnXcomRegionClick(Action *action)
{
	_game->popState();
}
void GraphsState::btnXcomCountryClick(Action *action)
{
	_game->popState();
}
void GraphsState::btnIncomeClick(Action *action)
{
	_game->popState();
}
void GraphsState::btnFinanceClick(Action *action)
{
	_game->popState();
}

}
