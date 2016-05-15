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

#include "FpsCounter.h"
#include <cmath>
#include "../Engine/Action.h"
#include "../Engine/Timer.h"
#include "../Engine/Options.h"
#include "NumberText.h"

namespace OpenXcom
{

/**
 * Creates a FPS counter of the specified size.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
FpsCounter::FpsCounter(int width, int height, int x, int y) : Surface(width, height, x, y), _frames(0)
{
	_visible = Options::fpsCounter;

	_timer = new Timer(1000);
	_timer->onTimer((SurfaceHandler)&FpsCounter::update);
	_timer->start();

	_text = new NumberText(width, height, x, y);
}

/**
 * Deletes FPS counter content.
 */
FpsCounter::~FpsCounter()
{
	delete _text;
	delete _timer;
}

/**
 * Replaces a certain amount of colors in the FPS counter palette.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void FpsCounter::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	Surface::setPalette(colors, firstcolor, ncolors);
	_text->setPalette(colors, firstcolor, ncolors);
}

/**
 * Sets the text color of the counter.
 * @param color The color to set.
 */
void FpsCounter::setColor(Uint8 color)
{
	_text->setColor(color);
}

/**
 * Shows / hides the FPS counter.
 * @param action Pointer to an action.
 */
void FpsCounter::handle(Action *action)
{
	if (action->getDetails()->type == SDL_KEYDOWN && action->getDetails()->key.keysym.sym == Options::keyFps)
	{
		_visible = !_visible;
		Options::fpsCounter = _visible;
	}
}

/**
 * Advances frame counter.
 */
void FpsCounter::think()
{
	_timer->think(0, this);
}

/**
 * Updates the amount of Frames per Second.
 */
void FpsCounter::update()
{
	int fps = (int)floor((double)_frames / _timer->getTime() * 1000);
	_text->setValue(fps);
	_frames = 0;
	_redraw = true;
}

/**
 * Draws the FPS counter.
 */
void FpsCounter::draw()
{
	Surface::draw();
	_text->blit(this);
}

void FpsCounter::addFrame()
{
	_frames++;
}

}
