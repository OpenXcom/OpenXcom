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
#include "../Engine/ShaderMove.h"
#include "../Engine/ShaderRepeat.h"
#include "../Engine/Options.h"
#include "../Savegame/TerrorSite.h"

namespace OpenXcom
{

const double Globe::QUAD_LONGITUDE = 0.05;
const double Globe::QUAD_LATITUDE = 0.2;
const double Globe::ROTATE_LONGITUDE = 0.25;
const double Globe::ROTATE_LATITUDE = 0.15;

///helper class for `Globe` for drawing earth globe with shadows
class GlobeStaticData
{
	///normal of each pixel in earth globe per zoom level
	std::vector<std::vector<Cord> > earth_data;
	///data sample used for noise in shading
	std::vector<Sint16> random_noise_data;
	
	///warper araund `earth_data` for `ShaderDraw` function
	std::vector<ShaderMove<Cord>* > earth;
	///warper araund `random_noise_data` for `ShaderDraw` function
	ShaderRepeat<Sint16>* random_noise;
	///list of dimension of earth on screen per zoom level
	std::vector<double> radius;

public: 
	///dimension of earth graphic
	const std::pair<int,int> earth_size;

	///array of shading gradient
	Sint16 shade_gradient[240];

	///earth inclination flag
	bool is_seasons;

	
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
	GlobeStaticData():
			earth_size(std::make_pair(260, 200))
	{
		
		radius.push_back(90);
		radius.push_back(120);
		radius.push_back(180);
		radius.push_back(280);
		radius.push_back(450);
		radius.push_back(720);
		earth_data.resize(radius.size());

		//filling normal field for each radius
		for(unsigned int r = 0; r<radius.size(); ++r)
		{
			earth_data[r].resize(earth_size.first * earth_size.second);
			for(int j=0; j<earth_size.second; ++j)
				for(int i=0; i<earth_size.first; ++i)
				{
					earth_data[r][earth_size.first*j + i] = circle_norm(earth_size.first/2, earth_size.second/2, radius[r], i+.5, j+.5);
				}
			earth.push_back(new ShaderMove<Cord>(earth_data[r], earth_size.first, earth_size.second));
			earth[r]->setMove(-earth_size.first/2, -earth_size.second/2);
		}

		//filling random noise "texture"
		const int random_surf_size = 60;
		random_noise_data.resize(random_surf_size * random_surf_size);
		for(unsigned int i=0; i< random_noise_data.size(); ++i)
			random_noise_data[i] = rand()%4;
		random_noise = new ShaderRepeat<Sint16>(random_noise_data, random_surf_size, random_surf_size );

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
	~GlobeStaticData()
	{
		for(unsigned int i=0; i< earth.size(); ++i)
			delete earth[i];
		delete random_noise;
	}
	
	inline const ShaderMove<Cord >& getEarthShape(size_t zoom)
	{
		return *earth[zoom];
	}
	inline const ShaderRepeat<Sint16>& getNoise()
	{
		return *random_noise;
	}
	inline double getRadius(size_t zoom)
	{
		return radius[zoom];
	}
	inline size_t getRadiusNum()
	{
		return radius.size();
	}
	inline void initSeasons()
	{
		is_seasons = Options::getBool("globeSeasons");
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

		if (temp.x < -110) temp.x = -31;
		else
		if (temp.x > 120) temp.x = 50;
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
Globe::Globe(Game *game, int cenX, int cenY, int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _cenLon(0.0), _cenLat(0.0), _rotLon(0.0), _rotLat(0.0), _cenX(cenX), _cenY(cenY), _zoom(0), _game(game), _blink(true), _detail(true), _cacheLand()
{
	_texture = new SurfaceSet(*_game->getResourcePack()->getSurfaceSet("TEXTURE.DAT"));

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
	
	static_data.initSeasons();
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
	*x = _cenX + (Sint16)floor(static_data.getRadius(_zoom) * cos(lat) * sin(lon - _cenLon));
	*y = _cenY + (Sint16)floor(static_data.getRadius(_zoom) * (cos(_cenLat) * sin(lat) - sin(_cenLat) * cos(lat) * cos(lon - _cenLon)));
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
	double c = asin(rho / (static_data.getRadius(_zoom)));

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
	if (_zoom < static_data.getRadiusNum() - 1)
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
	_zoom = static_data.getRadiusNum() - 1;
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
	for (std::vector<TerrorSite*>::iterator i = _game->getSavedGame()->getTerrorSites()->begin(); i != _game->getSavedGame()->getTerrorSites()->end(); ++i)
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
	drawCircle(_cenX+1, _cenY, static_data.getRadius(_zoom)+20, Palette::blockOffset(12)+0);
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

	if (static_data.is_seasons)
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
	
	ShaderMove<Cord> earth(static_data.getEarthShape(_zoom));
	earth.addMove(_cenX, _cenY);
	
	lock();
	ShaderDraw<CreateShadow>(ShaderSurface(this), earth, ShaderScalar(getSunDirection(_cenLon, _cenLat)), static_data.getNoise());
	unlock();
		
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

	// Draw the radar ranges
	/*
	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
		// Cheap hack to hide bases when they haven't been placed yet
		if (((*i)->getLongitude() != 0.0 || (*i)->getLatitude() != 0.0) &&
			!pointBack((*i)->getLongitude(), (*i)->getLatitude()))
		{
			int n = 16;
			for (int j = 1; j <= n; ++j)
			{
				double r = 0.25;
				double lon1 = (*i)->getLongitude() + r*cos(2*M_PI*j/n);
				double lat1 = (*i)->getLatitude() + r*sin(2*M_PI*j/n);
				double lon2 = (*i)->getLongitude() + r*cos(2*M_PI*(j+1)/n);
				double lat2 = (*i)->getLatitude() + r*sin(2*M_PI*(j+1)/n);
				if (!pointBack(lon2, lat2))
				{
					Sint16 x1, x2, y1, y2;
					polarToCart(lon1, lat1, &x1, &y1);
					polarToCart(lon2, lat2, &x2, &y2);
					_countries->drawLine(x1, y1, x2, y2, 10);
				}
			}
		}
	}
	*/
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
	///this is shade conversion from 0..31 levels of geoscape to battlescape levels 0..15
	int worldshades[32] = {  0, 0, 0, 0, 1, 1, 2, 2,
							 3, 3, 4, 4, 5, 5, 6, 6,
							 7, 7, 8, 8, 9, 9,10,11,
							11,12,12,13,13,14,15,15};

	*texture = -1;
	*shade = worldshades[ CreateShadow::getShadowValue(0, Cord(0.,0.,1.), getSunDirection(lon, lat), 0) ];
	for (std::list<Polygon*>::iterator i = _game->getResourcePack()->getPolygons()->begin(); i != _game->getResourcePack()->getPolygons()->end(); ++i)
	{
		if (insidePolygon(lon, lat, *i))
		{
			*texture = (*i)->getTexture();
			return;
		}
	}
}

}
