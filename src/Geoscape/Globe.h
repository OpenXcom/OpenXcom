/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#ifndef OPENXCOM_GLOBE_H
#define OPENXCOM_GLOBE_H

#include <vector>
#include <list>
#include "../Engine/InteractiveSurface.h"
#include "../Engine/FastLineClip.h"
#include "Cord.h"

namespace OpenXcom
{

class Game;
class Polygon;
class SurfaceSet;
class Timer;
class Target;
class LocalizedText;
class RuleGlobe;

/**
 * Interactive globe view of the world.
 * Takes a flat world map made out of land polygons with
 * polar coordinates and renders it as a 3D-looking globe
 * with cartesian coordinates that the player can interact with.
 */
class Globe : public InteractiveSurface
{
private:
	static const int NUM_LANDSHADES = 48;
	static const int NUM_SEASHADES = 72;
	static const int NEAR_RADIUS = 25;
	static const int DOGFIGHT_ZOOM = 3;
	static const int CITY_MARKER = 8;
	static const double ROTATE_LONGITUDE;
	static const double ROTATE_LATITUDE;

	RuleGlobe *_rules;
	double _cenLon, _cenLat, _rotLon, _rotLat, _hoverLon, _hoverLat;
	Sint16 _cenX, _cenY;
	size_t _zoom, _zoomOld, _zoomTexture;
	SurfaceSet *_texture, *_markerSet;
	Game *_game;
	Surface *_markers, *_countries, *_radars;
	bool _hover;
	int _blink;
	Timer *_blinkTimer, *_rotTimer;
	std::list<Polygon*> _cacheLand;
	FastLineClip *_clipper;
	double _radius, _radiusStep;
	///normal of each pixel in earth globe per zoom level
	std::vector<std::vector<Cord> > _earthData;
	///data sample used for noise in shading
	std::vector<Sint16> _randomNoiseData;
	///list of dimension of earth on screen per zoom level
	std::vector<double> _zoomRadius;

	bool _isMouseScrolling, _isMouseScrolled;
	int _xBeforeMouseScrolling, _yBeforeMouseScrolling;
	double _lonBeforeMouseScrolling, _latBeforeMouseScrolling;
	Uint32 _mouseScrollingStartTime;
	int _totalMouseMoveX, _totalMouseMoveY;
	bool _mouseMovedOverThreshold;

