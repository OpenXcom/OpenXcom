/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#include "../aresame.h"
#include "../Engine/Action.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Timer.h"
#include "../Resource/ResourcePack.h"
#include "Polygon.h"
#include "Polyline.h"
#include "../Engine/FastLineClip.h"
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
#include "../Engine/ShaderMove.h"
#include "../Engine/ShaderRepeat.h"
#include "../Engine/Options.h"
#include "../Savegame/TerrorSite.h"
#include "../Savegame/AlienBase.h"
#include "../Engine/LocalizedText.h"
#include "../Savegame/BaseFacility.h"
#include "../Ruleset/RuleBaseFacility.h"
#include "../Ruleset/RuleCraft.h"
#include "../Ruleset/Ruleset.h"

namespace OpenXcom
{

const double Globe::QUAD_LONGITUDE = 0.05;
const double Globe::QUAD_LATITUDE = 0.2;
const double Globe::ROTATE_LONGITUDE = 0.25;
const double Globe::ROTATE_LATITUDE = 0.15;

namespace
{
	
///helper class for `Globe` for drawing earth globe with shadows
struct GlobeStaticData
{
	///array of shading gradient
	Sint16 shade_gradient[240];
	///size of x & y of noise surface
	const int random_surf_size;
	
	/**
	 * Function returning normal vector of sphere surface
     * @param ox x cord of sphere center
     * @param oy y cord of sphere center
     * @param r radius of sphere
     * @param x cord of point where we getting this vector
     * @param y cord of point where we getting this vector
     * @return normal vector of sphere surface 
     */
	inline Cord circle_norm(double ox, double oy, double r, double x, double y)
	{
		const double limit = r*r;
		const double norm = 1./r;
		Cord ret;
		ret.x = (x-ox);
		ret.y = (y-oy);
		const double temp = (ret.x)*(ret.x) + (ret.y)*(ret.y);
		if(limit > temp)
		{
			ret.x *= norm;
			ret.y *= norm;
			ret.z = sqrt(limit - temp)*norm;
			return ret;
		}
		else
		{
			ret.x = 0.;
			ret.y = 0.;
			ret.z = 0.;
			return ret;
		}
	}
	
	//initialization	
	GlobeStaticData() : random_surf_size(60)
	{
		//filling terminator gradient LUT
		for (int i=0; i<240; ++i)
		{
			int j = i - 120;

			if (j<-66) j=-16;
			else
			if (j<-48) j=-15;
			else
			if (j<-33) j=-14;
			else
			if (j<-22) j=-13;
			else
			if (j<-15) j=-12;
			else
			if (j<-11) j=-11;
			else
			if (j<-9) j=-10;

			if (j>120) j=19;
			else
			if (j>98) j=18;
			else
			if (j>86) j=17;
			else
			if (j>74) j=16;
			else
			if (j>54) j=15;
			else
			if (j>38) j=14;
			else
			if (j>26) j=13;
			else
			if (j>18) j=12;
			else
			if (j>13) j=11;
			else
			if (j>10) j=10;
			else
			if (j>8) j=9;

			shade_gradient[i]= j+16;
		}

	}
};

GlobeStaticData static_data;

struct Ocean
{
	static inline void func(Uint8& dest, const int&, const int&, const int&, const int&)
	{
		dest = Palette::blockOffset(12) + 0;
	}
};

struct CreateShadow
{
	static inline Uint8 getShadowValue(const Uint8& dest, const Cord& earth, const Cord& sun, const Sint16& noise)
	{
		Cord temp = earth;
		//diff
		temp -= sun;
		//norm
		temp.x *= temp.x;
		temp.y *= temp.y;
		temp.z *= temp.z;
		temp.x += temp.z + temp.y;
		//we have norm of distance between 2 vectors, now stored in `x`

		temp.x -= 2;
		temp.x *= 125.;

		if (temp.x < -110)
			temp.x = -31;
		else if (temp.x > 120)
			temp.x = 50;
		else
			temp.x = static_data.shade_gradient[(Sint16)temp.x + 120];

		temp.x -= noise;

		if(temp.x > 0.)
		{
			const Sint16 val = (temp.x> 31)? 31 : (Sint16)temp.x;
			const int d = dest & helper::ColorGroup;
			if(d ==  Palette::blockOffset(12) || d ==  Palette::blockOffset(13))
			{
				//this pixel is ocean
				return Palette::blockOffset(12) + val;
			}
			else
			{
				//this pixel is land
				if (dest==0) return val;
				const int s = val / 3;
				const int e = dest+s;
				if(e > d + helper::ColorShade)
					return d + helper::ColorShade;
				return e;
			}
		}
		else
		{
			const int d = dest & helper::ColorGroup;
			if(d ==  Palette::blockOffset(12) || d ==  Palette::blockOffset(13))
			{
				//this pixel is ocean
				return Palette::blockOffset(12);
			}
			else
			{
				//this pixel is land
				return dest;
			}
		}
	}
	
