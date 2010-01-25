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
#ifndef __GLOBE_H__
#define __GLOBE_H__

#include <cmath>
#include <fstream>
#include <vector>
#include <string>
#include "SDL_gfxPrimitives.h"
#include "Surface.h"
#include "SurfaceSet.h"
#include "Polygon.h"
#include "Palette.h"

using namespace std;

#define PI 3.14159265358979323846

class Globe : public Surface
{
private:
	vector<Polygon*> _polygons;
	double _radius, _zoom, _rotX, _rotY;
	int _cenX, _cenY;
	Sint16 _testX, _testY;
	SurfaceSet *_texture;

	void polarToCart(double lat, double lon, Sint16 *x, Sint16 *y);
	void cartToPolar(Sint16 x, Sint16 y, double *lat, double *lon);
	double longitudeLoop(double lon);
	bool pointBack(double lon, double lat);
public:
	Globe(int cenX, int cenY, int width, int height, int x = 0, int y = 0);
	~Globe();
	void loadDat(string filename);
	void setTexture(SurfaceSet *texture);
	void blit(Surface *surface);
};

#endif