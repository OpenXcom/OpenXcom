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

#include "FpsCounter.h"
#include <cmath>
#include "../Engine/Palette.h"
#include "../Engine/Action.h"
#include "../Engine/Timer.h"
#include "NumberText.h"

/**
 * Creates a FPS counter of the specified size.
 * @param big Pointer to the big-size font.
 * @param small Pointer to the small-size font.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
FpsCounter::FpsCounter(int width, int height, int x, int y) : Surface(width, height, x, y), _frames(0)
{
	_timer = new Timer(1000);
	_timer->onTimer((SurfaceHandler)&FpsCounter::update);
	_timer->start();

	_text = new NumberText(width, height, x, y);
	_text->setColor(10);
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
	_text->setPalette(colors, firstcolor, ncolors);
}

/**
 * Show / hide FPS counter.
 * @param action Pointer to an action.
 */
void FpsCounter::handle(Action *action)
{
	if (action->getDetails()->type == SDL_KEYDOWN && action->getDetails()->key.keysym.sym == SDLK_F12)
	{
		_visible = !_visible;
	}
}

/**
 * Advance frame counter.
 */
void FpsCounter::think()
{
	_frames++;
	_timer->think(0, this);
}

void FpsCounter::update()
{
	int fps = (int)floor((double)_frames / _timer->getTime() * 1000);
	_text->setValue(fps);
	_frames = 0;
}

/**
 * Blits the FPS counter onto another surface.
 * @param surface Pointer to another surface.
 */
void FpsCounter::blit(Surface *surface)
{
	Surface::blit(surface);
	if (_visible && !_hidden)
	{
		_text->blit(surface);
	}
}
