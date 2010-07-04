/*
 * Copyright 2010 Daniel Albano
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
#include "NewBaseState.h"

/**
 * Initializes all the elements in the New Base screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to place.
 * @param globe Pointer to the Geoscape globe.
 * @param first Is this the first base in the game?
 */
NewBaseState::NewBaseState(Game *game, Base *base, Globe *globe, bool first) : State(game), _base(base), _globe(globe), _rotLon(0), _rotLat(0), _first(first)
{
	_screen = false;

	// Create objects
	_btnRotateLeft = new InteractiveSurface(12, 10, 259, 176);
	_btnRotateRight = new InteractiveSurface(12, 10, 283, 176);
	_btnRotateUp = new InteractiveSurface(13, 12, 271, 162);
	_btnRotateDown = new InteractiveSurface(13, 12, 271, 187);
	_btnZoomIn = new InteractiveSurface(23, 23, 295, 156);
	_btnZoomOut = new InteractiveSurface(13, 17, 300, 182);

	_window = new Window(256, 28, 0, 0);
	_btnCancel = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 54, 12, 186, 8);
	_txtTitle = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 200, 9, 8, 10);

	_rotTimer = new Timer(25);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_0")->getColors());
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);
	
	add(_btnRotateLeft);
	add(_btnRotateRight);
	add(_btnRotateUp);
	add(_btnRotateDown);
	add(_btnZoomIn);
	add(_btnZoomOut);

	add(_window);
	add(_btnCancel);
	add(_txtTitle);
	
	// Set up objects
	_globe->onMouseClick((EventHandler)&NewBaseState::globeClick);

	_btnRotateLeft->onMousePress((EventHandler)&NewBaseState::btnRotateLeftPress);
	_btnRotateLeft->onMouseRelease((EventHandler)&NewBaseState::btnRotateLeftRelease);

	_btnRotateRight->onMousePress((EventHandler)&NewBaseState::btnRotateRightPress);
	_btnRotateRight->onMouseRelease((EventHandler)&NewBaseState::btnRotateRightRelease);

	_btnRotateUp->onMousePress((EventHandler)&NewBaseState::btnRotateUpPress);
	_btnRotateUp->onMouseRelease((EventHandler)&NewBaseState::btnRotateUpRelease);

	_btnRotateDown->onMousePress((EventHandler)&NewBaseState::btnRotateDownPress);
	_btnRotateDown->onMouseRelease((EventHandler)&NewBaseState::btnRotateDownRelease);

	_btnZoomIn->onMouseClick((EventHandler)&NewBaseState::btnZoomInClick);

	_btnZoomOut->onMouseClick((EventHandler)&NewBaseState::btnZoomOutClick);

	_window->setColor(Palette::blockOffset(15)+2);
	_window->setBackground(game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnCancel->setColor(Palette::blockOffset(15)+2);
	_btnCancel->setText(_game->getResourcePack()->getLanguage()->getString(STR_CANCEL_UC));
	_btnCancel->onMouseClick((EventHandler)&NewBaseState::btnCancelClick);

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setText(_game->getResourcePack()->getLanguage()->getString(STR_SELECT_SITE_FOR_NEW_BASE));

	_rotTimer->onTimer((TimerHandler)&NewBaseState::globeRotate);

	if (_first)
	{
		_btnCancel->setVisible(false);
	}
}

/**
 *
 */
NewBaseState::~NewBaseState()
{
	
}

/**
 * Updates the timer display and resets the palette
 * since it's bound to change on other screens.
 */
void NewBaseState::init()
{
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_0")->getColors());
}

/**
 * Runs the globe rotation timer.
 */
void NewBaseState::think()
{
	_rotTimer->think(this);
}

/**
 * Handles the globe.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 */
void NewBaseState::handle(SDL_Event *ev, int scale)
{
	State::handle(ev, scale);
	_globe->handle(ev, scale, this);
}

/**
 * Rotates the globe by a set amount. Necessary
 * since the globe keeps rotating while the button
 * is pressed down.
 */
void NewBaseState::globeRotate()
{
	_globe->rotate(_rotLon, _rotLat);
}

/**
 * Processes any left-clicks on globe markers,
 * or right-clicks to scroll the globe.
 * @note Currently left-clicking is used to move around
 * a marker for testing globe functionality.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void NewBaseState::globeClick(SDL_Event *ev, int scale)
{
	double lon, lat;
	_globe->cartToPolar(ev->button.x / scale, ev->button.y / scale, &lon, &lat);
	
	if (ev->button.button == SDL_BUTTON_LEFT)
	{
		if (_globe->insideLand(lon, lat))
		{
			_base->setLongitude(lon);
			_base->setLatitude(lat);
			_game->pushState(new BaseNameState(_game, _base, _first));
		}
	}
	else if (ev->button.button == SDL_BUTTON_RIGHT)
	{
		_globe->center(lon, lat);
	}
}

/**
 * Starts rotating the globe to the left.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void NewBaseState::btnRotateLeftPress(SDL_Event *ev, int scale)
{
	_rotLon += LONGITUDE_SPEED;
	_rotTimer->start();
	globeRotate();
}

/**
 * Stops rotating the globe to the left.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void NewBaseState::btnRotateLeftRelease(SDL_Event *ev, int scale)
{
	_rotLon -= LONGITUDE_SPEED;
	_rotTimer->stop();
}

/**
 * Starts rotating the globe to the right.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void NewBaseState::btnRotateRightPress(SDL_Event *ev, int scale)
{
	_rotLon += -LONGITUDE_SPEED;
	_rotTimer->start();
	globeRotate();
}

/**
 * Stops rotating the globe to the right.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void NewBaseState::btnRotateRightRelease(SDL_Event *ev, int scale)
{
	_rotLon -= -LONGITUDE_SPEED;
	_rotTimer->stop();
}

/**
 * Starts rotating the globe upwards.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void NewBaseState::btnRotateUpPress(SDL_Event *ev, int scale)
{
	_rotLat += LATITUDE_SPEED;
	_rotTimer->start();
	globeRotate();
}

/**
 * Stops rotating the globe upwards.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void NewBaseState::btnRotateUpRelease(SDL_Event *ev, int scale)
{
	_rotLat -= LATITUDE_SPEED;
	_rotTimer->stop();
}

/**
 * Starts rotating the globe downwards.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void NewBaseState::btnRotateDownPress(SDL_Event *ev, int scale)
{
	_rotLat += -LATITUDE_SPEED;
	_rotTimer->start();
	globeRotate();
}

/**
 * Stops rotating the globe downwards.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void NewBaseState::btnRotateDownRelease(SDL_Event *ev, int scale)
{
	_rotLat -= -LATITUDE_SPEED;
	_rotTimer->stop();
}

/**
 * Zooms into the globe.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void NewBaseState::btnZoomInClick(SDL_Event *ev, int scale)
{
	_globe->zoom(1);
}

/**
 * Zooms out of the globe.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void NewBaseState::btnZoomOutClick(SDL_Event *ev, int scale)
{
	_globe->zoom(-1);
}

/**
 * Returns to the previous screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void NewBaseState::btnCancelClick(SDL_Event *ev, int scale)
{
	_game->popState();
}