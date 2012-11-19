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
#include "MiniMapState.h"
#include <iostream>
#include <sstream>
#include "../Engine/Game.h"
#include "../Engine/InteractiveSurface.h"
#include "../Engine/SurfaceSet.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Palette.h"
#include "../Engine/CrossPlatform.h"
#include "../Savegame/SavedGame.h"
#include "../Ruleset/MapDataSet.h"
#include "../Interface/Text.h"
#include "MiniMapView.h"
#include "Map.h"
#include "Camera.h"
#include "../Engine/Timer.h"
#include "../Engine/Action.h"

namespace OpenXcom
{
/**
 * Initializes all the elements in the MiniMapState screen.
 * @param game Pointer to the core game.
 * @param map The Battlescape map.
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
	_game->getResourcePack()->getSurface("SCANBORD.PCK")->blit(_surface);
	btnLvlUp->onMouseClick((ActionHandler)&MiniMapState::btnLevelUpClick);
	btnLvlDwn->onMouseClick((ActionHandler)&MiniMapState::btnLevelDownClick);
	btnOk->onMouseClick((ActionHandler)&MiniMapState::btnOkClick);
	_txtLevel->setBig();
	_txtLevel->setColor(Palette::blockOffset(4));
	_txtLevel->setHighContrast(true);
	std::wstringstream s;
	s << camera->getViewHeight();
	_txtLevel->setText(s.str());
	_timerAnimate = new Timer(125);
	_timerAnimate->onTimer((StateHandler)&MiniMapState::animate);
	_timerAnimate->start();
	_miniMapView->draw();
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
 * return to the previous screen
 * @param action Pointer to an action.
 */
void MiniMapState::btnOkClick (Action * action)
{
	_game->popState();
}

/**
 * Change the currently displayed minimap level
 * @param action Pointer to an action.
 */
void MiniMapState::btnLevelUpClick (Action * action)
{
	std::wstringstream s;
	s << _miniMapView->up();
	_txtLevel->setText(s.str());
}

/**
 * Change the currently displayed minimap level
 * @param action Pointer to an action.
 */
void MiniMapState::btnLevelDownClick (Action * action)
{
	std::wstringstream s;
	s << _miniMapView->down ();
	_txtLevel->setText(s.str());
}

/**
 * Animation handler. Update the minimap view animation
*/
void MiniMapState::animate()
{
	_miniMapView->animate();
}

/**
 * Handle timers
*/
void MiniMapState::think ()
{
	State::think();
	_timerAnimate->think(this, 0);
}
}
