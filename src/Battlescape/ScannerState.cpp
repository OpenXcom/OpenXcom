/*
 * Copyright 2011 OpenXcom Developers.
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
 * Initialize the Scanner State
 * @param game Pointer to the core game.
 * @param action Pointer to action data canister
 */
ScannerState::ScannerState (Game * game, BattleAction *action) : State (game), _action(action)
{
	_surface1 = new InteractiveSurface(320, 200);
	_surface1->loadSpk(CrossPlatform::getDataFile("UFOGRAPH/DETBORD.PCK"));
	_surface1->onMouseClick((ActionHandler)&ScannerState::mouseClick);

	_surface2 = new InteractiveSurface(320, 200);
	_surface2->loadSpk(CrossPlatform::getDataFile("UFOGRAPH/DETBORD2.PCK"));
	_surface2->onMouseClick((ActionHandler)&ScannerState::mouseClick);

	_scannerView = new ScannerView(152, 152, 56, 24, _game, _action->actor);

	add(_surface2);
	add(_scannerView);
	add(_surface1);

	_timerAnimate = new Timer(125);
	_timerAnimate->onTimer((StateHandler)&ScannerState::animate);
	_timerAnimate->start();

	update();
}

/**
 * Update scanner state
 */
void ScannerState::update()
{
	//_scannerView->draw();
}

/**
 * @param action Pointer to an action.
 */
void ScannerState::mouseClick(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		_game->popState();
	}
}

/**
 * Animation handler. Update the minimap view animation
*/
void ScannerState::animate()
{
	_scannerView->animate();
}

/**
 * Handle timers
*/
void ScannerState::think ()
{
	State::think();
	_timerAnimate->think(this, 0);
}


}
