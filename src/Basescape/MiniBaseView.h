/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#ifndef OPENXCOM_MINIBASEVIEW_H
#define OPENXCOM_MINIBASEVIEW_H

#include <vector>
#include "../Engine/InteractiveSurface.h"

namespace OpenXcom
{

class Base;
class SurfaceSet;

typedef void (State::* NewBaseSelectedHandler)(Base*);

/**
 * Mini view of a base.
 * Takes all the bases and displays their layout
 * and allows players to swap between them.
 */
class MiniBaseView : public InteractiveSurface
{
private:
	static const int MINI_SIZE = 14;

	std::vector<Base*> *_bases;
	SurfaceSet *_texture;
	size_t _base;
	NewBaseSelectedHandler _newBaseSelectedHandler;
public:
	static const size_t MAX_BASES = 8;
	/// Creates a new mini base view at the specified position and size.
	MiniBaseView(std::vector<Base*> *bases, NewBaseSelectedHandler newBaseSelectedHandler, int width, int height, int x = 0, int y = 0);
	/// Cleans up the mini base view.
	~MiniBaseView();
	/// Sets the texture for the mini base view.
	void setTexture(SurfaceSet *texture);
	/// Sets the selected base for the mini base view.
	void setSelectedBase(Base *base);
	/// Draws the mini base view.
	void draw();
	/// Handles a mouse click event.
	void mouseClick(Action *action, State *state);
};

}

#endif
