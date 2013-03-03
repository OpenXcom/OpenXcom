#ifndef ZOOM_H
#define ZOOM_H

#include <SDL.h>
#include "OpenGL.h"


namespace OpenXcom
{


class Zoom
{

	public:
	/// Flip screen given src and dst; might use software or OpenGL.
	static void flipWithZoom(SDL_Surface *src, SDL_Surface *dst, OpenGL *glOut);
	/// Copy src to dst, resizing as needed. Please don't use flipx or flipy as the optimized functions ignore these parameters.
	static int _zoomSurfaceY(SDL_Surface * src, SDL_Surface * dst, int flipx, int flipy);
	/// Check for SSE2 instructions using CPUID.
	static bool haveSSE2(); 

private:

};

}

#endif // ZOOM_H
