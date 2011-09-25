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
#include "MiniMapState.h"
#include <iostream>
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
#include <sstream>

namespace OpenXcom
{
/**
 * Initializes all the elements in the MiniMapState screen.
 * @param game Pointer to the core game.
 * @param map The Battlescape map
*/
MiniMapState::MiniMapState (Game * game, Map * map) : State (game), _map (map), _displayedLevel(0)
{
	_surface = new InteractiveSurface(320, 200);
	_surface->loadSpk(CrossPlatform::getDataFile("UFOGRAPH/SCANBORD.PCK"));
	_miniMapView = new MiniMapView(224, 150, 46, 15, game, map);
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
	btnLvlUp->onMouseClick((ActionHandler)&MiniMapState::btnLevelUpClick);
	btnLvlDwn->onMouseClick((ActionHandler)&MiniMapState::btnLevelDownClick);
	btnOk->onMouseClick((ActionHandler)&MiniMapState::btnOkClick);
	_txtLevel->setBig();
	_txtLevel->setColor(Palette::blockOffset(4));
	std::wstringstream s;
	s << _miniMapView->getDisplayedLevel ();
	_txtLevel->setText(s.str());	
}

/**
 * return to the previous screen
 * @param action Pointer to an action.
 */
void MiniMapState::btnOkClick (Action * action)
{
	_map->setViewHeight(_miniMapView->getDisplayedLevel ());
	_game->popState();
}

/**
 * Change the currently displayed minimap level
 * @param action Pointer to an action.
 */
void MiniMapState::btnLevelUpClick (Action * action)
{
	_miniMapView->up ();
	std::wstringstream s;
	s << _miniMapView->getDisplayedLevel ();
	_txtLevel->setText(s.str());	
}

/**
 * Change the currently displayed minimap level
 * @param action Pointer to an action.
 */
void MiniMapState::btnLevelDownClick (Action * action)
{
	_miniMapView->down ();
	std::wstringstream s;
	s << _miniMapView->getDisplayedLevel ();
	_txtLevel->setText(s.str());
}
}
