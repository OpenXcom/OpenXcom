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
#include "Screen.h"
#include <sstream>
#include <iomanip>
#include <limits.h>
#include "../lodepng.h"
#include "Exception.h"
#include "Surface.h"
#include "Logger.h"
#include "Action.h"
#include "Options.h"
#include "CrossPlatform.h"
#include "Zoom.h"
#include "OpenGL.h"
#include "Timer.h"
#include <SDL.h>

namespace OpenXcom
{

int Screen::BASE_WIDTH = 320;
int Screen::BASE_HEIGHT = 200;

/// Sets the _flags and _bpp variables based on game options; needed in more than one place now
void Screen::makeVideoFlags()
{
	_flags = SDL_SWSURFACE|SDL_HWPALETTE;
	if (Options::getBool("asyncBlit")) _flags |= SDL_ASYNCBLIT;
	if (isOpenGLEnabled()) _flags = SDL_OPENGL;
	if (Options::getBool("allowResize")) _flags |= SDL_RESIZABLE;
	if (Options::getBool("borderless")) _flags |= SDL_NOFRAME;
	if (_fullscreen)
	{
		_flags |= SDL_FULLSCREEN;
	}

	_bpp = (Screen::isHQXEnabled() || Screen::isOpenGLEnabled()) ? 32 : 8;
}


/**
 * Initializes a new display screen for the game to render contents to.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param bpp Bits-per-pixel.
 * @param fullscreen Fullscreen mode.
 * @warning Currently the game is designed for 8bpp, so there's no telling what'll
 * happen if you use a different value.
 */
Screen::Screen(int width, int height, int bpp, bool fullscreen, int windowedModePositionX, int windowedModePositionY) : _bpp(bpp), _scaleX(1.0), _scaleY(1.0), _fullscreen(fullscreen), _numColors(0), _firstColor(0), _surface(0)
{
	char *prev = (char*)"";
	if (!_fullscreen && (windowedModePositionX != -1 || windowedModePositionY != -1))
	{
		prev = SDL_getenv("SDL_VIDEO_WINDOW_POS");

		if (0 == prev) prev = (char*)"";
		std::stringstream ss;
		ss << "SDL_VIDEO_WINDOW_POS=" << std::dec << windowedModePositionX << "," << windowedModePositionY;
		
		SDL_putenv(const_cast<char*>(ss.str().c_str()));
	}
	setResolution(width, height);
	if (!_fullscreen  && (windowedModePositionX != -1 || windowedModePositionY != -1))
	{ // We don't want to put the window back to the starting position later when the window is resized.
		std::stringstream ss;
		ss << "SDL_VIDEO_WINDOW_POS=" << prev;

		SDL_putenv(const_cast<char*>(ss.str().c_str()));
	}
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
Surface *Screen::getSurface()
{
	_pushPalette = true;
	return _surface;
}

/**
 * Handles screen key shortcuts.
 * @param action Pointer to an action.
 */
void Screen::handle(Action *action)
{
	if (Options::getBool("debug"))
	{
		if (action->getDetails()->type == SDL_KEYDOWN && action->getDetails()->key.keysym.sym == SDLK_F8)
		{
			switch(Timer::gameSlowSpeed)
			{
				case 1: Timer::gameSlowSpeed = 5; break;
				case 5: Timer::gameSlowSpeed = 15; break;
				default: Timer::gameSlowSpeed = 1; break;
			}				
		}
	}
	
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
	if (getWidth() != BASE_WIDTH || getHeight() != BASE_HEIGHT || isOpenGLEnabled())
	{
		Zoom::flipWithZoom(_surface->getSurface(), _screen, &glOutput);
	}
	else
	{
		SDL_BlitSurface(_surface->getSurface(), 0, _screen, 0);
	}

	// perform any requested palette update
	if (_pushPalette && _numColors && _screen->format->BitsPerPixel == 8)
	{
		if (_screen->format->BitsPerPixel == 8 && SDL_SetColors(_screen, &(deferredPalette[_firstColor]), _firstColor, _numColors) == 0)
		{
			Log(LOG_DEBUG) << "Display palette doesn't match requested palette";
		}
		_numColors = 0;
		_pushPalette = false;
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
}

/**
 * Changes the 8bpp palette used to render the screen's contents.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void Screen::setPalette(SDL_Color* colors, int firstcolor, int ncolors, bool immediately)
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
	if (immediately && _screen->format->BitsPerPixel == 8 && SDL_SetColors(_screen, colors, firstcolor, ncolors) == 0)
	{
		Log(LOG_DEBUG) << "Display palette doesn't match requested palette";
	}

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
	makeVideoFlags();

	if (!_surface || (_surface && 
		(_surface->getSurface()->format->BitsPerPixel != _bpp || 
		_surface->getSurface()->w != BASE_WIDTH ||
		_surface->getSurface()->h != BASE_HEIGHT))) // don't reallocate _surface if not necessary, it's a waste of CPU cycles
	{
		if (_surface) delete _surface;
		_surface = new Surface((int)BASE_WIDTH, (int)BASE_HEIGHT, 0, 0, Screen::isHQXEnabled() ? 32 : 8); // only HQX needs 32bpp for this surface; the OpenGL class has its own 32bpp buffer
		if (_surface->getSurface()->format->BitsPerPixel == 8) _surface->setPalette(deferredPalette);
	}
	SDL_SetColorKey(_surface->getSurface(), 0, 0); // turn off color key! 

	Log(LOG_INFO) << "Attempting to set display to " << width << "x" << height << "x" << _bpp << "...";
	_screen = SDL_SetVideoMode(width, height, _bpp, _flags);
	if (_screen == 0)
	{
		throw Exception(SDL_GetError());
	}
	_scaleX = getWidth() / (double)BASE_WIDTH;
	_scaleY = getHeight() / (double)BASE_HEIGHT;

	if (isOpenGLEnabled()) 
	{
		glOutput.init(BASE_WIDTH, BASE_HEIGHT);
		glOutput.linear = Options::getBool("useOpenGLSmoothing"); // setting from shader file will override this, though
		glOutput.set_shader(CrossPlatform::getDataFile(Options::getString("useOpenGLShader")).c_str());
		glOutput.setVSync(Options::getBool("vSyncForOpenGL"));
		OpenGL::checkErrors = Options::getBool("checkOpenGLErrors");
	}

	Log(LOG_INFO) << "Display set to " << getWidth() << "x" << getHeight() << "x" << (int)_screen->format->BitsPerPixel << ".";
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
	SDL_Surface *screenshot = SDL_AllocSurface(0, getWidth(), getHeight(), 24, 0xff, 0xff00, 0xff0000, 0);


	if (isOpenGLEnabled())
	{
		GLenum format = GL_RGB;

		#ifndef __NO_OPENGL

		for (int y = 0; y < getHeight(); ++y)
		{
			glReadPixels(0, getHeight()-(y+1), getWidth(), 1, format, GL_UNSIGNED_BYTE, ((Uint8*)screenshot->pixels) + y*screenshot->pitch);
		}
		glErrorCheck();
		
		#endif
	} else
	{
		SDL_BlitSurface(_screen, 0, screenshot, 0);
	}

	unsigned error = lodepng::encode(filename, (const unsigned char *)(screenshot->pixels), getWidth(), getHeight(), LCT_RGB);
	if (error)
	{
		Log(LOG_ERROR) << "Saving to PNG failed: " << lodepng_error_text(error);
	}

	SDL_FreeSurface(screenshot);
}


/** Check whether useHQXFilter is set in Options and a compatible resolution
 *  has been selected.
 */
bool Screen::isHQXEnabled()
{
	int w = Options::getInt("displayWidth");
	int h = Options::getInt("displayHeight");

	if (Options::getBool("useHQXFilter") && (
		(w == Screen::BASE_WIDTH * 2 && h == Screen::BASE_HEIGHT * 2) || 
		(w == Screen::BASE_WIDTH * 3 && h == Screen::BASE_HEIGHT * 3) || 
		(w == Screen::BASE_WIDTH * 4 && h == Screen::BASE_HEIGHT * 4)))
	{
		return true;
	}

	return false;
}

/**
 * Check if openGl is enabled.
 * @return if it is enabled.
 */
bool Screen::isOpenGLEnabled()
{
#ifdef __NO_OPENGL
	return false;
#else
	return Options::getBool("useOpenGL");
#endif
}

/**
 * Gets the Horizontal offset from the mid-point of the screen, in pixels.
 * @return the horizontal offset.
 */
int Screen::getDX()
{
	return (BASE_WIDTH - 320) / 2;
}

/**
 * Gets the Vertical offset from the mid-point of the screen, in pixels.
 * @return the vertical offset.
 */
int Screen::getDY()
{
	return (BASE_HEIGHT - 200) / 2;
}

}
