/*
 * Copyright 2010 Daniel Albano
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
#ifndef OPENXCOM__OPTIONSSTATE_H
#define OPENXCOM__OPTIONSSTATE_H

#include "State.h"

class TextButton;
class Window;
class Text;

/**
 * Options window shown in the Geoscape for loading/saving
 * and quitting the game.
 * Not to be confused with an actual Settings window that
 * might come later for changing game settings during runtime.
 */
class OptionsState : public State
{
private:
	TextButton *_btnLoad, *_btnSave, *_btnAbandon, *_btnCancel;
	Window *_window;
	Text *_txtTitle;
public:
	/// Creates the Options state.
	OptionsState(Game *game);
	/// Cleans up the Options state.
	~OptionsState();
	/// Handler for clicking the Load Game button.
	void btnLoadClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Save Game button.
	void btnSaveClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Abandon Game button.
	void btnAbandonClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(SDL_Event *ev, int scale);
};

#endif
