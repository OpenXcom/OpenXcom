/*
 * Copyright 2010 OpenXcom Developers.
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
#ifndef OPENXCOM_MAP_H
#define OPENXCOM_MAP_H

#include "../Engine/InteractiveSurface.h"

class ResourcePack;
class SavedBattleGame;
class Timer;
class Surface;
class TerrainObject;

/**
 * Interactive map of the battlescape
 */
class Map : public InteractiveSurface
{
private:
	SavedBattleGame *_save;
	ResourcePack *_res;
	Timer *_scrollTimer, *_animTimer;

	int _MapOffsetX, _MapOffsetY;
	int _RMBClickX, _RMBClickY;
	bool _RMBDragging;
	int _TileSizeX, _TileSizeY, _TileSizeZ;
	int _selectorX, _selectorY;
	int _ScrollX, _ScrollY;
	int _animFrame;

	Surface* getSurface(TerrainObject *tob, int frame);
	void minMaxInt(int *value, const int minValue, const int maxValue);
public:
	/// Creates a new map at the specified position and size.
	Map(int width, int height, int x, int y);
	/// Cleans up the map.
	~Map();
	/// savedbattlegame contains all game content like Tiles, Soldiers, Items,...
	void setSavedGame(SavedBattleGame *save);
	/// resourcepack contains tilesheets for terrain and units,... for rendering
	void setResourcePack(ResourcePack *res);
	/// sets stuff up
	void init();
	/// handle timers
	void think();
	/// draw the surface
	void draw();
	/// draws the terrain
	void drawTerrain();
	/// blit the surface
	void blit(Surface *surface);
	/// Special handling for mouse presses.
	void mousePress(Action *action, State *state);
	/// Special handling for mouse releases.
	void mouseRelease(Action *action, State *state);
	/// Special handling for mouse clicks.
	void mouseClick(Action *action, State *state);
	/// Special handling for mous over
	void mouseOver(Action *action, State *state);
	/// Special handling for key presses.
	void keyboardPress(Action *action, State *state);
	/// Scrolls the view (eg when mouse is on the edge of the screen)
	void scroll();
	/// rotate the tileframes 0-7
	void animate();
	/// move map layer up
	void up();
	/// move map layer down
	void down();
	/// sets the battlescape selector position relative to mouseposition
	void setSelectorPosition(int mx, int my);
};

#endif
