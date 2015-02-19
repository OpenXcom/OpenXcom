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
#ifndef OPENXCOM_WINDOW_H
#define OPENXCOM_WINDOW_H

#include "../Engine/Surface.h"

namespace OpenXcom
{

class Timer;
class State;
class Sound;

/**
 * Enumeration for the type of animation when a window pops up.
 */
enum WindowPopup { POPUP_NONE, POPUP_HORIZONTAL, POPUP_VERTICAL, POPUP_BOTH };

/**
 * Box with a coloured border and custom background.
 * Pretty much used as the background in most of the interface. In fact
 * it's also used in screens, so it's not really much of a window, just a... box.
 * But box sounds lame.
 */
class Window : public Surface
{
private:
	static const double POPUP_SPEED;
	int _dx, _dy;
	Surface *_bg;
	Uint8 _color;
	WindowPopup _popup;
	double _popupStep;
	Timer *_timer;
	State *_state;
	bool _contrast, _screen, _thinBorder;
public:
	static Sound *soundPopup[3];
	/// Creates a new window with the specified size and position.
	Window(State *state, int width, int height, int x = 0, int y = 0, WindowPopup popup = POPUP_NONE);
	/// Cleans up the window.
	~Window();
	/// Sets the background surface.
	void setBackground(Surface *bg);
	/// Sets the border color.
	void setColor(Uint8 color);
	/// Gets the border color.
	Uint8 getColor() const;
	/// Sets the high contrast color setting.
	void setHighContrast(bool contrast);
	/// Handles the timers.
	void think();
	/// Popups the window.
	void popup();
	/// Draws the window.
	void draw();
	/// sets the X delta.
	void setDX(int dx);
	/// sets the Y delta.
	void setDY(int dy);
	/// Give this window a thin border.
	void setThinBorder();
};

}

#endif
