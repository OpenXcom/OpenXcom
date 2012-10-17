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
#include "GeoscapeCraftState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Savegame/Base.h"
#include "../Savegame/Craft.h"
#include "../Ruleset/RuleCraft.h"
#include "../Savegame/CraftWeapon.h"
#include "../Ruleset/RuleCraftWeapon.h"
#include "../Savegame/Target.h"
#include "../Savegame/Ufo.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Waypoint.h"
#include "SelectDestinationState.h"

namespace OpenXcom
{

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
	_btnBase = new TextButton(192, 12, 32, 124);
	_btnTarget = new TextButton(192, 12, 32, 140);
	_btnPatrol = new TextButton(192, 12, 32, 156);
	_btnCancel = new TextButton(192, 12, 32, 172);
	_txtTitle = new Text(200, 16, 32, 20);
	_txtStatus = new Text(200, 9, 32, 36);
	_txtBase = new Text(200, 9, 32, 52);
	_txtSpeed = new Text(200, 9, 32, 60);
	_txtMaxSpeed = new Text(200, 9, 32, 68);
	_txtAltitude = new Text(200, 9, 32, 76);
	_txtFuel = new Text(200, 9, 32, 84);
	_txtW1Name = new Text(120, 9, 32, 92);
	_txtW1Ammo = new Text(60, 9, 164, 92);
	_txtW2Name = new Text(120, 9, 32, 100);
	_txtW2Ammo = new Text(60, 9, 164, 100);
	_txtRedirect = new Text(230, 16, 13, 108);

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
	_window->setColor(Palette::blockOffset(15)-1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK12.SCR"));

	_btnBase->setColor(Palette::blockOffset(8)+5);
	_btnBase->setText(_game->getLanguage()->getString("STR_RETURN_TO_BASE"));
	_btnBase->onMouseClick((ActionHandler)&GeoscapeCraftState::btnBaseClick);

	_btnTarget->setColor(Palette::blockOffset(8)+5);
	_btnTarget->setText(_game->getLanguage()->getString("STR_SELECT_NEW_TARGET"));
	_btnTarget->onMouseClick((ActionHandler)&GeoscapeCraftState::btnTargetClick);

	_btnPatrol->setColor(Palette::blockOffset(8)+5);
	_btnPatrol->setText(_game->getLanguage()->getString("STR_PATROL"));
	_btnPatrol->onMouseClick((ActionHandler)&GeoscapeCraftState::btnPatrolClick);

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&GeoscapeCraftState::btnCancelClick);

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setBig();
	_txtTitle->setText(_craft->getName(_game->getLanguage()));

	_txtStatus->setColor(Palette::blockOffset(15)-1);
	_txtStatus->setSecondaryColor(Palette::blockOffset(8)+10);
	std::wstringstream ss;
	ss << _game->getLanguage()->getString("STR_STATUS_") << L'\x01';
	if (_waypoint != 0)
	{
		ss << _game->getLanguage()->getString("STR_INTERCEPTING_UFO") << _waypoint->getId();
	}
	else if (_craft->getLowFuel())
	{
		ss << _game->getLanguage()->getString("STR_LOW_FUEL_RETURNING_TO_BASE");
	}
	else if (_craft->getDestination() == 0)
	{
		ss << _game->getLanguage()->getString("STR_PATROLLING");
	}
	else if (_craft->getDestination() == (Target*)_craft->getBase())
	{
		ss << _game->getLanguage()->getString("STR_RETURNING_TO_BASE");
	}
	else
	{
		Ufo *u = dynamic_cast<Ufo*>(_craft->getDestination());
		if (u != 0)
		{
			if (!u->isCrashed())
			{
				ss << _game->getLanguage()->getString("STR_INTERCEPTING_UFO") << u->getId();
			}
			else
			{
				ss << _game->getLanguage()->getString("STR_DESTINATION_UC_") << u->getName(_game->getLanguage());
			}
		}
		else
		{
			ss << _game->getLanguage()->getString("STR_DESTINATION_UC_") << _craft->getDestination()->getName(_game->getLanguage());
		}
	}
	_txtStatus->setText(ss.str());

	_txtBase->setColor(Palette::blockOffset(15)-1);
	_txtBase->setSecondaryColor(Palette::blockOffset(8)+5);
	std::wstringstream ss2;
	ss2 << _game->getLanguage()->getString("STR_BASE_UC_") << L'\x01' << _craft->getBase()->getName();
	_txtBase->setText(ss2.str());

	_txtSpeed->setColor(Palette::blockOffset(15)-1);
	_txtSpeed->setSecondaryColor(Palette::blockOffset(8)+5);
	std::wstringstream ss3;
	ss3 << _game->getLanguage()->getString("STR_SPEED_") << L'\x01' << _craft->getSpeed();
	_txtSpeed->setText(ss3.str());

	_txtMaxSpeed->setColor(Palette::blockOffset(15)-1);
	_txtMaxSpeed->setSecondaryColor(Palette::blockOffset(8)+5);
	std::wstringstream ss4;
	ss4 << _game->getLanguage()->getString("STR_MAXIMUM_SPEED_UC") << L'\x01' << _craft->getRules()->getMaxSpeed();
	_txtMaxSpeed->setText(ss4.str());

