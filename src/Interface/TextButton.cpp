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
#include "TextButton.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include "Text.h"
#include "../Engine/Sound.h"
#include "../Engine/Action.h"
#include "ComboBox.h"

namespace OpenXcom
{

Sound *TextButton::soundPress;

/**
 * Sets up a text button with the specified size and position.
 * The text is centered on the button.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
TextButton::TextButton(int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _color(0), _group(0), _contrast(false), _geoscapeButton(false), _comboBox(0)
{
	_text = new Text(width, height, 0, 0);
	_text->setSmall();
	_text->setAlign(ALIGN_CENTER);
	_text->setVerticalAlign(ALIGN_MIDDLE);
	_text->setWordWrap(true);
}

/**
 * Deletes the contained Text.
 */
TextButton::~TextButton()
{
	delete _text;
}

bool TextButton::isButtonHandled(Uint8 button)
{
	if (_comboBox != 0)
	{
		return (button == SDL_BUTTON_LEFT);
	}
	else
	{
		return InteractiveSurface::isButtonHandled(button);
	}
}

/**
 * Changes the color for the button and text.
 * @param color Color value.
 */
void TextButton::setColor(Uint8 color)
{
	_color = color;
	_text->setColor(color);
	_redraw = true;
}

/**
 * Returns the color for the button and text.
 * @return Color value.
 */
Uint8 TextButton::getColor() const
{
	return _color;
}

/**
 * Changes the color for the text only.
 * @param color Color value.
 */
void TextButton::setTextColor(Uint8 color)
{
	_text->setColor(color);
	_redraw = true;
}

/**
 * Changes the text to use the big-size font.
 */
void TextButton::setBig()
{
	_text->setBig();
	_redraw = true;
}

/**
 * Changes the text to use the small-size font.
 */
void TextButton::setSmall()
{
	_text->setSmall();
	_redraw = true;
}

/**
 * Returns the font currently used by the text.
 * @return Pointer to font.
 */
Font *TextButton::getFont() const
{
	return _text->getFont();
}

/**
 * Changes the various resources needed for text rendering.
 * The different fonts need to be passed in advance since the
 * text size can change mid-text, and the language affects
 * how the text is rendered.
 * @param big Pointer to large-size font.
 * @param small Pointer to small-size font.
 * @param lang Pointer to current language.
 */
void TextButton::initText(Font *big, Font *small, Language *lang)
{
	_text->initText(big, small, lang);
	_redraw = true;
}

/**
 * Enables/disables high contrast color. Mostly used for
 * Battlescape UI.
 * @param contrast High contrast setting.
 */
void TextButton::setHighContrast(bool contrast)
{
	_contrast = contrast;
	_text->setHighContrast(contrast);
	_redraw = true;
}

/**
 * Changes the text of the button label.
 * @param text Text string.
 */
void TextButton::setText(const std::string &text)
{
	_text->setText(text);
	_redraw = true;
}

/**
 * Returns the text of the button label.
 * @return Text string.
 */
std::string TextButton::getText() const
{
	return _text->getText();
}

/**
 * Changes the button group this button belongs to.
 * @param group Pointer to the pressed button pointer in the group.
 * Null makes it a regular button.
 */
void TextButton::setGroup(TextButton **group)
{
	_group = group;
	_redraw = true;
}

/**
 * Replaces a certain amount of colors in the surface's palette.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void TextButton::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	Surface::setPalette(colors, firstcolor, ncolors);
	_text->setPalette(colors, firstcolor, ncolors);
}

/**
 * Draws the labeled button.
 * The colors are inverted if the button is pressed.
 */
void TextButton::draw()
{
	Surface::draw();
	SDL_Rect square;

	int mul = 1;
	if (_contrast)
	{
		mul = 2;
	}

	int color = _color + 1 * mul;

	square.x = 0;
	square.y = 0;
	square.w = getWidth();
	square.h = getHeight();

	for (int i = 0; i < 5; ++i)
	{
		drawRect(&square, color);

		if (i % 2 == 0)
		{
			square.x++;
			square.y++;
		}
		square.w--;
		square.h--;

		switch (i)
		{
		case 0:
			color = _color + 5 * mul;
			setPixel(square.w, 0, color);
			break;
		case 1:
			color = _color + 2 * mul;
			break;
		case 2:
			color = _color + 4 * mul;
			setPixel(square.w+1, 1, color);
			break;
		case 3:
			color = _color + 3 * mul;
			break;
		case 4:
			if (_geoscapeButton)
			{
				setPixel(0, 0, _color);
				setPixel(1, 1, _color);
			}
			break;
		}
	}

	bool press;
	if (_group == 0)
		press = isButtonPressed();
	else
		press = (*_group == this);

	if (press)
	{
		if (_geoscapeButton)
		{
			this->invert(_color + 2 * mul);
		}
		else
		{
			this->invert(_color + 3 * mul);
		}
	}
	_text->setInvert(press);

	_text->blit(this);
}

/**
 * Sets the button as the pressed button if it's part of a group.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void TextButton::mousePress(Action *action, State *state)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT && _group != 0)
	{
		TextButton *old = *_group;
		*_group = this;
		if (old != 0)
			old->draw();
		draw();
	}

	if (isButtonHandled(action->getDetails()->button.button))
	{
		if (soundPress != 0 && _group == 0)
		{
			soundPress->play(Mix_GroupAvailable(0));
		}

		if (_comboBox)
		{
			_comboBox->toggle();
		}

		draw();
		//_redraw = true;
	}
	InteractiveSurface::mousePress(action, state);
}

/**
 * Sets the button as the released button.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void TextButton::mouseRelease(Action *action, State *state)
{
	if (isButtonHandled(action->getDetails()->button.button))
	{
		draw();
		//_redraw = true;
	}
	InteractiveSurface::mouseRelease(action, state);
}

/**
 * Hooks up the button to work as part of an existing combobox,
 * toggling its state when it's pressed.
 * @param comboBox Pointer to ComboBox.
 */
void TextButton::setComboBox(ComboBox *comboBox)
{
	_comboBox = comboBox;
	if (_comboBox)
	{
		_text->setX(-6);
	}
	else
	{
		_text->setX(0);
	}
}

void TextButton::setWidth(int width)
{
	Surface::setWidth(width);
	_text->setWidth(width);
}

void TextButton::setHeight(int height)
{
	Surface::setHeight(height);
	_text->setHeight(height);
}

void TextButton::setGeoscapeButton(bool geo)
{
	_geoscapeButton = geo;
}

}
