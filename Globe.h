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

#include <cmath>
#include <fstream>
#include <vector>
#include <string>
#include "SDL_gfxPrimitives.h"
#include "State_Interactive.h"
#include "SurfaceSet.h"
#include "Polygon.h"
#include "Palette.h"

using namespace std;

#define PI 3.141592653589793238461
#define COORD_OUT_OF_BOUNDS -2.0f

/**
 * Interactive globe view of the world.
 * Takes a flat world map made out of land polygons with
 * polar coordinates and renders it as a 3D-looking globe
 * with cartesian coordinates that the player can interact with.
 * @todo Texture rendering, points inside polygons,
 * converting points with vertical rotation.
 */
class Globe : public InteractiveSurface
{
private:
	vector<Polygon*> *_polygons;
	double _radius, _zoom, _rotLon, _rotLat;
	int _cenX, _cenY;
	SurfaceSet *_texture;
	double _testLon, _testLat;

	/// Keeps longitudes between 0 and 2*PI.
	double longitudeLoop(double lon);
	/// Checks if a point is behind the globe.
	bool pointBack(double lon, double lat);
	/// Checks if a point is inside a polygon.
	bool insidePolygon(double lon, double lat, Polygon *poly);
public:
	/// Creates a new globe at the specified position and size.
	Globe(int cenX, int cenY, int width, int height, int x = 0, int y = 0);
	/// Cleans up the globe.
	~Globe();
	/// Loads a set of polygons from a DAT file.
	static void loadDat(string filename, vector<Polygon*> *polygons);
	/// Converts polar coordinates into cartesian coordinates.
	void polarToCart(double lon, double lat, Sint16 *x, Sint16 *y);
	/// Converts cartesian coordinates into polar coordinates.
	void cartToPolar(Sint16 x, Sint16 y, double *lon, double *lat);
	/// Sets the texture set for the globe's polygons.
	void setTexture(SurfaceSet *texture);
	/// Sets the set of polygons for the globe.
	void setPolygons(vector<Polygon*> *polygons);
	/// Rotates the globe a set amount.
	void rotate(double lon, double lat);
	/// Zooms the globe a set amount.
	void zoom(double amount);
	/// Centers the globe on a point.
	void center(double lon, double lat);
	/// Draws the globe.
	void draw();
	/// Handles mouse events.
	void handle(SDL_Event *ev, int scale, State *state);
	/// Testing the inside-polygon algorithm.
	void test(double lon, double lat);
};

#endif