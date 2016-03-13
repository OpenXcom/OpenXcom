#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "../Engine/State.h"

namespace OpenXcom
{

/**
 * Shows cutscenes: inspects the relevant rules and loads the appropriate state
 * for showing slideshows or videos.
 */
class CutsceneState : public State
{
private:
	std::string _cutsceneId;
public:
	/// Creates the CutsceneState.
	CutsceneState(const std::string &cutsceneId);
	/// Cleans up the CutsceneState.
	~CutsceneState();
	/// Replace this state on the stack with the real player state.
	void init();

	static const std::string WIN_GAME;
	static const std::string LOSE_GAME;
	// shared utility methods for SlideshowState and VideoState
	/// returns whether the display was letterboxed (for restoration in resetDisplay)
	static bool initDisplay();
	/// restores the screen to its original state
	static void resetDisplay(bool wasLetterboxed);
};

}
