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
#include "InterceptState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/Base.h"
#include "../Savegame/Craft.h"
#include "../Ruleset/RuleCraft.h"
#include "../Savegame/SavedGame.h"
#include "../Engine/Options.h"
#include "Globe.h"
#include "SelectDestinationState.h"
#include "ConfirmDestinationState.h"
#include "../Basescape/BasescapeState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Intercept window.
 * @param game Pointer to the core game.
 * @param globe Pointer to the Geoscape globe.
 * @param base Pointer to base to show contained crafts (NULL to show all crafts).
 * @param target Pointer to target to intercept (NULL to ask user for target).
 */
InterceptState::InterceptState(Globe *globe, Base *base, Target *target) : _globe(globe), _base(base), _target(target)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 320, 140, 0, 30, POPUP_HORIZONTAL);
	_btnCancel = new TextButton(_base ? 142 : 288, 16, 16, 146);
	_btnGotoBase = new TextButton(142, 16, 162, 146);
	_txtTitle = new Text(300, 17, 10, 46);
	_txtCraft = new Text(86, 9, 14, 70);
	_txtStatus = new Text(70, 9, 100, 70);
	_txtBase = new Text(80, 9, 170, 70);
	_txtWeapons = new Text(80, 17, 238, 62);
	_lstCrafts = new TextList(288, 64, 8, 78);

	// Set palette
	setInterface("geoCraftScreens");

	add(_window, "window", "geoCraftScreens");
	add(_btnCancel, "button", "geoCraftScreens");
	add(_btnGotoBase, "button", "geoCraftScreens");
	add(_txtTitle, "text1", "geoCraftScreens");
	add(_txtCraft, "text2", "geoCraftScreens");
	add(_txtStatus, "text2", "geoCraftScreens");
	add(_txtBase, "text2", "geoCraftScreens");
	add(_txtWeapons, "text2", "geoCraftScreens");
	add(_lstCrafts, "list", "geoCraftScreens");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getResourcePack()->getSurface("BACK12.SCR"));

	_btnCancel->setText(tr("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&InterceptState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&InterceptState::btnCancelClick, Options::keyCancel);
	_btnCancel->onKeyboardPress((ActionHandler)&InterceptState::btnCancelClick, Options::keyGeoIntercept);

	_btnGotoBase->setText(tr("STR_GO_TO_BASE"));
	_btnGotoBase->onMouseClick((ActionHandler)&InterceptState::btnGotoBaseClick);
	_btnGotoBase->setVisible(_base != 0);

	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(tr("STR_LAUNCH_INTERCEPTION"));

	_txtCraft->setText(tr("STR_CRAFT"));

	_txtStatus->setText(tr("STR_STATUS"));

	_txtBase->setText(tr("STR_BASE"));

	_txtWeapons->setText(tr("STR_WEAPONS_CREW_HWPS"));

	_lstCrafts->setColumns(4, 86, 70, 80, 46);
	_lstCrafts->setSelectable(true);
	_lstCrafts->setBackground(_window);
	_lstCrafts->setMargin(6);
	_lstCrafts->onMouseClick((ActionHandler)&InterceptState::lstCraftsLeftClick);
	_lstCrafts->onMouseClick((ActionHandler)&InterceptState::lstCraftsRightClick, SDL_BUTTON_RIGHT);

	int row = 0;
	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
		if (_base != 0 && (*i) != _base)
			continue;
		for (std::vector<Craft*>::iterator j = (*i)->getCrafts()->begin(); j != (*i)->getCrafts()->end(); ++j)
		{
			std::wostringstream ss;
			if ((*j)->getNumWeapons() > 0)
			{
				ss << L'\x01' << (*j)->getNumWeapons() << L'\x01';
			}
			else
			{
				ss << 0;
			}
			ss << "/";
			if ((*j)->getNumSoldiers() > 0)
			{
				ss << L'\x01' << (*j)->getNumSoldiers() << L'\x01';
			}
			else
			{
				ss << 0;
			}
			ss << "/";
			if ((*j)->getNumVehicles() > 0)
			{
				ss << L'\x01' << (*j)->getNumVehicles() << L'\x01';
			}
			else
			{
				ss << 0;
			}
			_crafts.push_back(*j);
			_lstCrafts->addRow(4, (*j)->getName(_game->getLanguage()).c_str(), tr((*j)->getStatus()).c_str(), (*i)->getName().c_str(), ss.str().c_str());
			if ((*j)->getStatus() == "STR_READY")
			{
				_lstCrafts->setCellColor(row, 1, _lstCrafts->getSecondaryColor());
			}
			row++;
		}
	}
}

/**
 *
 */
InterceptState::~InterceptState()
{

}

/**
 * Closes the window.
 * @param action Pointer to an action.
 */
void InterceptState::btnCancelClick(Action *)
{
	_game->popState();
}

/**
 * Goes to the base for the respective craft.
 * @param action Pointer to an action.
 */
void InterceptState::btnGotoBaseClick(Action *)
{
	_game->popState();
	_game->pushState(new BasescapeState(_base, _globe));
}

/**
 * Pick a target for the selected craft.
 * @param action Pointer to an action.
 */
void InterceptState::lstCraftsLeftClick(Action *)
{
	Craft* c = _crafts[_lstCrafts->getSelectedRow()];
	if (c->getStatus() == "STR_READY" || ((c->getStatus() == "STR_OUT" || Options::craftLaunchAlways) && !c->getLowFuel() && !c->getMissionComplete()))
	{
		_game->popState();
		if (_target == 0)
		{
			_game->pushState(new SelectDestinationState(c, _globe));
		}
		else
		{
			_game->pushState(new ConfirmDestinationState(c, _target));
		}
	}
}

/**
 * Centers on the selected craft.
 * @param action Pointer to an action.
 */
void InterceptState::lstCraftsRightClick(Action *)
{
	Craft* c = _crafts[_lstCrafts->getSelectedRow()];
	if (c->getStatus() == "STR_OUT")
	{
		_globe->center(c->getLongitude(), c->getLatitude());
		_game->popState();
	}
}

}
