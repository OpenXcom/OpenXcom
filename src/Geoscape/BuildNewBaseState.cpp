/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include "BuildNewBaseState.h"
#include <cmath>
#include "../fmath.h"
#include "../Engine/Game.h"
#include "../Engine/Action.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Engine/Timer.h"
#include "../Engine/Screen.h"
#include "../Interface/Window.h"
#include "Globe.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Savegame/Base.h"
#include "../Savegame/Craft.h"
#include "BaseNameState.h"
#include "ConfirmNewBaseState.h"
#include "../Engine/Options.h"
#include "../Menu/ErrorMessageState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Build New Base window.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to place.
 * @param globe Pointer to the Geoscape globe.
 * @param first Is this the first base in the game?
 */
BuildNewBaseState::BuildNewBaseState(Base *base, Globe *globe, bool first) : _base(base), _globe(globe), _first(first), _oldlat(0), _oldlon(0), _mousex(0), _mousey(0)
{
	int dx = _game->getScreen()->getDX();
	int dy = _game->getScreen()->getDY();
	_screen = false;

	_oldshowradar = Options::globeRadarLines;
	if (!_oldshowradar)
		Options::globeRadarLines = true;
	// Create objects
	_btnRotateLeft = new InteractiveSurface(12, 10, 259 + dx * 2, 176 + dy);
	_btnRotateRight = new InteractiveSurface(12, 10, 283 + dx * 2, 176 + dy);
	_btnRotateUp = new InteractiveSurface(13, 12, 271 + dx * 2, 162 + dy);
	_btnRotateDown = new InteractiveSurface(13, 12, 271 + dx * 2, 187 + dy);
	_btnZoomIn = new InteractiveSurface(23, 23, 295 + dx * 2, 156 + dy);
	_btnZoomOut = new InteractiveSurface(13, 17, 300 + dx * 2, 182 + dy);

	_window = new Window(this, 256, 28, 0, 0);
	_window->setX(dx);
	_window->setDY(0);
	_btnCancel = new TextButton(54, 12, 186 + dx, 8);
	_txtTitle = new Text(180, 16, 8 + dx, 6);

	_hoverTimer = new Timer(50);
	_hoverTimer->onTimer((StateHandler)&BuildNewBaseState::hoverRedraw);
	_hoverTimer->start();
	
	// Set palette
	setInterface("geoscape");

	add(_btnRotateLeft);
	add(_btnRotateRight);
	add(_btnRotateUp);
	add(_btnRotateDown);
	add(_btnZoomIn);
	add(_btnZoomOut);

	add(_window, "genericWindow", "geoscape");
	add(_btnCancel, "genericButton2", "geoscape");
	add(_txtTitle, "genericText", "geoscape");

	// Set up objects
	_globe->onMouseClick((ActionHandler)&BuildNewBaseState::globeClick);

	_btnRotateLeft->onMousePress((ActionHandler)&BuildNewBaseState::btnRotateLeftPress);
	_btnRotateLeft->onMouseRelease((ActionHandler)&BuildNewBaseState::btnRotateLeftRelease);
	_btnRotateLeft->onKeyboardPress((ActionHandler)&BuildNewBaseState::btnRotateLeftPress, Options::keyGeoLeft);
	_btnRotateLeft->onKeyboardRelease((ActionHandler)&BuildNewBaseState::btnRotateLeftRelease, Options::keyGeoLeft);

	_btnRotateRight->onMousePress((ActionHandler)&BuildNewBaseState::btnRotateRightPress);
	_btnRotateRight->onMouseRelease((ActionHandler)&BuildNewBaseState::btnRotateRightRelease);
	_btnRotateRight->onKeyboardPress((ActionHandler)&BuildNewBaseState::btnRotateRightPress, Options::keyGeoRight);
	_btnRotateRight->onKeyboardRelease((ActionHandler)&BuildNewBaseState::btnRotateRightRelease, Options::keyGeoRight);

	_btnRotateUp->onMousePress((ActionHandler)&BuildNewBaseState::btnRotateUpPress);
	_btnRotateUp->onMouseRelease((ActionHandler)&BuildNewBaseState::btnRotateUpRelease);
	_btnRotateUp->onKeyboardPress((ActionHandler)&BuildNewBaseState::btnRotateUpPress, Options::keyGeoUp);
	_btnRotateUp->onKeyboardRelease((ActionHandler)&BuildNewBaseState::btnRotateUpRelease, Options::keyGeoUp);

	_btnRotateDown->onMousePress((ActionHandler)&BuildNewBaseState::btnRotateDownPress);
	_btnRotateDown->onMouseRelease((ActionHandler)&BuildNewBaseState::btnRotateDownRelease);
	_btnRotateDown->onKeyboardPress((ActionHandler)&BuildNewBaseState::btnRotateDownPress, Options::keyGeoDown);
	_btnRotateDown->onKeyboardRelease((ActionHandler)&BuildNewBaseState::btnRotateDownRelease, Options::keyGeoDown);

	_btnZoomIn->onMouseClick((ActionHandler)&BuildNewBaseState::btnZoomInLeftClick, SDL_BUTTON_LEFT);
	_btnZoomIn->onMouseClick((ActionHandler)&BuildNewBaseState::btnZoomInRightClick, SDL_BUTTON_RIGHT);
	_btnZoomIn->onKeyboardPress((ActionHandler)&BuildNewBaseState::btnZoomInLeftClick, Options::keyGeoZoomIn);

	_btnZoomOut->onMouseClick((ActionHandler)&BuildNewBaseState::btnZoomOutLeftClick, SDL_BUTTON_LEFT);
	_btnZoomOut->onMouseClick((ActionHandler)&BuildNewBaseState::btnZoomOutRightClick, SDL_BUTTON_RIGHT);
	_btnZoomOut->onKeyboardPress((ActionHandler)&BuildNewBaseState::btnZoomOutLeftClick, Options::keyGeoZoomOut);

	// dirty hacks to get the rotate buttons to work in "classic" style
	_btnRotateLeft->setListButton();
	_btnRotateRight->setListButton();
	_btnRotateUp->setListButton();
	_btnRotateDown->setListButton();

	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnCancel->setText(tr("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&BuildNewBaseState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&BuildNewBaseState::btnCancelClick, Options::keyCancel);

	_txtTitle->setText(tr("STR_SELECT_SITE_FOR_NEW_BASE"));
	_txtTitle->setVerticalAlign(ALIGN_MIDDLE);
	_txtTitle->setWordWrap(true);

	if (_first)
	{
		_btnCancel->setVisible(false);
	}
}

/**
 *
 */
BuildNewBaseState::~BuildNewBaseState()
{
	if (Options::globeRadarLines != _oldshowradar)
	{
		Options::globeRadarLines = false;
	}
	delete _hoverTimer;
}

/**
 * Stops the globe and adds radar hover effect.
 */
void BuildNewBaseState::init()
{
	State::init();
	_globe->onMouseOver((ActionHandler)&BuildNewBaseState::globeHover);
	_globe->rotateStop();
	_globe->setNewBaseHover();
}

/**
 * Runs the globe rotation timer.
 */
void BuildNewBaseState::think()
{
	State::think();
	_globe->think();
	_hoverTimer->think(this, 0);
}

/**
 * Handles the globe.
 * @param action Pointer to an action.
 */
void BuildNewBaseState::handle(Action *action)
{
	State::handle(action);
	_globe->handle(action, this);
}

/**
 * Processes mouse-hover event for base placement,
 * @param action Pointer to an action.
 */
void BuildNewBaseState::globeHover(Action *action)
{
	_mousex = (int)floor(action->getAbsoluteXMouse());
	_mousey = (int)floor(action->getAbsoluteYMouse());
	if (!_hoverTimer->isRunning()) _hoverTimer->start();
}

void BuildNewBaseState::hoverRedraw(void)
{
	double lon, lat;
	_globe->cartToPolar(_mousex, _mousey, &lon, &lat);
	if (lon == lon && lat == lat)
	{
		_globe->setNewBaseHoverPos(lon,lat);
		_globe->setNewBaseHover();
	}
	if (Options::globeRadarLines && !(AreSame(_oldlat, lat) && AreSame(_oldlon, lon)) )
	{
		_oldlat=lat;
		_oldlon=lon;
		_globe->invalidate();
	}
}

/**
 * Processes any left-clicks for base placement,
 * or right-clicks to scroll the globe.
 * @param action Pointer to an action.
 */
void BuildNewBaseState::globeClick(Action *action)
{
	double lon, lat;
	int mouseX = (int)floor(action->getAbsoluteXMouse()), mouseY = (int)floor(action->getAbsoluteYMouse());
	_globe->cartToPolar(mouseX, mouseY, &lon, &lat);

	// Ignore window clicks
	if (mouseY < 28)
	{
		return;
	}

	// Clicking on a polygon for a base location
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		if (_globe->insideLand(lon, lat))
		{
			_base->setLongitude(lon);
			_base->setLatitude(lat);
			for (std::vector<Craft*>::iterator i = _base->getCrafts()->begin(); i != _base->getCrafts()->end(); ++i)
			{
				(*i)->setLongitude(lon);
				(*i)->setLatitude(lat);
			}
			if (_first)
			{
				_game->pushState(new BaseNameState(_base, _globe, _first));
			}
			else
			{
				_game->pushState(new ConfirmNewBaseState(_base, _globe));
			}
		}
		else
		{
			_game->pushState(new ErrorMessageState(tr("STR_XCOM_BASE_CANNOT_BE_BUILT"), _palette, _game->getRuleset()->getInterface("geoscape")->getElement("genericWindow")->color, "BACK01.SCR", _game->getRuleset()->getInterface("geoscape")->getElement("palette")->color));
		}
	}
}

/**
 * Starts rotating the globe to the left.
 * @param action Pointer to an action.
 */
void BuildNewBaseState::btnRotateLeftPress(Action *)
{
	_globe->rotateLeft();
}

/**
 * Stops rotating the globe to the left.
 * @param action Pointer to an action.
 */
void BuildNewBaseState::btnRotateLeftRelease(Action *)
{
	_globe->rotateStopLon();
}

/**
 * Starts rotating the globe to the right.
 * @param action Pointer to an action.
 */
void BuildNewBaseState::btnRotateRightPress(Action *)
{
	_globe->rotateRight();
}

/**
 * Stops rotating the globe to the right.
 * @param action Pointer to an action.
 */
void BuildNewBaseState::btnRotateRightRelease(Action *)
{
	_globe->rotateStopLon();
}

/**
 * Starts rotating the globe upwards.
 * @param action Pointer to an action.
 */
void BuildNewBaseState::btnRotateUpPress(Action *)
{
	_globe->rotateUp();
}

/**
 * Stops rotating the globe upwards.
 * @param action Pointer to an action.
 */
void BuildNewBaseState::btnRotateUpRelease(Action *)
{
	_globe->rotateStopLat();
}

/**
 * Starts rotating the globe downwards.
 * @param action Pointer to an action.
 */
void BuildNewBaseState::btnRotateDownPress(Action *)
{
	_globe->rotateDown();
}

/**
 * Stops rotating the globe downwards.
 * @param action Pointer to an action.
 */
void BuildNewBaseState::btnRotateDownRelease(Action *)
{
	_globe->rotateStopLat();
}

/**
 * Zooms into the globe.
 * @param action Pointer to an action.
 */
void BuildNewBaseState::btnZoomInLeftClick(Action *)
{
	_globe->zoomIn();
}

/**
 * Zooms the globe maximum.
 * @param action Pointer to an action.
 */
void BuildNewBaseState::btnZoomInRightClick(Action *)
{
	_globe->zoomMax();
}

/**
 * Zooms out of the globe.
 * @param action Pointer to an action.
 */
void BuildNewBaseState::btnZoomOutLeftClick(Action *)
{
	_globe->zoomOut();
}

/**
 * Zooms the globe minimum.
 * @param action Pointer to an action.
 */
void BuildNewBaseState::btnZoomOutRightClick(Action *)
{
	_globe->zoomMin();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void BuildNewBaseState::btnCancelClick(Action *)
{
	delete _base;
	_game->popState();
}

/**
 * Updates the scale.
 * @param dX delta of X;
 * @param dY delta of Y;
 */
void BuildNewBaseState::resize(int &dX, int &dY)
{
	for (std::vector<Surface*>::const_iterator i = _surfaces.begin(); i != _surfaces.end(); ++i)
	{
		(*i)->setX((*i)->getX() + dX / 2);
		if (*i != _window && *i != _btnCancel && *i != _txtTitle)
		{
			(*i)->setY((*i)->getY() + dY / 2);
		}
	}
}
}
