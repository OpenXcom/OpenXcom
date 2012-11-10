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
#include "MultipleTargetsState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/TextList.h"
#include "../Savegame/Target.h"
#include "../Savegame/Base.h"
#include "../Savegame/Craft.h"
#include "../Savegame/Ufo.h"
#include "GeoscapeState.h"
#include "ConfirmDestinationState.h"
#include "InterceptState.h"
#include "UfoDetectedState.h"
#include "GeoscapeCraftState.h"
#include "TargetInfoState.h"
#include "UfoHyperDetectedState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Multiple Targets window.
 * @param game Pointer to the core game.
 * @param targets List of targets to display.
 * @param craft Pointer to craft to retarget (NULL if none).
 * @param state Pointer to the Geoscape state.
 */
MultipleTargetsState::MultipleTargetsState(Game *game, std::vector<Target*> targets, Craft *craft, GeoscapeState *state) : State(game), _targets(targets), _craft(craft), _state(state)
{
	_screen = false;

	if (_targets.size() > 1)
	{
		size_t listHeight = _targets.size() * 8;
		int winHeight = listHeight + BUTTON_HEIGHT + OUTER_MARGIN * 2 + INNER_MARGIN + BORDER * 2;
		int winY = (200 - winHeight) / 2;
		int listY = winY + BORDER + OUTER_MARGIN;
		int btnY = listY + listHeight + INNER_MARGIN;

		// Create objects
		_window = new Window(this, 136, winHeight, 60, winY);
		_btnCancel = new TextButton(116, BUTTON_HEIGHT, 70, btnY);
		_lstTargets = new TextList(116, listHeight, 70, listY);

		// Set palette
		_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(7)), Palette::backPos, 16);

		add(_window);
		add(_btnCancel);
		add(_lstTargets);

		// Set up objects
		_window->setColor(Palette::blockOffset(8)+5);
		_window->setBackground(_game->getResourcePack()->getSurface("BACK15.SCR"));

		_btnCancel->setColor(Palette::blockOffset(8)+5);
		_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
		_btnCancel->onMouseClick((ActionHandler)&MultipleTargetsState::btnCancelClick);

		_lstTargets->setColor(Palette::blockOffset(8)+5);
		_lstTargets->setAlign(ALIGN_CENTER);
		_lstTargets->setColumns(1, 116);
		_lstTargets->setSelectable(true);
		_lstTargets->setBackground(_window);
		_lstTargets->onMouseClick((ActionHandler)&MultipleTargetsState::lstTargetsClick);
		for (std::vector<Target*>::iterator i = _targets.begin(); i != _targets.end(); ++i)
		{
			_lstTargets->addRow(1, (*i)->getName(_game->getLanguage()).c_str());
		}
	}
}

/**
 *
 */
MultipleTargetsState::~MultipleTargetsState()
{

}

/**
 * Resets the palette and ignores the window
 * if there's only one target.
 */
void MultipleTargetsState::init()
{
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(7)), Palette::backPos, 16);

	if (_targets.size() == 1)
	{
		popupTarget(*_targets.begin());
	}
}

/**
 * Displays the right popup for a specific target.
 * @param target Pointer to target.
 */
void MultipleTargetsState::popupTarget(Target *target)
{
	_game->popState();
	if (_craft == 0)
	{
		Base* b = dynamic_cast<Base*>(target);
		Craft* c = dynamic_cast<Craft*>(target);
		Ufo* u = dynamic_cast<Ufo*>(target);
		if (b != 0)
		{
			_game->pushState(new InterceptState(_game, _state->getGlobe(), b));
		}
		else if (c != 0)
		{
			_game->pushState(new GeoscapeCraftState(_game, c, _state->getGlobe(), 0));
		}
		else if (u != 0)
		{
			if(!(u->getHyperDetected()))
				_game->pushState(new UfoDetectedState(_game, u, _state, false));
			else
				_game->pushState(new UfoHyperDetectedState(_game, u, _state, false));
		}
		else
		{
			_game->pushState(new TargetInfoState(_game, target));
		}
	}
	else
	{
		_game->pushState(new ConfirmDestinationState(_game, _craft, target));
	}
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void MultipleTargetsState::btnCancelClick(Action *action)
{
	_game->popState();
}

/**
 * Pick a target to display.
 * @param action Pointer to an action.
 */
void MultipleTargetsState::lstTargetsClick(Action *action)
{
	Target* t = _targets[_lstTargets->getSelectedRow()];
	popupTarget(t);
}

}
