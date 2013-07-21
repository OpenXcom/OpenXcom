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

#include "Zoom.h"

//#include "Scalers/hq2x.hpp"

#include "Exception.h"
#include "Surface.h"
#include "Logger.h"
#include "Options.h"
#include "Screen.h"

#include "OpenGL.h"

// Scale2X
#include "Scalers/scalebit.h"

// HQX

#include "Scalers/common.h"
#include "Scalers/hqx.h"


#if (_MSC_VER >= 1400) || (defined(__MINGW32__) && defined(__SSE2__))

#ifndef __SSE2__
#define __SSE2__ true
#endif
// probably Visual Studio (or Intel C++ which should also work)
#include <intrin.h>
#endif

#ifdef __GNUC__
#ifndef __MORPHOS__
#include <cpuid.h>
#endif
#endif

#ifdef __SSE2__
#include <emmintrin.h> // for SSE2 intrinsics; see http://msdn.microsoft.com/en-us/library/has3d153%28v=vs.71%29.aspx
#endif



namespace OpenXcom
{


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


#if defined(__WORDSIZE) && (__WORDSIZE == 64) || defined(SIZE_MAX) && (SIZE_MAX > 0xFFFFFFFF)
#else
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
			dataSrc = SDL_Swap32(dataSrc);
			
			dataDst = SDL_Swap32( (dataSrc & 0xFF) | ((dataSrc & 0xFFFF) << 8) | 
				((dataSrc & 0xFF00) << 16)  );

			*pixelDst = dataDst;
			*pixelDst2 = dataDst;
			pixelDst++; // forward 4 bytes!
			pixelDst2++;

			dataSrc >>= 16; 

			dataDst = SDL_Swap32( (dataSrc & 0xFF) | ((dataSrc & 0xFFFF) << 8) | 
				((dataSrc & 0xFF00) << 16)  );

			*pixelDst = dataDst;
			*pixelDst2 = dataDst;
			pixelDst++; // forward 4 bytes!
			pixelDst2++;	
			#else
			
			dataDst = (dataSrc & 0xFF) | ((dataSrc & 0xFFFF) << 8) | 
				((dataSrc & 0xFF00) << 16) ;

			*pixelDst = dataDst;
			*pixelDst2 = dataDst;
			pixelDst++; // forward 4 bytes!
			pixelDst2++;

			dataSrc >>= 16; 

			dataDst = (dataSrc & 0xFF) | ((dataSrc & 0xFFFF) << 8) | 
				((dataSrc & 0xFF00) << 16) ;

			*pixelDst = dataDst;
			*pixelDst2 = dataDst;
			pixelDst++; // forward 4 bytes!
			pixelDst2++;	
			
