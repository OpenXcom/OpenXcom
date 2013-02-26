/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include <set>
#include <vector>

namespace OpenXcom
{

class ResourcePack;
class SavedBattleGame;
class Surface;
class MapData;
class Position;
class Tile;
class BattleUnit;
class BulletSprite;
class Projectile;
class Explosion;
class BattlescapeMessage;
class Camera;
class Timer;

enum CursorType { CT_NONE, CT_NORMAL, CT_AIM, CT_PSI, CT_WAYPOINT, CT_THROW };

/**
 * Interactive map of the battlescape
 */
class Map : public InteractiveSurface
{
private:
	static const int SCROLL_INTERVAL = 50;
	Timer *_scrollTimer;
	Game *_game;
	SavedBattleGame *_save;
	ResourcePack *_res;
	Surface *_arrow;
	int _spriteWidth, _spriteHeight;
	int _selectorX, _selectorY;
	CursorType _cursorType;
	int _cursorSize;
	int _animFrame;
	BulletSprite *_bullet[37];
	Projectile *_projectile;
	bool projectileInFOV;
	std::set<Explosion *> _explosions;
	bool explosionInFOV;
	bool _launch;
	BattlescapeMessage *_message;
	Camera *_camera;
	int _visibleMapHeight;
	void drawTerrain(Surface *surface);
	int getTerrainLevel(Position pos, int size);
	std::vector<Position> _waypoints;
	bool _unitDying;
public:
	/// Creates a new map at the specified position and size.
	Map(Game *game, int width, int height, int x, int y, int visibleMapHeight);
	/// Cleans up the map.
	~Map();
	/// sets stuff up
	void init();
	/// handle timers
	void think();
	/// draw the surface
	void draw();
	/// Sets the palette.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	/// Special handling for mouse clicks.
	void mouseClick(Action *action, State *state);
	/// Special handling for mouse over
	void mouseOver(Action *action, State *state);
	/// Special handling for key presses.
	void keyboardPress(Action *action, State *state);
	/// rotate the tileframes 0-7
	void animate(bool redraw);
	/// Sets the battlescape selector position relative to mouseposition.
	void setSelectorPosition(int mx, int my);
	/// Get the currently selected position.
	void getSelectorPosition(Position *pos) const;
	/// Calculate the offset of a soldier, when it is walking in the middle of 2 tiles.
	void calculateWalkingOffset(BattleUnit *unit, Position *offset);
	/// Set the 3D cursor type.
	void setCursorType(CursorType type, int size = 1);
	/// Get the 3D cursor type.
	CursorType getCursorType() const;
	/// Cache units.
	void cacheUnits();
	/// Cache unit.
	void cacheUnit(BattleUnit *unit);
	/// Set projectile
	void setProjectile(Projectile *projectile);
	/// Get projectile
	Projectile *getProjectile() const;
	/// Get explosion set
	std::set<Explosion*> *getExplosions();
	/// Get pointer to camera
	Camera *getCamera();
	void scroll();
	/// Get waypoints vector
	std::vector<Position> *getWaypoints();
	/// Set mouse-buttons' pressed state
	void setButtonsPressed(Uint8 button, bool pressed);
	void setUnitDying(bool flag);
};

}

#endif
