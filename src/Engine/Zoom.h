#ifndef ZOOM_H
#define ZOOM_H

#include <SDL.h>


namespace OpenXcom
{


class Zoom
{

public:
	static int _zoomSurfaceY(SDL_Surface * src, SDL_Surface * dst, int flipx, int flipy);
	static bool haveSSE2(); 

private:
	Zoom();
	~Zoom();

};

}

#endif // ZOOM_H
