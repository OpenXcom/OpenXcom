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
#ifndef OPENXCOM__WINDOW_H
#define OPENXCOM__WINDOW_H

#include "Surface.h"

enum WindowPopup { POPUP_NONE, POPUP_HORIZONTAL, POPUP_VERTICAL, POPUP_BOTH };

class Window : public Surface
{
private:
	Surface *_bg;
	Uint8 _color;
	WindowPopup _popup;
	double _popupStep;

public:
	Window(int width, int height, int x = 0, int y = 0, WindowPopup popup = POPUP_NONE);
	~Window();
	void setBg(Surface *bg);
	void setColor(Uint8 color);
	Uint8 getColor();
	void blit(Surface *surface);
};

#endif