#ifndef OPENXCOM_MINIMAPVIEW_H
#define OPENXCOM_MINIMAPVIEW_H

#include "../Engine/InteractiveSurface.h"

namespace OpenXcom
{
class Game;
class Map;
/**
   MiniMapView is the class used to display the map in the MiniMapState
*/
class MiniMapView : public InteractiveSurface
{
	Game * _game;
	Map * _map;
	int _startX;
	int _startY;
	int _lvl;
	/// Handle clicking on the MiniMap
	void mouseClick (Action *action, State *state);
public:
	/// Create the MiniMapView
	MiniMapView(int w, int h, int x, int y, Game * game, Map * map);
	/// Draw the minimap
	void draw();
	/// Blit the minimap to another surface
	void blit(Surface *surface);
	/// Change the displayed minimap level
	void up ();
	/// Change the displayed minimap level
	void down ();
	/// Get the displayed minimap level
	int getDisplayedLevel ();
	/// Set the displayed minimap level
	void setDisplayedLevel (int level);
};

}

#endif
