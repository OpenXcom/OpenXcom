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
#include "../Engine/Surface.h"

namespace OpenXcom
{

class NumberText;
class Timer;
class Action;

/**
 * Counts the amount of frames each second
 * and displays them in a NumberText surface.
 */
class FpsCounter : public Surface
{
private:
	NumberText *_text;
	Timer *_timer;
	int _frames;
public:
	/// Creates a new FPS counter linked to a game.
	FpsCounter(int width, int height, int x, int y);
	/// Cleans up all the FPS counter resources.
	~FpsCounter();
	/// Sets the FPS counter's palette.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	/// Sets the FpsCounter's color.
	void setColor(Uint8 color);
	/// Handles keyboard events.
	void handle(Action *action);
	/// Advances frame counter.
	void think();
	// Updates FPS counter.
	void update();
	/// Draws the FPS counter.
	void draw();
	void addFrame();
};

}
