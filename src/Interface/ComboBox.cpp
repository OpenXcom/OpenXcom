/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include "ComboBox.h"
#include "TextButton.h"
#include "Window.h"
#include "TextList.h"
#include "../Engine/State.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Action.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

const int ComboBox::HORIZONTAL_MARGIN = 2;
const int ComboBox::VERTICAL_MARGIN = 3;
const int ComboBox::MAX_ITEMS = 10;
const int ComboBox::BUTTON_WIDTH = 14;
const int ComboBox::TEXT_HEIGHT = 8;

/**
 * Sets up a combobox with the specified size and position.
 * @param state Pointer to state the combobox belongs to.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
ComboBox::ComboBox(State *state, int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _change(0), _sel(0), _state(state), _lang(0), _toggled(false)
{
	_button = new TextButton(width, height, x, y);
	_button->setComboBox(this);

	_arrow = new Surface(11, 8, x + width - BUTTON_WIDTH, y + 4);

	_window = new Window(state, width, MAX_ITEMS * 8 + VERTICAL_MARGIN * 2, x, y + height);
	_window->setThinBorder();

	_list = new TextList(width - HORIZONTAL_MARGIN * 2 - BUTTON_WIDTH + 1,
						MAX_ITEMS * TEXT_HEIGHT - 2,
						x + HORIZONTAL_MARGIN,
						y + height + VERTICAL_MARGIN);
	_list->setComboBox(this);
	_list->setColumns(1, _list->getWidth());
	_list->setSelectable(true);
	_list->setBackground(_window);
	_list->setAlign(ALIGN_CENTER);
	_list->setScrolling(true, 0);

	toggle(true);
}

/**
 * Deletes all the stuff contained in the list.
 */
ComboBox::~ComboBox()
{
	delete _button;
	delete _arrow;
	delete _window;
	delete _list;
}

/**
* Changes the position of the surface in the X axis.
* @param x X position in pixels.
*/
void ComboBox::setX(int x)
{
	Surface::setX(x);
	_button->setX(x);
	_arrow->setX(x + getWidth() - BUTTON_WIDTH);
	_window->setX(x);
	_list->setX(x + HORIZONTAL_MARGIN);
}

/**
* Changes the position of the surface in the Y axis.
* @param y Y position in pixels.
*/
void ComboBox::setY(int y)
{
	Surface::setY(y);
	_button->setY(y);
	_arrow->setY(y + 4);
	_window->setY(y + getHeight());
	_list->setY(y + getHeight() + VERTICAL_MARGIN);
}

/**
 * Replaces a certain amount of colors in the palette of all
 * the text contained in the list.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void ComboBox::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	Surface::setPalette(colors, firstcolor, ncolors);
	_button->setPalette(colors, firstcolor, ncolors);
	_arrow->setPalette(colors, firstcolor, ncolors);
	_window->setPalette(colors, firstcolor, ncolors);
	_list->setPalette(colors, firstcolor, ncolors);
}

/**
 * Changes the resources for the text in the combo box.
 * @param big Pointer to large-size font.
 * @param small Pointer to small-size font.
 * @param lang Pointer to current language.
 */
void ComboBox::initText(Font *big, Font *small, Language *lang)
{
	_lang = lang;
	_button->initText(big, small, lang);
	_list->initText(big, small, lang);
}

/**
 * Changes the surface used to draw the background of the combo box.
 * @param bg New background.
 */
void ComboBox::setBackground(Surface *bg)
{
	_window->setBackground(bg);
}

/**
 * Changes the color used to draw the combo box.
 * @param color Color value.
 */
void ComboBox::setColor(Uint8 color)
{
	_color = color;
	drawArrow();
	_button->setColor(_color);
	_window->setColor(_color);
	_list->setColor(_color);
}

/**
 * Returns the color used to draw the combo box.
 * @return Color value.
 */
Uint8 ComboBox::getColor() const
{
	return _color;
}

/**
 * Draws the arrow used to indicate the combo box.
 */
void ComboBox::drawArrow()
{
	_arrow->clear();

	SDL_Rect square;
	int color = _color + 1;
	if (color == 256)
		color++;

	// Draw arrow triangle 1
	square.x = 1;
	square.y = 2;
	square.w = 9;
	square.h = 1;

	for (; square.w > 1; square.w -= 2)
	{
		_arrow->drawRect(&square, color + 2);
		square.x++;
		square.y++;
	}
	_arrow->drawRect(&square, color + 2);

	// Draw arrow triangle 2
	square.x = 2;
	square.y = 2;
	square.w = 7;
	square.h = 1;

	for (; square.w > 1; square.w -= 2)
	{
		_arrow->drawRect(&square, color);
		square.x++;
		square.y++;
	}
	_arrow->drawRect(&square, color);
}

