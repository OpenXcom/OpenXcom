#ifndef OPENXCOM_MINIMAPVIEW_H
#define OPENXCOM_MINIMAPVIEW_H

#include "../Engine/InteractiveSurface.h"

namespace OpenXcom
{
class Game;
class Map;

class MiniMapView : public InteractiveSurface
{
	Game * _game;
	Map * _map;
	int _startX;
	int _startY;
	int _lvl;
	void mouseClick (Action *action, State *state);
public:
	MiniMapView(int w, int h, int x, int y, Game * game, Map * map);
	void draw();
	void blit(Surface *surface);
	void up ();
	void down ();
	int getDisplayedLevel ();
};

}

#endif
