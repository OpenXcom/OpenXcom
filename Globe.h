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
#ifndef OPENXCOM__GLOBE_H
#define OPENXCOM__GLOBE_H

#include <vector>
#include <string>
#include "InteractiveSurface.h"

using namespace std;

#define NUM_SHADES 8

class ResourcePack;
class Polygon;
class SurfaceSet;
class SavedGame;
class Timer;
class Target;

/**
 * Interactive globe view of the world.
 * Takes a flat world map made out of land polygons with
 * polar coordinates and renders it as a 3D-looking globe
 * with cartesian coordinates that the player can interact with.
 */
class Globe : public InteractiveSurface
{
private:
	vector<double> _radius;
	double _cenLon, _cenLat, _rotLon, _rotLat;
	int _cenX, _cenY;
	unsigned int _zoom;
	SurfaceSet *_texture[NUM_SHADES];
	ResourcePack *_res;
	SavedGame *_save;
	Surface *_markers, *_countries;
	bool _blink, _detail;
	Timer *_blinkTimer, *_rotTimer;
	vector<Polygon*> _ocean, _cacheOcean, _cacheLand;
	Surface *_mkXcomBase, *_mkAlienBase, *_mkCraft, *_mkWaypoint, *_mkCity;
	Surface *_mkFlyingUfo, *_mkLandedUfo, *_mkCrashedUfo, *_mkAlienSite;

	/// Checks if a point is behind the globe.
	bool pointBack(double lon, double lat);
	/// Checks if a point is inside a polygon.
	bool insidePolygon(double lon, double lat, Polygon *poly);
	/// Checks if a target is near a point.
	bool targetNear(Target* target, int x, int y);
public:
	/// Creates a new globe at the specified position and size.
	Globe(int cenX, int cenY, int width, int height, int x = 0, int y = 0);
	/// Cleans up the globe.
	~Globe();
	/// Loads a set of polygons from a DAT file.
	static void loadDat(string filename, vector<Polygon*> *polygons);
	/// Converts polar coordinates to cartesian coordinates.
	void polarToCart(double lon, double lat, Sint16 *x, Sint16 *y);
	/// Converts cartesian coordinates to polar coordinates.
	void cartToPolar(Sint16 x, Sint16 y, double *lon, double *lat);
	/// Sets the texture set for the globe's polygons.
	void setTexture(SurfaceSet *texture);
	/// Sets the resource pack to pull graphics from.
	void setResourcePack(ResourcePack *res);
	/// Sets the saved game to draw markers from.
	void setSavedGame(SavedGame *save);
	/// Starts rotating the globe left.
	void rotateLeft();
	/// Starts rotating the globe right.
	void rotateRight();
	/// Starts rotating the globe up.
	void rotateUp();
	/// Starts rotating the globe down.
	void rotateDown();
	/// Stops rotating the globe.
	void rotateStop();
	/// Zooms the globe in.
	void zoomIn();
	/// Zooms the globe out.
	void zoomOut();
	/// Zooms the globe minimum.
	void zoomMin();
	/// Zooms the globe maximum.
	void zoomMax();
	/// Centers the globe on a point.
	void center(double lon, double lat);
	/// Checks if a point is inside land.
	bool insideLand(double lon, double lat);
	/// Turns on/off the globe detail.
	void switchDetail();
	/// Gets all the targets near a point on the globe.
	vector<Target*> getTargets(int x, int y, bool craft);
	/// Caches visible globe polygons.
	void cachePolygons();
	/// Sets the palette of the globe.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	/// Handles the timers.
	void think();
	/// Blinks the markers.
	void blink();
	/// Rotates the globe.
	void rotate();
	/// Draws the whole globe.
	void draw();
	/// Draws the ocean of the globe.
	void drawOcean();
	/// Draws the land of the globe.
	void drawLand();
	/// Draws the country details of the globe.
	void drawDetail();
	/// Draws all the markers over the globe.
	void drawMarkers();
	/// Blits the globe onto another surface.
	void blit(Surface *surface);
	/// Special handling for mouse presses.
	void mousePress(SDL_Event *ev, int scale, State *state);
	/// Special handling for mouse releases.
	void mouseRelease(SDL_Event *ev, int scale, State *state);
	/// Special handling for mouse clicks.
	void mouseClick(SDL_Event *ev, int scale, State *state);
	/// Special handling for key presses.
	void keyboardPress(SDL_Event *ev, int scale, State *state);
};

#endif
