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
#include <vector>
#include <string>

namespace OpenXcom
{

class TextButton;
class TextList;
class Window;
class Language;

/**
 * Text button with a list dropdown when pressed.
 * Allows selection from multiple available options.
 */
class ComboBox : public InteractiveSurface
{
private:
	static const int HORIZONTAL_MARGIN;
	static const int VERTICAL_MARGIN;
	static const int MAX_ITEMS;
	static const int BUTTON_WIDTH;
	static const int TEXT_HEIGHT;

	TextButton *_button;
	Surface *_arrow;
	Window *_window;
	TextList *_list;

	ActionHandler _change;
	size_t _sel;
	State *_state;
	Language *_lang;
	Uint8 _color;
	bool _toggled;
	bool _popupAboveButton;

	void drawArrow();
	void setDropdown(int options);
public:
	/// Creates a combo box with the specified size and position.
	ComboBox(State *state, int width, int height, int x = 0, int y = 0, bool popupAboveButton = false);
	/// Cleans up the combo box.
	~ComboBox();
	/// Sets the X position of the surface.
	void setX(int x);
	/// Sets the Y position of the surface.
	void setY(int y);
	/// Sets the palette of the text list.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	/// Initializes the resources for the text list.
	void initText(Font *big, Font *small, Language *lang);
	/// Sets the background surface.
	void setBackground(Surface *bg);
	/// Sets the border color.
	void setColor(Uint8 color);
	/// Gets the border color.
	Uint8 getColor() const;
	/// Sets the high contrast color setting.
	void setHighContrast(bool contrast);
	/// Sets the arrow color of the text list.
	void setArrowColor(Uint8 color);
	/// Gets the selected option in the list.
	size_t getSelected() const;
	/// Gets the item that is currently hovered over in the popup list, or the current
	/// selected item if no item is hovered over.
	size_t getHoveredListIdx() const;
	/// Sets the button text without changing the selected option
	void setText(const std::wstring &text);
	/// Sets the selected option in the list.
	void setSelected(size_t sel);
	/// Sets the list of options.
	void setOptions(const std::vector<std::string> &options);
	/// Sets the list of options.
	void setOptions(const std::vector<std::wstring> &options);
	/// Blits the combo box onto another surface.
	void blit(Surface *surface);
	/// Thinks arrow buttons.
	void think();
	/// Handle arrow buttons.
	void handle(Action *action, State *state);
	/// Toggles the combo box state.
	void toggle(bool first = false);
	/// Hooks an action handler to when the slider changes.
	void onChange(ActionHandler handler);
	/// Hooks an action handler to moving the mouse in to the listbox when it is visible.
	void onListMouseIn(ActionHandler handler);
	/// Hooks an action handler to moving the mouse out of the listbox when it is visible.
	void onListMouseOut(ActionHandler handler);
	/// Hooks an action handler to moving the mouse over the listbox when it is visible.
	void onListMouseOver(ActionHandler handler);
};

}
