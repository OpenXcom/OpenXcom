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
#include "GeoscapeCraftState.h"
#include <sstream>
#include "Game.h"
#include "ResourcePack.h"
#include "Language.h"
#include "LangString.h"
#include "Font.h"
#include "Palette.h"
#include "TextButton.h"
#include "Window.h"
#include "Text.h"
#include "Base.h"
#include "Craft.h"
#include "RuleCraft.h"
#include "CraftWeapon.h"
#include "RuleCraftWeapon.h"
#include "Target.h"
#include "Ufo.h"
#include "SavedGame.h"
#include "Waypoint.h"
#include "SelectDestinationState.h"

using namespace std;

/**
 * Initializes all the elements in the Geoscape Craft window.
 * @param game Pointer to the core game.
 * @param craft Pointer to the craft to display.
 * @param globe Pointer to the Geoscape globe.
 * @param waypoint Pointer to the last UFO position (if redirecting the craft).
 */
GeoscapeCraftState::GeoscapeCraftState(Game *game, Craft *craft, Globe *globe, Waypoint *waypoint) : State(game), _craft(craft), _globe(globe), _waypoint(waypoint)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 240, 184, 8, 8, POPUP_BOTH);
	if (_waypoint != 0)
	{
		_btnBase = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 224, 12, 16, 132);
		_btnTarget = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 224, 12, 16, 164);
		_btnPatrol = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 224, 12, 16, 148);
	}
	else
	{
		_btnBase = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 192, 12, 32, 116);
		_btnTarget = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 192, 12, 32, 132);
		_btnPatrol = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 192, 12, 32, 148);
	}
	_btnCancel = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 192, 12, 32, 164);
	_txtTitle = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 190, 16, 32, 20);
	_txtStatus = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 190, 9, 32, 36);
	_txtBase = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 120, 9, 32, 52);
	_txtSpeed = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 120, 9, 32, 60);
	_txtMaxSpeed = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 120, 9, 32, 68);
	_txtAltitude = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 120, 9, 32, 76);
	_txtFuel = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 120, 9, 32, 84);
	_txtW1Name = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 120, 9, 32, 92);
	_txtW1Ammo = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 60, 9, 164, 92);
	_txtW2Name = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 120, 9, 32, 100);
	_txtW2Ammo = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 60, 9, 164, 100);
	_txtRedirect = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 230, 16, 13, 108);
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(4)), Palette::backPos, 16);

	add(_window);
	add(_btnBase);
	add(_btnTarget);
	add(_btnPatrol);
	add(_btnCancel);
	add(_txtTitle);
	add(_txtStatus);
	add(_txtBase);
	add(_txtSpeed);
	add(_txtMaxSpeed);
	add(_txtAltitude);
	add(_txtFuel);
	add(_txtW1Name);
	add(_txtW1Ammo);
	add(_txtW2Name);
	add(_txtW2Ammo);
	add(_txtRedirect);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+2);
	_window->setBackground(game->getResourcePack()->getSurface("BACK12.SCR"));

	_btnBase->setColor(Palette::blockOffset(8)+8);
	_btnBase->setText(_game->getResourcePack()->getLanguage()->getString(STR_RETURN_TO_BASE));
	_btnBase->onMouseClick((EventHandler)&GeoscapeCraftState::btnBaseClick);

	_btnTarget->setColor(Palette::blockOffset(8)+8);
	_btnTarget->setText(_game->getResourcePack()->getLanguage()->getString(STR_SELECT_NEW_TARGET));
	_btnTarget->onMouseClick((EventHandler)&GeoscapeCraftState::btnTargetClick);

	_btnPatrol->setColor(Palette::blockOffset(8)+8);
	_btnPatrol->setText(_game->getResourcePack()->getLanguage()->getString(STR_PATROL));
	_btnPatrol->onMouseClick((EventHandler)&GeoscapeCraftState::btnPatrolClick);

	_btnCancel->setColor(Palette::blockOffset(8)+8);
	_btnCancel->setText(_game->getResourcePack()->getLanguage()->getString(STR_CANCEL_UC));
	_btnCancel->onMouseClick((EventHandler)&GeoscapeCraftState::btnCancelClick);

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setBig();
	_txtTitle->setText(_craft->getName(_game->getResourcePack()->getLanguage()));

	_txtStatus->setColor(Palette::blockOffset(15)-1);
	stringstream ss;
	ss << _game->getResourcePack()->getLanguage()->getString(STR_STATUS_);
	if (_craft->getLowFuel())
	{
		ss << _game->getResourcePack()->getLanguage()->getString(STR_LOW_FUEL_RETURNING_TO_BASE);
	}
	else if (_craft->getDestination() == 0)
	{
		ss << _game->getResourcePack()->getLanguage()->getString(STR_PATROLLING);
	}
	else if (_craft->getDestination() == (Target*)_craft->getBase())
	{
		ss << _game->getResourcePack()->getLanguage()->getString(STR_RETURNING_TO_BASE);
	}
	else
	{
		Ufo *u = dynamic_cast<Ufo*>(_craft->getDestination());
		Waypoint *w = dynamic_cast<Waypoint*>(_craft->getDestination());
		if (u != 0)
		{
			ss << _game->getResourcePack()->getLanguage()->getString(STR_INTERCEPTING_UFO) << u->getId();
		}
		else if (w != 0)
		{
			ss << _game->getResourcePack()->getLanguage()->getString(STR_DESTINATION_WAY_POINT) << w->getId();
		}
	}
	_txtStatus->setText(ss.str());

	_txtBase->setColor(Palette::blockOffset(15)-1);
	stringstream ss2;
	ss2 << _game->getResourcePack()->getLanguage()->getString(STR_BASE_UC_) << _craft->getBase()->getName();
	_txtBase->setText(ss2.str());

	_txtSpeed->setColor(Palette::blockOffset(15)-1);
	stringstream ss3;
	ss3 << _game->getResourcePack()->getLanguage()->getString(STR_SPEED_) << _craft->getSpeed();
	_txtSpeed->setText(ss3.str());

	_txtMaxSpeed->setColor(Palette::blockOffset(15)-1);
	stringstream ss4;
	ss4 << _game->getResourcePack()->getLanguage()->getString(STR_MAXIMUM_SPEED_) << _craft->getRules()->getMaxSpeed();
	_txtMaxSpeed->setText(ss4.str());

	_txtAltitude->setColor(Palette::blockOffset(15)-1);
	stringstream ss5;
	ss5 << _game->getResourcePack()->getLanguage()->getString(STR_ALTITUDE_);
	_txtAltitude->setText(ss5.str());

	_txtFuel->setColor(Palette::blockOffset(15)-1);
	stringstream ss6;
	ss6 << _game->getResourcePack()->getLanguage()->getString(STR_FUEL_) << _craft->getFuelPercentage() << "%";
	_txtFuel->setText(ss6.str());

	_txtW1Name->setColor(Palette::blockOffset(15)-1);
	stringstream ss7;
	ss7 << _game->getResourcePack()->getLanguage()->getString(STR_WEAPON_1_);
	
	_txtW1Ammo->setColor(Palette::blockOffset(15)-1);
	stringstream ss8;
	ss8 << _game->getResourcePack()->getLanguage()->getString(STR_ROUNDS_);

	if (_craft->getRules()->getWeapons() > 0 && _craft->getWeapons()->at(0) != 0)
	{
		CraftWeapon *w1 = _craft->getWeapons()->at(0);

		ss7 << _game->getResourcePack()->getLanguage()->getString(w1->getRules()->getType());
		_txtW1Name->setText(ss7.str());
		
		ss8 << w1->getAmmo();
		_txtW1Ammo->setText(ss8.str());
	}
	else
	{
		ss7 << _game->getResourcePack()->getLanguage()->getString(STR_NONE);
		_txtW1Name->setText(ss7.str());
		_txtW1Ammo->setVisible(false);
	}

	_txtW2Name->setColor(Palette::blockOffset(15)-1);
	stringstream ss9;
	ss9 << _game->getResourcePack()->getLanguage()->getString(STR_WEAPON_2_);
	
	_txtW2Ammo->setColor(Palette::blockOffset(15)-1);
	stringstream ss10;
	ss10 << _game->getResourcePack()->getLanguage()->getString(STR_ROUNDS_);

	if (_craft->getRules()->getWeapons() > 1 && _craft->getWeapons()->at(1) != 0)
	{
		CraftWeapon *w2 = _craft->getWeapons()->at(1);

		ss9 << _game->getResourcePack()->getLanguage()->getString(w2->getRules()->getType());
		_txtW2Name->setText(ss9.str());
		
		ss10 << w2->getAmmo();
		_txtW2Ammo->setText(ss10.str());
	}
	else
	{
		ss9 << _game->getResourcePack()->getLanguage()->getString(STR_NONE);
		_txtW2Name->setText(ss9.str());
		_txtW2Ammo->setVisible(false);
	}

	_txtRedirect->setColor(Palette::blockOffset(15)-1);
	_txtRedirect->setBig();
	_txtRedirect->setAlign(ALIGN_CENTER);
	_txtRedirect->setText(_game->getResourcePack()->getLanguage()->getString(STR_REDIRECT_CRAFT));

	if (_waypoint == 0)
	{
		_txtRedirect->setVisible(false);
	}
	else
	{
		_btnCancel->setVisible(false);
		_btnTarget->setText(_game->getResourcePack()->getLanguage()->getString(STR_GO_TO_LAST_KNOWN_UFO_POSITION));
	}

	if (_craft->getLowFuel())
	{
		_btnBase->setVisible(false);
		_btnTarget->setVisible(false);
		_btnPatrol->setVisible(false);
	}
}