	_txtAltitude->setColor(Palette::blockOffset(15)-1);
	_txtAltitude->setSecondaryColor(Palette::blockOffset(8)+5);
	std::wstringstream ss5;
	ss5 << _game->getLanguage()->getString("STR_ALTITUDE_") << L'\x01' << _game->getLanguage()->getString(_craft->getAltitude());
	_txtAltitude->setText(ss5.str());

	_txtFuel->setColor(Palette::blockOffset(15)-1);
	_txtFuel->setSecondaryColor(Palette::blockOffset(8)+5);
	std::wstringstream ss6;
	ss6 << _game->getLanguage()->getString("STR_FUEL") << L'\x01' << _craft->getFuelPercentage() << "%";
	_txtFuel->setText(ss6.str());

	_txtW1Name->setColor(Palette::blockOffset(15)-1);
	_txtW1Name->setSecondaryColor(Palette::blockOffset(8)+5);
	std::wstringstream ss7;
	ss7 << _game->getLanguage()->getString("STR_WEAPON_1") << L'\x01';

	_txtW1Ammo->setColor(Palette::blockOffset(15)-1);
	_txtW1Ammo->setSecondaryColor(Palette::blockOffset(8)+5);
	std::wstringstream ss8;
	ss8 << _game->getLanguage()->getString("STR_ROUNDS_") << L'\x01';

	if (_craft->getRules()->getWeapons() > 0 && _craft->getWeapons()->at(0) != 0)
	{
		CraftWeapon *w1 = _craft->getWeapons()->at(0);

		ss7 << _game->getLanguage()->getString(w1->getRules()->getType());
		_txtW1Name->setText(ss7.str());

		ss8 << w1->getAmmo();
		_txtW1Ammo->setText(ss8.str());
	}
	else
	{
		ss7 << _game->getLanguage()->getString("STR_NONE_UC");
		_txtW1Name->setText(ss7.str());
		_txtW1Ammo->setVisible(false);
	}

	_txtW2Name->setColor(Palette::blockOffset(15)-1);
	_txtW2Name->setSecondaryColor(Palette::blockOffset(8)+5);
	std::wstringstream ss9;
	ss9 << _game->getLanguage()->getString("STR_WEAPON_2") << L'\x01';

	_txtW2Ammo->setColor(Palette::blockOffset(15)-1);
	_txtW2Ammo->setSecondaryColor(Palette::blockOffset(8)+5);
	std::wstringstream ss10;
	ss10 << _game->getLanguage()->getString("STR_ROUNDS_") << L'\x01';

	if (_craft->getRules()->getWeapons() > 1 && _craft->getWeapons()->at(1) != 0)
	{
		CraftWeapon *w2 = _craft->getWeapons()->at(1);

		ss9 << _game->getLanguage()->getString(w2->getRules()->getType());
		_txtW2Name->setText(ss9.str());

		ss10 << w2->getAmmo();
		_txtW2Ammo->setText(ss10.str());
	}
	else
	{
		ss9 << _game->getLanguage()->getString("STR_NONE_UC");
		_txtW2Name->setText(ss9.str());
		_txtW2Ammo->setVisible(false);
	}

	_txtRedirect->setColor(Palette::blockOffset(15)-1);
	_txtRedirect->setBig();
	_txtRedirect->setAlign(ALIGN_CENTER);
	_txtRedirect->setText(_game->getLanguage()->getString("STR_REDIRECT_CRAFT"));

	if (_waypoint == 0)
	{
		_txtRedirect->setVisible(false);
	}
	else
	{
		_btnCancel->setText(_game->getLanguage()->getString("STR_GO_TO_LAST_KNOWN_UFO_POSITION"));
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
 * @param action Pointer to an action.
 */
void GeoscapeCraftState::btnBaseClick(Action *action)
{
	_game->popState();
	_craft->returnToBase();
	delete _waypoint;
}

/**
 * Changes the craft's target.
 * @param action Pointer to an action.
 */
void GeoscapeCraftState::btnTargetClick(Action *action)
{
	_game->popState();
	_game->pushState(new SelectDestinationState(_game, _craft, _globe));
	delete _waypoint;
}

/**
 * Sets the craft to patrol the current location.
 * @param action Pointer to an action.
 */
void GeoscapeCraftState::btnPatrolClick(Action *action)
{
	_game->popState();
	_craft->setDestination(0);
	delete _waypoint;
}

/**
 * Closes the window.
 * @param action Pointer to an action.
 */
void GeoscapeCraftState::btnCancelClick(Action *action)
{
	// Go to the last known UFO position
	if (_waypoint != 0)
	{
		_waypoint->setId(_game->getSavedGame()->getId("STR_WAYPOINT"));
		_game->getSavedGame()->getWaypoints()->push_back(_waypoint);
		_craft->setDestination(_waypoint);
	}
	// Cancel
	_game->popState();
}

}
