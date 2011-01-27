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
#include "TextEdit.h"
#include <sstream>
#include "../Engine/Action.h"
#include "../Engine/Font.h"
#include "../Engine/Timer.h"

namespace OpenXcom
{

/**
 * Sets up a blank text edit with the specified size and position.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
TextEdit::TextEdit(int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _value(L""), _blink(true), _ascii('A')
{
	_validButton = SDL_BUTTON_LEFT;

	_text = new Text(width, height, 0, 0);
	_timer = new Timer(100);
	_timer->onTimer((SurfaceHandler)&TextEdit::blink);
}

/**
 * Deletes contents.
 */
TextEdit::~TextEdit()
{
	delete _text;
	delete _timer;
}

/**
 * Starts the blinking animation when
 * the text edit is focused.
 */
void TextEdit::focus()
{
	InteractiveSurface::focus();
	_blink = true;
	_timer->start();
	draw();
}

/**
 * Changes the text edit to use the big-size font.
 */
void TextEdit::setBig()
{
	_text->setBig();
}

/**
 * Changes the text edit to use the small-size font.
 */
void TextEdit::setSmall()
{
	_text->setSmall();
}

/**
 * Changes the various fonts for the text edit to use.
 * The different fonts need to be passed in advance since the
 * text size can change mid-text.
 * @param big Pointer to large-size font.
 * @param small Pointer to small-size font.
 */
void TextEdit::setFonts(Font *big, Font *small)
{
	_text->setFonts(big, small);
}

/**
 * Changes the string displayed on screen.
 * @param text Text string.
 */
void TextEdit::setText(const std::wstring &text)
{
	_value = text;
	draw();
}

/**
 * Returns the string displayed on screen.
 * @return Text string.
 */
std::wstring TextEdit::getText() const
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
 * Changes the way the text is aligned horizontally
 * relative to the drawing area.
 * @param align Horizontal alignment.
 */
void TextEdit::setAlign(TextHAlign align)
{
	_text->setAlign(align);
}

/**
 * Changes the way the text is aligned vertically
 * relative to the drawing area.
 * @param valign Vertical alignment.
 */
void TextEdit::setVerticalAlign(TextVAlign valign)
{
	_text->setVerticalAlign(valign);
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
Uint8 TextEdit::getColor() const
{
	return _text->getColor();
}

/**
 * Replaces a certain amount of colors in the text edit's palette.
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
	std::wstringstream ss;
#ifdef DINGOO
	ss << _value << _ascii;
#else
	ss << _value << "*";
#endif
	if (_isFocused && _blink)
		_text->setText(ss.str());
	else
		_text->setText(_value);

	clear();
	_text->blit(this);
}

/**
 * Checks if adding a certain character to
 * the text edit will exceed the maximum width.
 * Used to make sure user input stays within bounds.
 * @param c Character to add.
 * @return True if it exceeds, False if it doesn't.
 */
bool TextEdit::exceedsMaxWidth(wchar_t c)
{
	int w = 0;
	std::wstring s = _value;

	s += c;
	s += '*';
	for (std::wstring::iterator i = s.begin(); i < s.end(); i++)
		w += _text->getFont()->getChar(*i)->getCrop()->w + _text->getFont()->getSpacing();

	if (w > getWidth())
		return true;
	else
		return false;
}

/**
 * Focuses the text edit when it's pressed on.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void TextEdit::mousePress(Action *action, State *state)
{
	focus();
	InteractiveSurface::mousePress(action, state);
}

/**
 * Changes the text edit according to keyboard input, and
 * unfocuses the text if Enter is pressed.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void TextEdit::keyboardPress(Action *action, State *state)
{
	switch (action->getDetails()->key.keysym.sym)
	{
#ifdef DINGOO
	case SDLK_UP:
		_ascii++;
		if (_ascii > '~')
			_ascii = ' ';
		break;
	case SDLK_DOWN:
		_ascii--;
		if (_ascii < ' ')
			_ascii = '~';
		break;
	case SDLK_RIGHT:
		if (!exceedsMaxWidth(_ascii))
			_value += _ascii;
		break;
	case SDLK_LEFT:
#endif
	case SDLK_BACKSPACE:
		if (_value.length() > 0)
			_value.resize(_value.length() - 1);
		break;
	case SDLK_RETURN:
		_isFocused = false;
		_blink = false;
		_timer->stop();
		break;
	default:
		if (action->getDetails()->key.keysym.unicode != 0)
		{
			if (action->getDetails()->key.keysym.unicode >= ' ' && action->getDetails()->key.keysym.unicode <= '~' && !exceedsMaxWidth((wchar_t)action->getDetails()->key.keysym.unicode))
				_value += (wchar_t)action->getDetails()->key.keysym.unicode;
		}
	}
	draw();

	InteractiveSurface::keyboardPress(action, state);
}

}
