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
#include "SelectDestinationState.h"
#include <cmath>
#include "../Engine/Game.h"
#include "../Engine/Screen.h"
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
#include "../Savegame/SavedGame.h"
#include "../Savegame/Craft.h"
#include "../Ruleset/RuleCraft.h"
#include "ConfirmCydoniaState.h"
#include "../Engine/Options.h"

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
	int dx = Screen::getDX();
	int dy = Screen::getDY();
	_screen = false;

	// Create objects
	_btnRotateLeft = new InteractiveSurface(12, 10, 259 + dx * 2, 176 + dy);
	_btnRotateRight = new InteractiveSurface(12, 10, 283 + dx * 2, 176 + dy);
	_btnRotateUp = new InteractiveSurface(13, 12, 271 + dx * 2, 162 + dy);
	_btnRotateDown = new InteractiveSurface(13, 12, 271 + dx * 2, 187 + dy);
	_btnZoomIn = new InteractiveSurface(23, 23, 295 + dx * 2, 156 + dy);
	_btnZoomOut = new InteractiveSurface(13, 17, 300 + dx * 2, 182 + dy);

	_window = new Window(this, 256, 28, 0 + dx, 0);
	_window->setDY(0);
	_btnCancel = new TextButton(60, 12, 110 + dx, 8);
	_btnCydonia = new TextButton(60, 12, 180 + dx, 8);
	_txtTitle = new Text(100, 16, 10 + dx, 6);

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
	add(_btnCydonia);
	add(_txtTitle);


	// Set up objects
	_globe->onMouseClick((ActionHandler)&SelectDestinationState::globeClick);

	_btnRotateLeft->onMousePress((ActionHandler)&SelectDestinationState::btnRotateLeftPress);
	_btnRotateLeft->onMouseRelease((ActionHandler)&SelectDestinationState::btnRotateLeftRelease);
	_btnRotateLeft->onKeyboardPress((ActionHandler)&SelectDestinationState::btnRotateLeftPress, (SDLKey)Options::getInt("keyGeoLeft"));
	_btnRotateLeft->onKeyboardRelease((ActionHandler)&SelectDestinationState::btnRotateLeftRelease, (SDLKey)Options::getInt("keyGeoLeft"));

	_btnRotateRight->onMousePress((ActionHandler)&SelectDestinationState::btnRotateRightPress);
	_btnRotateRight->onMouseRelease((ActionHandler)&SelectDestinationState::btnRotateRightRelease);
	_btnRotateRight->onKeyboardPress((ActionHandler)&SelectDestinationState::btnRotateRightPress, (SDLKey)Options::getInt("keyGeoRight"));
	_btnRotateRight->onKeyboardRelease((ActionHandler)&SelectDestinationState::btnRotateRightRelease, (SDLKey)Options::getInt("keyGeoRight"));

	_btnRotateUp->onMousePress((ActionHandler)&SelectDestinationState::btnRotateUpPress);
	_btnRotateUp->onMouseRelease((ActionHandler)&SelectDestinationState::btnRotateUpRelease);
	_btnRotateUp->onKeyboardPress((ActionHandler)&SelectDestinationState::btnRotateUpPress, (SDLKey)Options::getInt("keyGeoUp"));
	_btnRotateUp->onKeyboardRelease((ActionHandler)&SelectDestinationState::btnRotateUpRelease, (SDLKey)Options::getInt("keyGeoUp"));

	_btnRotateDown->onMousePress((ActionHandler)&SelectDestinationState::btnRotateDownPress);
	_btnRotateDown->onMouseRelease((ActionHandler)&SelectDestinationState::btnRotateDownRelease);
	_btnRotateDown->onKeyboardPress((ActionHandler)&SelectDestinationState::btnRotateDownPress, (SDLKey)Options::getInt("keyGeoDown"));
	_btnRotateDown->onKeyboardRelease((ActionHandler)&SelectDestinationState::btnRotateDownRelease, (SDLKey)Options::getInt("keyGeoDown"));

	_btnZoomIn->onMouseClick((ActionHandler)&SelectDestinationState::btnZoomInLeftClick, SDL_BUTTON_LEFT);
	_btnZoomIn->onMouseClick((ActionHandler)&SelectDestinationState::btnZoomInRightClick, SDL_BUTTON_RIGHT);
	_btnZoomIn->onKeyboardPress((ActionHandler)&SelectDestinationState::btnZoomInLeftClick, (SDLKey)Options::getInt("keyGeoZoomIn"));

	_btnZoomOut->onMouseClick((ActionHandler)&SelectDestinationState::btnZoomOutLeftClick, SDL_BUTTON_LEFT);
	_btnZoomOut->onMouseClick((ActionHandler)&SelectDestinationState::btnZoomOutRightClick, SDL_BUTTON_RIGHT);
	_btnZoomOut->onKeyboardPress((ActionHandler)&SelectDestinationState::btnZoomOutLeftClick, (SDLKey)Options::getInt("keyGeoZoomOut"));

	// dirty hacks to get the rotate buttons to work in "classic" style
	_btnRotateLeft->setListButton();
	_btnRotateRight->setListButton();
	_btnRotateUp->setListButton();
	_btnRotateDown->setListButton();

	_window->setColor(Palette::blockOffset(15)-1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&SelectDestinationState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&SelectDestinationState::btnCancelClick, (SDLKey)Options::getInt("keyCancel"));

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setText(_game->getLanguage()->getString("STR_SELECT_DESTINATION"));
	_txtTitle->setVerticalAlign(ALIGN_MIDDLE);
	_txtTitle->setWordWrap(true);

	if (!_craft->getRules()->getSpacecraft() || !_game->getSavedGame()->isResearched("STR_CYDONIA_OR_BUST"))
	{
		_btnCydonia->setVisible(false);
	}
	else
	{
		_btnCydonia->setColor(Palette::blockOffset(8)+5);
		_btnCydonia->setText(_game->getLanguage()->getString("STR_CYDONIA"));
		_btnCydonia->onMouseClick((ActionHandler)&SelectDestinationState::btnCydoniaClick);
	}
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
void SelectDestinationState::btnRotateLeftPress(Action *)
{
	_globe->rotateLeft();
}

