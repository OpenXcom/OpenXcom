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
#ifndef OPENXCOM_STARTSTATE_H
#define OPENXCOM_STARTSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class Surface;

enum LoadingPhase { LOADING_STARTED, LOADING_FAILED, LOADING_SUCCESSFUL, LOADING_DONE };

/**
 * Initializes the game and loads all required content.
 */
class StartState : public State
{
private:
	Surface *_surface;
	SDL_Thread *_thread;
	bool _wasLetterBoxed;
public:
	static LoadingPhase loading;
	static std::string error;

	/// Creates the Start state.
	StartState(Game *game);
	/// Cleans up the Start state.
	~StartState();
	/// Reset everything.
	void init();
	/// Displays messages.
	void think();
	/// Handles key clicks.
	void handle(Action *action);
	/// Flash the window.
	void flash();
	/// Loads the game resources.
	static int load(void *game_ptr);
};

}

#endif