	static inline void func(Uint8& dest, const Cord& earth, const Cord& sun, const Sint16& noise, const int&)
	{
		if(dest && earth.z)
			dest = getShadowValue(dest, earth, sun, noise);
		else
			dest = 0;
	}
};

}//namespace


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
Globe::Globe(Game *game, int cenX, int cenY, int width, int height, int x, int y):
	InteractiveSurface(width, height, x, y),
	_rotLon(0.0), _rotLat(0.0),
	_cenX(cenX), _cenY(cenY), _game(game),
	_blink(true), _hover(false), _cacheLand()
{
	_texture = new SurfaceSet(*_game->getResourcePack()->getSurfaceSet("TEXTURE.DAT"));

	_countries = new Surface(width, height, x, y);
	_markers = new Surface(width, height, x, y);
	_radars = new Surface(width, height, x, y);
	_clipper = new FastLineClip(x, x+width, y, y+height);

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

	_cenLon = _game->getSavedGame()->getGlobeLongitude();
	_cenLat = _game->getSavedGame()->getGlobeLatitude();
	_zoom = _game->getSavedGame()->getGlobeZoom();

	_radius.push_back(0.45*height);
	_radius.push_back(0.60*height);
	_radius.push_back(0.90*height);
	_radius.push_back(1.40*height);
	_radius.push_back(2.25*height);
	_radius.push_back(3.60*height);
	_earthData.resize(_radius.size());

	//filling normal field for each radius
	for(unsigned int r = 0; r<_radius.size(); ++r)
	{
		_earthData[r].resize(width * height);
		for(int j=0; j<height; ++j)
			for(int i=0; i<width; ++i)
			{
				_earthData[r][width*j + i] = static_data.circle_norm(width/2, height/2, _radius[r], i+.5, j+.5);
			}
	}

	//filling random noise "texture"
	_randomNoiseData.resize(static_data.random_surf_size * static_data.random_surf_size);
	for(unsigned int i=0; i<_randomNoiseData.size(); ++i)
		_randomNoiseData[i] = rand()%4;

	cachePolygons();
}

/**
 * Deletes the contained surfaces.
 */
Globe::~Globe()
{
	delete _texture;

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
	delete _radars;
	delete _clipper;

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

void Globe::polarToCart(double lon, double lat, double *x, double *y) const
{
	// Orthographic projection
	*x = _cenX + _radius[_zoom] * cos(lat) * sin(lon - _cenLon);
	*y = _cenY + _radius[_zoom] * (cos(_cenLat) * sin(lat) - sin(_cenLat) * cos(lat) * cos(lon - _cenLon));
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
	double c = asin(rho / _radius[_zoom]);
	if ( AreSame(rho, 0.0) )
	{
		*lat = _cenLat;
		*lon = _cenLon;

	}
	else
	{
		*lat = asin((y * sin(c) * cos(_cenLat)) / rho + cos(c) * sin(_cenLat));
		*lon = atan2(x * sin(c),(rho * cos(_cenLat) * cos(c) - y * sin(_cenLat) * sin(c))) + _cenLon;
	}

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

	bool odd = false;
	for (int i = 0; i < poly->getPoints(); ++i)
	{
		int j = (i + 1) % poly->getPoints();

		/*double x = lon, y = lat,
			   x_i = poly->getLongitude(i), y_i = poly->getLatitude(i),
			   x_j = poly->getLongitude(j), y_j = poly->getLatitude(j);*/

		double x, y, x_i, x_j, y_i, y_j;
		polarToCart(poly->getLongitude(i), poly->getLatitude(i), &x_i, &y_i);
		polarToCart(poly->getLongitude(j), poly->getLatitude(j), &x_j, &y_j);
		polarToCart(lon, lat, &x, &y);

		if (((y_i < y && y_j >= y) || (y_j < y && y_i >= y)) && (x_i <= x || x_j <= x))
		{
			odd ^= (x_i + (y - y_i) / (y_j - y_i) * (x_j - x_i) < x);
		}
	}
	return odd;
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
		throw Exception(filename + " not found");
	}

	short value[10];

	while (mapFile.read((char*)&value, sizeof(value)))
	{
		Polygon* poly;
		int points;
		
		
		for( int z=0; z < 10 ; z++ )
		{
			char *ptr = (char *)&value[ z ];
			char tmp = ptr[ 0 ];
			ptr[ 0 ] = ptr[ 1 ];
			ptr[ 1 ] = tmp;
		}

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
		throw Exception("Invalid globe map");
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
		_game->getSavedGame()->setGlobeZoom(_zoom);
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
		_game->getSavedGame()->setGlobeZoom(_zoom);
		cachePolygons();
	}
}

/**
 * Zooms the globe out as far as possible.
 */
void Globe::zoomMin()
{
	_zoom = 0;
	_game->getSavedGame()->setGlobeZoom(_zoom);
	cachePolygons();
}

/**
 * Zooms the globe in as close as possible.
 */
void Globe::zoomMax()
{
	_zoom = _radius.size() - 1;
	_game->getSavedGame()->setGlobeZoom(_zoom);
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
	_game->getSavedGame()->setGlobeLongitude(_cenLon);
	_game->getSavedGame()->setGlobeLatitude(_cenLat);
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
	// We're only temporarily changing cenLon/cenLat so the "const" is actually preserved
	Globe* const globe = const_cast<Globe* const>(this); // WARNING: BAD CODING PRACTICE
	double oldLon = _cenLon, oldLat = _cenLat;
	globe->_cenLon = lon;
	globe->_cenLat = lat;
	for (std::list<Polygon*>::iterator i = _game->getResourcePack()->getPolygons()->begin(); i != _game->getResourcePack()->getPolygons()->end() && !inside; ++i)
	{
		inside = insidePolygon(lon, lat, *i);
	}
	globe->_cenLon = oldLon;
	globe->_cenLat = oldLat;
	return inside;
}

/**
 * Switches the amount of detail shown on the globe.
 * With detail on, country and city details are shown when zoomed in.
 */
void Globe::toggleDetail()
{
	_game->getSavedGame()->toggleDetail();
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
	if (pointBack(target->getLongitude(), target->getLatitude()))
		return false;
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
	for (std::vector<TerrorSite*>::iterator i = _game->getSavedGame()->getTerrorSites()->begin(); i != _game->getSavedGame()->getTerrorSites()->end(); ++i)
	{
		if (targetNear((*i), x, y))
		{
			v.push_back(*i);
		}
	}
	for (std::vector<AlienBase*>::iterator i = _game->getSavedGame()->getAlienBases()->begin(); i != _game->getSavedGame()->getAlienBases()->end(); ++i)
 	{
		if (!(*i)->isDiscovered())
		{
			continue;
		}
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
	
	_texture->setPalette(colors, firstcolor, ncolors);
	
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
	_radars->setPalette(colors, firstcolor, ncolors);
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
	_game->getSavedGame()->setGlobeLongitude(_cenLon);
	_game->getSavedGame()->setGlobeLatitude(_cenLat);
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
	drawRadars();
	drawShadow();
	drawMarkers();
	drawDetail();
}


/**
 * Renders the ocean, shading it according to the time of day.
 */
void Globe::drawOcean()
{
	lock();
	drawCircle(_cenX+1, _cenY, _radius[_zoom]+20, Palette::blockOffset(12)+0);
//	ShaderDraw<Ocean>(ShaderSurface(this));
	unlock();
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
		drawTexturedPolygon(x, y, (*i)->getPoints(), _texture->getFrame((*i)->getTexture() + zoom), 0, 0);
	}
}

/**
 * Get position of sun from point on globe
 * @param lon lontidue of position
 * @param lat latitude of position 
 * @return position of sun
 */
Cord Globe::getSunDirection(double lon, double lat) const
{
	const double curTime = _game->getSavedGame()->getTime()->getDaylight();
	const double rot = curTime * 2*M_PI;
	double sun;

	if (Options::getBool("globeSeasons"))
	{
		const int MonthDays1[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
		const int MonthDays2[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};

		int year=_game->getSavedGame()->getTime()->getYear();
		int month=_game->getSavedGame()->getTime()->getMonth()-1;
		int day=_game->getSavedGame()->getTime()->getDay()-1;

		double tm = (double)(( _game->getSavedGame()->getTime()->getHour() * 60
			+ _game->getSavedGame()->getTime()->getMinute() ) * 60
			+ _game->getSavedGame()->getTime()->getSecond() ) / 86400; //day fraction is also taken into account

		double CurDay;
		if (year%4 == 0 && !(year%100 == 0 && year%400 != 0))
			CurDay = (MonthDays2[month] + day + tm )/366 - 0.219; //spring equinox (start of astronomic year)
		else
			CurDay = (MonthDays1[month] + day + tm )/365 - 0.219;
		if (CurDay<0) CurDay += 1.;

		sun = -0.261 * sin(CurDay*2*M_PI);
	}
	else
		sun = 0;

	Cord sun_direction(cos(rot+lon), sin(rot+lon)*-sin(lat), sin(rot+lon)*cos(lat));

	Cord pole(0, cos(lat), sin(lat));

	if (sun>0)
		 sun_direction *= 1. - sun;
	else
		 sun_direction *= 1. + sun;

	pole *= sun;
	sun_direction += pole;
	double norm = sun_direction.norm();
	//norm should be always greater than 0
	norm = 1./norm;
	sun_direction *= norm;
	return sun_direction;
}


void Globe::drawShadow()
{
	ShaderMove<Cord> earth = ShaderMove<Cord>(_earthData[_zoom], getWidth(), getHeight());
	ShaderRepeat<Sint16> noise = ShaderRepeat<Sint16>(_randomNoiseData, static_data.random_surf_size, static_data.random_surf_size);
	
	earth.setMove(_cenX-getWidth()/2, _cenY-getHeight()/2);
	
	lock();
	ShaderDraw<CreateShadow>(ShaderSurface(this), earth, ShaderScalar(getSunDirection(_cenLon, _cenLat)), noise);
	unlock();
		
}


void Globe::XuLine(Surface* surface, Surface* src, double x1, double y1, double x2, double y2, Sint16)
{
	if (_clipper->LineClip(&x1,&y1,&x2,&y2) != 1) return; //empty line
	x1+=0.5;
	y1+=0.5;
	x2+=0.5;
	y2+=0.5;
	double deltax = x2-x1, deltay = y2-y1;
	bool inv;
	Sint16 tcol;
	double len,x0,y0,SX,SY;
	if (abs((int)y2-(int)y1) > abs((int)x2-(int)x1)) 
	{
		len=abs((int)y2-(int)y1);
		inv=false;
	}
	else
	{
		len=abs((int)x2-(int)x1);
		inv=true;
	}

	if (y2<y1) { 
    SY=-1;
  } else if ( AreSame(deltay, 0.0) ) {
    SY=0;
  } else {
    SY=1;
  }

	if (x2<x1) {
    SX=-1;
  } else if ( AreSame(deltax, 0.0) ) {
    SX=0;
  } else {
    SX=1;
  }

	x0=x1;  y0=y1;
	if (inv)
		SY=(deltay/len);
	else
		SX=(deltax/len);

	while(len>0)
	{
		if (x0>0 && y0>0 && x0<surface->getWidth() && y0<surface->getHeight())
		{
			tcol=src->getPixel((int)x0,(int)y0);
			const int d = tcol & helper::ColorGroup;
			if(d ==  Palette::blockOffset(12) || d ==  Palette::blockOffset(13))
			{
				//this pixel is ocean
				tcol = Palette::blockOffset(12) + 12;
			}
			else
			{
				const int e = tcol+4;
				if(e > d + helper::ColorShade)
					tcol = d + helper::ColorShade;
				else tcol = e;
			}
			surface->setPixel((int)x0,(int)y0,tcol);
		}
		x0+=SX;
		y0+=SY;
		len-=1.0;
	}
}


void Globe::drawRadars()
{
	_radars->clear();
	if (!_game->getSavedGame()->getRadarLines())
		return;
/*	Text *label = new Text(80, 9, 0, 0);
	label->setPalette(getPalette());
	label->setFonts(_game->getResourcePack()->getFont("Big.fnt"), _game->getResourcePack()->getFont("Small.fnt"));
	label->setAlign(ALIGN_LEFT);
	label->setColor(Palette::blockOffset(15)-1);
*/
	double x, y;
	double tr, range;
	double lat, lon;
	std::vector<double> ranges;

//	lock();
	_radars->lock();


	if (_hover)
	{
		const std::vector<std::string> &facilities = _game->getRuleset()->getBaseFacilitiesList();
		for (std::vector<std::string>::const_iterator i = facilities.begin(); i != facilities.end(); ++i)
		{
			range=_game->getRuleset()->getBaseFacility(*i)->getRadarRange();
			range = range * (1 / 60.0) * (M_PI / 180);
			drawGlobeCircle(_hoverLat,_hoverLon,range,48);
			if (Options::getBool("globeAllRadarsOnBaseBuild")) ranges.push_back(range);
		}
	}

	// Draw radars around bases
	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
		lat = (*i)->getLatitude();
		lon = (*i)->getLongitude();
		// Cheap hack to hide bases when they haven't been placed yet
		if (( !(AreSame(lon, 0.0) && AreSame(lat, 0.0)) )/* &&
			!pointBack((*i)->getLongitude(), (*i)->getLatitude())*/)
		{
			polarToCart(lon, lat, &x, &y);

			if (_hover && Options::getBool("globeAllRadarsOnBaseBuild"))
			{
				for (size_t j=0; j<ranges.size(); j++) drawGlobeCircle(lat,lon,ranges[j],48);
			}
			else
			{
				range = 0;
				for (std::vector<BaseFacility*>::iterator j = (*i)->getFacilities()->begin(); j != (*i)->getFacilities()->end(); ++j)
				{
					if ((*j)->getBuildTime() == 0)
					{
						tr = (*j)->getRules()->getRadarRange();
						if (tr > range) range = tr;
					}
				}
				range = range * (1 / 60.0) * (M_PI / 180);

				if (range>0) drawGlobeCircle(lat,lon,range,48);
			}
	
		}

		for (std::vector<Craft*>::iterator j = (*i)->getCrafts()->begin(); j != (*i)->getCrafts()->end(); ++j)
		{
			lat=(*j)->getLatitude();
			lon=(*j)->getLongitude();
			if ((*j)->getStatus()!= "STR_OUT")
				continue;
			polarToCart(lon, lat, &x, &y);
			range = (*j)->getRules()->getRadarRange();
			range = range * (1 / 60.0) * (M_PI / 180);

			if (range>0) drawGlobeCircle(lat,lon,range,24);
		}
	}

	_radars->unlock();
//	unlock();
//	delete label;
}

/**
 *	Draw globe range circle
 */
void Globe::drawGlobeCircle(double lat, double lon, double radius, int segments)
{
	double x, y, x2 = 0, y2 = 0;
	double lat1, lon1;
	double seg = M_PI / (static_cast<double>(segments) / 2);
	for (double az = 0; az <= M_PI*2+0.01; az+=seg) //48 circle segments
	{
		//calculating sphere-projected circle
		lat1 = asin(sin(lat) * cos(radius) + cos(lat) * sin(radius) * cos(az));
		lon1 = lon + atan2(sin(az) * sin(radius) * cos(lat), cos(radius) - sin(lat) * sin(lat1));
		polarToCart(lon1, lat1, &x, &y);
		if ( AreSame(az, 0.0) ) //first vertex is for initialization only
		{
			x2=x;
			y2=y;
			continue;
		}
		if (!pointBack(lon1,lat1))
			XuLine(_radars, this, x, y, x2, y2, 249);
		x2=x; y2=y;
	}
}


void Globe::setNewBaseHover(void)
{
	_hover=true;
}
void Globe::unsetNewBaseHover(void)
{
	_hover=false;
}
bool Globe::getNewBaseHover(void)
{
	return _hover;
}
void Globe::setNewBaseHoverPos(double lon, double lat)
{
	_hoverLon=lon;
	_hoverLat=lat;
}
bool Globe::getShowRadar(void)
{
	return _game->getSavedGame()->getRadarLines();
}


void Globe::drawVHLine(double lon1, double lat1, double lon2, double lat2, int colour)
{
	double sx = lon2 - lon1;
	double sy = lat2 - lat1;
	double ln1, lt1, ln2, lt2;
	int seg;
	Sint16 x1, y1, x2, y2;

	if (sx<0) sx += 2*M_PI;

	if (fabs(sx)<0.01)
	{
		seg = abs( sy/(2*M_PI)*48 );
		if (seg == 0) ++seg;
	}
	else
	{
		seg = abs( sx/(2*M_PI)*96 );
		if (seg == 0) ++seg;
	}

	sx /= seg;
	sy /= seg;

	for (int i = 0; i < seg; ++i)
	{
		ln1 = lon1 + sx*i;
		lt1 = lat1 + sy*i;
		ln2 = lon1 + sx*(i+1);
		lt2 = lat1 + sy*(i+1);

		if (!pointBack(ln2, lt2)&&!pointBack(ln1, lt1))
		{
			polarToCart(ln1,lt1,&x1,&y1);
			polarToCart(ln2,lt2,&x2,&y2);
			_countries->drawLine(x1, y1, x2, y2, colour);
		}
	}
}


/**
 * Draws the details of the countries on the globe,
 * based on the current zoom level.
 */
void Globe::drawDetail()
{
	_countries->clear();

	if (!_game->getSavedGame()->getDetail())
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
		Text *label = new Text(100, 9, 0, 0);
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
	
	static int debugType = 0;
	static bool canSwitchDebugType = false;
	if (_game->getSavedGame()->getDebugMode())
	{
		int color;
		canSwitchDebugType = true;
		if (debugType == 0)
		{
			color = 0;
			for (std::vector<Country*>::iterator i = _game->getSavedGame()->getCountries()->begin(); i != _game->getSavedGame()->getCountries()->end(); ++i)
			{
				color += 10;
				for(size_t k = 0; k != (*i)->getRules()->getLatMax().size(); ++k)
				{
					double lon2 = (*i)->getRules()->getLonMax().at(k);
					double lon1 = (*i)->getRules()->getLonMin().at(k);
					double lat2 = (*i)->getRules()->getLatMax().at(k);
					double lat1 = (*i)->getRules()->getLatMin().at(k);

					drawVHLine(lon1, lat1, lon2, lat1, color);
					drawVHLine(lon1, lat2, lon2, lat2, color);
					drawVHLine(lon1, lat1, lon1, lat2, color);
					drawVHLine(lon2, lat1, lon2, lat2, color);
				}
			}
		}
		else if (debugType == 1)
		{
			color = 0;
			for (std::vector<Region*>::iterator i = _game->getSavedGame()->getRegions()->begin(); i != _game->getSavedGame()->getRegions()->end(); ++i)
			{
				color += 10;
				for(size_t k = 0; k != (*i)->getRules()->getLatMax().size(); ++k)
				{
					double lon2 = (*i)->getRules()->getLonMax().at(k);
					double lon1 = (*i)->getRules()->getLonMin().at(k);
					double lat2 = (*i)->getRules()->getLatMax().at(k);
					double lat1 = (*i)->getRules()->getLatMin().at(k);

					drawVHLine(lon1, lat1, lon2, lat1, color);
					drawVHLine(lon1, lat2, lon2, lat2, color);
					drawVHLine(lon1, lat1, lon1, lat2, color);
					drawVHLine(lon2, lat1, lon2, lat2, color);
				}
			}
		}
	}
	else
	{
		if (canSwitchDebugType)
		{
			++debugType;
			if (debugType > 1) debugType = 0;
			canSwitchDebugType = false;
		}
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

	// Draw the terror site markers
	for (std::vector<TerrorSite*>::iterator i = _game->getSavedGame()->getTerrorSites()->begin(); i != _game->getSavedGame()->getTerrorSites()->end(); ++i)
	{
		if (pointBack((*i)->getLongitude(), (*i)->getLatitude()))
			continue;

		polarToCart((*i)->getLongitude(), (*i)->getLatitude(), &x, &y);

		_mkAlienSite->setX(x - 1);
		_mkAlienSite->setY(y - 1);
		_mkAlienSite->blit(_markers);
	}

	// Draw the Alien Base markers
	for (std::vector<AlienBase*>::iterator i = _game->getSavedGame()->getAlienBases()->begin(); i != _game->getSavedGame()->getAlienBases()->end(); ++i)
	{
		if (pointBack((*i)->getLongitude(), (*i)->getLatitude()))
			continue;

		polarToCart((*i)->getLongitude(), (*i)->getLatitude(), &x, &y);

		if ((*i)->isDiscovered())
		{
			_mkAlienBase->setX(x - 1);
			_mkAlienBase->setY(y - 1);
			_mkAlienBase->blit(_markers);
		}
	}

	// Draw the UFO markers
	for (std::vector<Ufo*>::iterator i = _game->getSavedGame()->getUfos()->begin(); i != _game->getSavedGame()->getUfos()->end(); ++i)
	{
		if (pointBack((*i)->getLongitude(), (*i)->getLatitude()))
			continue;
		Surface *marker = 0;
		switch ((*i)->getStatus())
		{
		case Ufo::DESTROYED:
			continue;
		case Ufo::FLYING:
			if (!(*i)->getDetected()) continue;
			marker = _mkFlyingUfo;
			break;
		case Ufo::LANDED:
			if (!(*i)->getDetected()) continue;
			marker = _mkLandedUfo;
			break;
		case Ufo::CRASHED:
			marker = _mkCrashedUfo;
			break;
		}
		polarToCart((*i)->getLongitude(), (*i)->getLatitude(), &x, &y);
		marker->setX(x - 1);
		marker->setY(y - 1);
		marker->blit(_markers);
	}

	// Draw the craft markers
	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
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
}

/**
 * Blits the globe onto another surface.
 * @param surface Pointer to another surface.
 */
void Globe::blit(Surface *surface)
{
	Surface::blit(surface);
	_radars->blit(surface);
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
	if (action->getDetails()->key.keysym.sym == Options::getInt("keyGeoToggleDetail"))
	{
		toggleDetail();
	}
	if (action->getDetails()->key.keysym.sym == Options::getInt("keyGeoToggleRadar"))
	{
		toggleRadarLines();
	}
}

/**
 * Get the polygons texture at a given point
 * @param lon Longitude of the point.
 * @param lat Latitude of the point.
 * @param texture pointer to texture ID returns -1 when polygon not found
 * @param shade pointer to shade
 */
void Globe::getPolygonTextureAndShade(double lon, double lat, int *texture, int *shade) const
{
	///this is shade conversion from 0..31 levels of geoscape to battlescape levels 0..15
	int worldshades[32] = {  0, 0, 0, 0, 1, 1, 2, 2,
							 3, 3, 4, 4, 5, 5, 6, 6,
							 7, 7, 8, 8, 9, 9,10,11,
							11,12,12,13,13,14,15,15};

	*texture = -1;
	*shade = worldshades[ CreateShadow::getShadowValue(0, Cord(0.,0.,1.), getSunDirection(lon, lat), 0) ];

	// We're only temporarily changing cenLon/cenLat so the "const" is actually preserved
	Globe* const globe = const_cast<Globe* const>(this); // WARNING: BAD CODING PRACTICE
	double oldLon = _cenLon, oldLat = _cenLat;
	globe->_cenLon = lon;
	globe->_cenLat = lat;
	for (std::list<Polygon*>::iterator i = _game->getResourcePack()->getPolygons()->begin(); i != _game->getResourcePack()->getPolygons()->end(); ++i)
	{
		if (insidePolygon(lon, lat, *i))
		{
			*texture = (*i)->getTexture();
			break;
		}
	}
	globe->_cenLon = oldLon;
	globe->_cenLat = oldLat;
}

/**
 * Checks if the globe is zoomed in to it's maximum.
 * @return Returns true if globe is at max zoom, otherwise returns false.
 */
bool Globe::isZoomedInToMax() const
{
	if(_zoom == _radius.size() - 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * Checks if the globe is zoomed out to it's maximum.
 * @return Returns true if globe is at max zoom, otherwise returns false.
 */
bool Globe::isZoomedOutToMax() const
{
	if(_zoom == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * Get the localized text for dictionary key @a id.
 * This function forwards the call to Language::getString(const std::string &).
 * @param id The dictionary key to search for.
 * @return A reference to the localized text.
 */
const LocalizedText &Globe::tr(const std::string &id) const
{
	return _game->getLanguage()->getString(id);
}

/**
 * Get a modifiable copy of the localized text for dictionary key @a id.
 * This function forwards the call to Language::getString(const std::string &, unsigned).
 * @param id The dictionary key to search for.
 * @param n The number to use for the proper version.
 * @return A copy of the localized text.
 */
LocalizedText Globe::tr(const std::string &id, unsigned n) const
{
	return _game->getLanguage()->getString(id, n);
}

void Globe::toggleRadarLines()
{
	_game->getSavedGame()->toggleRadarLines();
	drawRadars();
}

}//namespace OpenXcom
