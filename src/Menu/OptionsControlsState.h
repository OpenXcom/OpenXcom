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
#ifndef OPENXCOM__OPTIONSCONTROLSSTATE_H
#define OPENXCOM__OPTIONSCONTROLSSTATE_H

#include "../Engine/State.h"
#include <string>

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;

struct KeyOption {
	std::string option;
	std::string name;
	SDLKey key;
};

/**
 * Controls screen which allows the user to
 * customize the various key shortcuts in the game.
 */
class OptionsControlsState : public State
{
private:
	TextButton *_btnOk, *_btnCancel;
	Window *_window;
	Text *_txtTitle;
	TextList *_lstControls;
	static KeyOption _controlsGeneral[], _controlsGeo[], _controlsBattle[];
	int _countGeneral, _countGeo, _countBattle;
	int _selected;
	KeyOption *_selKey;

	void addControls(KeyOption keys[], int count);
	std::string ucWords(std::string str);
public:
	/// Creates the Saved Game state.
	OptionsControlsState(Game *game);
	/// Cleans up the Saved Game state.
	virtual ~OptionsControlsState();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
	/// Handler for clicking the Controls list.
	void lstControlsClick(Action *action);
	/// Handler for pressing a key in the Controls list.
	void lstControlsKeyPress(Action *action);
};

}

#endif
