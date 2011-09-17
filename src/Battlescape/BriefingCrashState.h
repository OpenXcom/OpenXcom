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
#ifndef OPENXCOM_BRIEFINGCRASHSTATE_H
#define OPENXCOM_BRIEFINGCRASHSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class Craft;

/**
 * Briefing screen which displays info
 * about a Crash Site mission.
 */
class BriefingCrashState : public State
{
private:
	TextButton *_btnOk;
	Window *_window;
	Text *_txtTitle, *_txtUfo, *_txtCraft, *_txtBriefing;
	Craft *_craft;
public:
	/// Creates the Crash Briefing state.
	BriefingCrashState(Game *game, Craft *craft);
	/// Cleans up the Crash Briefing state.
	~BriefingCrashState();
	/// Handler for clicking the Ok button.
	void btnOkClick(Action *action);
};

}

#endif
