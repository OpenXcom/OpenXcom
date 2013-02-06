/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include "Screen.h"
#include <sstream>
#include <iomanip>
#include <SDL_rotozoom.h>
#include <SDL_endian.h>
#include <limits.h>
#include "../lodepng.h"
#include "Exception.h"
#include "Surface.h"
#include "Logger.h"
#include "Action.h"
#include "Options.h"
#include "CrossPlatform.h"

#ifdef __GNUC__
#define GCC_SSE2
// TODO gcc intrinsics
#else
#	ifdef _WIN32
#define VS_SSE2 true
// probably Visual Studio (or Intel C++ which should also work)
#include <emmintrin.h> // for SSE2 intrinsics; see http://msdn.microsoft.com/en-us/library/has3d153%28v=vs.71%29.aspx
#	endif
#endif

namespace OpenXcom
{

const double Screen::BASE_WIDTH = 320.0;
const double Screen::BASE_HEIGHT = 200.0;

/**
 * Initializes a new display screen for the game to render contents to.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param bpp Bits-per-pixel.
 * @param fullscreen Fullscreen mode.
 * @warning Currently the game is designed for 8bpp, so there's no telling what'll
 * happen if you use a different value.
 */
Screen::Screen(int width, int height, int bpp, bool fullscreen) : _bpp(bpp), _scaleX(1.0), _scaleY(1.0), _fullscreen(fullscreen)
{
	_surface = new Surface((int)BASE_WIDTH, (int)BASE_HEIGHT);
	_flags = SDL_SWSURFACE|SDL_HWPALETTE;
	if (_fullscreen)
	{
		_flags |= SDL_FULLSCREEN;
	}
	setResolution(width, height);
	memset(deferredPalette, 0, 256*sizeof(SDL_Color));
	_numColors = _firstColor = 0;
}

/**
 * Deletes the buffer from memory. The display screen itself
 * is automatically freed once SDL shuts down.
 */
Screen::~Screen()
{
	delete _surface;
}

/**
 * Returns the screen's internal buffer surface. Any
 * contents that need to be shown will be blitted to this.
 * @return Pointer to the buffer surface.
 */
Surface *Screen::getSurface() const
{
	return _surface;
}

/**
 * Handles screen key shortcuts.
 * @param action Pointer to an action.
 */
void Screen::handle(Action *action)
{
	if (action->getDetails()->type == SDL_KEYDOWN && action->getDetails()->key.keysym.sym == SDLK_RETURN && (SDL_GetModState() & KMOD_ALT) != 0)
	{
		setFullscreen(!_fullscreen);
	}
	else if (action->getDetails()->type == SDL_KEYDOWN && action->getDetails()->key.keysym.sym == Options::getInt("keyScreenshot"))
	{
		std::stringstream ss;
		int i = 0;
		do
		{
			ss.str("");
			ss << Options::getUserFolder() << "screen" << std::setfill('0') << std::setw(3) << i << ".png";
			i++;
		}
		while (CrossPlatform::fileExists(ss.str()));

		std::vector<unsigned char> image;
		SDL_Color *palette = getPalette();

		for (int y = 0; y < getHeight(); ++y)
		{
			for (int x = 0; x < getWidth(); ++x)
			{
				Uint8 color = ((Uint8 *)_screen->pixels)[y * _screen->pitch + x * _screen->format->BytesPerPixel];
				image.push_back(palette[color].r);
				image.push_back(palette[color].g);
				image.push_back(palette[color].b);
			}
		}

		unsigned error = lodepng::encode(ss.str(), image, getWidth(), getHeight(), LCT_RGB);
		if (error)
		{
			Log(LOG_ERROR) << "Saving to PNG failed: " << lodepng_error_text(error);
		}
	}
}

/**
 *  Optimized 8 bit zoomer for resizing by a factor of 2. Doesn't flip.
 *  Used internally by _zoomSurfaceY() below.
 *  source and dest. widths must be multiples of 8 bytes for 64-bit access
 */
static int zoomSurface2X_64bit(SDL_Surface *src, SDL_Surface *dst)
{
	Uint64 dataSrc;
	Uint64 dataDst;
	Uint8 *pixelSrc = (Uint8*)src->pixels;
	Uint8 *pixelDstRow = (Uint8*)dst->pixels;
	int sx, sy;
	static bool proclaimed = false;
	
	if (!proclaimed)
	{
		proclaimed = true;
		Log(LOG_INFO) << "Using somewhat fast 2X zoom routine.";
	}

	for (sy = 0; sy < src->h; ++sy, pixelDstRow += dst->pitch*2)
	{
		Uint64 *pixelDst = (Uint64*)pixelDstRow;
		Uint64 *pixelDst2 = (Uint64*)(pixelDstRow + dst->pitch);	
		for (sx = 0; sx < src->w; sx += 8, pixelSrc += 8)
		{
			dataSrc = *((Uint64*) pixelSrc);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			// boo
			SDL_Swap64(dataSrc);
#endif
/* expanded form of of data shift: 
			dataDst = (dataSrc & 0xFF) | ((dataSrc & 0xFF) << 8) | 
				((dataSrc & 0xFF00 ) << 8) | ((dataSrc & 0xFF00)) << 16)  | 
				((dataSrc & 0xFF0000) << 16) | ((dataSrc & 0xFF0000) << 24) |
				((dataSrc & 0xFF000000) << 24) | ((dataSrc & 0xFF000000) << 32);
*/
			// compact form, combining terms with equal multipliers (shifts)
			dataDst = (dataSrc & 0xFF) | ((dataSrc & 0xFFFF) << 8) | 
				((dataSrc & 0xFFFF00) << 16)  | 
				((dataSrc & 0xFFFF0000) << 24) |
				((dataSrc & 0xFF000000) << 32);

			*pixelDst = dataDst;
			*pixelDst2 = dataDst;
			pixelDst++; // forward 8 bytes!
			pixelDst2++;
			dataSrc >>= 32;

			dataDst = (dataSrc & 0xFF) | ((dataSrc & 0xFFFF) << 8) | 
				((dataSrc & 0xFFFF00) << 16)  | 
				((dataSrc & 0xFFFF0000) << 24) |
				((dataSrc & 0xFF000000) << 32);

			*pixelDst = dataDst;
			*pixelDst2 = dataDst;
			pixelDst++;	// 8 bytes again		
			pixelDst2++;
		}
	}
	
	return 0;
}


/**
 *  Optimized 8 bit zoomer for resizing by a factor of 2. Doesn't flip.
 *  32-bit version for sad old x86 chips which run out of registers 
 *  with the 64-bit version.
 *  Used internally by _zoomSurfaceY() below.
 *  source and dest. widths must be multiples of 4 bytes for 32-bit access
 */
static int zoomSurface2X_32bit(SDL_Surface *src, SDL_Surface *dst)
{
	Uint32 dataSrc;
	Uint32 dataDst;
	Uint8 *pixelSrc = (Uint8*)src->pixels;
	Uint8 *pixelDstRow = (Uint8*)dst->pixels;
	int sx, sy;
	static bool proclaimed = false;
	
	if (!proclaimed)
	{
		proclaimed = true;
		Log(LOG_INFO) << "Using 32-bit 2X zoom routine.";
	}

	for (sy = 0; sy < src->h; ++sy, pixelDstRow += dst->pitch*2)
	{
		Uint32 *pixelDst = (Uint32*)pixelDstRow;
		Uint32 *pixelDst2 = (Uint32*)(pixelDstRow + dst->pitch);	
		for (sx = 0; sx < src->w; sx += 4, pixelSrc += 4)
		{
			dataSrc = *((Uint32*) pixelSrc);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			// boo
			SDL_Swap32(dataSrc);
#endif

			dataDst = (dataSrc & 0xFF) | ((dataSrc & 0xFFFF) << 8) | 
				((dataSrc & 0xFF00) << 16);

			*pixelDst = dataDst;
			*pixelDst2 = dataDst;
			pixelDst++; // forward 4 bytes!
			pixelDst2++;

			dataSrc >>= 16; 

			dataDst = (dataSrc & 0xFF) | ((dataSrc & 0xFFFF) << 8) | 
				((dataSrc & 0xFF00) << 16);

			*pixelDst = dataDst;
			*pixelDst2 = dataDst;
			pixelDst++; // forward 4 bytes!
			pixelDst2++;
		}
	}
	
	return 0;
}

/**
 *  Optimized 8 bit zoomer for resizing by a factor of 4. Doesn't flip.
 *  Used internally by _zoomSurfaceY() below.
 *  source and dest. widths must be multiples of 8 bytes for 64-bit access
 */
static int zoomSurface4X_64bit(SDL_Surface *src, SDL_Surface *dst)
{
	Uint64 dataSrc;
	Uint64 dataDst;
	Uint8 *pixelSrc = (Uint8*)src->pixels;
	Uint8 *pixelDst = (Uint8*)dst->pixels;
	int sx, sy;
	static bool proclaimed = false;
	
	if (!proclaimed)
	{
		proclaimed = true;
		Log(LOG_INFO) << "Using modestly fast 4X zoom routine.";
	}

	for (sy = 0; sy < src->h; ++sy, pixelDst += dst->pitch*3)
	{
		/* Uint8 *pixelDst = pixelDstRow;*/
		for (sx = 0; sx < src->w; sx += 8, pixelSrc += 8)
		{
			dataSrc = *((Uint64*) pixelSrc);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			// boo
			SDL_Swap64(dataSrc);
#endif
			/* expanded form of of data shift:
			dataDst = (dataSrc & 0xFF) | ((dataSrc & 0xFF) << 8) | 
				((dataSrc & 0xFF) << 16 | ((datasrc & 0xFF) << 24) |
				((dataSrc & 0xFF00 ) << 24) | ((dataSrc & 0xFF00) << 32)  | 
				((dataSrc & 0xFF00 ) << 40) | ((dataSrc & 0xFF00) << 48) ;
				 */
			for (int i = 0; i < 4; ++i)
			{
				// compact form, combining terms with equal multipliers (shifts)
				dataDst = (dataSrc & 0xFF) | ((dataSrc & 0xFF) << 8) | 
					((dataSrc & 0xFF) << 16) | 
					((dataSrc & 0xFFFF ) << 24) | ((dataSrc & 0xFF00) << 32)  | 
					((dataSrc & 0xFF00 ) << 40) | ((dataSrc & 0xFF00) << 48) ;

				*((Uint64*)pixelDst) = dataDst;
				*((Uint64*)(pixelDst + dst->pitch)) = dataDst;
				*((Uint64*)(pixelDst + dst->pitch*2)) = dataDst;
				*((Uint64*)(pixelDst + dst->pitch*3)) = dataDst;
				pixelDst+=8; // forward 8 bytes!
				dataSrc >>= 16;
			}
		}
	}
	
	return 0;
}


/**
 *  Optimized 8 bit zoomer for resizing by a factor of 4. Doesn't flip.
 *  32-bit version.
 *  Used internally by _zoomSurfaceY() below.
 *  source and dest. widths must be multiples of 4 bytes for 32-bit access
 */
static int zoomSurface4X_32bit(SDL_Surface *src, SDL_Surface *dst)
{
	Uint32 dataSrc;
	Uint32 dataDst;
	Uint8 *pixelSrc = (Uint8*)src->pixels;
	Uint8 *pixelDstRow = (Uint8*)dst->pixels;
	int sx, sy;
	static bool proclaimed = false;
	
	if (!proclaimed)
	{
		proclaimed = true;
		Log(LOG_INFO) << "Using 32-bit 4X zoom routine.";
	}

	for (sy = 0; sy < src->h; ++sy, pixelDstRow += dst->pitch*4)
	{
		Uint32 *pixelDst = (Uint32*)pixelDstRow;
		Uint32 *pixelDst2 = (Uint32*)(pixelDstRow + dst->pitch);
		Uint32 *pixelDst3 = (Uint32*)(pixelDstRow + 2*dst->pitch);
		Uint32 *pixelDst4 = (Uint32*)(pixelDstRow + 3*dst->pitch);
		for (sx = 0; sx < src->w; sx += 4, pixelSrc += 4)
		{
			dataSrc = *((Uint32*) pixelSrc);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			// boo
			SDL_Swap32(dataSrc);
#endif
			for (int i = 0; i < 4; ++i)
			{
				dataDst = (dataSrc & 0xFF) | ((dataSrc & 0xFF) << 8) | 
					((dataSrc & 0xFF) << 16) | ((dataSrc & 0xFF ) << 24); 

				*pixelDst = dataDst;
				*pixelDst2 = dataDst;
				*pixelDst3 = dataDst;
				*pixelDst4 = dataDst;
				pixelDst++; // forward 4 bytes!
				pixelDst2++;
				pixelDst3++;
				pixelDst4++;
				dataSrc >>= 8;
			}
		}
	}
	
	return 0;
}

#ifdef VS_SSE2
/**
 *  Optimized 8 bit zoomer for resizing by a factor of 4. Doesn't flip.
 *  Used internally by _zoomSurfaceY() below.
 *	This is an SSE2 version written with Visual Studio intrinsics.
 *  source and dest. widths must be multiples of 16 bytes for 128-bit access
 *  and it would help if they were aligned properly... :(
 */
static int zoomSurface4X_VStudio_SSE2(SDL_Surface *src, SDL_Surface *dst)
{
	__m128i dataSrc;
	__m128i dataDst;
	Uint8 *pixelSrc = (Uint8*)src->pixels;
	Uint8 *pixelDst = (Uint8*)dst->pixels;
	int sx, sy;
	static bool proclaimed = false;
	
	if (!proclaimed)
	{
		proclaimed = true;
		Log(LOG_INFO) << "Using SSE2 4X zoom routine.";
	}

	for (sy = 0; sy < src->h; ++sy, pixelDst += dst->pitch*3)
	{
		for (sx = 0; sx < src->w; sx += 16, pixelSrc += 16)
		{
			dataSrc = *((__m128i*) pixelSrc);

			for (int i = 0; i < 4; ++i)
			{
				dataDst = _mm_unpacklo_epi8(dataSrc, dataSrc); // http://msdn.microsoft.com/en-us/library/has3d153%28v=vs.71%29.aspx#vcref_mm_unpackhi_epi8
				dataDst = _mm_unpacklo_epi8(dataDst, dataDst);

				*((__m128i*)pixelDst) = dataDst;
				*((__m128i*)(pixelDst + dst->pitch)) = dataDst;
				*((__m128i*)(pixelDst + dst->pitch*2)) = dataDst;
				*((__m128i*)(pixelDst + dst->pitch*3)) = dataDst;
				pixelDst += 16; // forward 16 bytes!
				dataSrc = _mm_srli_si128(dataSrc, 4); // shift right by four bytes
			}
		}
	}
	
	return 0;
}

#endif


/**
 * Internal 8 bit Zoomer without smoothing.
 * Source code originally from SDL_gfx (LGPL) with permission by author.
 *
 * Zooms 8bit palette/Y 'src' surface to 'dst' surface.
 * Assumes src and dst surfaces are of 8 bit depth.
 * Assumes dst surface was allocated with the correct dimensions.
 *
 * @param src The surface to zoom (input).
 * @param dst The zoomed surface (output).
 * @param flipx Flag indicating if the image should be horizontally flipped.
 * @param flipy Flag indicating if the image should be vertically flipped.
 * @return 0 for success or -1 for error.
 */
int Screen::_zoomSurfaceY(SDL_Surface * src, SDL_Surface * dst, int flipx, int flipy)
{
	int x, y;
	static Uint32 *sax, *say;
	Uint32 *csax, *csay;
	int csx, csy;
	Uint8 *sp, *dp, *csp;
	int dgap;
	static bool proclaimed = false;
	
// if we're scaling by a factor of 2 or 4, try to use a more efficient function	

	if (src->format->BytesPerPixel == 1 && dst->format->BytesPerPixel == 1)
	{
#ifdef VS_SSE2
		if (dst->w == src->w * 4 && dst->h == src->h * 4) return  zoomSurface4X_VStudio_SSE2(src, dst);
#endif
// __WORDSIZE is defined on Linux, SIZE_MAX on Windows
#if defined(__WORDSIZE) && (__WORDSIZE == 64) || defined(SIZE_MAX) && (SIZE_MAX > 0xFFFFFFFF)
		if (dst->w == src->w * 2 && dst->h == src->h * 2) return  zoomSurface2X_64bit(src, dst);
		else if (dst->w == src->w * 4 && dst->h == src->h * 4) return  zoomSurface4X_64bit(src, dst);
#else
		if (sizeof(void *) == 8)
		{
			if (dst->w == src->w * 2 && dst->h == src->h * 2) return  zoomSurface2X_64bit(src, dst);
			else if (dst->w == src->w * 4 && dst->h == src->h * 4) return  zoomSurface4X_64bit(src, dst);
		}
		else
		{
			if (dst->w == src->w * 2 && dst->h == src->h * 2) return  zoomSurface2X_32bit(src, dst);
			else if (dst->w == src->w * 4 && dst->h == src->h * 4) return  zoomSurface4X_32bit(src, dst);
		}
#endif
	}
	
	if (!proclaimed)
	{
		Log(LOG_INFO) << "Using slower scaling routine. For best results, try a resolution of 640x400 or 1280x800.";
	}
	
	/*
	* Allocate memory for row increments
	*/
	if ((sax = (Uint32 *) realloc(sax, (dst->w + 1) * sizeof(Uint32))) == NULL) {
		sax = 0;
		return (-1);
	}
	if ((say = (Uint32 *) realloc(say, (dst->h + 1) * sizeof(Uint32))) == NULL) {
		say = 0;
		//free(sax);
		return (-1);
	}

	/*
	* Pointer setup
	*/
	sp = csp = (Uint8 *) src->pixels;
	dp = (Uint8 *) dst->pixels;
	dgap = dst->pitch - dst->w;

	if (flipx) csp += (src->w-1);
	if (flipy) csp  = ( (Uint8*)csp + src->pitch*(src->h-1) );

	/*
	* Precalculate row increments
	*/
	csx = 0;
	csax = sax;
	for (x = 0; x < dst->w; x++) {
		csx += src->w;
		*csax = 0;
		while (csx >= dst->w) {
			csx -= dst->w;
			(*csax)++;
		}
		(*csax) *= (flipx ? -1 : 1);
		csax++;
	}
	csy = 0;
	csay = say;
	for (y = 0; y < dst->h; y++) {
		csy += src->h;
		*csay = 0;
		while (csy >= dst->h) {
			csy -= dst->h;
			(*csay)++;
		}
		(*csay) *= src->pitch * (flipy ? -1 : 1);
		csay++;
	}
	/*
	* Draw
	*/
	csay = say;
	for (y = 0; y < dst->h; y++) {
		csax = sax;
		sp = csp;
		for (x = 0; x < dst->w; x++) {
			/*
			* Draw
			*/
			*dp = *sp;
			/*
			* Advance source pointers
			*/
			sp += (*csax);
			csax++;
			/*
			* Advance destination pointer
			*/
			dp++;
		}
		/*
		* Advance source pointer (for row)
		*/
		csp += (*csay);
		csay++;

		/*
		* Advance destination pointers
		*/
		dp += dgap;
	}

	/*
	* Never remove temp arrays
	*/
	//free(sax);
	//free(say);

	return 0;
}

/**
 * Renders the buffer's contents onto the screen, applying
 * any necessary filters or conversions in the process.
 * If the scaling factor is bigger than 1, the entire contents
 * of the buffer are resized by that factor (eg. 2 = doubled)
 * before being put on screen.
 */
void Screen::flip()
{
	if (getWidth() != BASE_WIDTH || getHeight() != BASE_HEIGHT)
	{
		_zoomSurfaceY(_surface->getSurface(), _screen, 0, 0);
	}
	else
	{
		SDL_BlitSurface(_surface->getSurface(), 0, _screen, 0);
	}

	// perform any requested palette update
	if (_numColors)
	{
		if (SDL_SetColors(_screen, &(deferredPalette[_firstColor]), _firstColor, _numColors) == 0)
		{
			Log(LOG_ERROR) << "Display palette doesn't match requested palette";
		}
		_numColors = 0;
	}

	if (SDL_Flip(_screen) == -1)
	{
		throw Exception(SDL_GetError());
	}
}

/**
 * Clears all the contents out of the internal buffer.
 */
void Screen::clear()
{
	_surface->clear();
	SDL_Rect square;
	square.x = 0;
	square.y = 0;
	square.w = getWidth();
	square.h = getHeight();
	SDL_FillRect(_screen, &square, 0);
}

/**
 * Changes the 8bpp palette used to render the screen's contents.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void Screen::setPalette(SDL_Color* colors, int firstcolor, int ncolors)
{
	if (_numColors && (_numColors != ncolors) && (_firstColor != firstcolor))
	{
		// an initial palette setup has not been comitted to the screen yet
		// just update it with whatever colors are being sent now
		memcpy(&(deferredPalette[firstcolor]), colors, sizeof(SDL_Color)*ncolors);
		_numColors = 256; // all the use cases are just a full palette with 16-color follow-ups
		_firstColor = 0;
	} else
	{
		memcpy(&(deferredPalette[firstcolor]), colors, sizeof(SDL_Color) * ncolors);
		_numColors = ncolors;
		_firstColor = firstcolor;
	}

	_surface->setPalette(colors, firstcolor, ncolors);

	// defer actual update of screen until SDL_Flip()
	//if (SDL_SetColors(_screen, colors, firstcolor, ncolors) == 0)
	//{
	//	Log(LOG_ERROR) << "Display palette doesn't match requested palette";
	//}

	// Sanity check
	/*
	SDL_Color *newcolors = _screen->format->palette->colors;
	for (int i = firstcolor, j = 0; i < firstcolor + ncolors; i++, j++)
	{
		Log(LOG_DEBUG) << (int)newcolors[i].r << " - " << (int)newcolors[i].g << " - " << (int)newcolors[i].b;
		Log(LOG_DEBUG) << (int)colors[j].r << " + " << (int)colors[j].g << " + " << (int)colors[j].b;
		if (newcolors[i].r != colors[j].r ||
			newcolors[i].g != colors[j].g ||
			newcolors[i].b != colors[j].b)
		{
			Log(LOG_ERROR) << "Display palette doesn't match requested palette";
			break;
		}
	}
	*/
}

