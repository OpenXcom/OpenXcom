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
#include "../Engine/InteractiveSurface.h"

namespace OpenXcom
{

class Font;
class Language;
class Frame;
class TextButton;
class Text;

/**
 * Horizontal slider control to select from a range of values.
 */
class Slider : public InteractiveSurface
{
private:
	Frame *_frame;
	Text *_txtMinus, *_txtPlus;
	TextButton *_button;
	double _pos;
	int _min, _max, _value;
	bool _pressed;
	ActionHandler _change;
	int _thickness, _textness, _minX, _maxX, _offsetX;
	
	/// Sets the slider's position.
	void setPosition(double pos);
public:
	/// Creates a new slider with the specified size and position.
	Slider(int width, int height, int x = 0, int y = 0);
	/// Cleans up the slider.
	~Slider();
	/// Sets the X position of the surface.
	void setX(int x);
	/// Sets the Y position of the surface.
	void setY(int y);
	/// Initializes the slider's resources.
	void initText(Font *big, Font *small, Language *lang);
	/// Sets the slider's high contrast color setting.
	void setHighContrast(bool contrast);
	/// Sets the slider's color.
	void setColor(Uint8 color);
	/// Gets the slider's color.
	Uint8 getColor() const;
	/// Sets the slider's palette.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	/// Sets the slider's range.
	void setRange(int min, int max);
	/// Sets the slider's value.
	void setValue(int value);
	/// Gets the slider's value.
	int getValue() const;
	/// Blits the slider onto another surface.
	void blit(Surface *surface);
	/// Moves the slider.
	void handle(Action *action, State *state);
	/// Special handling for mouse presses.
	void mousePress(Action *action, State *state);
	/// Special handling for mouse releases.
	void mouseRelease(Action *action, State *state);
	/// Hooks an action handler to when the slider changes.
	void onChange(ActionHandler handler);
};

}
