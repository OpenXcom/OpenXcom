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
#include "TextEdit.h"
#include <cmath>
#include "../Engine/Action.h"
#include "../Engine/Font.h"
#include "../Engine/Timer.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Sets up a blank text edit with the specified size and position.
 * @param state Pointer to state the text edit belongs to.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
TextEdit::TextEdit(State *state, int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _blink(true), _modal(true), _ascii(L'A'), _caretPos(0), _textEditConstraint(TEC_NONE), _change(0), _state(state)
{
	_isFocused = false;
	_text = new Text(width, height, 0, 0);
	_timer = new Timer(100);
	_timer->onTimer((SurfaceHandler)&TextEdit::blink);
	_caret = new Text(16, 17, 0, 0);
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
	// In case it was left focused
	SDL_EnableKeyRepeat(0, SDL_DEFAULT_REPEAT_INTERVAL);
	_state->setModal(0);
}

/**
 * Passes events to internal components.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void TextEdit::handle(Action *action, State *state)
{
	InteractiveSurface::handle(action, state);
	if (_isFocused && _modal && action->getDetails()->type == SDL_MOUSEBUTTONDOWN &&
		(action->getAbsoluteXMouse() < getX() || action->getAbsoluteXMouse() >= getX() + getWidth() ||
		 action->getAbsoluteYMouse() < getY() || action->getAbsoluteYMouse() >= getY() + getHeight()))
	{
		setFocus(false);
	}
}

/**
 * Controls the blinking animation when
 * the text edit is focused.
 * @param focus True if focused, false otherwise.
 * @param modal True to lock input to this control, false otherwise.
 */
void TextEdit::setFocus(bool focus, bool modal)
{
	_modal = modal;
	if (focus != _isFocused)
	{
		_redraw = true;
		InteractiveSurface::setFocus(focus);
		if (_isFocused)
		{
			SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
			_caretPos = _value.length();
			_blink = true;
			_timer->start();
			if (_modal)
				_state->setModal(this);
		}
		else
		{
			_blink = false;
			_timer->stop();
			SDL_EnableKeyRepeat(0, SDL_DEFAULT_REPEAT_INTERVAL);
			if (_modal)
				_state->setModal(0);
		}
	}
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
 * @param lang Pointer to current language.
 */
void TextEdit::initText(Font *big, Font *small, Language *lang)
{
	_text->initText(big, small, lang);
	_caret->initText(big, small, lang);
}

/**
 * Changes the string displayed on screen.
 * @param text Text string.
 */
void TextEdit::setText(const std::wstring &text)
{
	_value = text;
	_caretPos = _value.length();
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
 * Restricts the text to only numerical input or signed numerical input.
 * @param constraint TextEditConstraint to be applied.
 */
void TextEdit::setConstraint(TextEditConstraint constraint)
{
	_textEditConstraint = constraint;
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
	if (Options::keyboardMode == KEYBOARD_OFF)
	{
		std::wstring newValue = _value;
		if (_isFocused && _blink)
		{
			newValue += _ascii;
			_text->setText(newValue);
		}
	}
	clear();
	_text->blit(this);
	if (Options::keyboardMode == KEYBOARD_ON)
	{
		if (_isFocused && _blink)
		{
			int x = 0;
			switch (_text->getAlign())
			{
			case ALIGN_LEFT:
				x = 0;
				break;
			case ALIGN_CENTER:
				x = (_text->getWidth() - _text->getTextWidth()) / 2;
				break;
			case ALIGN_RIGHT:
				x = _text->getWidth() - _text->getTextWidth();
				break;
			}
			for (size_t i = 0; i < _caretPos; ++i)
			{
				x += _text->getFont()->getCharSize(_value[i]).w;
			}
			_caret->setX(x);
			int y = 0;
			switch (_text->getVerticalAlign())
			{
			case ALIGN_TOP:
				y = 0;
				break;
			case ALIGN_MIDDLE:
				y = (int)ceil((getHeight() - _text->getTextHeight()) / 2.0);
				break;
			case ALIGN_BOTTOM:
				y = getHeight() - _text->getTextHeight();
				break;
			}
			_caret->setY(y);
			_caret->blit(this);
		}
	}
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
		w += _text->getFont()->getCharSize(*i).w;
	}

	return (w > getWidth());
}

/**
 * Checks if input key character is valid to
 * be inserted at caret position in the text edit
 * without breaking the text edit constraint.
 * @param key Key code.
 * @return True if character can be inserted, False if it cannot.
 */
bool TextEdit::isValidChar(Uint16 key)
{
	switch (_textEditConstraint)
	{
	case TEC_NUMERIC_POSITIVE:
		return key >= L'0' && key <= L'9';
		break;

	// If constraint is "(signed) numeric", need to check:
	// - user does not input a character before '-' or '+'
	// - user enter either figure anywhere, or a sign at first position
	case TEC_NUMERIC:
		if (_caretPos > 0)
		{
			return key >= L'0' && key <= L'9';
		}
		else
		{
			return ((key >= L'0' && key <= L'9') || key == L'+' || key == L'-') &&
				(_value.size() == 0 || (_value[0] != L'+' && _value[0] != L'-'));
		}
		break;

	case TEC_NONE:
		return (key >= L' ' && key <= L'~') || key >= 160;
		break;

	default:
		return false;
		break;
	}
}

/**
 * Focuses the text edit when it's pressed on.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void TextEdit::mousePress(Action *action, State *state)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		if (!_isFocused)
		{
			setFocus(true);
		}
		else
		{
			double mouseX = action->getRelativeXMouse();
			double scaleX = action->getXScale();
			double w = 0;
			int c = 0;
			for (std::wstring::iterator i = _value.begin(); i < _value.end(); ++i)
			{
				if (mouseX <= w)
				{
					break;
				}
				w += (double)_text->getFont()->getCharSize(*i).w / 2 * scaleX;
				if (mouseX <= w)
				{
					break;
				}
				c++;
				w += (double) _text->getFont()->getCharSize(*i).w / 2 * scaleX;
			}
			_caretPos = c;
		}
	}
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
	if (Options::keyboardMode == KEYBOARD_OFF)
	{
		switch (action->getDetails()->key.keysym.sym)
		{
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
		default: break;
		}
	}
	else if (Options::keyboardMode == KEYBOARD_ON)
	{
		switch (action->getDetails()->key.keysym.sym)
		{
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
		case SDLK_KP_ENTER:
			if (!_value.empty())
			{
				setFocus(false);
			}
			break;
		default:
			Uint16 key = action->getDetails()->key.keysym.unicode;			
			if (isValidChar(key) && !exceedsMaxWidth((wchar_t)key))
			{
				_value.insert(_caretPos, 1, (wchar_t)action->getDetails()->key.keysym.unicode);
				_caretPos++;
			}
		}
	}
	_redraw = true;
	if (_change)
	{
		(state->*_change)(action);
	}

	InteractiveSurface::keyboardPress(action, state);
}

/**
 * Sets a function to be called every time the text changes.
 * @param handler Action handler.
 */
void TextEdit::onChange(ActionHandler handler)
{
	_change = handler;
}

}
