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
#ifndef OPENXCOM__MINIBASEVIEW_H
#define OPENXCOM__MINIBASEVIEW_H

#include <vector>
#include "InteractiveSurface.h"

using namespace std;

class Base;
class SurfaceSet;

/**
 * Mini view of a base.
 * Takes all the bases and displays their layout
 * and allows players to swap between them.
 */
class MiniBaseView : public InteractiveSurface
{
private:
	vector<Base*> *_bases;
	SurfaceSet *_texture;
	unsigned int _base, _hoverBase;
public:
	/// Creates a new mini base view at the specified position and size.
	MiniBaseView(int width, int height, int x = 0, int y = 0);
	/// Cleans up the mini base view.
	~MiniBaseView();
	/// Sets the base list to display.
	void setBases(vector<Base*> *bases);
	/// Sets the texture for the mini base view.
	void setTexture(SurfaceSet *texture);
	/// Gets the base the mouse is over.
	unsigned int getHoveredBase();
	/// Sets the selected base for the mini base view.
	void setSelectedBase(unsigned int base);
	/// Draws the mini base view.
	void draw();
	/// Special handling for mouse presses.
	void mousePress(SDL_Event *ev, int scale, State *state);
	/// Special handling for mouse releases.
	void mouseRelease(SDL_Event *ev, int scale, State *state);
	/// Special handling for mouse clicks.
	void mouseClick(SDL_Event *ev, int scale, State *state);
	/// Special handling for mouse hovers.
	void mouseOver(SDL_Event *ev, int scale, State *state);
};

#endif