/**
 * Returns the screen's 8bpp palette.
 * @return Pointer to the palette's colors.
 */
SDL_Color *Screen::getPalette() const
{
	return _surface->getPalette();
}

/**
 * Returns the width of the screen.
 * @return Width in pixels.
 */
int Screen::getWidth() const
{
	return _screen->w;
}

/**
 * Returns the height of the screen.
 * @return Height in pixels
 */
int Screen::getHeight() const
{
	return _screen->h;
}

/**
 * Changes the screen's resolution. The display surface
 * and palette have to be reset for this to happen properly.
 * @param width Width in pixels.
 * @param height Height in pixels.
 */
void Screen::setResolution(int width, int height)
{
	_scaleX = width / BASE_WIDTH;
	_scaleY = height / BASE_HEIGHT;
	Log(LOG_INFO) << "Attempting to set display to " << width << "x" << height << "x" << _bpp << "...";
	_screen = SDL_SetVideoMode(width, height, _bpp, _flags);
	if (_screen == 0)
	{
		throw Exception(SDL_GetError());
	}
	Log(LOG_INFO) << "Display set to " << _screen->w << "x" << _screen->h << "x" << (int)_screen->format->BitsPerPixel << ".";
	setPalette(getPalette());
}

/**
 * Switches the screen between full-screen and/or windowed.
 * The screen has to be reset for this to happen properly.
 * @param full True for full-screen, False for windowed.
 */
void Screen::setFullscreen(bool full)
{
	if (full == _fullscreen)
		return;
	_fullscreen = full;
	if (_fullscreen)
	{
		_flags |= SDL_FULLSCREEN;
	}
	else
	{
		_flags &= ~SDL_FULLSCREEN;
	}
	Options::setBool("fullscreen", _fullscreen);
	setResolution(getWidth(), getHeight());
}

/**
 * Returns the screen's X scale.
 * @return Scale factor.
 */
double Screen::getXScale() const
{
	return _scaleX;
}

/**
 * Returns the screen's Y scale.
 * @return Scale factor.
 */
double Screen::getYScale() const
{
	return _scaleY;
}

}
