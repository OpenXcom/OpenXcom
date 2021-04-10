/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include <algorithm>
#include "../fmath.h"
#include "../Engine/Action.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Timer.h"
#include "../Mod/Mod.h"
#include "../Mod/Polygon.h"
#include "../Mod/Polyline.h"
#include "../Engine/FastLineClip.h"
#include "../Engine/Game.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/GameTime.h"
#include "../Savegame/Base.h"
#include "../Savegame/Country.h"
#include "../Mod/RuleCountry.h"
#include "../Interface/Text.h"
#include "../Engine/LocalizedText.h"
#include "../Mod/RuleRegion.h"
#include "../Savegame/Region.h"
#include "../Mod/City.h"
#include "../Savegame/Target.h"
#include "../Savegame/Ufo.h"
#include "../Savegame/Craft.h"
#include "../Savegame/Waypoint.h"
#include "../Engine/ShaderMove.h"
#include "../Engine/ShaderRepeat.h"
#include "../Engine/Options.h"
#include "../Savegame/MissionSite.h"
#include "../Savegame/AlienBase.h"
#include "../Engine/Language.h"
#include "../Savegame/BaseFacility.h"
#include "../Mod/RuleBaseFacility.h"
#include "../Mod/RuleCraft.h"
#include "../Mod/RuleGlobe.h"
#include "../Interface/Cursor.h"
#include "../Engine/Screen.h"

namespace OpenXcom
{

const double Globe::ROTATE_LONGITUDE = 0.10;
const double Globe::ROTATE_LATITUDE = 0.06;

Uint8 Globe::OCEAN_COLOR;
bool Globe::OCEAN_SHADING;
Uint8 Globe::COUNTRY_LABEL_COLOR;
Uint8 Globe::LINE_COLOR;
Uint8 Globe::CITY_LABEL_COLOR;
Uint8 Globe::BASE_LABEL_COLOR;

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
	static inline Cord circle_norm(double ox, double oy, double r, double x, double y)
	{
		const double limit = r*r;
		const double norm = 1./r;
		Cord ret;
		ret.x = (x-ox);
		ret.y = (y-oy);
		const double temp = (ret.x)*(ret.x) + (ret.y)*(ret.y);
		if (limit > temp)
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
		dest = Globe::OCEAN_COLOR;
	}
};

struct CreateShadow
{
	static inline Uint8 getShadowValue(const Cord& earth, const Cord& sun, const Sint16& noise)
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

