#ifndef OPENXCOM_MINIMAP_H
#define OPENXCOM_MINIMAP_H

#include "../Engine/State.h"

namespace OpenXcom
{
class Map;
class InteractiveSurface;
class Text;
class MiniMapView;

class MiniMapState : public State
{
	Map * _map;
	InteractiveSurface * _surface;
	MiniMapView * _miniMapView;
	int _displayedLevel;
	Text * _txtLevel;
public:
	MiniMapState (Game * game, Map * map);
	void btnOkClick (Action * action);
	void btnLevelUpClick (Action * action);
	void btnLevelDownClick (Action * action);
	void mapClick ();
};
}

#endif
