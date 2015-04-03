/*
 * Copyright 2010-2015 OpenXcom Developers.
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
class FlcPlayer;
/**
 * Shows the intro cinematic.
 */
class IntroState : public State
{
private:
	std::vector<std::string> _introFiles;
	std::string _introSoundFileDOS, _introSoundFileWin;
	bool _wasLetterBoxed;
	int _oldMusic, _oldSound;
	FlcPlayer *_flcPlayer;
public:
	/// Creates the Intro state.
	IntroState(bool wasLetterBoxed);
	/// Cleans up the Intro state.
	~IntroState();
	/// Starts the intro.
	void init();
  void end();
};

}

#endif
