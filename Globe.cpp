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
#include "Globe.h"

Globe::Globe(int cenX, int cenY, int width, int height, int x, int y) : Surface(width, height, x, y), _polygons(), _cenX(cenX), _cenY(cenY), _radius(90), _zoom(1), _rotX(0), _rotY(0)
{
	_testX = _cenX - 20;
	_testY = _cenY - 20;
}

Globe::~Globe()
{
}

void Globe::polarToCart(double lat, double lon, Sint16 *x, Sint16 *y)
{
	lon = longitudeLoop(lon + _rotX);
	
	// Pre-calculate why not
	double sin_lon = sin(lon), sin_lat = sin(lat), sin_rotY = sin(_rotY);
	double cos_lon = cos(lon), cos_lat = cos(lat), cos_rotY = cos(_rotY);

	// Convert polar coordinates to cartesian
	*x = _cenX + (int)floor(_radius * _zoom * sin_lon * cos_lat);
	*y = _cenY + (int)floor(_radius * _zoom * sin_lat);
	//*y = _cenY + (int)floor(_radius * _zoom * (sin_lat * cos_rotY + cos_lat * sin_rotY * cos_lon));	
}

void Globe::cartToPolar(Sint16 x, Sint16 y, double *lat, double *lon)
{
	// Convert cartesian coordinates to polar
	*lat = asin((y - _cenY) / (_radius * _zoom));
	*lon = longitudeLoop(asin((x - _cenX) / (_radius * _zoom * cos(*lat))) - _rotX);
}

double Globe::longitudeLoop(double lon)
{
	// Keeps longitudes within 0 and 2*PI for algorithm's sake
	while (lon >= 2*PI)
		lon -= 2*PI;
	while (lon < 0)
		lon += 2*PI;

	return lon;
}

bool Globe::pointBack(double lon, double lat)
{
	// Is the point on the back side of the sphere?
	//return (longitudeLoop(lon + _rotX) > PI/2 && longitudeLoop(lon + _rotX) < 3*PI/2);
	return (sin(_rotY) * sin(lat) + cos(longitudeLoop(lon + _rotX)) * cos(lat) <= 0);
}

void Globe::loadDat(string filename)
{
	// Load file
	ifstream mapFile (filename.c_str(), ios::in | ios::binary);
	if (!mapFile)
	{
		throw "Failed to load DAT";
	}
	
	short value[10];

	while (mapFile.read((char*)&value, sizeof(value)))
	{
		Polygon* poly;
		int points;

		if (value[6] != -1)
		{
			points = 4;
		}
		else
		{
			points = 3;
			
		}
		poly = new Polygon(points);

		for (int i = 0, j = 0; i < points; i++)
		{
			// Correct X-Com degrees and convert to radians
			double lonRad = value[j++] * 0.125f * PI / 180;
			double latRad = value[j++] * 0.125f * PI / 180;

			poly->setLongitude(i, lonRad);
			poly->setLatitude(i, latRad);
		}
		poly->setTexture(value[8]);

		_polygons.push_back(poly);
	}

	if (!mapFile.eof())
	{
		throw "Invalid data from file";
	}

	mapFile.close();
}

void Globe::setTexture(SurfaceSet *texture)
{
	_texture = texture;
}

void Globe::blit(Surface *surface)
{
	Sint16 x[4], y[4];

	clear();
	SDL_LockSurface(this->getSurface());

	filledCircleRGBA(this->getSurface(), _cenX, _cenY, (Sint16)floor(_radius * _zoom), 16, 16, 16, Palette::blockOffset(12));
	for (vector<Polygon*>::iterator i = _polygons.begin(); i < _polygons.end(); i++)
	{
		// Don't draw if polygon is facing back
		bool backFace = true;
		for (int j = 0; j < (*i)->getPoints(); j++)
		{
			backFace = backFace && pointBack((*i)->getLongitude(j), (*i)->getLatitude(j));
		}
		if (backFace)
			continue;
		

		for (int j = 0; j < (*i)->getPoints(); j++)
		{
			polarToCart((*i)->getLatitude(j), (*i)->getLongitude(j), &x[j], &y[j]);
		}
		//texturedPolygon(getSurface(), (Sint16*)&x, (Sint16*)&y, (*i)->getPoints(), _texture->getSurface()->getSurface(), 0, 0);
		filledPolygonColor(getSurface(), (Sint16*)&x, (Sint16*)&y, (*i)->getPoints(), _texture->getSurface()->getPixel(0, 32*(*i)->getTexture()));
	}

	double testlat[4], testlon[4];
	Sint16 testx[4], testy[4];
	int xx[4] = {-1, -1, 1, 1};
	int yy[4] = {-1, 1, 1, -1};
	for (int i = 0; i < 4; i++) {
		cartToPolar(_testX, _testY, &testlat[i], &testlon[i]);
		polarToCart(testlat[i], testlon[i], &testx[i], &testy[i]);
		testx[i]+=xx[i];
		testy[i]+=yy[i];
	}

	polygonColor(getSurface(), (Sint16*)&testx, (Sint16*)&testy, 4, 1);

	SDL_UnlockSurface(this->getSurface());

	_rotX = longitudeLoop(_rotX + 0.01f);

	Surface::blit(surface);
}