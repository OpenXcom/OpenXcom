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
#include "../Mod/RuleVideo.h"

namespace OpenXcom
{

class InteractiveSurface;
class Text;
class Timer;

/**
 * Shows slideshow sequences.
 */
class SlideshowState : public State
{
private:
	const SlideshowHeader &_slideshowHeader;
	const std::vector<SlideshowSlide> *_slideshowSlides;
	bool _wasLetterboxed;
	std::vector<InteractiveSurface *> _slides;
	std::vector<Text *>_captions;
	int _curScreen;
	Timer *_transitionTimer;
public:
	/// Creates the Slideshow state.
	SlideshowState(const SlideshowHeader &slideshowHeader, const std::vector<SlideshowSlide> *slideshowRule);
	/// Cleans up the Slideshow state.
	~SlideshowState();
	/// Handle timers.
	void think();
	/// Handler for waiting the screen.
	void screenTimer();
	/// Handler for clicking the screen.
	void screenClick(Action *action);
	/// Handler for skipping the screen.
	void screenSkip(Action *action);
};

}
