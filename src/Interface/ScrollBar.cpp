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
#include "ScrollBar.h"
#include "../fmath.h"
#include <algorithm>
#include "../Engine/Action.h"
#include "TextList.h"
#include "../Engine/Palette.h"

namespace OpenXcom
{

/**
 * Sets up a scrollbar with the specified size and position.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
ScrollBar::ScrollBar(int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _list(0), _color(0), _pressed(false), _contrast(false), _offset(0), _bg(0)
{
	_track = new Surface(width-2, height, x+1, y);
	_thumb = new Surface(width, height, x, y);
	_thumbRect.x = 0;
	_thumbRect.y = 0;
	_thumbRect.w = 0;
	_thumbRect.h = 0;
}

/**
 * Deletes contents.
 */
ScrollBar::~ScrollBar()
{
	delete _track;
	delete _thumb;
}

/**
 * Changes the position of the surface in the X axis.
 * @param x X position in pixels.
 */
void ScrollBar::setX(int x)
{
	Surface::setX(x);
	_track->setX(x+1);
	_thumb->setX(x);
}

/**
 * Changes the position of the surface in the Y axis.
 * @param y Y position in pixels.
 */
void ScrollBar::setY(int y)
{
	Surface::setY(y);
	_track->setY(y);
	_thumb->setY(y);
}

/**
 * Changes the height of the scrollbar.
 * @param height New height in pixels.
 */
void ScrollBar::setHeight(int height)
{
	Surface::setHeight(height);
	_track->setHeight(height);
	_thumb->setHeight(height);
	_redraw = true;
}

/**
 * Changes the color used to render the scrollbar.
 * @param color Color value.
 */
void ScrollBar::setColor(Uint8 color)
{
	_color = color;
}

/**
 * Returns the color used to render the scrollbar.
 * @return Color value.
 */
Uint8 ScrollBar::getColor() const
{
	return _color;
}

/**
 * Enables/disables high contrast color. Mostly used for
 * Battlescape text.
 * @param contrast High contrast setting.
 */
void ScrollBar::setHighContrast(bool contrast)
{
	_contrast = contrast;
}

/**
 * Changes the list associated with the scrollbar.
 * This makes the button scroll that list.
 * @param list Pointer to text list.
 */
void ScrollBar::setTextList(TextList *list)
{
	_list = list;
}

/**
 * Changes the surface used to draw the background of the track.
 * @param bg New background.
 */
void ScrollBar::setBackground(Surface *bg)
{
	_bg = bg;
}

/**
 * Replaces a certain amount of colors in the scrollbar's palette.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void ScrollBar::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	Surface::setPalette(colors, firstcolor, ncolors);
	_track->setPalette(colors, firstcolor, ncolors);
	_thumb->setPalette(colors, firstcolor, ncolors);
}

/**
 * Automatically updates the scrollbar
 * when the mouse moves.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void ScrollBar::handle(Action *action, State *state)
{
	InteractiveSurface::handle(action, state);
	if (_pressed && (action->getDetails()->type == SDL_MOUSEMOTION || action->getDetails()->type == SDL_MOUSEBUTTONDOWN))
	{
		int cursorY = action->getAbsoluteYMouse() - getY();
		int y = std::min(std::max(cursorY + _offset, 0), getHeight() - _thumbRect.h + 1);
		double scale = (double)_list->getRows() / getHeight();
		int scroll = (int)Round(y * scale);
		_list->scrollTo(scroll);
	}
}

/**
 * Blits the scrollbar contents.
 * @param surface Pointer to surface to blit onto.
 */
void ScrollBar::blit(Surface *surface)
{
	Surface::blit(surface);
	if (_visible && !_hidden)
	{
		_track->blit(surface);
		_thumb->blit(surface);
		invalidate();
	}
}

/**
 * The scrollbar only moves while the button is pressed.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void ScrollBar::mousePress(Action *action, State *state)
{
	InteractiveSurface::mousePress(action, state);
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		int cursorY = action->getAbsoluteYMouse() - getY();
		if (cursorY >= _thumbRect.y && cursorY < _thumbRect.y + _thumbRect.h)
		{
			_offset = _thumbRect.y - cursorY;
		}
		else
		{
			_offset = -_thumbRect.h / 2;
		}
		_pressed = true;		
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_WHEELUP)
	{
		_list->scrollUp(false, true);
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_WHEELDOWN)
	{
		_list->scrollDown(false, true);
	}
}

/**
 * The scrollbar stops moving when the button is released.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void ScrollBar::mouseRelease(Action *action, State *state)
{
	InteractiveSurface::mouseRelease(action, state);
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_pressed = false;
		_offset = 0;
	}
}

/**
 * Updates the thumb according to the current list position.
 */
void ScrollBar::draw()
{
	Surface::draw();
	drawTrack();
	drawThumb();
}

/**
 * Draws the track (background bar) semi-transparent.
 */
void ScrollBar::drawTrack()
{
	if (_bg)
	{
		_track->copy(_bg);
		if (_list->getComboBox())
		{
			_track->offset(+1, Palette::backPos);
		}
		else
		{
			_track->offsetBlock(-5);
		}
	}
}

/**
 * Draws the thumb (button) as a hollow square.
 */
void ScrollBar::drawThumb()
{
	double scale = (double)getHeight() / _list->getRows();
	_thumbRect.x = 0;
	_thumbRect.y = (int)floor(_list->getScroll() * scale);
	_thumbRect.w = _thumb->getWidth();
	_thumbRect.h = (int)ceil(_list->getVisibleRows() * scale);
	
	// Draw base button
	_thumb->clear();
	_thumb->lock();

	SDL_Rect square = _thumbRect;
	int color = _color + 2;

	square.w--;
	square.h--;

	_thumb->drawRect(&square, color);

	square.x++;
	square.y++;
	color = _color + 5;

	_thumb->drawRect(&square, color);

	square.w--;
	square.h--;
	color = _color + 4;

	_thumb->drawRect(&square, color);

	_thumb->setPixel(_thumbRect.x, _thumbRect.y, _color + 1);
	_thumb->setPixel(_thumbRect.x, _thumbRect.y + _thumbRect.h - 1, _color + 4);
	_thumb->setPixel(_thumbRect.x + _thumbRect.w - 1, _thumbRect.y, _color + 4);

	// Hollow it out
	if ((int)square.h - 4 > 0)
	{
		color = _color + 5;

		square.x++;
		square.y++;
		square.w -= 3;
		square.h -= 3;

		_thumb->drawRect(&square, color);

		square.x++;
		square.y++;
		color = _color + 2;

		_thumb->drawRect(&square, color);

		square.w--;
		square.h--;
		color = 0;

		_thumb->drawRect(&square, color);

		_thumb->setPixel(_thumbRect.x + 2 + _thumbRect.w - 1 - 4, _thumbRect.y + 2 + _thumbRect.h - 1 - 4, _color + 1);
		_thumb->setPixel(_thumbRect.x + 2, _thumbRect.y + 2 + _thumbRect.h - 1 - 4, _color + 4);
		_thumb->setPixel(_thumbRect.x + 2 + _thumbRect.w - 1 - 4, _thumbRect.y + 2, _color + 4);
	}
	_thumb->unlock();
}

}
