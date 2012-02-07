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
#define _USE_MATH_DEFINES
#include "Globe.h"
#include <cmath>
#include <fstream>
#include "../Engine/Action.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Timer.h"
#include "../Resource/ResourcePack.h"
#include "Polygon.h"
#include "Polyline.h"
#include "../Engine/Palette.h"
#include "../Engine/Game.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/GameTime.h"
#include "../Savegame/Base.h"
#include "../Savegame/Country.h"
#include "../Ruleset/RuleCountry.h"
#include "../Interface/Text.h"
#include "../Engine/Font.h"
#include "../Engine/Language.h"
#include "../Engine/Exception.h"
#include "../Ruleset/RuleRegion.h"
#include "../Savegame/Region.h"
#include "../Ruleset/City.h"
#include "../Savegame/Target.h"
#include "../Savegame/Ufo.h"
#include "../Savegame/Craft.h"
#include "../Savegame/Waypoint.h"

namespace OpenXcom
{

const double Globe::QUAD_LONGITUDE = 0.05;
const double Globe::QUAD_LATITUDE = 0.2;
const double Globe::ROTATE_LONGITUDE = 0.25;
const double Globe::ROTATE_LATITUDE = 0.15;

/**
 * Sets up a globe with the specified size and position.
 * @param game Pointer to core game.
 * @param cenX X position of the center of the globe.
 * @param cenY Y position of the center of the globe.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
Globe::Globe(Game *game, int cenX, int cenY, int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _radius(), _cenLon(-0.01), _cenLat(-0.1), _rotLon(0.0), _rotLat(0.0), _cenX(cenX), _cenY(cenY), _zoom(0), _game(game), _blink(true), _detail(true), _cacheLand()
{
	_texture[0] = _game->getResourcePack()->getSurfaceSet("TEXTURE.DAT");
	for (int shade = 1; shade < NUM_SHADES; shade++)
	{
		_texture[shade] = new SurfaceSet(*_texture[0]);
		for (int f = 0; f < _texture[shade]->getTotalFrames(); f++)
			_texture[shade]->getFrame(f)->offset(shade);
	}

	_radius.push_back(90);
	_radius.push_back(120);
	_radius.push_back(180);
	_radius.push_back(360);
	_radius.push_back(450);
	_radius.push_back(720);

	_countries = new Surface(width, height, x, y);
	_markers = new Surface(width, height, x, y);

	// Animation timers
	_blinkTimer = new Timer(100);
	_blinkTimer->onTimer((SurfaceHandler)&Globe::blink);
	_blinkTimer->start();
	_rotTimer = new Timer(50);
	_rotTimer->onTimer((SurfaceHandler)&Globe::rotate);

	// Globe markers
	_mkXcomBase = new Surface(3, 3);
	_mkXcomBase->lock();
	_mkXcomBase->setPixel(0, 0, 9);
	_mkXcomBase->setPixel(1, 0, 9);
	_mkXcomBase->setPixel(2, 0, 9);
	_mkXcomBase->setPixel(0, 1, 9);
	_mkXcomBase->setPixel(2, 1, 9);
	_mkXcomBase->setPixel(0, 2, 9);
	_mkXcomBase->setPixel(1, 2, 9);
	_mkXcomBase->setPixel(2, 2, 9);
	_mkXcomBase->unlock();

	_mkAlienBase = new Surface(3, 3);
	_mkAlienBase->lock();
	_mkAlienBase->setPixel(0, 0, 1);
	_mkAlienBase->setPixel(1, 0, 1);
	_mkAlienBase->setPixel(2, 0, 1);
	_mkAlienBase->setPixel(0, 1, 1);
	_mkAlienBase->setPixel(2, 1, 1);
	_mkAlienBase->setPixel(0, 2, 1);
	_mkAlienBase->setPixel(1, 2, 1);
	_mkAlienBase->setPixel(2, 2, 1);
	_mkAlienBase->unlock();

	_mkCraft = new Surface(3, 3);
	_mkCraft->lock();
	_mkCraft->setPixel(1, 0, 11);
	_mkCraft->setPixel(0, 1, 11);
	_mkCraft->setPixel(2, 1, 11);
	_mkCraft->setPixel(1, 2, 11);
	_mkCraft->unlock();

	_mkWaypoint = new Surface(3, 3);
	_mkWaypoint->lock();
	_mkWaypoint->setPixel(0, 0, 3);
	_mkWaypoint->setPixel(0, 2, 3);
	_mkWaypoint->setPixel(1, 1, 3);
	_mkWaypoint->setPixel(2, 0, 3);
	_mkWaypoint->setPixel(2, 2, 3);
	_mkWaypoint->unlock();

	_mkCity = new Surface(3, 3);
	_mkCity->lock();
	_mkCity->setPixel(0, 0, 14);
	_mkCity->setPixel(1, 0, 14);
	_mkCity->setPixel(2, 0, 14);
	_mkCity->setPixel(0, 1, 14);
	_mkCity->setPixel(1, 1, 11);
	_mkCity->setPixel(2, 1, 14);
	_mkCity->setPixel(0, 2, 14);
	_mkCity->setPixel(1, 2, 14);
	_mkCity->setPixel(2, 2, 14);
	_mkCity->unlock();

	_mkFlyingUfo = new Surface(3, 3);
	_mkFlyingUfo->lock();
	_mkFlyingUfo->setPixel(1, 0, 13);
	_mkFlyingUfo->setPixel(0, 1, 13);
	_mkFlyingUfo->setPixel(1, 1, 13);
	_mkFlyingUfo->setPixel(2, 1, 13);
	_mkFlyingUfo->setPixel(1, 2, 13);
	_mkFlyingUfo->unlock();

	_mkLandedUfo = new Surface(3, 3);
	_mkLandedUfo->lock();
	_mkLandedUfo->setPixel(0, 0, 7);
	_mkLandedUfo->setPixel(0, 2, 7);
	_mkLandedUfo->setPixel(1, 1, 7);
	_mkLandedUfo->setPixel(2, 0, 7);
	_mkLandedUfo->setPixel(2, 2, 7);
	_mkLandedUfo->unlock();

	_mkCrashedUfo = new Surface(3, 3);
	_mkCrashedUfo->lock();
	_mkCrashedUfo->setPixel(0, 0, 5);
	_mkCrashedUfo->setPixel(0, 2, 5);
	_mkCrashedUfo->setPixel(1, 1, 5);
	_mkCrashedUfo->setPixel(2, 0, 5);
	_mkCrashedUfo->setPixel(2, 2, 5);
	_mkCrashedUfo->unlock();

	_mkAlienSite = new Surface(3, 3);
	_mkAlienSite->lock();
	_mkAlienSite->setPixel(1, 0, 1);
	_mkAlienSite->setPixel(0, 1, 1);
	_mkAlienSite->setPixel(1, 1, 1);
	_mkAlienSite->setPixel(2, 1, 1);
	_mkAlienSite->setPixel(1, 2, 1);
	_mkAlienSite->unlock();

	cachePolygons();
}

/**
 * Deletes the contained surfaces.
 */
Globe::~Globe()
{
	for (int i = 1; i < NUM_SHADES; ++i)
		delete _texture[i];

	delete _blinkTimer;
	delete _rotTimer;
	delete _countries;
	delete _markers;
	delete _mkXcomBase;
	delete _mkAlienBase;
	delete _mkCraft;
	delete _mkWaypoint;
	delete _mkCity;
	delete _mkFlyingUfo;
	delete _mkLandedUfo;
	delete _mkCrashedUfo;
	delete _mkAlienSite;

	for (std::list<Polygon*>::iterator i = _cacheLand.begin(); i != _cacheLand.end(); ++i)
	{
		delete *i;
	}
}

/**
 * Converts a polar point into a cartesian point for
 * mapping a polygon onto the 3D-looking globe.
 * @param lon Longitude of the polar point.
 * @param lat Latitude of the polar point.
 * @param x Pointer to the output X position.
 * @param y Pointer to the output Y position.
 */
void Globe::polarToCart(double lon, double lat, Sint16 *x, Sint16 *y) const
{
	// Orthographic projection
	*x = _cenX + (Sint16)floor(_radius[_zoom] * cos(lat) * sin(lon - _cenLon));
	*y = _cenY + (Sint16)floor(_radius[_zoom] * (cos(_cenLat) * sin(lat) - sin(_cenLat) * cos(lat) * cos(lon - _cenLon)));
}

/**
 * Converts a cartesian point into a polar point for
 * mapping a globe click onto the flat world map.
 * @param x X position of the cartesian point.
 * @param y Y position of the cartesian point.
 * @param lon Pointer to the output longitude.
 * @param lat Pointer to the output latitude.
 */
void Globe::cartToPolar(Sint16 x, Sint16 y, double *lon, double *lat) const
{
	// Orthographic projection
	x -= _cenX;
	y -= _cenY;

	double rho = sqrt((double)(x*x + y*y));
	double c = asin(rho / (_radius[_zoom]));

	*lat = asin((y * sin(c) * cos(_cenLat)) / rho + cos(c) * sin(_cenLat));
	*lon = atan2(x * sin(c),(rho * cos(_cenLat) * cos(c) - y * sin(_cenLat) * sin(c))) + _cenLon;

	// Keep between 0 and 2xPI
	while (*lon < 0)
		*lon += 2 * M_PI;
	while (*lon >= 2 * M_PI)
		*lon -= 2 * M_PI;
}

/**
 * Checks if a polar point is on the back-half of the globe,
 * invisible to the player.
 * @param lon Longitude of the point.
 * @param lat Latitude of the point.
 * @return True if it's on the back, False if it's on the front.
 */
bool Globe::pointBack(double lon, double lat) const
{
	double c = cos(_cenLat) * cos(lat) * cos(lon - _cenLon) + sin(_cenLat) * sin(lat);

	return c < 0;
}


/** Return latitude of last visible to player point on given longitude.
 * @param lon Longitude of the point.
 * @return Longitude of last visible point.
 */
double Globe::lastVisibleLat(double lon) const
{
//	double c = cos(_cenLat) * cos(lat) * cos(lon - _cenLon) + sin(_cenLat) * sin(lat);
//		tan(lat) = -cos(_cenLat) * cos(lon - _cenLon)/sin(_cenLat) ;
	return atan(-cos(_cenLat) * cos(lon - _cenLon)/sin(_cenLat));
}

/**
 * Checks if a polar point is inside a certain polygon.
 * @param lon Longitude of the point.
 * @param lat Latitude of the point.
 * @param poly Pointer to the polygon.
 * @return True if it's inside, False if it's outside.
 */
bool Globe::insidePolygon(double lon, double lat, Polygon *poly) const
{
	bool backFace = true;
	for (int i = 0; i < poly->getPoints(); ++i)
	{
		backFace = backFace && pointBack(poly->getLongitude(i), poly->getLatitude(i));
	}
	if (backFace != pointBack(lon, lat))
		return false;

	bool c = false;
	for (int i = 0; i < poly->getPoints(); ++i)
	{
		int j = (i + 1) % poly->getPoints();

		Sint16 x, y, x_i, x_j, y_i, y_j;
		polarToCart(poly->getLongitude(i), poly->getLatitude(i), &x_i, &y_i);
		polarToCart(poly->getLongitude(j), poly->getLatitude(j), &x_j, &y_j);
		polarToCart(lon, lat, &x, &y);

		if ( ((y_i > y) != (y_j > y)) &&
			 (x < (x_j - x_i) * (y - y_i) / (y_j - y_i) + x_i) )
		{
			c = !c;
		}
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
void Globe::loadDat(const std::string &filename, std::list<Polygon*> *polygons)
{
	// Load file
	std::ifstream mapFile (filename.c_str(), std::ios::in | std::ios::binary);
	if (!mapFile)
	{
		throw Exception("Failed to load DAT");
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

		for (int i = 0, j = 0; i < points; ++i)
		{
			// Correct X-Com degrees and convert to radians
			double lonRad = value[j++] * 0.125f * M_PI / 180;
			double latRad = value[j++] * 0.125f * M_PI / 180;

			poly->setLongitude(i, lonRad);
			poly->setLatitude(i, latRad);
		}
		poly->setTexture(value[8]);

		polygons->push_back(poly);
	}

	if (!mapFile.eof())
	{
		throw Exception("Invalid data from file");
	}

	mapFile.close();
}

/**
 * Sets a leftwards rotation speed and starts the timer.
 */
void Globe::rotateLeft()
{
	_rotLon = -ROTATE_LONGITUDE;
	_rotTimer->start();
}

/**
 * Sets a rightwards rotation speed and starts the timer.
 */
void Globe::rotateRight()
{
	_rotLon = ROTATE_LONGITUDE;
	_rotTimer->start();
}

/**
 * Sets a upwards rotation speed and starts the timer.
 */
void Globe::rotateUp()
{
	_rotLat = -ROTATE_LATITUDE;
	_rotTimer->start();
}

/**
 * Sets a downwards rotation speed and starts the timer.
 */
void Globe::rotateDown()
{
	_rotLat = ROTATE_LATITUDE;
	_rotTimer->start();
}

/**
 * Resets the rotation speed and timer.
 */
void Globe::rotateStop()
{
	_rotLon = 0.0;
	_rotLat = 0.0;
	_rotTimer->stop();
}

/**
 * Increases the zoom level on the globe.
 */
void Globe::zoomIn()
{
	if (_zoom < _radius.size() - 1)
	{
		_zoom++;
		cachePolygons();
	}
}

/**
 * Decreases the zoom level on the globe.
 */
void Globe::zoomOut()
{
	if (_zoom > 0)
	{
		_zoom--;
		cachePolygons();
	}
}

/**
 * Zooms the globe out as far as possible.
 */
void Globe::zoomMin()
{
	_zoom = 0;
	cachePolygons();
}

/**
 * Zooms the globe in as close as possible.
 */
void Globe::zoomMax()
{
	_zoom = _radius.size() - 1;
	cachePolygons();
}

/**
 * Rotates the globe to center on a certain
 * polar point on the world map.
 * @param lon Longitude of the point.
 * @param lat Latitude of the point.
 */
void Globe::center(double lon, double lat)
{
	_cenLon = lon;
	_cenLat = lat;
	// HORRIBLE HORRORS CONTAINED WITHIN
	if (_cenLon > -0.01 && _cenLon < 0.01)
	{
		_cenLon = -0.01;
	}
	if (_cenLat > -0.01 && _cenLat < 0.01)
	{
		_cenLat = -0.1;
	}
	cachePolygons();
}

/**
 * Checks if a polar point is inside the globe's landmass.
 * @param lon Longitude of the point.
 * @param lat Latitude of the point.
 * @return True if it's inside, False if it's outside.
 */
bool Globe::insideLand(double lon, double lat) const
{
	bool inside = false;
	for (std::list<Polygon*>::iterator i = _game->getResourcePack()->getPolygons()->begin(); i != _game->getResourcePack()->getPolygons()->end() && !inside; ++i)
	{
		inside = insidePolygon(lon, lat, *i);
	}
	return inside;
}

/**
 * Switches the amount of detail shown on the globe.
 * With detail on, country and city details are shown when zoomed in.
 */
void Globe::toggleDetail()
{
	_detail = !_detail;
	drawDetail();
}

/**
 * Checks if a certain target is near a certain cartesian point
 * (within a circled area around it) over the globe.
 * @param target Pointer to target.
 * @param x X coordinate of point.
 * @param y Y coordinate of point.
 * @return True if it's near, false otherwise.
 */
bool Globe::targetNear(Target* target, int x, int y) const
{
	Sint16 tx, ty;
	polarToCart(target->getLongitude(), target->getLatitude(), &tx, &ty);

	int dx = x - tx;
	int dy = y - ty;
	return (dx * dx + dy * dy <= NEAR_RADIUS);
}

/**
 * Returns a list of all the targets currently near a certain
 * cartesian point over the globe.
 * @param x X coordinate of point.
 * @param y Y coordinate of point.
 * @param craft Only get craft targets.
 * @return List of pointers to targets.
 */
std::vector<Target*> Globe::getTargets(int x, int y, bool craft) const
{
	std::vector<Target*> v;
	if (!craft)
	{
		for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
		{
			if ((*i)->getLongitude() == 0.0 && (*i)->getLatitude() == 0.0)
				continue;

			if (targetNear((*i), x, y))
			{
				v.push_back(*i);
			}

			for (std::vector<Craft*>::iterator j = (*i)->getCrafts()->begin(); j != (*i)->getCrafts()->end(); ++j)
			{
				if ((*j)->getLongitude() == (*i)->getLongitude() && (*j)->getLatitude() == (*i)->getLatitude() && (*j)->getDestination() == 0)
					continue;

				if (targetNear((*j), x, y))
				{
					v.push_back(*j);
				}
			}
		}
	}
	for (std::vector<Ufo*>::iterator i = _game->getSavedGame()->getUfos()->begin(); i != _game->getSavedGame()->getUfos()->end(); ++i)
	{
		if (!(*i)->getDetected())
			continue;

		if (targetNear((*i), x, y))
		{
			v.push_back(*i);
		}
	}
	for (std::vector<Waypoint*>::iterator i = _game->getSavedGame()->getWaypoints()->begin(); i != _game->getSavedGame()->getWaypoints()->end(); ++i)
	{
		if (targetNear((*i), x, y))
		{
			v.push_back(*i);
		}
	}
	return v;
}

/**
 * Takes care of pre-calculating all the polygons currently visible
 * on the globe and caching them so they only need to be recalculated
 * when the globe is actually moved.
 */
void Globe::cachePolygons()
{
	cache(_game->getResourcePack()->getPolygons(), &_cacheLand);
	_redraw = true;
}

/**
 * Caches a set of polygons.
 * @param polygons Pointer to list of polygons.
 * @param cache Pointer to cache.
 */
void Globe::cache(std::list<Polygon*> *polygons, std::list<Polygon*> *cache)
{
	// Clear existing cache
	for (std::list<Polygon*>::iterator i = cache->begin(); i != cache->end(); ++i)
	{
		delete *i;
	}
	cache->clear();

	// Pre-calculate values to cache
	for (std::list<Polygon*>::iterator i = polygons->begin(); i != polygons->end(); ++i)
	{
		// Is quad on the back face?
		bool backFace = true;
		for (int j = 0; j < (*i)->getPoints(); ++j)
		{
			backFace = backFace && pointBack((*i)->getLongitude(j), (*i)->getLatitude(j));
		}
		if (backFace)
			continue;

		Polygon* p = new Polygon(**i);

		// Convert coordinates
		for (int j = 0; j < p->getPoints(); ++j)
		{
			Sint16 x, y;
			polarToCart(p->getLongitude(j), p->getLatitude(j), &x, &y);
			p->setX(j, x);
			p->setY(j, y);
		}

		cache->push_back(p);
	}
}

/**
 * Replaces a certain amount of colors in the palette of the globe.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void Globe::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	Surface::setPalette(colors, firstcolor, ncolors);
	for (int shade = 0; shade < NUM_SHADES; shade++)
	{
		_texture[shade]->setPalette(colors, firstcolor, ncolors);
	}
	_countries->setPalette(colors, firstcolor, ncolors);
	_markers->setPalette(colors, firstcolor, ncolors);
	_mkXcomBase->setPalette(colors, firstcolor, ncolors);
	_mkAlienBase->setPalette(colors, firstcolor, ncolors);
	_mkCraft->setPalette(colors, firstcolor, ncolors);
	_mkWaypoint->setPalette(colors, firstcolor, ncolors);
	_mkCity->setPalette(colors, firstcolor, ncolors);
	_mkFlyingUfo->setPalette(colors, firstcolor, ncolors);
	_mkLandedUfo->setPalette(colors, firstcolor, ncolors);
	_mkCrashedUfo->setPalette(colors, firstcolor, ncolors);
	_mkAlienSite->setPalette(colors, firstcolor, ncolors);
}

/**
 * Keeps the animation timers running.
 */
void Globe::think()
{
	_blinkTimer->think(0, this);
	_rotTimer->think(0, this);
}

/**
 * Makes the globe markers blink.
 */
void Globe::blink()
{
	_blink = !_blink;

	int off = 0;
	if (_blink)
		off = -1;
	else
		off = 1;

	_mkXcomBase->offset(off);
	_mkAlienBase->offset(off);
	_mkCraft->offset(off);
	_mkWaypoint->offset(off);
	_mkFlyingUfo->offset(off);
	_mkLandedUfo->offset(off);
	_mkCrashedUfo->offset(off);
	_mkAlienSite->offset(off);

	drawMarkers();
}

/**
 * Rotates the globe by a set amount. Necessary
 * since the globe keeps rotating while a button
 * is pressed down.
 */
void Globe::rotate()
{
	_cenLon += _rotLon;
	_cenLat += _rotLat;
	// DON'T UNLEASH THE TERRORS
	if (_cenLon > -0.01 && _cenLon < 0.01)
	{
		_cenLon = -0.01;
	}
	if (_cenLat > -0.01 && _cenLat < 0.01)
	{
		_cenLat = -0.1;
	}
	cachePolygons();
}

/**
 * Draws the whole globe, part by part.
 */
void Globe::draw()
{
	Surface::draw();
	drawOcean();
	drawLand();
	drawMarkers();
	drawDetail();
}

/**
 * Draws a segment of the ocean shade along the longitude.
 * @param startLon Starting longitude.
 * @param endLon Ending longitude.
 * @param colourShift Colour shade.
 */
void Globe::fillLongitudeSegments(double startLon, double endLon, int colourShift)
{
	double traceLon, traceLat, endLan, startLan;
	double dL; // dL - delta of Latitude and used as delta of pie
	Sint16 direction, x, y;
	std::vector<Sint16> polyPointsX, polyPointsY, polyPointsX2, polyPointsY2;
	Sint16 *dx, *dy;
	double sx, sy;
	double angle1 = 0.0, angle2 = 0.0;
	bool bigLonAperture = false;

	if (abs(startLon-endLon) > 1)
	{
		bigLonAperture = 1;
	}

	// find two latitudes where
	startLan = lastVisibleLat(startLon);
	endLan   = lastVisibleLat(endLon);

	traceLon = startLon;

	// If North pole visible, we want to head south (+1), if South pole, head North (-1)
	if (!pointBack(traceLon, -M_PI_2))
	{
		direction = 1;
	}
	else
	{
		direction = -1;
	}

	// Draw globe depending on the direction
	if (direction == 1)
	{
		// draw fisrt longtitude line from pole
		traceLon = startLon;
		dL = (startLan + M_PI_2) / 20;
		for (traceLat = -M_PI_2; traceLat < startLan; traceLat += dL)
		{
			polarToCart(traceLon, traceLat, &x, &y);
			polyPointsX.push_back(x);
			polyPointsY.push_back(y);
		}

		// if aperture of longtitude is big then we need find first angle of sector
		if (bigLonAperture)
		{
			sx = x - _cenX;
			sy = y - _cenY;
			angle1 = atan(sy / sx);
			if (sx < 0) angle1 += M_PI;
		}

		// draw second longtitude line from pole
		traceLon = endLon;
		dL = (endLan + M_PI_2) / 20;
		for (traceLat = -M_PI_2; traceLat < endLan; traceLat += dL)
		{
			polarToCart(traceLon, traceLat, &x, &y);
			polyPointsX2.push_back(x);
			polyPointsY2.push_back(y);
		}

		// if aperture of longtitudes is big we need find second angle of sector and draw pie of circle between two longtitudes
		if (bigLonAperture)
		{
			sx = x - _cenX;
			sy = y - _cenY;
			angle2 = atan(sy/sx);
			if (sx < 0)
			{
				angle2 += M_PI;
			}

			// draw sector part of circle
			if (angle1 > angle2)
			{
				dL = (angle1 - angle2) / 20;
				for (double a = angle2 + dL / 2; a < angle1; a += dL)
				{
					x = _cenX + (Sint16)floor(_radius[_zoom] * cos(a));
					y = _cenY + (Sint16)floor(_radius[_zoom] * sin(a));
					polyPointsX2.push_back(x);
					polyPointsY2.push_back(y);
				}
			}
			else
			{
				dL = (2*M_PI + angle1 - angle2) / 20;
				for (double a = angle2 + dL / 2; a < 2*M_PI + angle1; a += dL)
				{
					x = _cenX + (Sint16)floor(_radius[_zoom] * cos(a));
					y = _cenY + (Sint16)floor(_radius[_zoom] * sin(a));
					polyPointsX2.push_back(x);
					polyPointsY2.push_back(y);
				}
			}
		}
	}
	else // another direction
	{
		// draw fisrt longtitude line from pole
		traceLon = startLon;
		dL = (startLan - M_PI_2) / 20;
		for (traceLat = M_PI_2; traceLat > startLan; traceLat += dL)
		{
			polarToCart(traceLon, traceLat, &x, &y);
			polyPointsX.push_back(x);
			polyPointsY.push_back(y);
		}

		// if aperture of longtitude is big then we need find first angle of sector of pie between longtitudes
		if (bigLonAperture)
		{
			sx = x - _cenX;
			sy = y - _cenY;
			angle1 = atan(sy / sx);
			if (sx < 0)
			{
				angle1 += M_PI;
			}
		}

		// draw second longtitude line from pole
		traceLon = endLon;
		dL = (endLan - M_PI_2) / 20;
		for (traceLat = M_PI_2; traceLat > endLan; traceLat += dL)
		{
			polarToCart(traceLon, traceLat, &x, &y);
			polyPointsX2.push_back(x);
			polyPointsY2.push_back(y);
		}

		// if aperture of longtitudes is big we need find second angle of sector and draw pie of circle between two longtitudes
		if (bigLonAperture)
		{
			sx = x - _cenX;
			sy = y - _cenY;
			angle2 = atan(sy / sx);
			if (sx < 0)
			{
				angle2 += M_PI;
			}
			if (angle2 > angle1)
			{
				dL = (angle2 - angle1) / 20;
				for (double a = angle1 + dL / 2; a < angle2; a += dL)
				{
					x = _cenX + (Sint16)floor(_radius[_zoom] * cos(a));
					y = _cenY + (Sint16)floor(_radius[_zoom] * sin(a));
					polyPointsX.push_back(x);
					polyPointsY.push_back(y);
				}
			}
			else
			{
				dL = (2*M_PI + angle2 - angle1) / 20;
				for (double a = angle1 + dL / 2; a < 2*M_PI + angle2; a += dL)
				{
					x = _cenX + (Sint16)floor(_radius[_zoom] * cos(a));
					y = _cenY + (Sint16)floor(_radius[_zoom] * sin(a));
					polyPointsX.push_back(x);
					polyPointsY.push_back(y);
				}
			}
		}
	}

	dx = new Sint16[polyPointsX.size()+polyPointsX2.size()];
	dy = new Sint16[polyPointsX.size()+polyPointsX2.size()];

	if (!polyPointsX.empty() || !polyPointsX2.empty())
	{
		for (unsigned int i = 0; i < polyPointsX.size(); ++i)
		{
			dx[i] = polyPointsX.at(i);
			dy[i] = polyPointsY.at(i);
		}

		for (unsigned int i = 0 ; i < polyPointsX2.size() ; ++i)
		{
			dx[i+polyPointsX.size()] = polyPointsX2.at(polyPointsX2.size()-1-i);
			dy[i+polyPointsX.size()] = polyPointsY2.at(polyPointsX2.size()-1-i);
		}
		drawPolygon(dx, dy, polyPointsX.size()+polyPointsX2.size(), Palette::blockOffset(12) + colourShift);
	}

	delete[] dx;
	delete[] dy;
}

/**
 * Renders the ocean, shading it according to the time of day.
 */
void Globe::drawOcean()
{
	double curTime = _game->getSavedGame()->getTime()->getDaylight();
	double dayLon = -curTime * 2*M_PI;
	double nightLon = dayLon + M_PI;

	lock();

	drawCircle(_cenX, _cenY, (Sint16)floor(_radius[_zoom]), Palette::blockOffset(12)+28);

	fillLongitudeSegments(dayLon   + QUAD_LONGITUDE, nightLon - QUAD_LONGITUDE, 0);
	fillLongitudeSegments(dayLon - QUAD_LONGITUDE, dayLon, 16);
	fillLongitudeSegments(dayLon, dayLon + QUAD_LONGITUDE, 8);
	fillLongitudeSegments(nightLon - QUAD_LONGITUDE, nightLon, 8);
	fillLongitudeSegments(nightLon, nightLon + QUAD_LONGITUDE, 16);

	unlock();
}

/**
 * Calculates the shade of a polygon based
 * on its coordinates and current game time.
 * @param p Pointer to polygon.
 * @return Shade value (0-7).
 */
int Globe::getShade(Polygon *p) const
{
	int _shades[] = {3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3,
					 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 5, 4};
	double minLon = 100.0, maxLon = -100.0, curTime = _game->getSavedGame()->getTime()->getDaylight();
	bool pole = false;

	// Convert coordinates
	for (int j = 0; j < p->getPoints(); ++j)
	{
		double tmpLon = p->getLongitude(j);
		double tmpLat = p->getLatitude(j);

		if (abs(tmpLat) < (M_PI_2 - 0.0001)) //pole vertexes have no longitude
		{
			if (tmpLon < minLon && tmpLon >= (maxLon - M_PI))
				minLon = tmpLon;
			if (tmpLon > maxLon && tmpLon <= (minLon + M_PI))
				maxLon = tmpLon;
		}
		else
		{
			pole = true;
		}
	}

	int shade = (int)((curTime + (((minLon + maxLon) / 2) / (2 * M_PI))) * NUM_LANDSHADES);
	shade = _shades[shade % NUM_LANDSHADES];
	if (pole)
	{
		shade = (int)(shade * 0.6 + 4 * (1 - 0.6)); // twilight zone
	}
	return shade;
}


/**
 * Renders the land, taking all the visible world polygons
 * and texturing and shading them accordingly.
 */
void Globe::drawLand()
{
	Sint16 x[4], y[4];

	for (std::list<Polygon*>::iterator i = _cacheLand.begin(); i != _cacheLand.end(); ++i)
	{
		// Convert coordinates
		for (int j = 0; j < (*i)->getPoints(); ++j)
		{
			x[j] = (*i)->getX(j);
			y[j] = (*i)->getY(j);
		}

		// Apply textures according to zoom and shade
		int zoom = (2 - (int)floor(_zoom / 2.0)) * NUM_TEXTURES;
		int shade = getShade(*i);
		drawTexturedPolygon(x, y, (*i)->getPoints(), _texture[shade]->getFrame((*i)->getTexture() + zoom), 0, 0);
	}
}

/**
 * Draws the details of the countries on the globe,
 * based on the current zoom level.
 */
void Globe::drawDetail()
{
	_countries->clear();

	if (!_detail)
		return;

	// Draw the country borders
	if (_zoom >= 1)
	{
		// Lock the surface
		_countries->lock();

		for (std::list<Polyline*>::iterator i = _game->getResourcePack()->getPolylines()->begin(); i != _game->getResourcePack()->getPolylines()->end(); ++i)
		{
			Sint16 x[2], y[2];
			for (int j = 0; j < (*i)->getPoints() - 1; ++j)
			{
				// Don't draw if polyline is facing back
				if (pointBack((*i)->getLongitude(j), (*i)->getLatitude(j)) || pointBack((*i)->getLongitude(j + 1), (*i)->getLatitude(j + 1)))
					continue;

				// Convert coordinates
				polarToCart((*i)->getLongitude(j), (*i)->getLatitude(j), &x[0], &y[0]);
				polarToCart((*i)->getLongitude(j + 1), (*i)->getLatitude(j + 1), &x[1], &y[1]);

				_countries->drawLine(x[0], y[0], x[1], y[1], Palette::blockOffset(10)+2);
			}
		}

		// Unlock the surface
		_countries->unlock();
	}

	// Draw the country names
	if (_zoom >= 2)
	{
		Text *label = new Text(80, 9, 0, 0);
		label->setPalette(getPalette());
		label->setFonts(_game->getResourcePack()->getFont("Big.fnt"), _game->getResourcePack()->getFont("Small.fnt"));
		label->setAlign(ALIGN_CENTER);
		label->setColor(Palette::blockOffset(15)-1);

		Sint16 x, y;
		for (std::vector<Country*>::iterator i = _game->getSavedGame()->getCountries()->begin(); i != _game->getSavedGame()->getCountries()->end(); ++i)
		{
			// Don't draw if label is facing back
			if (pointBack((*i)->getRules()->getLabelLongitude(), (*i)->getRules()->getLabelLatitude()))
				continue;

			// Convert coordinates
			polarToCart((*i)->getRules()->getLabelLongitude(), (*i)->getRules()->getLabelLatitude(), &x, &y);

			label->setX(x - 40);
			label->setY(y);
			label->setText(_game->getLanguage()->getString((*i)->getRules()->getType()));
			label->blit(_countries);
		}

		delete label;
	}

	// Draw the city markers
	if (_zoom >= 3)
	{
		Text *label = new Text(80, 9, 0, 0);
		label->setPalette(getPalette());
		label->setFonts(_game->getResourcePack()->getFont("Big.fnt"), _game->getResourcePack()->getFont("Small.fnt"));
		label->setAlign(ALIGN_CENTER);
		label->setColor(Palette::blockOffset(8)+10);

		Sint16 x, y;
		for (std::vector<Region*>::iterator i = _game->getSavedGame()->getRegions()->begin(); i != _game->getSavedGame()->getRegions()->end(); ++i)
		{
			for (std::vector<City*>::iterator j = (*i)->getRules()->getCities()->begin(); j != (*i)->getRules()->getCities()->end(); ++j)
			{
				// Don't draw if city is facing back
				if (pointBack((*j)->getLongitude(), (*j)->getLatitude()))
					continue;

				// Convert coordinates
				polarToCart((*j)->getLongitude(), (*j)->getLatitude(), &x, &y);

				_mkCity->setX(x - 1);
				_mkCity->setY(y - 1);
				_mkCity->setPalette(getPalette());
				_mkCity->blit(_countries);

				label->setX(x - 40);
				label->setY(y + 2);
				label->setText(_game->getLanguage()->getString((*j)->getName()));
				label->blit(_countries);
			}
		}

		delete label;
	}
}

/**
 * Draws the markers of all the various things going
 * on around the world on top of the globe.
 */
void Globe::drawMarkers()
{
	Sint16 x, y;
	_markers->clear();

	// Draw the base markers
	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
		// Cheap hack to hide bases when they haven't been placed yet
		if (((*i)->getLongitude() != 0.0 || (*i)->getLatitude() != 0.0) &&
			!pointBack((*i)->getLongitude(), (*i)->getLatitude()))
		{
			polarToCart((*i)->getLongitude(), (*i)->getLatitude(), &x, &y);

			_mkXcomBase->setX(x - 1);
			_mkXcomBase->setY(y - 1);
			_mkXcomBase->blit(_markers);
		}
		// Draw the craft markers
		for (std::vector<Craft*>::iterator j = (*i)->getCrafts()->begin(); j != (*i)->getCrafts()->end(); ++j)
		{
			// Hide crafts docked at base
			if ((*j)->getStatus() != "STR_OUT" || pointBack((*j)->getLongitude(), (*j)->getLatitude()))
				continue;

			polarToCart((*j)->getLongitude(), (*j)->getLatitude(), &x, &y);

			_mkCraft->setX(x - 1);
			_mkCraft->setY(y - 1);
			_mkCraft->blit(_markers);
		}
	}

	// Draw the UFO markers
	for (std::vector<Ufo*>::iterator i = _game->getSavedGame()->getUfos()->begin(); i != _game->getSavedGame()->getUfos()->end(); ++i)
	{
		if (pointBack((*i)->getLongitude(), (*i)->getLatitude()))
			continue;

		polarToCart((*i)->getLongitude(), (*i)->getLatitude(), &x, &y);

		if ((*i)->getDetected())
		{
			if ((*i)->isCrashed())
			{
				_mkCrashedUfo->setX(x - 1);
				_mkCrashedUfo->setY(y - 1);
				_mkCrashedUfo->blit(_markers);
			}
			else
			{
				_mkFlyingUfo->setX(x - 1);
				_mkFlyingUfo->setY(y - 1);
				_mkFlyingUfo->blit(_markers);
			}
		}
	}

	// Draw the waypoint markers
	for (std::vector<Waypoint*>::iterator i = _game->getSavedGame()->getWaypoints()->begin(); i != _game->getSavedGame()->getWaypoints()->end(); ++i)
	{
		if (pointBack((*i)->getLongitude(), (*i)->getLatitude()))
			continue;

		polarToCart((*i)->getLongitude(), (*i)->getLatitude(), &x, &y);

		_mkWaypoint->setX(x - 1);
		_mkWaypoint->setY(y - 1);
		_mkWaypoint->blit(_markers);
	}
}

/**
 * Blits the globe onto another surface.
 * @param surface Pointer to another surface.
 */
void Globe::blit(Surface *surface)
{
	Surface::blit(surface);
	_countries->blit(surface);
	_markers->blit(surface);
}

/**
 * Ignores any mouse clicks that are outside the globe.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Globe::mousePress(Action *action, State *state)
{
	double lon, lat;
	cartToPolar((Sint16)floor(action->getAbsoluteXMouse()), (Sint16)floor(action->getAbsoluteYMouse()), &lon, &lat);

	// Check for errors
	if (lat == lat && lon == lon)
		InteractiveSurface::mousePress(action, state);
}

/**
 * Ignores any mouse clicks that are outside the globe.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Globe::mouseRelease(Action *action, State *state)
{
	double lon, lat;
	cartToPolar((Sint16)floor(action->getAbsoluteXMouse()), (Sint16)floor(action->getAbsoluteYMouse()), &lon, &lat);

	// Check for errors
	if (lat == lat && lon == lon)
		InteractiveSurface::mouseRelease(action, state);
}

/**
 * Ignores any mouse clicks that are outside the globe
 * and handles globe rotation and zooming.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Globe::mouseClick(Action *action, State *state)
{
	double lon, lat;
	cartToPolar((Sint16)floor(action->getAbsoluteXMouse()), (Sint16)floor(action->getAbsoluteYMouse()), &lon, &lat);

	// Check for errors
	if (lat == lat && lon == lon)
	{
		InteractiveSurface::mouseClick(action, state);

		// Handle globe control
		if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
		{
			center(lon, lat);
		}
		else if (action->getDetails()->button.button == SDL_BUTTON_WHEELUP)
		{
			zoomIn();
		}
		else if (action->getDetails()->button.button == SDL_BUTTON_WHEELDOWN)
		{
			zoomOut();
		}
	}
}

/**
 * Handles globe keyboard shortcuts.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Globe::keyboardPress(Action *action, State *state)
{
	InteractiveSurface::keyboardPress(action, state);
	if (action->getDetails()->key.keysym.sym == SDLK_TAB)
	{
		toggleDetail();
	}
}

/**
 * Get the polygons texture at a given point
 * @param lon Longitude of the point.
 * @param lat Latitude of the point.
 * @param texture pointer to texture ID returns -1 when polygon not found
 * @param shade pointer to shade
 */
void Globe::getPolygonTextureAndShade(double lon, double lat, int *texture, int *shade)
{
	*texture = -1;
	for (std::list<Polygon*>::iterator i = _game->getResourcePack()->getPolygons()->begin(); i != _game->getResourcePack()->getPolygons()->end(); ++i)
	{
		if (insidePolygon(lon, lat, *i))
		{
			*texture = (*i)->getTexture();
			*shade = getShade(*i);
			return;
		}
	}
}

}
