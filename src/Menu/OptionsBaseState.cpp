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
#include "OptionsBaseState.h"
#include "../Engine/Game.h"
#include "../Engine/Options.h"
#include "MainMenuState.h"
#include "../Geoscape/GeoscapeState.h"
#include "../Battlescape/BattlescapeState.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Options window.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 */
OptionsBaseState::OptionsBaseState(Game *game, OptionsOrigin origin) : State(game), _origin(origin)
{
}

/**
 *
 */
OptionsBaseState::~OptionsBaseState()
{

}

/**
 * Initializes UI colors according to origin.
 */
void OptionsBaseState::init()
{
	if (_origin == OPT_BATTLESCAPE)
	{
		applyBattlescapeTheme();
	}
}

/**
 * Saves options and returns to the proper origin screen.
 */
void OptionsBaseState::saveOptions()
{
	Options::save();
	if (_origin == OPT_MENU)
	{
		_game->setState(new MainMenuState(_game));
	}
	else if (_origin == OPT_GEOSCAPE)
	{
		_game->setState(new GeoscapeState(_game));
	}
	else if (_origin == OPT_BATTLESCAPE)
	{
		_game->setState(new GeoscapeState(_game));
		BattlescapeState *bs = new BattlescapeState(_game);
		_game->pushState(bs);
		_game->getSavedGame()->getSavedBattle()->setBattleState(bs);
	}
}

}