/**
 * Stops rotating the globe to the left.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnRotateLeftRelease(Action *)
{
	_globe->rotateStop();
}

/**
 * Starts rotating the globe to the right.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnRotateRightPress(Action *)
{
	_globe->rotateRight();
}

/**
 * Stops rotating the globe to the right.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnRotateRightRelease(Action *)
{
	_globe->rotateStop();
}

/**
 * Starts rotating the globe upwards.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnRotateUpPress(Action *)
{
	_globe->rotateUp();
}

/**
 * Stops rotating the globe upwards.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnRotateUpRelease(Action *)
{
	_globe->rotateStop();
}

/**
 * Starts rotating the globe downwards.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnRotateDownPress(Action *)
{
	_globe->rotateDown();
}

/**
 * Stops rotating the globe downwards.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnRotateDownRelease(Action *)
{
	_globe->rotateStop();
}

/**
 * Zooms into the globe.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnZoomInLeftClick(Action *)
{
	_globe->zoomIn();
}

/**
 * Zooms the globe maximum.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnZoomInRightClick(Action *)
{
	_globe->zoomMax();
}

/**
 * Zooms out of the globe.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnZoomOutLeftClick(Action *)
{
	_globe->zoomOut();
}

/**
 * Zooms the globe minimum.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnZoomOutRightClick(Action *)
{
	_globe->zoomMin();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SelectDestinationState::btnCancelClick(Action *)
{
	_game->popState();
}

void SelectDestinationState::btnCydoniaClick(Action *)
{
	if (_craft->getNumSoldiers() > 0)
	{
		_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(5)), Palette::backPos, 16);
		_game->pushState(new ConfirmCydoniaState(_game, _craft));
	}
}
}
