/*
 * Copyright 2010 Daniel Albano
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
#include "TextEdit.h"
#include <sstream>
#include "Font.h"
#include "Timer.h"

using namespace std;

/**
 * Sets up a blank text with the specified size and position.
 * The different fonts need to be passed in advance since the
 * text size can change mid-text.
 * @param big Pointer to the big-size font.
 * @param small Pointer to the small-size font.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
TextEdit::TextEdit(Font *big, Font *small, int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _value(""), _blink(true)
{
	_text = new Text(big, small, width, height, 0, 0);
	_timer = new Timer(100);
	_timer->onTimer((SurfaceHandler)&TextEdit::blink);
}

/**
 *
 */
TextEdit::~TextEdit()
{
	delete _text;
}

/**
 * Starts the blinking animation when
 * the text is focused.
 */
void TextEdit::focus()
{
	InteractiveSurface::focus();
	_blink = true;
	_timer->start();
	draw();
}

/**
 * Changes the text to use the big-size font.
 */
void TextEdit::setBig()
{
	_text->setBig();
}

/**
 * Changes the text to use the small-size font.
 */
void TextEdit::setSmall()
{
	_text->setSmall();
}

/**
 * Changes the string displayed on screen.
 * @param text Text string.
 */
void TextEdit::setText(string text)
{
	_value = text;
	draw();
}

/**
 * Returns the string displayed on screen.
 * @return Text string.
 */
string TextEdit::getText()
{
	return _value;
}

/**
 * Enables/disables text wordwrapping. When enabled, lines of
 * text are automatically split to ensure they stay within the
 * drawing area, otherwise they simply go off the edge.
 * @param wrap Wordwrapping setting.
 */
void TextEdit::setWordWrap(bool wrap)
{
	_text->setWordWrap(wrap);
}

/**
 * Enables/disables color inverting. Mostly used to make
 * button text look pressed along with the button.
 * @param invert Invert setting.
 */
void TextEdit::setInvert(bool invert)
{
	_text->setInvert(invert);
}

/**
 * Changes the way the text is aligned relative to the
 * drawing area.
 * @param align Text alignment.
 */
void TextEdit::setAlign(TextAlign align)
{
	_text->setAlign(align);
}

/**
 * Changes the color used to render the text. Unlike regular graphics,
 * fonts are greyscale so they need to be assigned a specific position
 * in the palette to be displayed.
 * @param color Color value.
 */
void TextEdit::setColor(Uint8 color)
{
	_text->setColor(color);
}

/**
 * Returns the color used to render the text.
 * @return Color value.
 */
Uint8 TextEdit::getColor()
{
	return _text->getColor();
}

/**
 * Replaces a certain amount of colors in the surface's palette.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void TextEdit::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	Surface::setPalette(colors, firstcolor, ncolors);
	_text->setPalette(colors, firstcolor, ncolors);
}

/**
 * Keeps the animation timers running.
 */
void TextEdit::think()
{
	_timer->think(0, this);
}

/**
 * Plays the blinking animation when the
 * text edit is focused.
 */
void TextEdit::blink()
{
	_blink = !_blink;
	draw();
}

/**
 * Adds a flashing * to the end of the text
 * to show when it's focused and editable.
 */
void TextEdit::draw()
{
	stringstream ss;
	ss << _value << "*";
	if (_isFocused && _blink)
		_text->setText(ss.str());
	else
		_text->setText(_value);

	clear();
	_text->blit(this);
}

/**
 * Focuses the text when it's pressed on.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void TextEdit::mousePress(SDL_Event *ev, int scale, State *state)
{
	if (ev->button.button == SDL_BUTTON_LEFT)
	{
		focus();
		InteractiveSurface::mousePress(ev, scale, state);
	}
}

/**
 * Only accepts left clicks.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void TextEdit::mouseRelease(SDL_Event *ev, int scale, State *state)
{
	if (ev->button.button == SDL_BUTTON_LEFT)
	{
		InteractiveSurface::mouseRelease(ev, scale, state);
	}
}

/**
 * Only accepts left clicks.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void TextEdit::mouseClick(SDL_Event *ev, int scale, State *state)
{
	if (ev->button.button == SDL_BUTTON_LEFT)
	{
		InteractiveSurface::mouseClick(ev, scale, state);
	}
}

/**
 * Changes the text according to keyboard input, and unfocuses the
 * text if Enter is pressed.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void TextEdit::keyboardPress(SDL_Event *ev, int scale, State *state)
{
	switch (ev->key.keysym.sym)
	{
	case SDLK_RETURN:
		_isFocused = false;
		_blink = false;
		_timer->stop();
		break;
	case SDLK_BACKSPACE:
		if (_value.length() > 0)
			_value.resize(_value.length() - 1);
		break;
	default:
		if (ev->key.keysym.unicode != 0)
		{
			int w = 0;
			string s = _value;
			s += (char)ev->key.keysym.unicode;
			s += '*';
			for (string::iterator c = s.begin(); c < s.end(); c++)
				w += _text->getFont()->getChar(*c)->getCrop()->w + _text->getFont()->getSpacing();

			if (w <= _width)
				_value += (char)ev->key.keysym.unicode;
		}
	}
	draw();

	InteractiveSurface::keyboardPress(ev, scale, state);
}
