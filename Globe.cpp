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

/**
 * Sets up a globe with the specified size and position.
 * @param cenX X position of the center of the globe.
 * @param cenY Y position of the center of the globe.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
Globe::Globe(int cenX, int cenY, int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _polygons(), _radius(90), _zoom(1), _rotLon(0), _rotLat(0), _cenX(cenX), _cenY(cenY), _testLon(0), _testLat(0)
{
}

/**
 *
 */
Globe::~Globe()
{
}

/**
 * Converts a polar point into a cartesian point for
 * mapping a polygon onto the 3D-looking globe.
 * @param lon Longitude of the polar point.
 * @param lat Latitude of the polar point.
 * @param x Pointer to the output X position.
 * @param y Pointer to the output Y position.
 */
void Globe::polarToCart(double lon, double lat, Sint16 *x, Sint16 *y)
{
	lon = longitudeLoop(lon + _rotLon);

	*x = _cenX + (int)floor(_radius * _zoom * sin(lon) * cos(lat));
	//*y = _cenY + (int)floor(_radius * _zoom * sin(lat));
	*y = _cenY + (int)floor(_radius * _zoom * (sin(lat) * cos(_rotLat) + cos(lat) * sin(_rotLat) * cos(lon)));
}

/**
 * Converts a cartesian point into a polar point for
 * mapping a globe click onto the flat world map.
 * If the cartesian point is outside the globe,
 * COORD_OUT_OF_BOUNDS is returned.
 * @param x X position of the cartesian point.
 * @param y Y position of the cartesian point.
 * @param lon Pointer to the output longitude.
 * @param lat Pointer to the output latitude.
 */
void Globe::cartToPolar(Sint16 x, Sint16 y, double *lon, double *lat)
{	
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

/**
 * Keeps longitudes within 0 and 2*PI for algorithm's sake.
 * @param lon Longitude.
 * @return Corrected longitude.
 */
double Globe::longitudeLoop(double lon)
{
	while (lon >= 2*PI)
		lon -= 2*PI;
	while (lon < 0)
		lon += 2*PI;

	return lon;
}

/**
 * Checks if a polar point is on the back-half of the globe,
 * invisible to the player, using Haversine's formula.
 * @param lon Longitude of the point.
 * @param lat Latitude of the point.
 * @return True if it's on the back, False if it's on the front.
 */
bool Globe::pointBack(double lon, double lat)
{
	double dLat = -_rotLat - lat;
	double dLon = -_rotLon - lon;

	// Haversine's formula
	double a = sin(dLat / 2) * sin(dLat / 2) +
			cos(lat) * cos(-_rotLat) * 
			sin(dLon / 2) * sin(dLon / 2); 
	double c = 2 * atan2(sqrt(a), sqrt(1 - a)); 
	
	// Distance to front center point must be at most PI/2 otherwise it's hidden.
	return c > PI/2;
}

/**
 * Checks if a polar point is inside a certain polygon.
 * @param lon Longitude of the point.
 * @param lat Latitude of the point.
 * @param poly Pointer to the polygon.
 * @return True if it's inside, False if it's outside.
 */
bool Globe::insidePolygon(double lon, double lat, Polygon *poly)
{
	bool c = false;
	for (int i = 0; i < poly->getPoints(); i++)
	{
		if (pointBack(lon, lat) != pointBack(poly->getLongitude(i), poly->getLatitude(i)))
			break;

		int j = (i + 1) % poly->getPoints();

		Sint16 x, y, x_i, x_j, y_i, y_j;
		polarToCart(poly->getLongitude(i), poly->getLatitude(i), &x_i, &y_i);
		polarToCart(poly->getLongitude(j), poly->getLatitude(j), &x_j, &y_j);
		polarToCart(lon, lat, &x, &y);

		if ( ((y_i > y) != (y_j > y)) &&
			 (x < (x_j - x_i) * (y - y_i) / (y_j - y_i) + x_i) )
			c = !c;
	}
	return c;
}

/**
 * Loads a series of map polar coordinates in X-Com format,
 * converts them and stores them in a set of polygons.
 * @param filename Filename of the DAT file.
 * @param polygons Pointer to the polygon set.
 * @sa http://www.ufopaedia.org/index.php?title=WORLD.DAT
 */
void Globe::loadDat(string filename, vector<Polygon*> *polygons)
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

		polygons->push_back(poly);
	}

	if (!mapFile.eof())
	{
		throw "Invalid data from file";
	}

	mapFile.close();
}

/**
 * Changes the set of textures for the globe to use
 * to fill the polygons.
 * @param texture Pointer to the texture surface set.
 */