	/// Sets the globe zoom factor.
	void setZoom(size_t zoom);
	/// Checks if a point is behind the globe.
	bool pointBack(double lon, double lat) const;
	/// Return latitude of last visible to player point on given longitude.
	double lastVisibleLat(double lon) const;
	/// Checks if a point is inside a polygon.
	bool insidePolygon(double lon, double lat, Polygon *poly) const;
	/// Checks if a target is near a point.
	bool targetNear(Target* target, int x, int y) const;
	/// Caches a set of polygons.
	void cache(std::list<Polygon*> *polygons, std::list<Polygon*> *cache);
	/// Get position of sun relative to given position in polar cords and date.
	Cord getSunDirection(double lon, double lat) const;
	/// Draw globe range circle.
	void drawGlobeCircle(double lat, double lon, double radius, int segments);
	/// Special "transparent" line.
	void XuLine(Surface* surface, Surface* src, double x1, double y1, double x2, double y2, int shade);
	/// Draw line on globe surface.
	void drawVHLine(Surface *surface, double lon1, double lat1, double lon2, double lat2, Uint8 color);
	/// Draw flight path.
	void drawPath(Surface *surface, double lon1, double lat1, double lon2, double lat2);
	/// Draw target marker.
	void drawTarget(Target *target);
public:
	static Uint8 oceanColor1;
	static Uint8 oceanColor2;
	/// Creates a new globe at the specified position and size.
	Globe(Game* game, int cenX, int cenY, int width, int height, int x = 0, int y = 0);
	/// Cleans up the globe.
	~Globe();
	/// Converts polar coordinates to cartesian coordinates.
	void polarToCart(double lon, double lat, Sint16 *x, Sint16 *y) const;
	/// Converts polar coordinates to cartesian coordinates.
	void polarToCart(double lon, double lat, double *x, double *y) const;
	/// Converts cartesian coordinates to polar coordinates.
	void cartToPolar(Sint16 x, Sint16 y, double *lon, double *lat) const;
	/// Sets the texture set for the globe's polygons.
	void setTexture(SurfaceSet *texture);
	/// Starts rotating the globe left.
	void rotateLeft();
	/// Starts rotating the globe right.
	void rotateRight();
	/// Starts rotating the globe up.
	void rotateUp();
	/// Starts rotating the globe down.
	void rotateDown();
	/// Stops rotating the globe.
	void rotateStop();
	/// Stops longitude rotation of the globe.
	void rotateStopLon();
	/// Stops latitude rotation of the globe.
	void rotateStopLat();
	/// Zooms the globe in.
	void zoomIn();
	/// Zooms the globe out.
	void zoomOut();
	/// Zooms the globe minimum.
	void zoomMin();
	/// Zooms the globe maximum.
	void zoomMax();
	/// Saves the zoom level for dogfights.
	void saveZoomDogfight();
	/// Zooms the globe in for dogfights.
	bool zoomDogfightIn();
	/// Zooms the globe out for dogfights.
	bool zoomDogfightOut();
	/// Gets the current zoom.
	size_t getZoom() const;
	/// Centers the globe on a point.
	void center(double lon, double lat);
	/// Checks if a point is inside land.
	bool insideLand(double lon, double lat) const;
	/// Turns on/off the globe detail.
	void toggleDetail();
	/// Gets all the targets near a point on the globe.
	std::vector<Target*> getTargets(int x, int y, bool craft) const;
	/// Caches visible globe polygons.
	void cachePolygons();
	/// Sets the palette of the globe.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	/// Handles the timers.
	void think();
	/// Blinks the markers.
	void blink();
	/// Rotates the globe.
	void rotate();
	/// Draws the whole globe.
	void draw();
	/// Draws the ocean of the globe.
	void drawOcean();
	/// Draws the land of the globe.
	void drawLand();
	/// Draws the shadow.
	void drawShadow();
	/// Draws the radar ranges of the globe.
	void drawRadars();
	/// Draws the flight paths of the globe.
	void drawFlights();
	/// Draws the country details of the globe.
	void drawDetail();
	/// Draws all the markers over the globe.
	void drawMarkers();
	/// Blits the globe onto another surface.
	void blit(Surface *surface);
	/// Special handling for mouse hover.
	void mouseOver(Action *action, State *state);
	/// Special handling for mouse presses.
	void mousePress(Action *action, State *state);
	/// Special handling for mouse releases.
	void mouseRelease(Action *action, State *state);
	/// Special handling for mouse clicks.
	void mouseClick(Action *action, State *state);
	/// Special handling for key presses.
	void keyboardPress(Action *action, State *state);
	/// Get the polygons texture and shade at the given point.
	void getPolygonTextureAndShade(double lon, double lat, int *texture, int *shade) const;
	/// Get the localized text.
	const LocalizedText &tr(const std::string &id) const;
	/// Get the localized text.
	LocalizedText tr(const std::string &id, unsigned n) const;
	/// Sets hover base position.
	void setNewBaseHoverPos(double lon, double lat);
	/// Turns on new base hover mode.
	void setNewBaseHover(void);
	/// Turns off new base hover mode.
	void unsetNewBaseHover(void);
	/// Gets state of base hover mode
	bool getNewBaseHover(void);
	/// Gets _detail variable
	bool getShowRadar(void);
	/// set the _radarLines variable
	void toggleRadarLines();
	/// Update the resolution settings, we just resized the window.
	void resize();
	/// Set up the radius of earth and stuff.
	void setupRadii(int width, int height);
	/// Move the mouse back to where it started after we finish drag scrolling.
	void stopScrolling(Action *action);
};

}

#endif
