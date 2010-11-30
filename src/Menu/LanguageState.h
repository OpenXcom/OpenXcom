/*
 * Copyright 2010 OpenXcom Developers.
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

#include <string>
#include "../Engine/State.h"

class TextButton;
class Window;

/**
 * Language window that lets the player pick a
 * language at the start of the game.
 */
class LanguageState : public State
{
private:
	TextButton *_btnEnglish, *_btnGerman, *_btnFrench, *_btnItalian, *_btnSpanish;
	Window *_window;
public:
	/// Creates the Start state.
	LanguageState(Game *game);
	/// Cleans up the Start state.
	~LanguageState();
	/// Changes the game language.
	void changeLanguage(const std::string &lang);
	/// Handler for clicking the English button.
	void btnEnglishClick(Action *action);
	/// Handler for clicking the Deutsche button.
	void btnGermanClick(Action *action);
	/// Handler for clicking the Francais button.
	void btnFrenchClick(Action *action);
	/// Handler for clicking the Italiano button.
	void btnItalianClick(Action *action);
	/// Handler for clicking the Espanol button.
	void btnSpanishClick(Action *action);
};

#endif
