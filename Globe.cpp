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

Globe::Globe(int cenX, int cenY, int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _polygons(), _cenX(cenX), _cenY(cenY), _radius(90), _zoom(1), _rotLon(0), _rotLat(0), _testLon(0), _testLat(0)
{
}

Globe::~Globe()
{
}

void Globe::polarToCart(double lon, double lat, Sint16 *x, Sint16 *y)
{
	lon = longitudeLoop(lon + _rotLon);
	
	// Pre-calculate why not
	double sin_lon = sin(lon), sin_lat = sin(lat), sin_rotLat = sin(_rotLat);
	double cos_lon = cos(lon), cos_lat = cos(lat), cos_rotLat = cos(_rotLat);

	// Convert polar coordinates to cartesian
	*x = _cenX + (int)floor(_radius * _zoom * sin_lon * cos_lat);
	*y = _cenY + (int)floor(_radius * _zoom * sin_lat);
	//*y = _cenY + (int)floor(_radius * _zoom * (sin_lat * cos_rotLat + cos_lat * sin_rotLat * cos_lon));	
}

void Globe::cartToPolar(Sint16 x, Sint16 y, double *lon, double *lat)
{	
	// Convert cartesian coordinates to polar
	*lat = asin((y - _cenY) / (_radius * _zoom));
	*lon = asin((x - _cenX) / (_radius * _zoom * cos(*lat)));

	// Check for errors
	if (!(*lat == *lat && *lon == *lon))
	{
		*lat = COORD_OUT_OF_BOUNDS;
		*lon = COORD_OUT_OF_BOUNDS;
	}
	else
	{
		*lon = longitudeLoop(*lon - _rotLon);
	}
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
	return (longitudeLoop(lon + _rotLon) > PI/2 && longitudeLoop(lon + _rotLon) < 3*PI/2);
	//return (sin(_rotLat) * sin(lat) + cos(longitudeLoop(lon + _rotLon)) * cos(lat) <= 0);
}

bool Globe::insidePolygon(double lon, double lat, Polygon *poly)
{
	
	double sign = 0;
	// Is the point inside terrain?
	for (int i = 0; i < poly->getPoints(); i++)
	{
		int j = (i + 1) % poly->getPoints();

		double lat_i = poly->getLatitude(i), lat_j = poly->getLatitude(j);
		double lon_i = poly->getLongitude(i), lon_j = poly->getLongitude(j);
		/*
		if (lon_i < PI/6 && lon_j > 11*(PI/6))
			lon_j -= 2*PI;
		else if (lon_j < PI/6 && lon_i > 11*(PI/6))
			lon_i -= 2*PI;
		*/
		double d = (lat_j - lat_i) * lon + (lon_i - lon_j) * lat + lat_i * lon_j - lon_i * lat_j;

		if ((d < 0 && sign > 0) || (d > 0 && sign < 0))
			return false;
		else
			sign = d;
	}
	return true;
	
	/*
	int i, j;
	bool c = false;
	for (i = 0, j = nvert-1; i < nvert; j = i++) {
		if ( ((verty[i]>testy) != (verty[j]>testy)) &&
			 (testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]) )
		c = !c;
	}
	return c;
	*/
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

void Globe::rotate(double lon, double lat)
{
	_rotLon = longitudeLoop(_rotLon + lon);
	//_rotLat += lat;
}

void Globe::zoom(double amount)
{
	_zoom += amount;
	if (_zoom < 1.0f)
		_zoom = 1.0f;
	else if (_zoom > 2.5f)
		_zoom = 2.5f;
}

void Globe::center(double lon, double lat)
{
	_rotLon = -lon;
	//_rotLat = lat;
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
			polarToCart((*i)->getLongitude(j), (*i)->getLatitude(j), &x[j], &y[j]);
		}
		//texturedPolygon(getSurface(), (Sint16*)&x, (Sint16*)&y, (*i)->getPoints(), _texture->getSurface()->getSurface(), 0, 0);
		filledPolygonColor(getSurface(), (Sint16*)&x, (Sint16*)&y, (*i)->getPoints(), _texture->getSurface()->getPixel(0, 32*(*i)->getTexture()));
		//polygonColor(getSurface(), (Sint16*)&x, (Sint16*)&y, (*i)->getPoints(), _texture->getSurface()->getPixel(0, 32*(*i)->getTexture()));
	}
	
	if (!pointBack(_testLon, _testLat))
	{
		Sint16 testx[4], testy[4];
		int xx[4] = {-1, -1, 1, 1};
		int yy[4] = {-1, 1, 1, -1};
		for (int i = 0; i < 4; i++)
		{
			polarToCart(_testLon, _testLat, &testx[i], &testy[i]);
			testx[i]+=xx[i];
			testy[i]+=yy[i];
		}

		polygonColor(getSurface(), (Sint16*)&testx, (Sint16*)&testy, 4, 1);
	}
	
	SDL_UnlockSurface(this->getSurface());

	Surface::blit(surface);
}

void Globe::handle(SDL_Event *ev, int scale, State *state)
{
	double pos[2];
	cartToPolar(ev->button.x / scale, ev->button.y / scale, &pos[0], &pos[1]);

	if (pos[0] != COORD_OUT_OF_BOUNDS)
		InteractiveSurface::handle(ev, scale, state);
}

void Globe::test(double lon, double lat)
{
	_testLon = lon;
	_testLat = lat;
	bool inside = false;
	for (vector<Polygon*>::iterator i = _polygons.begin(); i < _polygons.end() && !inside; i++)
	{
		inside = insidePolygon(_testLon, _testLat, *i);
	}
	cout << inside << endl;
}