		return Clamp(temp.x, 0., 31.);
	}

	static inline Uint8 getOceanShadow(const Uint8& shadow)
	{
		return Globe::OCEAN_COLOR + shadow;
	}

	static inline Uint8 getLandShadow(const Uint8& dest, const Uint8& shadow)
	{
		if (shadow == 0) return dest;
		const int s = shadow / 3;
		const int e = dest + s;
		const int d = dest & helper::ColorGroup;
		if (e > d + helper::ColorShade)
			return d + helper::ColorShade;
		return e;
	}

	static inline bool isOcean(const Uint8& dest)
	{
		return Globe::OCEAN_SHADING && dest >= Globe::OCEAN_COLOR && dest < Globe::OCEAN_COLOR + 32;
	}

	static inline void func(Uint8& dest, const Cord& earth, const Cord& sun, const Sint16& noise, const int&)
	{
		if (dest && earth.z)
		{
			const Uint8 shadow = getShadowValue(earth, sun, noise);
			//this pixel is ocean
			if (isOcean(dest))
			{
				dest = getOceanShadow(shadow);
			}
			//this pixel is land
			else
			{
				dest = getLandShadow(dest, shadow);
			}
		}
		else
		{
			dest = 0;
		}
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
Globe::Globe(Game* game, int cenX, int cenY, int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _cenX(cenX), _cenY(cenY), _rotLon(0.0), _rotLat(0.0), _hoverLon(0.0), _hoverLat(0.0), _craftLon(0.0), _craftLat(0.0), _craftRange(0.0), _game(game), _hover(false), _craft(false), _blink(-1),
																					_isMouseScrolling(false), _isMouseScrolled(false), _xBeforeMouseScrolling(0), _yBeforeMouseScrolling(0), _lonBeforeMouseScrolling(0.0), _latBeforeMouseScrolling(0.0), _mouseScrollingStartTime(0), _totalMouseMoveX(0), _totalMouseMoveY(0), _mouseMovedOverThreshold(false)
{
	_rules = game->getMod()->getGlobe();
	_texture = new SurfaceSet(*_game->getMod()->getSurfaceSet("TEXTURE.DAT"));
	_markerSet = new SurfaceSet(*_game->getMod()->getSurfaceSet("GlobeMarkers"));

	_countries = new Surface(width, height, x, y);
	_markers = new Surface(width, height, x, y);
	_radars = new Surface(width, height, x, y);
	_clipper = new FastLineClip(x, x+width, y, y+height);

	// Animation timers
	_blinkTimer = new Timer(100);
	_blinkTimer->onTimer((SurfaceHandler)&Globe::blink);
	_blinkTimer->start();
	_rotTimer = new Timer(10);
	_rotTimer->onTimer((SurfaceHandler)&Globe::rotate);

	_cenLon = _game->getSavedGame()->getGlobeLongitude();
	_cenLat = _game->getSavedGame()->getGlobeLatitude();
	_zoom = _game->getSavedGame()->getGlobeZoom();
	_zoomOld = _zoom;

	setupRadii(width, height);
	setZoom(_zoom);

	//filling random noise "texture"
	_randomNoiseData.resize(static_data.random_surf_size * static_data.random_surf_size);
	for (size_t i=0; i<_randomNoiseData.size(); ++i)
		_randomNoiseData[i] = rand()%4;

	cachePolygons();
}

/**
 * Deletes the contained surfaces.
 */
Globe::~Globe()
{
	delete _blinkTimer;
	delete _rotTimer;
	delete _countries;
	delete _markers;
	delete _texture;
	delete _markerSet;
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
	*x = _cenX + (Sint16)floor(_radius * cos(lat) * sin(lon - _cenLon));
	*y = _cenY + (Sint16)floor(_radius * (cos(_cenLat) * sin(lat) - sin(_cenLat) * cos(lat) * cos(lon - _cenLon)));
}

void Globe::polarToCart(double lon, double lat, double *x, double *y) const
{
	// Orthographic projection
	*x = _cenX + _radius * cos(lat) * sin(lon - _cenLon);
	*y = _cenY + _radius * (cos(_cenLat) * sin(lat) - sin(_cenLat) * cos(lat) * cos(lon - _cenLon));
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
	double c = asin(rho / _radius);
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

	return c < 0.0;
}

Polygon* Globe::getPolygonFromLonLat(double lon, double lat) const
{
	const double zDiscard=0.75f;
	double coslat = cos(lat);
	double sinlat = sin(lat);

	for (std::list<Polygon*>::iterator i = _rules->getPolygons()->begin(); i != _rules->getPolygons()->end(); ++i)
	{
		double x, y, z, x2, y2;
		double clat, clon;
		z = 0;
		for (int j = 0; j < (*i)->getPoints(); ++j)
		{
			z = coslat * cos((*i)->getLatitude(j)) * cos((*i)->getLongitude(j) - lon) + sinlat * sin((*i)->getLatitude(j));
			if (z<zDiscard) break; //discarded
		}
		if (z<zDiscard) continue; //discarded

		bool odd = false;

		clat = (*i)->getLatitude(0); //initial point
		clon = (*i)->getLongitude(0);
		x = cos(clat) * sin(clon - lon);
		y = coslat * sin(clat) - sinlat * cos(clat) * cos(clon - lon);

		for (int j = 0; j < (*i)->getPoints(); ++j)
		{
			int k = (j + 1) % (*i)->getPoints(); //index of next point in poly
			clat = (*i)->getLatitude(k);
			clon = (*i)->getLongitude(k);

			x2 = cos(clat) * sin(clon - lon);
			y2 = coslat * sin(clat) - sinlat * cos(clat) * cos(clon - lon);
			if ( ((y>0)!=(y2>0)) && (0 < (x2-x)*(0-y)/(y2-y)+x) )
				odd = !odd;
			x = x2;
			y = y2;

		}
		if (odd) return *i;
	}
	return NULL;
}

/**
 * Sets a leftwards rotation speed and starts the timer.
 */
void Globe::rotateLeft()
{
	_rotLon = -ROTATE_LONGITUDE;
	if (!_rotTimer->isRunning()) _rotTimer->start();
}

/**
 * Sets a rightwards rotation speed and starts the timer.
 */
void Globe::rotateRight()
{
	_rotLon = ROTATE_LONGITUDE;
	if (!_rotTimer->isRunning()) _rotTimer->start();
}

/**
 * Sets a upwards rotation speed and starts the timer.
 */
void Globe::rotateUp()
{
	_rotLat = -ROTATE_LATITUDE;
	if (!_rotTimer->isRunning()) _rotTimer->start();
}

/**
 * Sets a downwards rotation speed and starts the timer.
 */
void Globe::rotateDown()
{
	_rotLat = ROTATE_LATITUDE;
	if (!_rotTimer->isRunning()) _rotTimer->start();
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
 * Resets longitude rotation speed and timer.
 */
void Globe::rotateStopLon()
{
	_rotLon = 0.0;
	if (AreSame(_rotLat, 0.0))
	{
		_rotTimer->stop();
	}
}

/**
 * Resets latitude rotation speed and timer.
 */
void Globe::rotateStopLat()
{
	_rotLat = 0.0;
	if (AreSame(_rotLon, 0.0))
	{
		_rotTimer->stop();
	}
}

/**
 * Changes the current globe zoom factor.
 * @param zoom New zoom.
 */
void Globe::setZoom(size_t zoom)
{
	_zoom = Clamp(zoom, (size_t)0u, _zoomRadius.size() - 1);
	_zoomTexture = (2 - (int)floor(_zoom / 2.0)) * (_texture->getTotalFrames() / 3);
	_radius = _zoomRadius[_zoom];
	_game->getSavedGame()->setGlobeZoom(_zoom);
	if (_isMouseScrolling)
	{
		_lonBeforeMouseScrolling = _cenLon;
		_latBeforeMouseScrolling = _cenLat;
		_totalMouseMoveX = 0; _totalMouseMoveY = 0;
	}
	invalidate();
}

/**
 * Increases the zoom level on the globe.
 */
void Globe::zoomIn()
{
	if (_zoom < _zoomRadius.size() - 1)
	{
		setZoom(_zoom + 1);
	}
}

/**
 * Decreases the zoom level on the globe.
 */
void Globe::zoomOut()
{
	if (_zoom > 0)
	{
		setZoom(_zoom - 1);
	}
}

/**
 * Zooms the globe out as far as possible.
 */
void Globe::zoomMin()
{
	if (_zoom > 0)
	{
		setZoom(0);
	}
}

/**
 * Zooms the globe in as close as possible.
 */
void Globe::zoomMax()
{
	if (_zoom < _zoomRadius.size() - 1)
	{
		setZoom(_zoomRadius.size() - 1);
	}
}

/**
 * Stores the zoom used before a dogfight.
 */
void Globe::saveZoomDogfight()
{
	_zoomOld = _zoom;
}

/**
 * Zooms the globe smoothly into dogfight level.
 * @return Is the globe already zoomed in?
 */
bool Globe::zoomDogfightIn()
{
	if (_zoom < DOGFIGHT_ZOOM)
	{
		double radiusNow = _radius;
		if (radiusNow + _radiusStep >= _zoomRadius[DOGFIGHT_ZOOM])
		{
			setZoom(DOGFIGHT_ZOOM);
		}
		else
		{
			if (radiusNow + _radiusStep >= _zoomRadius[_zoom + 1])
				_zoom++;
			setZoom(_zoom);
			_radius = radiusNow + _radiusStep;
		}
		return false;
	}
	return true;
}

/**
 * Zooms the globe smoothly out of dogfight level.
 * @return Is the globe already zoomed out?
 */
bool Globe::zoomDogfightOut()
{
	if (_zoom > _zoomOld)
	{
		double radiusNow = _radius;
		if (radiusNow - _radiusStep <= _zoomRadius[_zoomOld])
		{
			setZoom(_zoomOld);
		}
		else
		{
			if (radiusNow - _radiusStep <= _zoomRadius[_zoom - 1])
				_zoom--;
			setZoom(_zoom);
			_radius = radiusNow - _radiusStep;
		}
		return false;
	}
	return true;
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
	invalidate();
}

/**
 * Checks if a polar point is inside the globe's landmass.
 * @param lon Longitude of the point.
 * @param lat Latitude of the point.
 * @return True if it's inside, False if it's outside.
 */
bool Globe::insideLand(double lon, double lat) const
{
	return (getPolygonFromLonLat(lon,lat))!=NULL;
}

/**
 * Switches the amount of detail shown on the globe.
 * With detail on, country and city details are shown when zoomed in.
 */
void Globe::toggleDetail()
{
	Options::globeDetail = !Options::globeDetail;
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
	for (std::vector<MissionSite*>::iterator i = _game->getSavedGame()->getMissionSites()->begin(); i != _game->getSavedGame()->getMissionSites()->end(); ++i)
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
	cache(_rules->getPolygons(), &_cacheLand);
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
		double closest = 0.0;
		double z;
		double furthest = 0.0;
		for (int j = 0; j < (*i)->getPoints(); ++j)
		{
			z = cos(_cenLat) * cos((*i)->getLatitude(j)) * cos((*i)->getLongitude(j) - _cenLon) + sin(_cenLat) * sin((*i)->getLatitude(j));
			if (z > closest)
				closest = z;
			else if (z < furthest)
				furthest = z;
		}
		if (-furthest > closest)
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
	_markerSet->setPalette(colors, firstcolor, ncolors);

	_countries->setPalette(colors, firstcolor, ncolors);
	_markers->setPalette(colors, firstcolor, ncolors);
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
	_blink = -_blink;

	for (std::map<int, Surface*>::iterator i = _markerSet->getFrames()->begin(); i != _markerSet->getFrames()->end(); ++i)
	{
		if (i->first != CITY_MARKER)
			i->second->offset(_blink);
	}

	drawMarkers();
}

/**
 * Rotates the globe by a set amount. Necessary
 * since the globe keeps rotating while a button
 * is pressed down.
 */
void Globe::rotate()
{
	_cenLon += _rotLon * ((110 - Options::geoScrollSpeed) / 100.0) / (_zoom+1);
	_cenLat += _rotLat * ((110 - Options::geoScrollSpeed) / 100.0) / (_zoom+1);
	_game->getSavedGame()->setGlobeLongitude(_cenLon);
	_game->getSavedGame()->setGlobeLatitude(_cenLat);
	invalidate();
}

/**
 * Draws the whole globe, part by part.
 */
void Globe::draw()
{
	if (_redraw)
	{
		cachePolygons();
	}
	Surface::draw();
	drawOcean();
	drawLand();
	drawRadars();
	drawFlights();
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
	drawCircle(_cenX+1, _cenY, _radius+20, OCEAN_COLOR);
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
		drawTexturedPolygon(x, y, (*i)->getPoints(), _texture->getFrame((*i)->getTexture() + _zoomTexture), 0, 0);
	}
}

/**
 * Get position of sun from point on globe
 * @param lon longitude of position
 * @param lat latitude of position
 * @return position of sun
 */
Cord Globe::getSunDirection(double lon, double lat) const
{
	const double curTime = _game->getSavedGame()->getTime()->getDaylight();
	const double rot = curTime * 2*M_PI;
	double sun;

	if (Options::globeSeasons)
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


void Globe::XuLine(Surface* surface, Surface* src, double x1, double y1, double x2, double y2, int shade)
{
	if (_clipper->LineClip(&x1,&y1,&x2,&y2) != 1) return; //empty line

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

	while (len>0)
	{
		tcol=src->getPixel((int)x0,(int)y0);
		if (tcol)
		{
			if (CreateShadow::isOcean(tcol))
			{
				tcol = CreateShadow::getOceanShadow(shade + 8);
			}
			else
			{
				tcol = CreateShadow::getLandShadow(tcol, shade * 3);
			}
			surface->setPixel((int)x0,(int)y0,tcol);
		}
		x0+=SX;
		y0+=SY;
		len-=1.0;
	}
}

/**
 * Draws the radar ranges of player bases on the globe.
 */
void Globe::drawRadars()
{
	_radars->clear();

	// Draw craft circle instead of radar circles to avoid confusion
	if (_craft)
	{
		_radars->lock();

		if (_craftRange < M_PI)
		{
			drawGlobeCircle(_craftLat, _craftLon, _craftRange, 64);
			drawGlobeCircle(_craftLat, _craftLon, _craftRange - 0.025, 64, 2);
		}

		_radars->unlock();
		return;
	}

	if (!Options::globeRadarLines)
		return;

	double tr, range;
	double lat, lon;
	std::vector<double> ranges;

	_radars->lock();

	if (_hover)
	{
		const std::vector<std::string> &facilities = _game->getMod()->getBaseFacilitiesList();
		for (std::vector<std::string>::const_iterator i = facilities.begin(); i != facilities.end(); ++i)
		{
			range = Nautical(_game->getMod()->getBaseFacility(*i)->getRadarRange());
			drawGlobeCircle(_hoverLat,_hoverLon,range,48);
			if (Options::globeAllRadarsOnBaseBuild) ranges.push_back(range);
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
			if (_hover && Options::globeAllRadarsOnBaseBuild)
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
				range = Nautical(range);

				if (range>0) drawGlobeCircle(lat,lon,range,48);
			}

		}

		for (std::vector<Craft*>::iterator j = (*i)->getCrafts()->begin(); j != (*i)->getCrafts()->end(); ++j)
		{
			if ((*j)->getStatus() != "STR_OUT")
				continue;
			lat=(*j)->getLatitude();
			lon=(*j)->getLongitude();
			range = Nautical((*j)->getRules()->getRadarRange());

			if (range>0) drawGlobeCircle(lat,lon,range,24);
		}
	}

	_radars->unlock();
}

/**
 *	Draw globe range circle
 */
void Globe::drawGlobeCircle(double lat, double lon, double radius, int segments, int frac)
{
	double x, y, x2 = 0, y2 = 0;
	double lat1, lon1;
	double seg = M_PI / (static_cast<double>(segments) / 2);
	int i = 0;
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
		if (!pointBack(lon1,lat1) && i % frac == 0)
			XuLine(_radars, this, x, y, x2, y2, 6);
		x2=x; y2=y;
		i++;
	}
}

void Globe::setNewBaseHover(bool hover)
{
	_hover=hover;
}

void Globe::setNewBaseHoverPos(double lon, double lat)
{
	_hoverLon=lon;
	_hoverLat=lat;
}

void Globe::drawVHLine(Surface *surface, double lon1, double lat1, double lon2, double lat2, Uint8 color)
{
	double sx = lon2 - lon1;
	double sy = lat2 - lat1;
	double ln1, lt1, ln2, lt2;
	int seg;
	Sint16 x1, y1, x2, y2;

	if (sx<0) sx += 2*M_PI;

	if (fabs(sx)<0.01)
	{
		seg = std::abs(sy/(2*M_PI)*48);
		if (seg == 0) ++seg;
	}
	else
	{
		seg = std::abs(sx/(2*M_PI)*96);
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
			surface->drawLine(x1, y1, x2, y2, color);
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

	if (!Options::globeDetail)
		return;

	// Draw the country borders
	if (_zoom >= 1)
	{
		// Lock the surface
		_countries->lock();

		for (std::list<Polyline*>::iterator i = _rules->getPolylines()->begin(); i != _rules->getPolylines()->end(); ++i)
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

				_countries->drawLine(x[0], y[0], x[1], y[1], LINE_COLOR);
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
		label->initText(_game->getMod()->getFont("FONT_BIG"), _game->getMod()->getFont("FONT_SMALL"), _game->getLanguage());
		label->setAlign(ALIGN_CENTER);
		label->setColor(COUNTRY_LABEL_COLOR);

		Sint16 x, y;
		for (std::vector<Country*>::iterator i = _game->getSavedGame()->getCountries()->begin(); i != _game->getSavedGame()->getCountries()->end(); ++i)
		{
			// Don't draw if label is facing back
			if (pointBack((*i)->getRules()->getLabelLongitude(), (*i)->getRules()->getLabelLatitude()))
				continue;

			// Convert coordinates
			polarToCart((*i)->getRules()->getLabelLongitude(), (*i)->getRules()->getLabelLatitude(), &x, &y);

			label->setX(x - 50);
			label->setY(y);
			label->setText(_game->getLanguage()->getString((*i)->getRules()->getType()));
			label->blit(_countries);
		}

		delete label;
	}

	// Draw the city and base markers
	if (_zoom >= 3)
	{
		Text *label = new Text(100, 9, 0, 0);
		label->setPalette(getPalette());
		label->initText(_game->getMod()->getFont("FONT_BIG"), _game->getMod()->getFont("FONT_SMALL"), _game->getLanguage());
		label->setAlign(ALIGN_CENTER);
		label->setColor(CITY_LABEL_COLOR);

		Sint16 x, y;
		for (std::vector<Region*>::iterator i = _game->getSavedGame()->getRegions()->begin(); i != _game->getSavedGame()->getRegions()->end(); ++i)
		{
			for (std::vector<City*>::iterator j = (*i)->getRules()->getCities()->begin(); j != (*i)->getRules()->getCities()->end(); ++j)
			{
				drawTarget(*j, _countries);

				// Don't draw if city is facing back
				if (pointBack((*j)->getLongitude(), (*j)->getLatitude()))
					continue;

				// Convert coordinates
				polarToCart((*j)->getLongitude(), (*j)->getLatitude(), &x, &y);

				label->setX(x - 50);
				label->setY(y + 2);
				label->setText((*j)->getName(_game->getLanguage()));
				label->blit(_countries);
			}
		}
		// Draw bases names
		for (std::vector<Base*>::iterator j = _game->getSavedGame()->getBases()->begin(); j != _game->getSavedGame()->getBases()->end(); ++j)
		{
			if ((*j)->getMarker() == -1 || pointBack((*j)->getLongitude(), (*j)->getLatitude()))
				continue;
			polarToCart((*j)->getLongitude(), (*j)->getLatitude(), &x, &y);
			label->setX(x - 50);
			label->setY(y + 2);
			label->setColor(BASE_LABEL_COLOR);
			label->setText((*j)->getName());
			label->blit(_countries);
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
				for (size_t k = 0; k != (*i)->getRules()->getLatMax().size(); ++k)
				{
					double lon2 = (*i)->getRules()->getLonMax().at(k);
					double lon1 = (*i)->getRules()->getLonMin().at(k);
					double lat2 = (*i)->getRules()->getLatMax().at(k);
					double lat1 = (*i)->getRules()->getLatMin().at(k);

					drawVHLine(_countries, lon1, lat1, lon2, lat1, color);
					drawVHLine(_countries, lon1, lat2, lon2, lat2, color);
					drawVHLine(_countries, lon1, lat1, lon1, lat2, color);
					drawVHLine(_countries, lon2, lat1, lon2, lat2, color);
				}
			}
		}
		else if (debugType == 1)
		{
			color = 0;
			for (std::vector<Region*>::iterator i = _game->getSavedGame()->getRegions()->begin(); i != _game->getSavedGame()->getRegions()->end(); ++i)
			{
				color += 10;
				for (size_t k = 0; k != (*i)->getRules()->getLatMax().size(); ++k)
				{
					double lon2 = (*i)->getRules()->getLonMax().at(k);
					double lon1 = (*i)->getRules()->getLonMin().at(k);
					double lat2 = (*i)->getRules()->getLatMax().at(k);
					double lat1 = (*i)->getRules()->getLatMin().at(k);

					drawVHLine(_countries, lon1, lat1, lon2, lat1, color);
					drawVHLine(_countries, lon1, lat2, lon2, lat2, color);
					drawVHLine(_countries, lon1, lat1, lon1, lat2, color);
					drawVHLine(_countries, lon2, lat1, lon2, lat2, color);
				}
			}
		}
		else if (debugType == 2)
		{
			for (std::vector<Region*>::iterator i = _game->getSavedGame()->getRegions()->begin(); i != _game->getSavedGame()->getRegions()->end(); ++i)
			{
				color = -1;
				for (std::vector<MissionZone>::const_iterator j = (*i)->getRules()->getMissionZones().begin(); j != (*i)->getRules()->getMissionZones().end(); ++j)
				{
					color += 2;
					for (std::vector<MissionArea>::const_iterator k = (*j).areas.begin(); k != (*j).areas.end(); ++k)
					{
						double lon2 = (*k).lonMax;
						double lon1 = (*k).lonMin;
						double lat2 = (*k).latMax;
						double lat1 = (*k).latMin;

						drawVHLine(_countries, lon1, lat1, lon2, lat1, color);
						drawVHLine(_countries, lon1, lat2, lon2, lat2, color);
						drawVHLine(_countries, lon1, lat1, lon1, lat2, color);
						drawVHLine(_countries, lon2, lat1, lon2, lat2, color);
					}
				}
			}
		}
	}
	else
	{
		if (canSwitchDebugType)
		{
			++debugType;
			if (debugType > 2) debugType = 0;
			canSwitchDebugType = false;
		}
	}
}

void Globe::drawPath(Surface *surface, double lon1, double lat1, double lon2, double lat2)
{
	double length;
	Sint16 count;
	double x1, y1, x2, y2;
	CordPolar p1, p2;
	Cord a(CordPolar(lon1, lat1));
	Cord b(CordPolar(lon2, lat2));

	if (-b == a)
		return;

	b -= a;

	//longer path have more parts
	length = b.norm();
	length *= length*15;
	count = length + 1;
	b /= count;
	p1 = CordPolar(a);
	polarToCart(p1.lon, p1.lat, &x1, &y1);
	for (int i = 0; i < count; ++i)
	{
		a += b;
		p2 = CordPolar(a);
		polarToCart(p2.lon, p2.lat, &x2, &y2);

		if (!pointBack(p1.lon, p1.lat) && !pointBack(p2.lon, p2.lat))
		{
			XuLine(surface, this, x1, y1, x2, y2, 8);
		}

		p1 = p2;
		x1 = x2;
		y1 = y2;
	}
}

/**
 * Draws the flight paths of player craft flying on the globe.
 */
void Globe::drawFlights()
{
	//_radars->clear();

	if (!Options::globeFlightPaths)
		return;

	// Lock the surface
	_radars->lock();

	// Draw the craft flight paths
	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
		for (std::vector<Craft*>::iterator j = (*i)->getCrafts()->begin(); j != (*i)->getCrafts()->end(); ++j)
		{
			// Hide crafts docked at base
			if ((*j)->getStatus() != "STR_OUT" || (*j)->getDestination() == 0 /*|| pointBack((*j)->getLongitude(), (*j)->getLatitude())*/)
				continue;

			double lon1 = (*j)->getLongitude();
			double lat1 = (*j)->getLatitude();
			double lon2 = (*j)->getDestination()->getLongitude();
			double lat2 = (*j)->getDestination()->getLatitude();

			if ((*j)->isMeetCalculated())
			{
				lon2 = (*j)->getMeetLongitude();
				lat2 = (*j)->getMeetLatitude();
			}
			drawPath(_radars, lon1, lat1, lon2, lat2);

			if ((*j)->isMeetCalculated())
			{
				lon1 = (*j)->getDestination()->getLongitude();
				lat1 = (*j)->getDestination()->getLatitude();

				drawPath(_radars, lon1, lat1, lon2, lat2);
			}
		}
	}

	// Unlock the surface
	_radars->unlock();
}