			#endif
		}
		
	}
	
	return 0;
}
#endif

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
	Uint8 *pixelDstRow = (Uint8*)dst->pixels;
	int sx, sy;
	static bool proclaimed = false;
	
	if (!proclaimed)
	{
		proclaimed = true;
		Log(LOG_INFO) << "Using modestly fast 4X zoom routine.";
	}

	for (sy = 0; sy < src->h; ++sy, pixelDstRow += dst->pitch*4)
	{
		Uint8 *pixelDst = pixelDstRow;
	
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


#if defined(__WORDSIZE) && (__WORDSIZE == 64) || defined(SIZE_MAX) && (SIZE_MAX > 0xFFFFFFFF)
#else
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
			dataSrc = SDL_Swap32(dataSrc);
			
			for (int i = 0; i < 4; ++i)
			{
				dataDst = SDL_Swap32( (dataSrc & 0xFF) | ((dataSrc & 0xFF) << 8) | 
					((dataSrc & 0xFF) << 16) | ((dataSrc & 0xFF ) << 24) ); 
#else
			for (int i = 0; i < 4; ++i)
			{
				dataDst = (dataSrc & 0xFF) | ((dataSrc & 0xFF) << 8) | 
					((dataSrc & 0xFF) << 16) | ((dataSrc & 0xFF ) << 24); 

#endif

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
#endif

/**
 *  Optimized 8 bit zoomer for resizing by a factor of 4. Doesn't flip.
 *  32-bit version.
 *  Used internally by _zoomSurfaceY() below.
 *  source and dest. widths must be multiples of 4 bytes for 32-bit access
 */
static int zoomSurface2X_XAxis_32bit(SDL_Surface *src, SDL_Surface *dst)
{
	Uint32 dataSrc;
	Uint32 dataDst;
	Uint8 *pixelSrc;
	Uint8 *pixelDstRow = (Uint8*)dst->pixels;
	Uint8 *pixelSrcRow = (Uint8*)src->pixels;
	int sx;
	int dsty;
	static bool proclaimed = false;

	static Uint32 *say = 0;
	Uint32 *csay;
	int csy;
	
	if (!proclaimed)
	{
		proclaimed = true;
		Log(LOG_INFO) << "Using mediocre scaling routine due to screen height.";
	}
	
	if ((say = (Uint32 *) realloc(say, (dst->h + 1) * sizeof(Uint32))) == NULL) {
		say = 0;
		return (-1);
	}

	csy = 0;
	csay = say;
	for (int y = 0; y < dst->h; y++) {
		csy += src->h;
		*csay = 0;
		while (csy >= dst->h) {
			csy -= dst->h;
			(*csay)++;
		}
		(*csay) *= src->pitch;
		csay++;
	}
	
	for (dsty = 0; dsty < dst->h; ++dsty, pixelDstRow += dst->pitch)
	{
		if (!say[dsty]) continue;

		Uint32 *pixelDst = (Uint32*)pixelDstRow;
		pixelSrc = pixelSrcRow;
		pixelSrcRow += say[dsty];

		for (sx = 0; sx < src->w; sx += 4, pixelSrc += 4)
		{
			dataSrc = *((Uint32*) pixelSrc);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			// boo
			dataSrc = SDL_Swap32(dataSrc);
			
			for (int i = 0; i < 2; ++i)
			{
				dataDst = SDL_Swap32( (dataSrc & 0xFF) | ((dataSrc & 0xFFFF) << 8) | 
					((dataSrc & 0xFF00) << 16) );
			
#else
			for (int i = 0; i < 2; ++i)
			{
				dataDst = (dataSrc & 0xFF) | ((dataSrc & 0xFFFF) << 8) | 
					((dataSrc & 0xFF00) << 16);

#endif

				int j = 0;
				do
				{
					if (dsty + j >= dst->h) break;

					*(pixelDst + (dst->pitch/sizeof(Uint32))*j) = dataDst;
				} while(say[dsty + ++j] == 0); // fill in all relevant rows
				
				dataSrc >>= 16;
				pixelDst++; // forward 4 bytes!
			}
		}
	}
	
	return 0;
}


/**
 *  Optimized 8 bit zoomer for resizing by a factor of 2. Doesn't flip.
 *  32-bit version.
 *  Used internally by _zoomSurfaceY() below.
 *  source and dest. widths must be multiples of 4 bytes for 32-bit access
 */
static int zoomSurface4X_XAxis_32bit(SDL_Surface *src, SDL_Surface *dst)
{
	Uint32 dataSrc;
	Uint32 dataDst;
	Uint8 *pixelSrc;
	Uint8 *pixelDstRow = (Uint8*)dst->pixels;
	Uint8 *pixelSrcRow = (Uint8*)src->pixels;
	int sx;
	int dsty;
	static bool proclaimed = false;

	static Uint32 *say = 0;
	Uint32 *csay;
	int csy;
	
	if (!proclaimed)
	{
		proclaimed = true;
		Log(LOG_INFO) << "Using mediocre scaling routine due to screen height.";
	}
	
	if ((say = (Uint32 *) realloc(say, (dst->h + 1) * sizeof(Uint32))) == NULL) {
		say = 0;
		return (-1);
	}
	
	csy = 0;
	csay = say;
	for (int y = 0; y < dst->h; y++) {
		csy += src->h;
		*csay = 0;
		while (csy >= dst->h) {
			csy -= dst->h;
			(*csay)++;
		}
		(*csay) *= src->pitch;
		csay++;
	}

	for (dsty = 0; dsty < dst->h; ++dsty, pixelDstRow += dst->pitch)
	{
		if (!say[dsty]) continue;

		Uint32 *pixelDst = (Uint32*)pixelDstRow;
		pixelSrc = pixelSrcRow;
		pixelSrcRow += say[dsty];

		for (sx = 0; sx < src->w; sx += 4, pixelSrc += 4)
		{
			dataSrc = *((Uint32*) pixelSrc);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			// boo
			dataSrc = SDL_Swap32(dataSrc);
			
			for (int i = 0; i < 4; ++i)
			{
				dataDst = SDL_Swap32( (dataSrc & 0xFF) | ((dataSrc & 0xFF) << 8) | 
					((dataSrc & 0xFF) << 16) | ((dataSrc & 0xFF ) << 24) ); 
			
#else
			for (int i = 0; i < 4; ++i)
			{
				dataDst = (dataSrc & 0xFF) | ((dataSrc & 0xFF) << 8) | 
					((dataSrc & 0xFF) << 16) | ((dataSrc & 0xFF ) << 24); 
#endif
				int j = 0;
				do
				{
					if (dsty + j >= dst->h) break;

					*(pixelDst + (dst->pitch/sizeof(Uint32))*j) = dataDst;
				} while(say[dsty + ++j] == 0); // fill in all relevant rows
				
				dataSrc >>= 8;
				pixelDst++; // forward 4 bytes!
			}
		}
	}
	
	return 0;
}

#ifdef __SSE2__
/**
 *  Optimized 8 bit zoomer for resizing by a factor of 4. Doesn't flip.
 *  Used internally by _zoomSurfaceY() below.
 *	This is an SSE2 version written with Intel intrinsics.
 *  source and dest. widths must be multiples of 16 bytes for 128-bit access
 *  and it would help if they were aligned properly... :(
 */
static int zoomSurface4X_SSE2(SDL_Surface *src, SDL_Surface *dst)
{
	__m128i dataSrc;
	__m128i dataDst;
	Uint8 *pixelSrc = (Uint8*)src->pixels;
	Uint8 *pixelDstRow = (Uint8*)dst->pixels;
	int sx, sy;
	static bool proclaimed = false;

	if (!proclaimed)
	{
		proclaimed = true;
		Log(LOG_INFO) << "Using SSE2 4X zoom routine.";
	}

	for (sy = 0; sy < src->h; ++sy, pixelDstRow += dst->pitch*4)
	{
		__m128i *pixelDst =  (__m128i*)pixelDstRow;
		__m128i *pixelDst2 = (__m128i*)((Uint8*)pixelDstRow + dst->pitch);
		__m128i *pixelDst3 = (__m128i*)((Uint8*)pixelDstRow + dst->pitch*2);
		__m128i *pixelDst4 = (__m128i*)((Uint8*)pixelDstRow + dst->pitch*3);
		for (sx = 0; sx < src->w; sx += 16, pixelSrc += 16)
		{
			dataSrc = *((__m128i*) pixelSrc);

			__m128i halfDone = _mm_unpacklo_epi8(dataSrc, dataSrc); 
			dataDst = _mm_unpacklo_epi8(halfDone, halfDone);

/* #define WRITE_DST if ((char*)pixelDst4 + 128 > (char*)dst->pixels+(dst->w*dst->pitch)) { Log(LOG_ERROR) << "HELL"; exit(0); } \ */
#define WRITE_DST			*(pixelDst++) = dataDst; \
			*(pixelDst2++) = dataDst; \
			*(pixelDst3++) = dataDst; \
			*(pixelDst4++) = dataDst; \
			
			WRITE_DST;
			
			dataDst = _mm_unpackhi_epi8(halfDone, halfDone);
			
			WRITE_DST;
			
			halfDone = _mm_unpackhi_epi8(dataSrc, dataSrc);
			dataDst = _mm_unpacklo_epi8(halfDone, halfDone);
			
			WRITE_DST;
			
			dataDst = _mm_unpackhi_epi8(halfDone, halfDone);
			
			WRITE_DST;
		}	
	}

	return 0;
}

/**
 *  Optimized 8 bit zoomer for resizing by a factor of 2. Doesn't flip.
 *  Used internally by _zoomSurfaceY() below.
 *	This is an SSE2 version written with Intel intrinsics.
 *  source and dest. widths must be multiples of 16 bytes for 128-bit access
 *  and it would help if they were aligned properly... :(
 */
static int zoomSurface2X_SSE2(SDL_Surface *src, SDL_Surface *dst)
{
	__m128i dataSrc;
	__m128i dataDst;
	Uint8 *pixelSrc = (Uint8*)src->pixels;
	Uint8 *pixelDstRow = (Uint8*)dst->pixels;
	int sx, sy;
	static bool proclaimed = false;
	
	if (!proclaimed)
	{
		proclaimed = true;
		Log(LOG_INFO) << "Using SSE2 2X zoom routine.";
	}

	for (sy = 0; sy < src->h; ++sy, pixelDstRow += dst->pitch*2)
	{
		__m128i *pixelDst =  (__m128i*)pixelDstRow;
		__m128i *pixelDst2 = (__m128i*)((Uint8*)pixelDstRow + dst->pitch);

		for (sx = 0; sx < src->w; sx += 16, pixelSrc += 16)
		{
			dataSrc = *((__m128i*) pixelSrc);

			dataDst = _mm_unpacklo_epi8(dataSrc, dataSrc); 

#undef WRITE_DST
#define WRITE_DST			*(pixelDst++) = dataDst; \
			*(pixelDst2++) = dataDst; \
			
			WRITE_DST;
			
			dataDst = _mm_unpackhi_epi8(dataSrc, dataSrc);
			
			WRITE_DST;
		}
	}
	
	return 0;
}

/** Checks the SSE2 feature bit returned by the CPUID instruction
 */
bool Zoom::haveSSE2()
{
#ifdef __GNUC__
	unsigned int CPUInfo[4];
	__get_cpuid(1, CPUInfo, CPUInfo+1, CPUInfo+2, CPUInfo+3);
#elif _WIN32
	int CPUInfo[4];
	__cpuid(CPUInfo, 1);
#else
	return false;
#endif

	return (CPUInfo[3] & 0x04000000) ? true : false;
}

#endif

/**
 * Wrapper around various software and OpenGL screen buffer pushing functions which zoom.
 * Basically called just from Screen::flip()
 */
void Zoom::flipWithZoom(SDL_Surface *src, SDL_Surface *dst, OpenGL *glOut)
{
	if (Screen::isOpenGLEnabled() && glOut->buffer_surface)
	{
		SDL_BlitSurface(src, 0, glOut->buffer_surface->getSurface(), 0); // TODO; this is less than ideal...

		glOut->refresh(glOut->linear, glOut->iwidth, glOut->iheight, dst->w, dst->h);
		SDL_GL_SwapBuffers();
	} else
	{
		_zoomSurfaceY(src, dst, 0, 0);
	}
}


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
int Zoom::_zoomSurfaceY(SDL_Surface * src, SDL_Surface * dst, int flipx, int flipy)
{
	int x, y;
	static Uint32 *sax, *say;
	Uint32 *csax, *csay;
	int csx, csy;
	Uint8 *sp, *dp, *csp;
	int dgap;
	static bool proclaimed = false;

	if (Options::getBool("useHQXFilter"))
	{
		static bool initDone = false;

		if (!initDone)
		{
			hqxInit();
			initDone = true;
		}

		// HQX_API void HQX_CALLCONV hq2x_32_rb( uint32_t * src, uint32_t src_rowBytes, uint32_t * dest, uint32_t dest_rowBytes, int width, int height );

		if (dst->w == src->w * 2 && dst->h == src->h * 2)
		{
			hq2x_32_rb((uint32_t*) src->pixels, src->pitch, (uint32_t*) dst->pixels, dst->pitch, src->w, src->h);
			return 0;
		}

		if (dst->w == src->w * 3 && dst->h == src->h * 3)
		{
			hq3x_32_rb((uint32_t*) src->pixels, src->pitch, (uint32_t*) dst->pixels, dst->pitch, src->w, src->h);
			return 0;
		}

		if (dst->w == src->w * 4 && dst->h == src->h * 4)
		{
			hq4x_32_rb((uint32_t*) src->pixels, src->pitch, (uint32_t*) dst->pixels, dst->pitch, src->w, src->h);
			return 0;
		}

	}

	if (Options::getBool("useScaleFilter"))
	{
		// check the resolution to see which of scale2x, scale3x, etc. we need

		if (dst->w == src->w * 2 && dst->h == src->h *2 && !scale_precondition(2, src->format->BytesPerPixel, src->w, src->h))
		{
			scale(2, dst->pixels, dst->pitch, src->pixels, src->pitch, src->format->BytesPerPixel, src->w, src->h);
			return 0;
		}

		if (dst->w == src->w * 3 && dst->h == src->h *3 && !scale_precondition(3, src->format->BytesPerPixel, src->w, src->h))
		{
			scale(3, dst->pixels, dst->pitch, src->pixels, src->pitch, src->format->BytesPerPixel, src->w, src->h);
			return 0;
		}

		if (dst->w == src->w * 4 && dst->h == src->h *4 && !scale_precondition(4, src->format->BytesPerPixel, src->w, src->h))
		{
			scale(4, dst->pixels, dst->pitch, src->pixels, src->pitch, src->format->BytesPerPixel, src->w, src->h);
			return 0;
		}

	}

	// if we're scaling by a factor of 2 or 4, try to use a more efficient function	

	if (src->format->BytesPerPixel == 1 && dst->format->BytesPerPixel == 1)
	{

#ifdef __SSE2__
		static bool _haveSSE2 = haveSSE2();

		if (_haveSSE2 &&
			!((ptrdiff_t)src->pixels % 16) && 
			!((ptrdiff_t)dst->pixels % 16)) // alignment check
		{
			if (dst->w == src->w * 2 && dst->h == src->h * 2) return  zoomSurface2X_SSE2(src, dst);
			else if (dst->w == src->w * 4 && dst->h == src->h * 4) return  zoomSurface4X_SSE2(src, dst);
		} else
		{
			static bool complained = false;

			if (!complained)
			{
				Log(LOG_ERROR) << "Misaligned surface buffers.";
			}
		}
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

		// maybe X is scaled by 2 or 4 but not Y?
		if (dst->w == src->w * 4) return zoomSurface4X_XAxis_32bit(src, dst);
		else if (dst->w == src->w * 2) return zoomSurface2X_XAxis_32bit(src, dst);
	}

	if (!proclaimed)
	{
		Log(LOG_INFO) << "Using slower scaling routine. For best results, try a resolution of 640x400 or 1280x800.";
		proclaimed = true;
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


}

