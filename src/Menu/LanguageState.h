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
#ifndef OPENXCOM_LANGUAGESTATE_H
#define OPENXCOM_LANGUAGESTATE_H

#include <vector>
#include <string>
#include "../Engine/State.h"

namespace OpenXcom
{

class Window;
class Text;
class TextList;

/**
 * Language window that lets the player pick a
 * language at the start of the game.
 */
class LanguageState : public State
{
private:
	Window *_window;
	TextList *_lstLanguages;
	std::vector<std::string> _langs;
public:
	/// Creates the Start state.
	LanguageState(Game *game);
	/// Cleans up the Start state.
	~LanguageState();
	/// Handler for clicking the Language list.
	void lstLanguagesClick(Action *action);
};

}

#endif