/**
 * Draws the marker for a specified target on the globe.
 * @param target Pointer to globe target.
 */
void Globe::drawTarget(Target *target, Surface *surface)
{
	if (target->getMarker() != -1 && !pointBack(target->getLongitude(), target->getLatitude()))
	{
		Sint16 x, y;
		polarToCart(target->getLongitude(), target->getLatitude(), &x, &y);
		Surface *marker = _markerSet->getFrame(target->getMarker());
		marker->setX(x - marker->getWidth() / 2);
		marker->setY(y - marker->getHeight() / 2);
		marker->blit(surface);
	}
}

/**
 * Draws the markers of all the various things going
 * on around the world on top of the globe.
 */
void Globe::drawMarkers()
{
	_markers->clear();

	// Draw the base markers
	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
		drawTarget(*i, _markers);
	}

	// Draw the waypoint markers
	for (std::vector<Waypoint*>::iterator i = _game->getSavedGame()->getWaypoints()->begin(); i != _game->getSavedGame()->getWaypoints()->end(); ++i)
	{
		drawTarget(*i, _markers);
	}

	// Draw the mission site markers
	for (std::vector<MissionSite*>::iterator i = _game->getSavedGame()->getMissionSites()->begin(); i != _game->getSavedGame()->getMissionSites()->end(); ++i)
	{
		drawTarget(*i, _markers);
	}

	// Draw the alien base markers
	for (std::vector<AlienBase*>::iterator i = _game->getSavedGame()->getAlienBases()->begin(); i != _game->getSavedGame()->getAlienBases()->end(); ++i)
	{
		drawTarget(*i, _markers);
	}

	// Draw the UFO markers
	for (std::vector<Ufo*>::iterator i = _game->getSavedGame()->getUfos()->begin(); i != _game->getSavedGame()->getUfos()->end(); ++i)
	{
		drawTarget(*i, _markers);
	}

	// Draw the craft markers
	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
		for (std::vector<Craft*>::iterator j = (*i)->getCrafts()->begin(); j != (*i)->getCrafts()->end(); ++j)
		{
			drawTarget(*j, _markers);
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
 * Ignores any mouse hovers that are outside the globe.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Globe::mouseOver(Action *action, State *state)
{
	double lon, lat;
	cartToPolar((Sint16)floor(action->getAbsoluteXMouse()), (Sint16)floor(action->getAbsoluteYMouse()), &lon, &lat);

	if (_isMouseScrolling && action->getDetails()->type == SDL_MOUSEMOTION)
	{
		// The following is the workaround for a rare problem where sometimes
		// the mouse-release event is missed for any reason.
		// (checking: is the dragScroll-mouse-button still pressed?)
		// However if the SDL is also missed the release event, then it is to no avail :(
		if (0 == (SDL_GetMouseState(0, 0)&SDL_BUTTON(Options::geoDragScrollButton)))
		{ // so we missed again the mouse-release :(
			// Check if we have to revoke the scrolling, because it was too short in time, so it was a click
			if ((!_mouseMovedOverThreshold) && ((int)(SDL_GetTicks() - _mouseScrollingStartTime) <= (Options::dragScrollTimeTolerance)))
			{
				center(_lonBeforeMouseScrolling, _latBeforeMouseScrolling);
			}
			_isMouseScrolled = _isMouseScrolling = false;
			stopScrolling(action);
			return;
		}

		_isMouseScrolled = true;

		if (Options::touchEnabled == false)
		{
			// Set the mouse cursor back
			SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
			SDL_WarpMouseInWindow(_game->getScreen()->getWindow(), (_game->getScreen()->getWidth() - 100) / 2 , _game->getScreen()->getHeight() / 2);
			SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
		}

		// Check the threshold
		_totalMouseMoveX += action->getDetails()->motion.xrel;
		_totalMouseMoveY += action->getDetails()->motion.yrel;

		if (!_mouseMovedOverThreshold)
			_mouseMovedOverThreshold = ((std::abs(_totalMouseMoveX) > Options::dragScrollPixelTolerance) || (std::abs(_totalMouseMoveY) > Options::dragScrollPixelTolerance));

		// Scrolling
		if (Options::geoDragScrollInvert)
		{
			double newLon = ((double)_totalMouseMoveX / action->getXScale()) * ROTATE_LONGITUDE/(_zoom+1)/2;
			double newLat = ((double)_totalMouseMoveY / action->getYScale()) * ROTATE_LATITUDE/(_zoom+1)/2;
			center(_lonBeforeMouseScrolling + newLon / (Options::geoScrollSpeed / 10), _latBeforeMouseScrolling + newLat / (Options::geoScrollSpeed / 10));
		}
		else
		{
			double newLon = -action->getDetails()->motion.xrel * ROTATE_LONGITUDE/(_zoom+1)/2;
			double newLat = -action->getDetails()->motion.yrel * ROTATE_LATITUDE/(_zoom+1)/2;
			center(_cenLon + newLon / (Options::geoScrollSpeed / 10), _cenLat + newLat / (Options::geoScrollSpeed / 10));
		}

		if (Options::touchEnabled == false)
		{
			// We don't want to see the mouse-cursor jumping :)
			action->setMouseAction(_xBeforeMouseScrolling, _yBeforeMouseScrolling, getX(), getY());
			action->getDetails()->motion.x = _xBeforeMouseScrolling; action->getDetails()->motion.y = _yBeforeMouseScrolling;
		}

		_game->getCursor()->handle(action);
	}

	if (Options::touchEnabled == false &&
		_isMouseScrolling &&
		(action->getDetails()->motion.x != _xBeforeMouseScrolling ||
		action->getDetails()->motion.y != _yBeforeMouseScrolling))
	{
		action->setMouseAction(_xBeforeMouseScrolling, _yBeforeMouseScrolling, getX(), getY());
		action->getDetails()->motion.x = _xBeforeMouseScrolling; action->getDetails()->motion.y = _yBeforeMouseScrolling;
	}
	// Check for errors
	if (lat == lat && lon == lon)
	{
		InteractiveSurface::mouseOver(action, state);
	}
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

	if (action->getDetails()->button.button == Options::geoDragScrollButton)
	{
		_isMouseScrolling = true;
		_isMouseScrolled = false;
		SDL_GetMouseState(&_xBeforeMouseScrolling, &_yBeforeMouseScrolling);
		_lonBeforeMouseScrolling = _cenLon;
		_latBeforeMouseScrolling = _cenLat;
		_totalMouseMoveX = 0; _totalMouseMoveY = 0;
		_mouseMovedOverThreshold = false;
		_mouseScrollingStartTime = SDL_GetTicks();
	}
	// Check for errors
	if (lat == lat && lon == lon)
	{
		InteractiveSurface::mousePress(action, state);
	}
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
	if (action->getDetails()->button.button == Options::geoDragScrollButton)
	{
		stopScrolling(action);
	}
	// Check for errors
	if (lat == lat && lon == lon)
	{
		InteractiveSurface::mouseRelease(action, state);
	}
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

	// The following is the workaround for a rare problem where sometimes
	// the mouse-release event is missed for any reason.
	// However if the SDL is also missed the release event, then it is to no avail :(
	// (this part handles the release if it is missed and now an other button is used)
	if (_isMouseScrolling)
	{
		if (action->getDetails()->button.button != Options::geoDragScrollButton
			&& 0 == (SDL_GetMouseState(0, 0)&SDL_BUTTON(Options::geoDragScrollButton)))
		{ // so we missed again the mouse-release :(
			// Check if we have to revoke the scrolling, because it was too short in time, so it was a click
			if ((!_mouseMovedOverThreshold) && ((int)(SDL_GetTicks() - _mouseScrollingStartTime) <= (Options::dragScrollTimeTolerance)))
			{
				center(_lonBeforeMouseScrolling, _latBeforeMouseScrolling);
			}
			_isMouseScrolled = _isMouseScrolling = false;
			stopScrolling(action);
		}
	}

	// DragScroll-Button release: release mouse-scroll-mode
	if (_isMouseScrolling)
	{
		// While scrolling, other buttons are ineffective
		if (action->getDetails()->button.button == Options::geoDragScrollButton)
		{
			_isMouseScrolling = false;
			stopScrolling(action);
		}
		else
		{
			return;
		}
		// Check if we have to revoke the scrolling, because it was too short in time, so it was a click
		if ((!_mouseMovedOverThreshold) && ((int)(SDL_GetTicks() - _mouseScrollingStartTime) <= (Options::dragScrollTimeTolerance)))
		{
			_isMouseScrolled = false;
			stopScrolling(action);
			center(_lonBeforeMouseScrolling, _latBeforeMouseScrolling);
		}
		if (_isMouseScrolled) return;
	}

	// Check for errors
	if (lat == lat && lon == lon)
	{
		InteractiveSurface::mouseClick(action, state);
		if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
		{
			center(lon, lat);
		}
	}
}

/**
 * Ignores any mouse clicks that are outside the globe
 * and handles globe rotation and zooming.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Globe::mouseWheel(Action* action, State* state)
{
	InteractiveSurface::mouseWheel(action, state);
	if (action->getDetails()->wheel.y > 0)
	{
		zoomIn();
	}
	else if (action->getDetails()->wheel.y < 0)
	{
		zoomOut();
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
	if (action->getDetails()->key.keysym.sym == Options::keyGeoToggleDetail)
	{
		toggleDetail();
	}
	if (action->getDetails()->key.keysym.sym == Options::keyGeoToggleRadar)
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

	*shade = worldshades[ CreateShadow::getShadowValue(Cord(0.,0.,1.), getSunDirection(lon, lat), 0) ];
	Polygon *t = getPolygonFromLonLat(lon,lat);
	*texture = (t==NULL)? -1 : t->getTexture();
}

/**
 * Returns the current globe zoom factor.
 * @return Current zoom (0-5).
 */
size_t Globe::getZoom() const
{
	return _zoom;
}

/*
 * Turns Radar lines on or off.
 */
void Globe::toggleRadarLines()
{
	Options::globeRadarLines = !Options::globeRadarLines;
	drawRadars();
}

/*
 * Resizes the geoscape.
 */
void Globe::resize()
{
	Surface *surfaces[4] = {this, _markers, _countries, _radars};
	int width = Options::baseXGeoscape - 64;
	int height = Options::baseYGeoscape;

	for (int i = 0; i < 4; ++i)
	{
		surfaces[i]->setWidth(width);
		surfaces[i]->setHeight(height);
		surfaces[i]->invalidate();
	}
	_clipper->Wxrig = width;
	_clipper->Wybot = height;
	_cenX = width / 2;
	_cenY = height / 2;
	setupRadii(width, height);
	invalidate();
}

/*
 * Set up the Radius of earth at the various zoom levels.
 * @param width the new width of the globe.
 * @param height the new height of the globe.
 */
void Globe::setupRadii(int width, int height)
{
	_zoomRadius.clear();

	_zoomRadius.push_back(0.45*height);
	_zoomRadius.push_back(0.60*height);
	_zoomRadius.push_back(0.90*height);
	_zoomRadius.push_back(1.40*height);
	_zoomRadius.push_back(2.25*height);
	_zoomRadius.push_back(3.60*height);

	_radius = _zoomRadius[_zoom];
	_radiusStep = (_zoomRadius[DOGFIGHT_ZOOM] - _zoomRadius[0]) / 10.0;

	_earthData.resize(_zoomRadius.size());
	//filling normal field for each radius

	for (size_t r = 0; r<_zoomRadius.size(); ++r)
	{
		_earthData[r].resize(width * height);
		for (int j=0; j<height; ++j)
			for (int i=0; i<width; ++i)
			{
				_earthData[r][width*j + i] = static_data.circle_norm(width/2, height/2, _zoomRadius[r], i+.5, j+.5);
			}
	}
}

/**
 * Move the mouse back to where it started after we finish drag scrolling.
 * @param action Pointer to an action.
 */
void Globe::stopScrolling(Action *action)
{
	SDL_WarpMouseInWindow(_game->getScreen()->getWindow(), _xBeforeMouseScrolling, _yBeforeMouseScrolling);
	action->setMouseAction(_xBeforeMouseScrolling, _yBeforeMouseScrolling, getX(), getY());
}

void Globe::setCraftRange(double lon, double lat, double range)
{
	_craft = (range > 0.0);
	_craftLon = lon;
	_craftLat = lat;
	_craftRange = range;
}

}
