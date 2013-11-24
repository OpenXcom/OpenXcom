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
	_surface = new InteractiveSurface(320, 200);
	_miniMapView = new MiniMapView(222, 150, 49, 15, game, camera, battleGame);
	InteractiveSurface * btnLvlUp = new InteractiveSurface(18, 20, 24, 62);
	InteractiveSurface * btnLvlDwn = new InteractiveSurface(18, 20, 24, 88);
	InteractiveSurface * btnOk = new InteractiveSurface(32, 32, 275, 145);
	_txtLevel = new Text (20, 25, 281, 75);
	add(_surface);
	add(_miniMapView);
	add(btnLvlUp);
	add(btnLvlDwn);
	add(btnOk);
	add(_txtLevel);

	centerAllSurfaces();

	if (Screen::getDY() > 50)
	{
		_screen = false;
		SDL_Rect current;
		current.w = 223;
		current.h = 151;
		current.x = 46;
		current.y = 14;
		_surface->drawRect(&current, Palette::blockOffset(15)+15);
	}

	_game->getResourcePack()->getSurface("SCANBORD.PCK")->blit(_surface);
	btnLvlUp->onMouseClick((ActionHandler)&MiniMapState::btnLevelUpClick);
	btnLvlDwn->onMouseClick((ActionHandler)&MiniMapState::btnLevelDownClick);
	btnOk->onMouseClick((ActionHandler)&MiniMapState::btnOkClick);
	btnOk->onKeyboardPress((ActionHandler)&MiniMapState::btnOkClick, (SDLKey)Options::getInt("keyCancel"));
	btnOk->onKeyboardPress((ActionHandler)&MiniMapState::btnOkClick, (SDLKey)Options::getInt("keyBattleMap"));
	_txtLevel->setBig();
	_txtLevel->setColor(Palette::blockOffset(4));
	_txtLevel->setHighContrast(true);
	std::wstringstream s;
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
	_game->popState();
}

/**
 * Changes the currently displayed minimap level.
 * @param action Pointer to an action.
 */
void MiniMapState::btnLevelUpClick(Action *)
{
	std::wstringstream s;
	s << _miniMapView->up();
	_txtLevel->setText(s.str());
}

/**
 * Changes the currently displayed minimap level.
 * @param action Pointer to an action.
 */
void MiniMapState::btnLevelDownClick(Action *)
{
	std::wstringstream s;
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
