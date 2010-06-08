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

#include <cmath>
#include "State_Interactive.h"
#include "SurfaceSet.h"
#include "Base.h"
#include "BaseFacility.h"
#include "Craft.h"

#define MAX_BASES 8
#define MINI_SIZE 14

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
	int _selBase;
public:
	/// Creates a new mini base view at the specified position and size.
	MiniBaseView(int width, int height, int x = 0, int y = 0);
	/// Cleans up the mini base view.
	~MiniBaseView();

	void setBases(vector<Base*> *bases);
	void setTexture(SurfaceSet *texture);
	int getSelectedBase();
	void draw();
	void handle(SDL_Event *ev, int scale, State *state);
};

#endif