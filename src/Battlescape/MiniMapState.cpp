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
#include "MiniMapState.h"
#include <iostream>
#include <sstream>
#include "../Engine/Game.h"
#include "../Engine/Screen.h"
#include "../Engine/InteractiveSurface.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Palette.h"
#include "../Interface/Text.h"
#include "MiniMapView.h"
#include "Camera.h"
#include "../Engine/Timer.h"
#include "../Engine/Action.h"
#include "../Engine/Options.h"

namespace OpenXcom
{
/**
 * Initializes all the elements in the MiniMapState screen.
 * @param game Pointer to the core game.
 * @param camera The Battlescape camera.
 * @param battleGame The Battlescape save.
 */
MiniMapState::MiniMapState (Game * game, Camera * camera, SavedBattleGame * battleGame) : State(game)
{
	if (Options::maximizeInfoScreens)
	{
		Options::baseXResolution = Screen::ORIGINAL_WIDTH;
		Options::baseYResolution = Screen::ORIGINAL_HEIGHT;
		_game->getScreen()->resetDisplay(false);
	}

	_bg = new Surface(320, 200);
	_miniMapView = new MiniMapView(221, 148, 48, 16, game, camera, battleGame);
	_btnLvlUp = new InteractiveSurface(18, 20, 24, 62);
	_btnLvlDwn = new InteractiveSurface(18, 20, 24, 88);
	_btnOk = new InteractiveSurface(32, 32, 275, 145);
	_txtLevel = new Text(20, 25, 281, 75);
	
	// Set palette
	setPalette("PAL_BATTLESCAPE");

	add(_bg);
	add(_miniMapView);
	add(_btnLvlUp);
	add(_btnLvlDwn);
	add(_btnOk);
	add(_txtLevel);

	centerAllSurfaces();

	if (_game->getScreen()->getDY() > 50)
	{
		_screen = false;
		_bg->drawRect(46, 14, 223, 151, Palette::blockOffset(15)+15);
	}

	_game->getResourcePack()->getSurface("SCANBORD.PCK")->blit(_bg);
	_btnLvlUp->onMouseClick((ActionHandler)&MiniMapState::btnLevelUpClick);
	_btnLvlDwn->onMouseClick((ActionHandler)&MiniMapState::btnLevelDownClick);
	_btnOk->onMouseClick((ActionHandler)&MiniMapState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&MiniMapState::btnOkClick, Options::keyCancel);
	_btnOk->onKeyboardPress((ActionHandler)&MiniMapState::btnOkClick, Options::keyBattleMap);
	_txtLevel->setBig();
	_txtLevel->setColor(Palette::blockOffset(4));
	_txtLevel->setHighContrast(true);
	std::wostringstream s;
	s << camera->getViewLevel();
	_txtLevel->setText(s.str());
	_timerAnimate = new Timer(125);
	_timerAnimate->onTimer((StateHandler)&MiniMapState::animate);
	_timerAnimate->start();
	_miniMapView->draw();
}

/**
 *
 */
MiniMapState::~MiniMapState()
{
	delete _timerAnimate;
}

/**
 * Handles mouse-wheeling.
 * @param action Pointer to an action.
 */
void MiniMapState::handle(Action *action)
{
	State::handle(action);
	if (action->getDetails()->type == SDL_MOUSEBUTTONDOWN)
	{
		if (action->getDetails()->button.button == SDL_BUTTON_WHEELUP)
		{
			btnLevelUpClick(action);
		}
		else if (action->getDetails()->button.button == SDL_BUTTON_WHEELDOWN)
		{
			btnLevelDownClick(action);
		}
	}
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void MiniMapState::btnOkClick(Action *)
{
	if (Options::maximizeInfoScreens)
	{
		Screen::updateScale(Options::battlescapeScale, Options::battlescapeScale, Options::baseXBattlescape, Options::baseYBattlescape, true);
		_game->getScreen()->resetDisplay(false);
	}
	_game->popState();
}

/**
 * Changes the currently displayed minimap level.
 * @param action Pointer to an action.
 */
void MiniMapState::btnLevelUpClick(Action *)
{
	std::wostringstream s;
	s << _miniMapView->up();
	_txtLevel->setText(s.str());
}

/**
 * Changes the currently displayed minimap level.
 * @param action Pointer to an action.
 */
void MiniMapState::btnLevelDownClick(Action *)
{
	std::wostringstream s;
	s << _miniMapView->down ();
	_txtLevel->setText(s.str());
}

/**
 * Animation handler. Updates the minimap view animation.
*/
void MiniMapState::animate()
{
	_miniMapView->animate();
}

/**
 * Handles timers.
*/
void MiniMapState::think ()
{
	State::think();
	_timerAnimate->think(this, 0);
}
}
