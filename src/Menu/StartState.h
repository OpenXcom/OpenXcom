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
#ifndef OPENXCOM_STARTSTATE_H
#define OPENXCOM_STARTSTATE_H

#include "../Engine/State.h"
#include "../Resource/XcomResourcePack.h"

namespace OpenXcom
{

class Surface;

enum LoadingPhase { LOADING_NONE, LOADING_STARTED, LOADING_FAILED, LOADING_SUCCESSFUL };

/**
 * Initializes the game and loads all required content.
 */
class StartState : public State
{
private:
	Surface *_surface;
	LoadingPhase _load;
public:
	/// Creates the Start state.
	StartState(Game *game);
	/// Cleans up the Start state.
	~StartState();
	/// Loads the game resources.
	void think();
	/// Handles key clicks.
	void handle(Action *action);
	/// This method makes our modifications to the XcomResourcePack.
	XcomResourcePack *makeModifications(XcomResourcePack *pack);
};

}

#endif
