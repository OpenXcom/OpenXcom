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
#ifndef OPENXCOM__BASEVIEW_H
#define OPENXCOM__BASEVIEW_H

#include <cmath>
#include "State_Interactive.h"
#include "SurfaceSet.h"
#include "Base.h"
#include "BaseFacility.h"
#include "Craft.h"
#include "Font.h"
#include "Text.h"

#define GRID_SIZE 32
#define BASE_SIZE 6

/**
 * Interactive view of a base.
 * Takes a certain base and displays all its facilities
 * and status, allowing players to manage them.
 */
class BaseView : public InteractiveSurface
{
private:
	Base *_base;
	SurfaceSet *_texture;
	BaseFacility *_facilities[BASE_SIZE][BASE_SIZE], *_selFacility;
	Font *_big, *_small;
public:
	/// Creates a new base view at the specified position and size.
	BaseView(Font *big, Font *small, int width, int height, int x = 0, int y = 0);
	/// Cleans up the base view.
	~BaseView();

	void setBase(Base *base);
	void setTexture(SurfaceSet *texture);
	BaseFacility *getSelectedFacility();
	void draw();
	void handle(SDL_Event *ev, int scale, State *state);
};

#endif