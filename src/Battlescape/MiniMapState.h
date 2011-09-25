#ifndef OPENXCOM_MINIMAP_H
#define OPENXCOM_MINIMAP_H

#include "../Engine/State.h"

namespace OpenXcom
{
class Map;
class InteractiveSurface;
class Text;
class MiniMapView;

/**
   The MiniMap is a representation of a Battlescape map. Allow to see more of the map.
*/
class MiniMapState : public State
{
	Map * _map;
	InteractiveSurface * _surface;
	MiniMapView * _miniMapView;
	int _displayedLevel;
	Text * _txtLevel;
public:
	/// Create the MiniMapState
	MiniMapState (Game * game, Map * map);
	/// Handler for the OK button
	void btnOkClick (Action * action);
	/// Handler for the one level up button
	void btnLevelUpClick (Action * action);
	/// Handler for the one level down button
	void btnLevelDownClick (Action * action);
};
}

#endif
