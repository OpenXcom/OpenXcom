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
#include "SelectDestinationState.h"
#include <cmath>
#include "../Engine/Game.h"
#include "../Engine/Action.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Interface/Window.h"
#include "Globe.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Savegame/Waypoint.h"
#include "MultipleTargetsState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Select Destination window.
 * @param game Pointer to the core game.
 * @param craft Pointer to the craft to target.
 * @param globe Pointer to the Geoscape globe.
 */
SelectDestinationState::SelectDestinationState(Game *game, Craft *craft, Globe *globe) : State(game), _craft(craft), _globe(globe)
{
	_screen = false;

	// Create objects
	_btnRotateLeft = new InteractiveSurface(12, 10, 259, 176);
	_btnRotateRight = new InteractiveSurface(12, 10, 283, 176);
	_btnRotateUp = new InteractiveSurface(13, 12, 271, 162);
	_btnRotateDown = new InteractiveSurface(13, 12, 271, 187);
	_btnZoomIn = new InteractiveSurface(23, 23, 295, 156);
	_btnZoomOut = new InteractiveSurface(13, 17, 300, 182);

	_window = new Window(this, 256, 28, 0, 0);
	_btnCancel = new TextButton(60, 12, 110, 8);
	_txtTitle = new Text(100, 9, 10, 10);

	// Set palette
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
	_globe->onMouseClick((ActionHandler)&SelectDestinationState::globeClick);

	_btnRotateLeft->onMousePress((ActionHandler)&SelectDestinationState::btnRotateLeftPress);
	_btnRotateLeft->onMouseRelease((ActionHandler)&SelectDestinationState::btnRotateLeftRelease);

	_btnRotateRight->onMousePress((ActionHandler)&SelectDestinationState::btnRotateRightPress);
	_btnRotateRight->onMouseRelease((ActionHandler)&SelectDestinationState::btnRotateRightRelease);

	_btnRotateUp->onMousePress((ActionHandler)&SelectDestinationState::btnRotateUpPress);
	_btnRotateUp->onMouseRelease((ActionHandler)&SelectDestinationState::btnRotateUpRelease);

	_btnRotateDown->onMousePress((ActionHandler)&SelectDestinationState::btnRotateDownPress);
	_btnRotateDown->onMouseRelease((ActionHandler)&SelectDestinationState::btnRotateDownRelease);

	_btnZoomIn->onMouseClick((ActionHandler)&SelectDestinationState::btnZoomInLeftClick, SDL_BUTTON_LEFT);
	_btnZoomIn->onMouseClick((ActionHandler)&SelectDestinationState::btnZoomInRightClick, SDL_BUTTON_RIGHT);

	_btnZoomOut->onMouseClick((ActionHandler)&SelectDestinationState::btnZoomOutLeftClick, SDL_BUTTON_LEFT);
	_btnZoomOut->onMouseClick((ActionHandler)&SelectDestinationState::btnZoomOutRightClick, SDL_BUTTON_RIGHT);

	_window->setColor(Palette::blockOffset(15)-1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&SelectDestinationState::btnCancelClick);

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setText(_game->getLanguage()->getString("STR_SELECT_DESTINATION"));
}

/**
 *
 */
SelectDestinationState::~SelectDestinationState()
{

}

/**
 * Resets the palette since it's bound to change on other screens.
 */
void SelectDestinationState::init()
{
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_0")->getColors());
}

/**
 * Runs the globe rotation timer.
 */
void SelectDestinationState::think()
{
	State::think();
	_globe->think();
}

/**
 * Handles the globe.
 * @param action Pointer to an action.
 */
void SelectDestinationState::handle(Action *action)
{
	State::handle(action);
	_globe->handle(action, this);
}

/**
 * Processes any left-clicks for picking a target,
 * or right-clicks to scroll the globe.
 * @param action Pointer to an action.
 */
void SelectDestinationState::globeClick(Action *action)
{
	double lon, lat;
	int mouseX = (int)floor(action->getAbsoluteXMouse()), mouseY = (int)floor(action->getAbsoluteYMouse());
	_globe->cartToPolar(mouseX, mouseY, &lon, &lat);

	// Ignore window clicks
	if (mouseY < 28)
	{
		return;
	}

	// Clicking on a valid target
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		std::vector<Target*> v = _globe->getTargets(mouseX, mouseY, true);
		if (v.empty())
		{
			Waypoint *w = new Waypoint();
			w->setLongitude(lon);
			w->setLatitude(lat);
			v.push_back(w);
		}
		_game->pushState(new MultipleTargetsState(_game, v, _craft, 0));
	}
}

/**
 * Starts rotating the globe to the left.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnRotateLeftPress(Action *action)
{
	_globe->rotateLeft();
}

/**
 * Stops rotating the globe to the left.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnRotateLeftRelease(Action *action)
{
	_globe->rotateStop();
}

/**
 * Starts rotating the globe to the right.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnRotateRightPress(Action *action)
{
	_globe->rotateRight();
}

/**
 * Stops rotating the globe to the right.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnRotateRightRelease(Action *action)
{
	_globe->rotateStop();
}

/**
 * Starts rotating the globe upwards.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnRotateUpPress(Action *action)
{
	_globe->rotateUp();
}

/**
 * Stops rotating the globe upwards.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnRotateUpRelease(Action *action)
{
	_globe->rotateStop();
}

/**
 * Starts rotating the globe downwards.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnRotateDownPress(Action *action)
{
	_globe->rotateDown();
}

/**
 * Stops rotating the globe downwards.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnRotateDownRelease(Action *action)
{
	_globe->rotateStop();
}

/**
 * Zooms into the globe.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnZoomInLeftClick(Action *action)
{
	_globe->zoomIn();
}

/**
 * Zooms the globe maximum.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnZoomInRightClick(Action *action)
{
	_globe->zoomMax();
}

/**
 * Zooms out of the globe.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnZoomOutLeftClick(Action *action)
{
	_globe->zoomOut();
}

/**
 * Zooms the globe minimum.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnZoomOutRightClick(Action *action)
{
	_globe->zoomMin();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnCancelClick(Action *action)
{
	_game->popState();
}

}