void Globe::setTexture(SurfaceSet *texture)
{
	_texture = texture;
	draw();
}

/**
 * Changes the set of polygons for the globe to render.
 * @param polygons Pointer to the set of polygons.
 */
void Globe::setPolygons(vector<Polygon*> *polygons)
{
	_polygons = polygons;
}

/**
 * Rotates the globe by a specified amount of degrees.
 * @param lon Rotation in longitude.
 * @param lat Rotation in latitude.
 */
void Globe::rotate(double lon, double lat)
{
	_rotLon = longitudeLoop(_rotLon + lon);
	_rotLat += lat;
	draw();
}

/**
 * Zooms the globe by a specified amount.
 * @param amount Positive to zoom in, negative to zoom out.
 */
void Globe::zoom(double amount)
{
	_zoom += amount;
	if (_zoom < 1.0f)
		_zoom = 1.0f;
	else if (_zoom > 2.5f)
		_zoom = 2.5f;
	draw();
}

/**
 * Rotates the globe to center on a certain
 * polar point on the world map.
 * @param lon Longitude.
 * @param lat Latitude.
 */
void Globe::center(double lon, double lat)
{
	_rotLon = -lon;
	_rotLat = -lat;
	draw();
}

/**
 * Draws the whole globe,
 * rendering the ocean-coloured circle and all the
 * map polygons on top, converting the polar coordinates
 * to cartesian to simulate a 3D globe.
 */
void Globe::draw()
{
	Sint16 x[4], y[4];

	clear();
	// Lock the surface
	lock();

	filledCircleColor(this->getSurface(), _cenX, _cenY, (Sint16)floor(_radius * _zoom), Palette::getRGBA(this->getPalette(), Palette::blockOffset(12)));

	for (vector<Polygon*>::iterator i = _polygons->begin(); i < _polygons->end(); i++)
	{
		// Don't draw if polygon is facing back
		bool backFace = true;
		for (int j = 0; j < (*i)->getPoints(); j++)
		{
			backFace = backFace && pointBack((*i)->getLongitude(j), (*i)->getLatitude(j));
		}
		if (backFace)
			continue;
		
		// Convert coordinates
		for (int j = 0; j < (*i)->getPoints(); j++)
		{
			polarToCart((*i)->getLongitude(j), (*i)->getLatitude(j), &x[j], &y[j]);
		}
		//texturedPolygon(getSurface(), (Sint16*)&x, (Sint16*)&y, (*i)->getPoints(), _texture->getSurface()->getSurface(), 0, 0);
		filledPolygonColor(getSurface(), (Sint16*)&x, (Sint16*)&y, (*i)->getPoints(), Palette::getRGBA(this->getPalette(), _texture->getSurface()->getPixel(0, 32*(*i)->getTexture())));
		//polygonColor(getSurface(), (Sint16*)&x, (Sint16*)&y, (*i)->getPoints(), _texture->getSurface()->getPixel(0, 32*(*i)->getTexture()));
	}
	
	// Draw the test marker
	if (!pointBack(_testLon, _testLat))
	{
		Sint16 testx[4], testy[4];
		int xx[4] = {-1, -1, 1, 1};
		int yy[4] = {-1, 1, 1, -1};
		for (int i = 0; i < 4; i++)
		{
			polarToCart(_testLon, _testLat, &testx[i], &testy[i]);
			testx[i] += xx[i];
			testy[i] += yy[i];
		}

		polygonColor(getSurface(), (Sint16*)&testx, (Sint16*)&testy, 4, Palette::getRGBA(this->getPalette(), 1));
	}
	
	// Unlock the surface
	unlock();
}

/**
 * Ignores any mouse clicks that are outside the globe.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void Globe::handle(SDL_Event *ev, int scale, State *state)
{
	double pos[2];
	cartToPolar(ev->button.x / scale, ev->button.y / scale, &pos[0], &pos[1]);

	if (pos[0] != COORD_OUT_OF_BOUNDS)
		InteractiveSurface::handle(ev, scale, state);
}

/**
 * Places a test marker on the globe and outputs if
 * it's inside one of the polygons of the map.
 * @param lon Longitude.
 * @param lat Latitude.
 */
void Globe::test(double lon, double lat)
{
	_testLon = lon;
	_testLat = lat;
	bool inside = false;
	for (vector<Polygon*>::iterator i = _polygons->begin(); i < _polygons->end() && !inside; i++)
	{
		inside = insidePolygon(_testLon, _testLat, *i);
	}
	cout << inside << endl;
	draw();
}
