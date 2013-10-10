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
#ifndef OPENXCOM_OPTIONSLANGUAGESTATE_H
#define OPENXCOM_OPTIONSLANGUAGESTATE_H

#include <vector>
#include <string>
#include "OptionsBaseState.h"

namespace OpenXcom
{

class Window;
class Text;
class TextList;
class TextButton;

/**
 * Language window that lets the player pick the language
 * used for all in-game text.
 */
class OptionsLanguageState : public OptionsBaseState
{
private:
	Text *_txtTitle;
	Window *_window;
	TextList *_lstLanguages;
	TextButton *_btnCancel;
	std::vector<std::string> _langs;
public:
	/// Creates the Start state.
	OptionsLanguageState(Game *game, OptionsOrigin origin);
	/// Cleans up the Start state.
	~OptionsLanguageState();
	/// Handler for clicking the Language list.
	void lstLanguagesClick(Action *action);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
};

}

#endif
