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
#ifndef OPENXCOM_ADVANCEDOPTIONSSTATE_H
#define OPENXCOM_ADVANCEDOPTIONSSTATE_H

#include "../Engine/State.h"
#include <SDL.h>

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;
class InteractiveSurface;

/**
 * Options window that displays all
 * the settings the player can configure.
 */
class AdvancedOptionsState : public State
{
private:
	Window *_window;
	Text *_txtTitle, *_txtSubTitle, *_txtDescription;
	TextList *_lstOptions;
	unsigned int _sel;
	TextButton *_btnOk, *_btnCancel, *_btnDefault;
	std::vector<std::string> _settingSet;
public:
	/// Creates the Options state.
	AdvancedOptionsState(Game *game);
	/// Cleans up the Options state.
	~AdvancedOptionsState();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
	/// Handler for clicking the Restore Defaults button.
	void btnDefaultClick(Action *action);
	void lstOptionsClick(Action *action);
	void lstOptionsMouseOver(Action *action);
	void lstOptionsMouseOut(Action *action);
};

}

#endif
