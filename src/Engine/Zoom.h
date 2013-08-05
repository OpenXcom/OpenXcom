/*
 * Copyright 2010-2013 OpenXcom Developers.
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