/**
 * Enables/disables high contrast color. Mostly used for
 * Battlescape UI.
 * @param contrast High contrast setting.
 */
void ComboBox::setHighContrast(bool contrast)
{
	_button->setHighContrast(contrast);
	_window->setHighContrast(contrast);
	_list->setHighContrast(contrast);
}

/**
 * Changes the color of the arrow buttons in the list.
 * @param color Color value.
 */
void ComboBox::setArrowColor(Uint8 color)
{
	_list->setArrowColor(color);
}

/**
 * Returns the currently selected option.
 * @return Selected row.
 */
size_t ComboBox::getSelected() const
{
	return _sel;
}

/**
 * Changes the currently selected option.
 * @param sel Selected row.
 */
void ComboBox::setSelected(size_t sel)
{
	_sel = sel;
	if (_sel < _list->getTexts())
	{
		_button->setText(_list->getCellText(_sel, 0));
	}
}

/**
 * Updates the size of the dropdown list based on
 * the number of options available.
 * @param options Number of options.
 */
void ComboBox::setDropdown(int options)
{
	int items = std::min(options, MAX_ITEMS);
	int h = _button->getFont()->getHeight() + _button->getFont()->getSpacing();
	int dy = (Options::baseYResolution - 200) / 2;
	while (_window->getY() + items * h + VERTICAL_MARGIN * 2 > 200 + dy)
	{
		items--;
	}
	_window->setHeight(items * h + VERTICAL_MARGIN * 2);
	_list->setHeight(items * h);
}

/**
 * Changes the list of available options to choose from.
 * @param options List of string IDs.
 */
void ComboBox::setOptions(const std::vector<std::string> &options)
{
	setDropdown(options.size());
	_list->clearList();
	for (std::vector<std::string>::const_iterator i = options.begin(); i != options.end(); ++i)
	{
		_list->addRow(1, _lang->getString(*i).c_str());
	}
	setSelected(_sel);
	_list->draw();
}

/**
 * Changes the list of available options to choose from.
 * @param options List of localized strings.
 */
void ComboBox::setOptions(const std::vector<std::wstring> &options)
{
	setDropdown(options.size());
	_list->clearList();
	for (std::vector<std::wstring>::const_iterator i = options.begin(); i != options.end(); ++i)
	{
		_list->addRow(1, i->c_str());
	}
	setSelected(_sel);
}

/**
 * Blits the combo box components.
 * @param surface Pointer to surface to blit onto.
 */
void ComboBox::blit(Surface *surface)
{
	Surface::blit(surface);
	_list->invalidate();
	if (_visible && !_hidden)
	{
		_button->blit(surface);
		_arrow->blit(surface);
		_window->blit(surface);
		_list->blit(surface);
	}
}

/**
 * Passes events to internal components.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void ComboBox::handle(Action *action, State *state)
{
	_button->handle(action, state);
	_list->handle(action, state);
	InteractiveSurface::handle(action, state);
	if (_window->getVisible() && action->getDetails()->type == SDL_MOUSEBUTTONDOWN &&
		(action->getAbsoluteXMouse() < getX() || action->getAbsoluteXMouse() >= getX() + getWidth() ||
		 action->getAbsoluteYMouse() < getY() || action->getAbsoluteYMouse() >= getY() + getHeight() + _window->getHeight()))
	{
		toggle();
	}
	if (_toggled)
	{
		if (_change)
		{
			(state->*_change)(action);
		}
		_toggled = false;
	}
}

/**
 * Passes ticks to arrow buttons.
 */
void ComboBox::think()
{
	_button->think();
	_arrow->think();
	_window->think();
	_list->think();
	InteractiveSurface::think();
}

/**
 * Opens/closes the combo box list.
 * @param first Is it the initialization toggle?
 */
void ComboBox::toggle(bool first)
{
	_window->setVisible(!_window->getVisible());
	_list->setVisible(!_list->getVisible());
	_state->setModal(_window->getVisible() ? this : 0);
	if (!first && !_window->getVisible())
	{
		_toggled = true;
	}
	if (_list->getVisible())
	{
		if (_sel < _list->getVisibleRows()/2)
		{
			_list->scrollTo(0);
		}
		else
		{
			_list->scrollTo(_sel - _list->getVisibleRows()/2);
		}
	}
}

/**
 * Sets a function to be called every time the slider's value changes.
 * @param handler Action handler.
 */
void ComboBox::onChange(ActionHandler handler)
{
	_change = handler;
}

}
