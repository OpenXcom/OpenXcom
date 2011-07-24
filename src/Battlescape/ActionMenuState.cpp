/*
 * Copyright 2010 OpenXcom Developers.
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
#include "ActionMenuState.h"
#include <sstream>
#include <cmath>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Action.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/BattleItem.h"
#include "../Ruleset/RuleItem.h"
#include "ActionMenuItem.h"
#include "PrimeGrenadeState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Action Menu window.
 * @param game Pointer to the core game.
 * @param action Pointer to the action.
 */
ActionMenuState::ActionMenuState(Game *game, BattleAction *action) : State(game), _action(action)
{
	_screen = false;
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_4")->getColors());

	for (int i = 0; i < 5; ++i)
	{
		_actionMenu[i] = new ActionMenuItem(this, i, _game->getResourcePack()->getFont("BIGLETS.DAT"));
		add(_actionMenu[i]);
		_actionMenu[i]->setVisible(false);
		_actionMenu[i]->onMouseClick((ActionHandler)&ActionMenuState::btnActionMenuItemClick);
	}

	// Build up the popup menu
	int id = 0, tu;
	std::wstring strAcc = _game->getLanguage()->getString("STR_ACC");
	std::wstring strTU = _game->getLanguage()->getString("STR_TUS");
	std::wstringstream ss1, ss2;

	// throwing
	tu = _action->actor->getActionTUs(BA_THROW, _action->weapon);
	ss1 << strAcc.c_str() << (int)floor(_action->actor->getThrowingAccuracy() * 100) << "%";
	ss2 << strTU.c_str() << tu;
	_actionMenu[id]->setAction(BA_THROW, _game->getLanguage()->getString("STR_THROW"), ss1.str(), ss2.str(), tu);
	_actionMenu[id]->setVisible(true);
	id++;
	ss1.str(L"");
	ss2.str(L"");

	// priming
	if ((_action->weapon->getRules()->getBattleType() == BT_GRENADE || _action->weapon->getRules()->getBattleType() == BT_PROXIMITYGRENADE)
		&& _action->weapon->getExplodeTurn() == 0)
	{
		tu = _action->actor->getActionTUs(BA_PRIME, _action->weapon);
		ss2 << strTU.c_str() << tu;
		_actionMenu[id]->setAction(BA_PRIME, _game->getLanguage()->getString("STR_PRIME_GRENADE"), ss1.str(), ss2.str(), tu);
		_actionMenu[id]->setVisible(true);
		id++;
		ss1.str(L"");
		ss2.str(L"");
	}

	if (_action->weapon->getRules()->getAccuracyAuto() != 0)
	{
		tu = _action->actor->getActionTUs(BA_AUTOSHOT, _action->weapon);
		ss1 << strAcc.c_str() << (int)floor(_action->actor->getFiringAccuracy(_action->weapon->getRules()->getAccuracyAuto()) * 100) << "%";
		ss2 << strTU.c_str() << tu;
		_actionMenu[id]->setAction(BA_AUTOSHOT, _game->getLanguage()->getString("STR_AUTO_SHOT"), ss1.str(), ss2.str(), tu);
		_actionMenu[id]->setVisible(true);
		id++;
		ss1.str(L"");
		ss2.str(L"");
	}
	if (_action->weapon->getRules()->getAccuracySnap() != 0)
	{
		tu = _action->actor->getActionTUs(BA_SNAPSHOT, _action->weapon);
		ss1 << strAcc.c_str() << (int)floor(_action->actor->getFiringAccuracy(_action->weapon->getRules()->getAccuracySnap()) * 100) << "%";
		ss2 << strTU.c_str() << tu;
		_actionMenu[id]->setAction(BA_SNAPSHOT, _game->getLanguage()->getString("STR_SNAP_SHOT"), ss1.str(), ss2.str(), tu);
		_actionMenu[id]->setVisible(true);
		id++;
		ss1.str(L"");
		ss2.str(L"");
	}
	if (_action->weapon->getRules()->getAccuracyAimed() != 0)
	{
		tu = _action->actor->getActionTUs(BA_AIMEDSHOT, _action->weapon);
		ss1 << strAcc.c_str() << (int)floor(_action->actor->getFiringAccuracy(_action->weapon->getRules()->getAccuracyAimed()) * 100) << "%";
		ss2 << strTU.c_str() << tu;
		_actionMenu[id]->setAction(BA_AIMEDSHOT, _game->getLanguage()->getString("STR_AIMED_SHOT"), ss1.str(), ss2.str(), tu);
		_actionMenu[id]->setVisible(true);
		id++;
		ss1.str(L"");
		ss2.str(L"");
	}

}

/**
 *
 */
ActionMenuState::~ActionMenuState()
{

}

/**
 * Execute the action corresponding with this action menu item.
 * @param action Pointer to an action.
 */
void ActionMenuState::btnActionMenuItemClick(Action *action)
{
	int btnID = -1;

	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		_game->popState();
		return;
	}

	// got to find out which button was pressed
	for (int i = 0; i < 10 && btnID == -1; ++i)
	{
		if (action->getSender() == _actionMenu[i])
		{
			btnID = i;
		}
	}

	if (btnID != -1)
	{
		_action->type = _actionMenu[btnID]->getAction();
		_action->TU = _actionMenu[btnID]->getTUs();
		if (_action->type == BA_PRIME)
		{
			if (ALT_GRENADE)
			{
				_action->value = 1;
				_game->popState();
			}
			else
			{
				_game->pushState(new PrimeGrenadeState(_game, _action));
			}
		}
		else
		{
			_action->targeting = true;
			_game->popState();
		}
	}
}

}
