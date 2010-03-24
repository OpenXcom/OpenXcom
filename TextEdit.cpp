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

TextEdit::TextEdit(Font *big, Font *small, int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _value("")
{
	_text = new Text(big, small, width, height, 0, 0);
	_i = 0;
}

TextEdit::~TextEdit()
{
	delete _text;
}

void TextEdit::setBig()
{
	_text->setBig();
}

void TextEdit::setSmall()
{
	_text->setSmall();
}

void TextEdit::setText(string text)
{
	_value = text;
}

string TextEdit::getText()
{
	return _value;
}

void TextEdit::setWordWrap(bool wrap)
{
	_text->setWordWrap(wrap);
}

void TextEdit::setInvert(bool invert)
{
	_text->setInvert(invert);
}

void TextEdit::setAlign(TextAlign align)
{
	_text->setAlign(align);
}

void TextEdit::setColor(Uint8 color)
{
	_text->setColor(color);
}

Uint8 TextEdit::getColor()
{
	return _text->getColor();
}

void TextEdit::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	Surface::setPalette(colors, firstcolor, ncolors);
	_text->setPalette(colors, firstcolor, ncolors);
}

void TextEdit::blit(Surface *surface)
{
	clear();

	if (_isFocused)
		_i = (_i + 1) % 30;
	else
		_i = 0;

	stringstream ss;
	ss << _value << "*";
	if (_isFocused && _i < 15)
		_text->setText(ss.str());
	else
		_text->setText(_value);
	_text->blit(this);

	Surface::blit(surface);
}

void TextEdit::handle(SDL_Event *ev, int scale, State *state)
{
	if (ev->button.button == SDL_BUTTON_LEFT)
		InteractiveSurface::handle(ev, scale, state);
}

void TextEdit::mousePress(SDL_Event *ev, int scale, State *state)
{
	_isFocused = true;

	InteractiveSurface::mousePress(ev, scale, state);
}

void TextEdit::mouseRelease(SDL_Event *ev, int scale, State *state)
{
	InteractiveSurface::mouseRelease(ev, scale, state);
}

void TextEdit::keyboardPress(SDL_Event *ev, int scale, State *state)
{
	switch (ev->key.keysym.sym)
	{
	case SDLK_RETURN:
		_isFocused = false;
		break;
	case SDLK_BACKSPACE:
		if (_value.length() > 0)
			_value.resize(_value.length() - 1);
		break;
	default:
		if (ev->key.keysym.sym >= '!' && ev->key.keysym.sym < 162)
		{
			if (ev->key.keysym.mod & KMOD_CAPS || ev->key.keysym.mod & KMOD_LSHIFT || ev->key.keysym.mod & KMOD_RSHIFT)
				_value += toupper(ev->key.keysym.sym);
			else
				_value += ev->key.keysym.sym;
		}
	}

	InteractiveSurface::keyboardPress(ev, scale, state);
}

void TextEdit::keyboardRelease(SDL_Event *ev, int scale, State *state)
{
	InteractiveSurface::keyboardRelease(ev, scale, state);
}