/**
 *
 */
GeoscapeCraftState::~GeoscapeCraftState()
{
	
}

/**
 * Resets the palette.
 */
void GeoscapeCraftState::init()
{
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(4)), Palette::backPos, 16);
}

/**
 * Returns the craft back to its base.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void GeoscapeCraftState::btnBaseClick(SDL_Event *ev, int scale)
{
	delete _waypoint;
	_game->popState();
	_craft->setDestination(_craft->getBase());
}

/**
 * Changes the craft's target.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void GeoscapeCraftState::btnTargetClick(SDL_Event *ev, int scale)
{
	_game->popState();
	// Go to the last known UFO position
	if (_waypoint != 0)
	{
		_waypoint->setId(*_game->getSavedGame()->getWaypointId());
		(*_game->getSavedGame()->getWaypointId())++;
		_game->getSavedGame()->getWaypoints()->push_back(_waypoint);
		_craft->setDestination(_waypoint);
	}
	// Select a new destination for the craft
	else
	{
		_game->pushState(new SelectDestinationState(_game, _craft, _globe));
	}
}

/**
 * Sets the craft to patrol the current location.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void GeoscapeCraftState::btnPatrolClick(SDL_Event *ev, int scale)
{
	delete _waypoint;
	_game->popState();
	_craft->setDestination(0);
}

/**
 * Closes the window.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void GeoscapeCraftState::btnCancelClick(SDL_Event *ev, int scale)
{
	delete _waypoint;
	_game->popState();
}
