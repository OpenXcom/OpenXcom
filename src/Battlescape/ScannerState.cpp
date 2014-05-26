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
#include "ScannerState.h"
#include "ScannerView.h"
#include "../Engine/InteractiveSurface.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Engine/CrossPlatform.h"
#include "../Engine/Action.h"
#include "../Engine/Palette.h"
#include "../Engine/Timer.h"
#include "../Engine/Screen.h"
#include "../Engine/Options.h"
#include "../Interface/Text.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/BattleUnit.h"
#include "../Ruleset/RuleItem.h"
#include "../Resource/ResourcePack.h"
#include <iostream>
#include <sstream>

namespace OpenXcom
{

/**
 * Initializes the Scanner State.
 * @param game Pointer to the core game.
 * @param action Pointer to an action.
 */
ScannerState::ScannerState (Game * game, BattleAction *action) : State (game), _action(action)
{
	Options::baseXResolution = Screen::ORIGINAL_WIDTH;
	Options::baseYResolution = Screen::ORIGINAL_HEIGHT;
	_game->getScreen()->resetDisplay(false);

	_surface1 = new InteractiveSurface(320, 200);
	_surface2 = new InteractiveSurface(320, 200);
	_scannerView = new ScannerView(152, 152, 56, 24, _game, _action->actor);

	if (_game->getScreen()->getDY() > 50)
	{
		_screen = false;
	}

	// Set palette
	setPalette("PAL_BATTLESCAPE");

	add(_surface2);
	add(_scannerView);
	add(_surface1);

	centerAllSurfaces();

	_game->getResourcePack()->getSurface("DETBORD.PCK")->blit(_surface1);
	_game->getResourcePack()->getSurface("DETBORD2.PCK")->blit(_surface2);

	_timerAnimate = new Timer(125);
	_timerAnimate->onTimer((StateHandler)&ScannerState::animate);
	_timerAnimate->start();

	update();
}

ScannerState::~ScannerState()
{
	delete _timerAnimate;
}

/**
 * Closes the window on right-click.
 * @param action Pointer to an action.
 */
void ScannerState::handle(Action *action)
{
	State::handle(action);
	if (action->getDetails()->type == SDL_MOUSEBUTTONDOWN && action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		Screen::updateScale(Options::battlescapeScale, Options::battlescapeScale, Options::baseXBattlescape, Options::baseYBattlescape, true);
		_game->getScreen()->resetDisplay(false);
		_game->popState();
	}
}

/**
 * Updates scanner state.
 */
void ScannerState::update()
{
	//_scannerView->draw();
}

/**
 * Animation handler. Updates the minimap view animation.
*/
void ScannerState::animate()
{
	_scannerView->animate();
}

/**
 * Handles timers.
*/
void ScannerState::think ()
{
	State::think();
	_timerAnimate->think(this, 0);
}


}
