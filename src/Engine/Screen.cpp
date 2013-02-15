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
#include <SDL_endian.h>
#include <limits.h>
#include "../lodepng.h"
#include "Exception.h"
#include "Surface.h"
#include "Logger.h"
#include "Action.h"
#include "Options.h"
#include "CrossPlatform.h"
#include "Zoom.h"


namespace OpenXcom
{

int Screen::BASE_WIDTH = 320;
int Screen::BASE_HEIGHT = 200;

/**
 * Initializes a new display screen for the game to render contents to.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param bpp Bits-per-pixel.
 * @param fullscreen Fullscreen mode.
 * @warning Currently the game is designed for 8bpp, so there's no telling what'll
 * happen if you use a different value.
 */
Screen::Screen(int width, int height, int bpp, bool fullscreen) : _bpp(bpp), _scaleX(1.0), _scaleY(1.0), _fullscreen(fullscreen), _numColors(0), _firstColor(0)
{
	Screen::BASE_WIDTH = Options::getInt("baseXResolution");
	Screen::BASE_HEIGHT = Options::getInt("baseYResolution");
	_surface = new Surface((int)BASE_WIDTH, (int)BASE_HEIGHT, 0, 0, bpp);
	_flags = SDL_SWSURFACE|SDL_HWPALETTE;
	if (Options::getBool("asyncBlit")) _flags |= SDL_ASYNCBLIT;
	if (_fullscreen)
	{
		_flags |= SDL_FULLSCREEN;
	}
	setResolution(width, height);
	memset(deferredPalette, 0, 256*sizeof(SDL_Color));
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
		screenshot(ss.str());
		return;
	}
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
		Zoom::_zoomSurfaceY(_surface->getSurface(), _screen, 0, 0);
	}
	else
	{
		SDL_BlitSurface(_surface->getSurface(), 0, _screen, 0);
	}

	// perform any requested palette update
	if (_numColors && _screen->format->BitsPerPixel == 8)
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
		memmove(&(deferredPalette[firstcolor]), colors, sizeof(SDL_Color)*ncolors);
		_numColors = 256; // all the use cases are just a full palette with 16-color follow-ups
		_firstColor = 0;
	} else
	{
		memmove(&(deferredPalette[firstcolor]), colors, sizeof(SDL_Color) * ncolors);
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
	return (SDL_Color*)deferredPalette;
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
	_scaleX = width / (double)BASE_WIDTH;
	_scaleY = height / (double)BASE_HEIGHT;
	Log(LOG_INFO) << "Attempting to set display to " << width << "x" << height << "x" << _bpp << "...";
	_screen = SDL_SetVideoMode(width, height, _bpp, _flags);
	if (_screen == 0)
	{
		throw Exception(SDL_GetError());
	}

	Log(LOG_INFO) << "Display set to " << _screen->w << "x" << _screen->h << "x" << (int)_screen->format->BitsPerPixel << ".";
	if (_screen->format->BitsPerPixel == 8)
	{
		setPalette(getPalette());
	}
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

/**
 * Saves a screenshot of the screen's contents.
 * @param filename Filename of the PNG file.
 */
void Screen::screenshot(const std::string &filename) const
{
	std::vector<unsigned char> image;
	SDL_Color *palette = getPalette();

	for (int y = 0; y < getHeight(); ++y)
	{
		for (int x = 0; x < getWidth(); ++x)
		{
			switch(_screen->format->BytesPerPixel)
			{
				Uint8 color;
				Uint32 colors;
			case 1:
				color = ((Uint8 *)_screen->pixels)[y * _screen->pitch + x * _screen->format->BytesPerPixel];
				image.push_back(palette[color].r);
				image.push_back(palette[color].g);
				image.push_back(palette[color].b);
				break;
			case 2:
			case 3:
			case 4:
				colors = *(Uint32*)(((Uint8 *)_screen->pixels) + y * _screen->pitch + x * _screen->format->BytesPerPixel);
				image.push_back((colors & _screen->format->Rmask) >> _screen->format->Rshift);
				image.push_back((colors & _screen->format->Gmask) >> _screen->format->Gshift);
				image.push_back((colors & _screen->format->Bmask) >> _screen->format->Bshift);
				break;
			default:
				return; // not likely
			}
		}
	}

	unsigned error = lodepng::encode(filename, image, getWidth(), getHeight(), LCT_RGB);
	if (error)
	{
		Log(LOG_ERROR) << "Saving to PNG failed: " << lodepng_error_text(error);
	}
}

}
