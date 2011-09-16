#include "MiniMapView.h"
#include "../Savegame/Tile.h"
#include "Map.h"
#include "../Engine/Action.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Engine/Game.h"
#include "../Engine/SurfaceSet.h"
#include "../Resource/ResourcePack.h"
#include "../Savegame/SavedGame.h"
#include <sstream>

namespace OpenXcom
{

MiniMapView::MiniMapView(int w, int h, int x, int y, Game * game, Map * map) : InteractiveSurface(w, h, x, y), _game(game), _map(map), _startX(0),  _startY(0), _lvl(0)
{
	setCenter(_map->getCenterX (), _map->getCenterY());
}

void MiniMapView::blit(Surface *surface)
{
	draw();
	InteractiveSurface::blit (surface);
}

void MiniMapView::draw()
{
	SavedBattleGame * battle = _game->getSavedGame()->getBattleGame();
	int rwidth = 4;
	int rheight = 4;
	SurfaceSet * set = _game->getResourcePack()->getSurfaceSet("SCANG.DAT");
	int py = _startY;
	int beginX = 0, dummy, beginY = 0;
	_map->convertScreenToMap(0, 0, &beginX, &dummy);
	_map->convertScreenToMap(0, _map->getHeight(), &dummy, &beginY);
	for (int y = getHeight () - rheight; y >= 0; y-=rheight)
	{
		int px = _startX;
		for (int x = 0; x < getWidth (); x += rwidth)
		{
			SDL_Rect r;
			r.x = x;
			r.y = y;
			r.w = rwidth;
			r.h = rheight;
			MapData * data = 0;
			int lvl = _lvl;
			Tile * t = 0;
			while ((!t || !data) && lvl >= 0)
			{
				Position p (px, py, lvl--);
				t = battle->getTile(p);
				if (t)
				{
					int i = 3;
					while (!data && i >= 0)
					{
						data = t->getMapData(i--);
					}
				}
			}
			Surface * s = 0;
			if(data && set)
			{
				s = set->getFrame (data->getMiniMapIndex()+35);
			}
			if(!s || !t->isDiscovered(2))
			{
				drawRect(&r, 0);
			}
			else
			{
				s->blitNShade(this, x, y, 0);
			}
			px++;
		}
		py++;
	}
}

void MiniMapView::up ()
{
	const int MAX_LEVEL = 3;
	_lvl++;
	if (_lvl > MAX_LEVEL)
	{
		_lvl = MAX_LEVEL;
	}
	else
	{
		draw();
	}
}

void MiniMapView::down ()
{
	_lvl--;
	if (_lvl < 0)
	{
		_lvl = 0;
	}
	else
	{
		draw();
	}
}

void MiniMapView::mouseClick (Action *action, State *state)
{
	InteractiveSurface::mouseClick(action, state);
	draw();
}

int MiniMapView::getDisplayedLevel ()
{
	return _lvl;
}

void MiniMapView::setDisplayedLevel (int level)
{
	_lvl = level;
}

void MiniMapView::setCenter(int x, int y)
{
	_startX = x - ((getWidth () / CELL_WIDTH) / 2);
	_startY = y - ((getHeight () / CELL_HEIGHT) / 2);
	if(_startX < 0) _startX = 0;
	if(_startY < 0) _startY = 0;
}

}
