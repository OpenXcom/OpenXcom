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
#ifndef OPENXCOM_VIDEOSTATE_H
#define OPENXCOM_VIDEOSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

/**
 * Shows video cinematics.
 */
class VideoState : public State
{
private:
	const std::vector<std::string> *_videos;
	bool _useUfoAudioSequence;
public:
	/// Creates the Intro state.
	VideoState(const std::vector<std::string> *videos, bool useUfoAudioSequence);
	/// Cleans up the Intro state.
	~VideoState();
	/// Plays the video
	void init();
};

}

#endif
