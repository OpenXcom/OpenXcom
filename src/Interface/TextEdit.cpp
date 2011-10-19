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
TextEdit::TextEdit(int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _value(L""), _blink(true), _ascii(L'A'), _caretPos(0)
{
	_validButton = SDL_BUTTON_LEFT;

	_text = new Text(width, height, 0, 0);
	_timer = new Timer(100);
	_timer->onTimer((SurfaceHandler)&TextEdit::blink);
	_caret = new Text(16, 16, 0, 0);
	_caret->setText(L"|");
}

/**
 * Deletes contents.
 */
TextEdit::~TextEdit()
{
	delete _text;
	delete _caret;
	delete _timer;
}

/**
 * Starts the blinking animation when
 * the text edit is focused.
 */
void TextEdit::focus()
{
	if (!_isFocused)
	{
		_caretPos = _value.length();
		_blink = true;
		_timer->start();
		_redraw = true;
	}
	InteractiveSurface::focus();
}

/**
 * Changes the text edit to use the big-size font.
 */
void TextEdit::setBig()
{
	_text->setBig();
	_caret->setBig();
}

/**
 * Changes the text edit to use the small-size font.
 */
void TextEdit::setSmall()
{
	_text->setSmall();
	_caret->setSmall();
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
	_caret->setFonts(big, small);
}

/**
 * Changes the string displayed on screen.
 * @param text Text string.
 */
void TextEdit::setText(const std::wstring &text)
{
	_value = text;
	_redraw = true;
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
	_caret->setInvert(invert);
}

/**
 * Enables/disables high contrast color. Mostly used for
 * Battlescape text.
 * @param contrast High contrast setting.
 */
void TextEdit::setHighContrast(bool contrast)
{
	_text->setHighContrast(contrast);
	_caret->setHighContrast(contrast);
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
	_caret->setColor(color);
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
 * Changes the secondary color used to render the text. The text
 * switches between the primary and secondary color whenever there's
 * a 0x01 in the string.
 * @param color Color value.
 */
void TextEdit::setSecondaryColor(Uint8 color)
{
	_text->setSecondaryColor(color);
}

/**
 * Returns the secondary color used to render the text.
 * @return Color value.
 */
Uint8 TextEdit::getSecondaryColor() const
{
	return _text->getSecondaryColor();
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
	_caret->setPalette(colors, firstcolor, ncolors);
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
	_redraw = true;
}

/**
 * Adds a flashing | caret to the text
 * to show when it's focused and editable.
 */
void TextEdit::draw()
{
	Surface::draw();
	_text->setText(_value);
#ifdef DINGOO
	std::wstring newValue = _value;
	if (_isFocused && _blink)
	{
		newValue += _ascii;
		_text->setText(newValue);
    }
#endif
	clear();
	_text->blit(this);
#ifndef DINGOO
	if (_isFocused && _blink)
	{
		int x = 0;
		for (unsigned int i = 0; i < _caretPos; ++i)
		{
			if (_value[i] == ' ')
			{
				x += _text->getFont()->getWidth() / 2;
			}
			else
			{
				x += _text->getFont()->getChar(_value[i])->getCrop()->w + _text->getFont()->getSpacing();
			}
		}
		_caret->setX(x);
		_caret->blit(this);
    }
#endif
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
	for (std::wstring::iterator i = s.begin(); i < s.end(); ++i)
	{
		if (*i == ' ')
		{
			w += _text->getFont()->getWidth() / 2;
		}
		else
		{
			w += _text->getFont()->getChar(*i)->getCrop()->w + _text->getFont()->getSpacing();
		}
	}

	return (w > getWidth());
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
		if (_ascii > L'~')
		{
			_ascii = L' ';
		}
		break;
	case SDLK_DOWN:
		_ascii--;
		if (_ascii < L' ')
		{
			_ascii = L'~';
		}
		break;
	case SDLK_LEFT:
		if (_value.length() > 0)
		{
			_value.resize(_value.length() - 1);
		}
		break;
	case SDLK_RIGHT:
		if (!exceedsMaxWidth(_ascii))
		{
			_value += _ascii;
		}
		break;
#else
    case SDLK_LEFT:
        if (_caretPos > 0)
		{
			_caretPos--;
		}
        break;
    case SDLK_RIGHT:
		if (_caretPos < _value.length())
		{
			_caretPos++;
		}
        break;
    case SDLK_HOME:
        _caretPos = 0;
        break;
    case SDLK_END:
        _caretPos = _value.length();
        break;
	case SDLK_BACKSPACE:
		if (_caretPos > 0)
		{
			_value.erase(_caretPos - 1, 1);
			_caretPos--;
        }
		break;
	case SDLK_DELETE:
        if (_caretPos < _value.length())
		{
			_value.erase(_caretPos, 1);
        }
	    break;
	case SDLK_RETURN:
		_isFocused = false;
		_blink = false;
		_timer->stop();
		break;
	default:
		if (action->getDetails()->key.keysym.unicode != 0)
		{
			if (action->getDetails()->key.keysym.unicode >= L' ' && action->getDetails()->key.keysym.unicode <= L'~' && !exceedsMaxWidth((wchar_t)action->getDetails()->key.keysym.unicode))
			{
			    _value.insert(_caretPos, 1, (wchar_t)action->getDetails()->key.keysym.unicode);
				_caretPos++;
			}
		}
#endif
	}
	_redraw = true;

	InteractiveSurface::keyboardPress(action, state);
}

void TextEdit::caretAtEnd()
{
	_caretPos = _value.length();
}

}
