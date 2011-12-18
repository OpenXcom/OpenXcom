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
#include "../Engine/Options.h"
#include "ActionMenuItem.h"
#include "PrimeGrenadeState.h"
#include "MedikitState.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "Pathfinding.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Action Menu window.
 * @param game Pointer to the core game.
 * @param action Pointer to the action.
 * @param x Position on the x-axis.
 * @param y position on the y-axis.
 */
ActionMenuState::ActionMenuState(Game *game, BattleAction *action, int x, int y) : State(game), _action(action)
{
	_screen = false;

	for (int i = 0; i < 6; ++i)
	{
		_actionMenu[i] = new ActionMenuItem(this, i, _game->getResourcePack()->getFont("Big.fnt"), x, y);
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
	if (_action->weapon->getRules()->getTUUse() != 0)
	{
		tu = _action->weapon->getRules()->getTUUse();
		ss2 << strTU.c_str() << tu;
		_actionMenu[id]->setAction(BA_MEDIKIT, _game->getLanguage()->getString("STR_USE_MEDI_KIT"), L"", ss2.str(), tu);
		_actionMenu[id]->setVisible(true);
		id++;
		ss2.str(L"");
	}

}

/**
 * Deletes the ActionMenuState
 */
ActionMenuState::~ActionMenuState()
{

}

/**
 * Closes the window on right-click.
 * @param action Pointer to an action.
 */
void ActionMenuState::handle(Action *action)
{
	State::handle(action);

	if (action->getDetails()->type == SDL_MOUSEBUTTONDOWN && action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		_game->popState();
	}

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
			if (Options::getBool("battleAltGrenade"))
			{
				_action->value = 0;
				_game->popState();
			}
			else
			{
				_game->pushState(new PrimeGrenadeState(_game, _action));
			}
		}
		else if (_action->type == BA_MEDIKIT)
		{
			BattleUnit *targetUnit = NULL;
			std::vector<BattleUnit*> *const units (_game->getSavedGame()->getBattleGame()->getUnits());
			for(std::vector<BattleUnit*>::const_iterator i = units->begin (); i != units->end () && !targetUnit; ++i)
			{
				// we can heal a unit that is at the same position, unconscious and healable(=woundable)
				if ((*i)->getPosition() == _action->actor->getPosition() && *i != _action->actor && (*i)->getStatus () == STATUS_UNCONSCIOUS && (*i)->getUnit()->isWoundable())
				{
					targetUnit = *i;
				}
			}
			if (!targetUnit)
			{
				Position p;
				Pathfinding::directionToVector(_action->actor->getDirection(), &p);
				Tile * tile (_game->getSavedGame()->getBattleGame()->getTile(_action->actor->getPosition() + p));
				if (tile->getUnit() && tile->getUnit()->getUnit()->isWoundable())
					targetUnit = tile->getUnit();
			}
			if (targetUnit)
			{
				_game->pushState (new MedikitState (_game, targetUnit, _action));
			}
			else
			{
				_action->result = "STR_THERE_IS_NO_ONE_THERE";
				_game->popState();
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
