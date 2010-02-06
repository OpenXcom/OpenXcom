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

#define PI 3.14159265358979323846
#define COORD_OUT_OF_BOUNDS -2.0f

class Globe : public InteractiveSurface
{
private:
	vector<Polygon*> _polygons;
	double _radius, _zoom, _rotLon, _rotLat;
	int _cenX, _cenY;
	SurfaceSet *_texture;
	double _testLon, _testLat;

	double longitudeLoop(double lon);
	bool pointBack(double lon, double lat);
	bool insidePolygon(double lon, double lat, Polygon *poly);
public:
	Globe(int cenX, int cenY, int width, int height, int x = 0, int y = 0);
	~Globe();
	void polarToCart(double lon, double lat, Sint16 *x, Sint16 *y);
	void cartToPolar(Sint16 x, Sint16 y, double *lon, double *lat);
	void loadDat(string filename);
	void setTexture(SurfaceSet *texture);
	void rotate(double lon, double lat);
	void zoom(double amount);
	void center(double lon, double lat);
	void blit(Surface *surface);
	void handle(SDL_Event *ev, int scale, State *state);
	void test(double lon, double lat);
};

#endif