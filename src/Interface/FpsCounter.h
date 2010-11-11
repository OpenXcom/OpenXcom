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

#ifndef OPENXCOM_FPSCOUNTER_H
#define OPENXCOM_FPSCOUNTER_H

#include "../Engine/Surface.h"

class Text;
class Timer;
class Font;
class Action;

/**
 * Counts the amount of frames each second
 * and displays them in a Text surface.
 */
class FpsCounter : public Surface
{
private:
	Text *_text;
	Timer *_timer;
	int _frames;
public:
	/// Creates a new FPS counter linked to a game.
	FpsCounter(int width, int height, int x, int y);
	/// Cleans up all the FPS counter resources.
	~FpsCounter();

	void setFonts(Font* big, Font* small);
	/// Sets the FPS counter's palette.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	/// Handles keyboard events.
	void handle(Action *action);
	/// Advances frame counter.
	void think();
	// Updates FPS counter.
	void update();
	/// Blits the FPS counter to the screen.
	void blit(Surface *surface);
};

#endif
