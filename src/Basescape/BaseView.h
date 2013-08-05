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
#ifndef OPENXCOM_BASEVIEW_H
#define OPENXCOM_BASEVIEW_H

#include "../Engine/InteractiveSurface.h"

namespace OpenXcom
{

class Base;
class SurfaceSet;
class BaseFacility;
class RuleBaseFacility;
class Font;
class Timer;

/**
 * Interactive view of a base.
 * Takes a certain base and displays all its facilities
 * and status, allowing players to manage them.
 */
class BaseView : public InteractiveSurface
{
private:
	static const int BASE_SIZE = 6;
	static const int GRID_SIZE = 32;

	Base *_base;
	SurfaceSet *_texture;
	BaseFacility *_facilities[BASE_SIZE][BASE_SIZE], *_selFacility;
	Font *_big, *_small;
	int _gridX, _gridY, _selSize;
	Surface *_selector;
	bool _blink;
	Timer *_timer;
	/// Updates the neighborFacility's build time. This is for internal use only (reCalcQueuedBuildings()).
	void updateNeighborFacilityBuildTime(BaseFacility* facility, BaseFacility* neighbor);
public:
	/// Creates a new base view at the specified position and size.
	BaseView(int width, int height, int x = 0, int y = 0);
	/// Cleans up the base view.
	~BaseView();
	/// Sets the base view's various fonts.
	void setFonts(Font *big, Font *small);
	/// Sets the base to display.
	void setBase(Base *base);
	/// Sets the texture for this base view.
	void setTexture(SurfaceSet *texture);
	/// Gets the currently selected facility.
	BaseFacility *getSelectedFacility() const;
	/// Prevents any mouseover bugs on dismantling base facilities before setBase has had time to update the base.
	void resetSelectedFacility();
	/// Gets the X position of the currently selected square.
	int getGridX() const;
	/// Gets the Y position of the currently selected square.
	int getGridY() const;
	/// Sets whether the base view is selectable.
	void setSelectable(int size);
	/// Checks if a facility can be placed.
	bool isPlaceable(RuleBaseFacility *rule) const;
	/// Checks if the placed facility is placed in queue or not.
	bool isQueuedBuilding(RuleBaseFacility *rule) const;
	/// ReCalculates the remaining build-time of all queued buildings.
	void reCalcQueuedBuildings();
	/// Counts the squares connected to a grid position.
	int countConnected(int x, int y, int **grid, BaseFacility *remove = 0) const;
	/// Handles the timers.
	void think();
	/// Blinks the selector.
	void blink();
	/// Draws the base view.
	void draw();
	/// Blits the base view onto another surface.
	void blit(Surface *surface);
	/// Special handling for mouse hovers.
	void mouseOver(Action *action, State *state);
	/// Special handling for mouse hovering out.
	void mouseOut(Action *action, State *state);
};

}

#endif
