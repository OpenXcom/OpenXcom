/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#include "DelayedSaveState.h"
#include "BattlescapeState.h"
#include "../Menu/SaveGameState.h"
#include "../Engine/Game.h"

namespace OpenXcom
{

/**
 * Runs on construction of the state.
 * Very simple state, runs interference between the autosave and the screen clearing on the "new turn" screen.
 * This all happens before panic is handled, and forces the text to be rendered over the battlescape rather than the "next turn" screen.
 */
DelayedSaveState::DelayedSaveState(BattlescapeGame *parent, Game *game, SaveType type) : BattleState(parent), _game(game), _type(type), _firstRun(true)
{
}

/**
 * Runs on deletion of the state.
 */
DelayedSaveState::~DelayedSaveState()
{
}

/**
 * Runs on initialization of the state.
 */
void DelayedSaveState::init()
{
	_parent->setStateInterval(BattlescapeState::DEFAULT_ANIM_SPEED);
}

/**
 * Runs on every frame of the state.
 */
void DelayedSaveState::think()
{
	if (!_firstRun)
	{
		_firstRun = false;
	}
	else
	{
		_parent->popState();
		_game->pushState(new SaveGameState(_game, OPT_BATTLESCAPE, _type));
	}
}

/**
 * Runs for mayor of the state.
 */
}