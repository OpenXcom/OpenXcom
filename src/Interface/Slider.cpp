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
#include "Slider.h"
#include <cmath>
#include <algorithm>
#include <sstream>
#include "../Engine/Action.h"
#include "../Engine/Font.h"
#include "../Interface/TextButton.h"
#include "../Interface/Frame.h"

namespace OpenXcom
{

/**
 * Sets up a slider with the specified size and position.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
Slider::Slider(int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _value(0), _min(0), _max(100), _pressed(false)
{
	int thickness = 5;
	_frame = new Frame(width, thickness, x, y + (height - thickness) / 2);
	_button = new TextButton(20, height, x, y);

	_frame->setThickness(thickness);

	_minX = getX();
	_maxX = getX() + getWidth() - _button->getWidth();

	setValue(_value);
}

/**
 * Deletes contents.
 */
Slider::~Slider()
{
	delete _frame;
	delete _button;
}

/**
 * Changes the various fonts for the slider to use.
 * @param big Pointer to large-size font.
 * @param small Pointer to small-size font.
 */
void Slider::setFonts(Font *big, Font *small)
{
	_button->setFonts(big, small);
}

/**
 * Enables/disables high contrast color. Mostly used for
 * Battlescape.
 * @param contrast High contrast setting.
 */
void Slider::setHighContrast(bool contrast)
{
	_frame->setHighContrast(contrast);
	_button->setHighContrast(contrast);
}

/**
 * Changes the color used to render the slider.
 * @param color Color value.
 */
void Slider::setColor(Uint8 color)
{
	_frame->setColor(color);
	_button->setColor(color);
}

/**
 * Returns the color used to render the slider.
 * @return Color value.
 */
Uint8 Slider::getColor() const
{
	return _button->getColor();
}

/**
 * Replaces a certain amount of colors in the slider's palette.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void Slider::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	Surface::setPalette(colors, firstcolor, ncolors);
	_frame->setPalette(colors, firstcolor, ncolors);
	_button->setPalette(colors, firstcolor, ncolors);
}

/**
* Automatically updates the slider
* when the mouse moves.
* @param action Pointer to an action.
* @param state State that the action handlers belong to.
*/
void Slider::handle(Action *action, State *state)
{
	InteractiveSurface::handle(action, state);
	//_button->handle(action, state);
	if (_pressed && (action->getDetails()->type == SDL_MOUSEMOTION || action->getDetails()->type == SDL_MOUSEBUTTONDOWN))
	{
		int cursorX = (int) floor(action->getDetails()->motion.x / action->getXScale());
		double buttonX = std::min(std::max(_minX, cursorX - _button->getWidth() / 2), _maxX);
		double pos = (buttonX - getX()) / (_maxX - _minX);
		int val = (int) floor(_min + (_max - _min) * pos);
		setValue(val);
	}
}

/**
* Moves the slider to the new value position.
* @param value New value.
*/
void Slider::setValue(int value)
{
	_value = value;

	double val = _value;
	double pos = (val - _min) / (_max - _min);
	int width = _maxX - _minX;
	_button->setX((int) floor(getX() + width * pos));

	std::wstringstream ss;
	ss << _value;
	_button->setText(ss.str());
}

/**
* Returns the current value of the slider.
* @return Value.
*/
int Slider::getValue() const
{
	return _value;
}

/**
 * Blits the slider contents.
 * to show when it's focused and editable.
 */
void Slider::blit(Surface *surface)
{
	Surface::blit(surface);
	_frame->blit(surface);
	_button->blit(surface);
}

/**
 * The slider only moves while the button is pressed.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Slider::mousePress(Action *action, State *state)
{
	InteractiveSurface::mousePress(action, state);
	_pressed = true;
}

/**
* The slider stops moving when the button is released.
* @param action Pointer to an action.
* @param state State that the action handlers belong to.
*/
void Slider::mouseRelease(Action *action, State *state)
{
	InteractiveSurface::mouseRelease(action, state);
	_pressed = false;
}


}
