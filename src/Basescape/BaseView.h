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
class Language;
class Timer;

typedef void (State::* BaseViewClickHandler)(Action*);

/**
 * Interactive view of a base.
 * Takes a certain base and displays all its facilities
 * and status, allowing players to manage them.
 */
class BaseView : public InteractiveSurface
{
private:
	static const int GRID_SIZE = 32;

	Game *_game;
	Base *_base;
	SurfaceSet *_texture;
	// !!! We must use [] for writing, and .at() to reading, because the first one creates a new element if it does not found, and the second throws an exception when the element does not exist!
	// Nay! Use getFacilityAt instead of .at()! (it returns 0 instead of an exception)
	std::map<int, std::map<int, BaseFacility*> > _facilities;
	int _sideLeft, _sideTop, _sideRight, _sideBottom; // These are facility-coordinates (x,y) of the sides of the base
	BaseFacility *_selFacility;
	Font *_big, *_small;
	Language *_lang;
	int _gridX, _gridY, _selSize;
	Surface *_selector;
	bool _blink;
	Timer *_timer;
	int _cameraPosX, _cameraPosY;
	Surface *_shadowSurface;
	// these are required for scrolling
	bool isMouseScrolling;
	bool isMouseScrolled;
	int xBeforeMouseScrolling, yBeforeMouseScrolling;
	int mouseScrollX, mouseScrollY;
	int posBeforeMouseScrollingX, posBeforeMouseScrollingY;
	Uint32 mouseScrollingStartTime;
	int totalMouseMoveX, totalMouseMoveY;
	bool mouseMovedOverThreshold;
	BaseViewClickHandler _leftClickHandler;
	BaseViewClickHandler _rightClickHandler;
	bool _showExtraSpace;
	/// Handles pressing on the BaseView.
	void mousePress(Action *action, State *state);
	/// Handles clicking on the BaseView.
	void mouseClick(Action *action, State *state);
	/// Handles moving mouse over the BaseView.
	void mouseOver(Action *action, State *state);
	/// Handles moving the mouse into the BaseView surface.
	void mouseIn(Action *action, State *state);
	/// Special handling for mouse hovering out.
	void mouseOut(Action *action, State *state);
	/// Updates the neighborFacility's build time. This is for internal use only (reCalcQueuedBuildings()).
	void updateNeighborFacilityBuildTime(BaseFacility* facility, BaseFacility* neighbor);
public:
	static const int MIN_BASE_SIZE = 6;
	/// Creates a new base view at the specified position and size.
	BaseView(Game * game, BaseViewClickHandler leftClickHandler, BaseViewClickHandler rightClickHandler, bool showExtraSpace, int width, int height, int x = 0, int y = 0);
	/// Cleans up the base view.
	~BaseView();
	/// Sets the surface's palette.
	virtual void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	/// Initializes the base view's various resources.
	void initText(Font *big, Font *small, Language *lang);
	/// Sets the base to display.
	void setBase(Base *base);
	/// Sets the texture for this base view.
	void setTexture(SurfaceSet *texture);
	/// Gets the currently selected facility.
	BaseFacility *getSelectedFacility() const;
	/// Gets the name of the currently selected facility.
	std::wstring getSelectedFacilityName() const;
	/// Returns the Facility at the given indexes (searches _facilities), or returns 0 if it does not exist
	BaseFacility *getFacilityAt(int x, int y) const;
	/// Prevents any mouseover bugs on dismantling base facilities before setBase has had time to update the base.
	void resetSelectedFacility();
	/// Gets the X position of the currently selected square.
	int getGridX() const;
	/// Gets the Y position of the currently selected square.
	int getGridY() const;
	/// Gets the X position of the camera.
	int getCameraPosX() const;
	/// Gets the Y position of the camera.
	int getCameraPosY() const;
	/// Sets the position of the camera.  Note: setBase() must be called (at least 1 after the constructor) before calling this! (we have to know the limits)
	void setCameraPos(int x, int y);
	/// Sets whether the base view is selectable.
	void setSelectable(int size);
	/// Checks if a facility can be placed.
	bool isPlaceable(RuleBaseFacility *rule) const;
	/// Checks if the placed facility is placed in queue or not.
	bool isQueuedBuilding(RuleBaseFacility *rule) const;
	/// ReCalculates the remaining build-time of all queued buildings.
	void reCalcQueuedBuildings();
	/// Handles the timers.
	void think();
	/// Blinks the selector.
	void blink();
	/// Draws the base view.
	void draw();
	/// Blits the base view onto another surface.
	void blit(Surface *surface);
	/// Sets the left-click handler.
	void setLeftClickHandler(BaseViewClickHandler leftClickHandler);
	/// Sets the right-click handler.
	void setRightClickHandler(BaseViewClickHandler rightClickHandler);
	/// Tries to dismantle the selected facility.
	void tryDismantle(SDL_Color *palette);
};

}

#endif
