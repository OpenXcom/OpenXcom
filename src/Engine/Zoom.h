#ifndef ZOOM_H
#define ZOOM_H

#include <SDL.h>


namespace OpenXcom
{


class Zoom
{

public:
	/// Copy src to dst, resizing as needed. Please don't use flipx or flipy.
	static int _zoomSurfaceY(SDL_Surface * src, SDL_Surface * dst, int flipx, int flipy);
	/// Check for SSE2 instructions using CPUID.
	static bool haveSSE2(); 

private:

};

}

#endif // ZOOM_H
