/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#include "../Engine/Options.h"
#include "Position.h"
#include <set>
#include <vector>

namespace OpenXcom
{

class ResourcePack;
class SavedBattleGame;
class Surface;
class SurfaceSet;
class BattleUnit;
class Projectile;
class Explosion;
class BattlescapeMessage;
class Camera;
class Timer;
class Text;

enum CursorType { CT_NONE, CT_NORMAL, CT_AIM, CT_PSI, CT_WAYPOINT, CT_THROW };
/**
 * Interactive map of the battlescape.
 */
class Map : public InteractiveSurface
{
private:
	static const int SCROLL_INTERVAL = 15;
	static const int BULLET_SPRITES = 35;
	Timer *_scrollMouseTimer, *_scrollKeyTimer;
	Game *_game;
	SavedBattleGame *_save;
	ResourcePack *_res;
	Surface *_arrow;
	int _spriteWidth, _spriteHeight;
	int _selectorX, _selectorY;
	int _mouseX, _mouseY;
	CursorType _cursorType;
	int _cursorSize;
	int _animFrame;
	Projectile *_projectile;
	bool _projectileInFOV;
	std::list<Explosion *> _explosions;
	bool _explosionInFOV, _launch;
	BattlescapeMessage *_message;
	Camera *_camera;
	int _visibleMapHeight;
	std::vector<Position> _waypoints;
	bool _unitDying, _smoothCamera, _smoothingEngaged;
	PathPreview _previewSetting;
	Text *_txtAccuracy;
	SurfaceSet *_projectileSet;

	void drawTerrain(Surface *surface);
	int getTerrainLevel(Position pos, int size);
	int _iconHeight, _iconWidth, _messageColor;
public:
	/// Creates a new map at the specified position and size.
	Map(Game* game, int width, int height, int x, int y, int visibleMapHeight);
	/// Cleans up the map.
	~Map();
	/// Initializes the map.
	void init();
	/// Handles timers.
	void think();
	/// Draws the surface.
	void draw();
	/// Sets the palette.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	/// Special handling for mouse press.
	void mousePress(Action *action, State *state);
	/// Special handling for mouse release.
	void mouseRelease(Action *action, State *state);
	/// Special handling for mouse over
	void mouseOver(Action *action, State *state);
	/// Special handling for key presses.
	void keyboardPress(Action *action, State *state);
	/// Special handling for key releases.
	void keyboardRelease(Action *action, State *state);
	/// Rotates the tileframes 0-7
	void animate(bool redraw);
	/// Sets the battlescape selector position relative to mouseposition.
	void setSelectorPosition(int mx, int my);
	/// Gets the currently selected position.
	void getSelectorPosition(Position *pos) const;
	/// Calculates the offset of a soldier, when it is walking in the middle of 2 tiles.
	void calculateWalkingOffset(BattleUnit *unit, Position *offset);
	/// Sets the 3D cursor type.
	void setCursorType(CursorType type, int size = 1);
	/// Gets the 3D cursor type.
	CursorType getCursorType() const;
	/// Caches units.
	void cacheUnits();
	/// Caches the unit.
	void cacheUnit(BattleUnit *unit);
	/// Sets projectile.
	void setProjectile(Projectile *projectile);
	/// Gets projectile.
	Projectile *getProjectile() const;
	/// Gets explosion set.
	std::list<Explosion*> *getExplosions();
	/// Gets the pointer to the camera.
	Camera *getCamera();
	/// Mouse-scrolls the camera.
	void scrollMouse();
	/// Keyboard-scrolls the camera.
	void scrollKey();
	/// Get waypoints vector.
	std::vector<Position> *getWaypoints();
	/// Set mouse-buttons' pressed state.
	void setButtonsPressed(Uint8 button, bool pressed);
	/// Sets the unitDying flag.
	void setUnitDying(bool flag);
	/// Refreshes the battlescape selector after scrolling.
	void refreshSelectorPosition();
	/// Special handling for updating map height.
	void setHeight(int height);
	/// Special handling for updating map width.
	void setWidth(int width);
	/// Get the vertical position of the hidden movement screen.
	const int getMessageY();
	/// Get the icon height.
	const int getIconHeight();
	/// Get the icon width.
	const int getIconWidth();
	/// Convert a map position to a sound angle.
	const int getSoundAngle(Position pos);
};

}

#endif
