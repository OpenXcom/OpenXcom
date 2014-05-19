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
#ifndef OPENXCOM_INTROSTATE_H
#define OPENXCOM_INTROSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

/**
 * Shows the intro cinematic.
 */
class IntroState : public State
{
private:
	std::string _introFile, _introSoundFileDOS, _introSoundFileWin;
	bool _wasLetterBoxed;
	int _oldVolume;
public:
	/// Creates the Intro state.
	IntroState(Game *game, bool wasLetterBoxed);
	/// Cleans up the Intro state.
	~IntroState();
	/// Starts the intro.
	void init();
};

}

#endif
