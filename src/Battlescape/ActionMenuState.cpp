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
#include "ScannerState.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "Pathfinding.h"
#include "../Ruleset/Armor.h"

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
		_actionMenu[i] = new ActionMenuItem(i, _game->getResourcePack()->getFont("Big.fnt"), x, y);
		add(_actionMenu[i]);
		_actionMenu[i]->setVisible(false);
		_actionMenu[i]->onMouseClick((ActionHandler)&ActionMenuState::btnActionMenuItemClick);
	}

	// Build up the popup menu
	int id = 0;
	RuleItem *weapon = _action->weapon->getRules();

	// throwing (if not a fixed weapon)
	if (!weapon->isFixed())
	{
		addItem(BA_THROW, "STR_THROW", &id);
	}

	// priming
	if ((weapon->getBattleType() == BT_GRENADE || weapon->getBattleType() == BT_PROXIMITYGRENADE)
		&& _action->weapon->getExplodeTurn() == 0)
	{
		addItem(BA_PRIME, "STR_PRIME_GRENADE", &id);
	}

	if (weapon->getBattleType() == BT_FIREARM)
	{
		if (weapon->isWaypoint())
		{
			addItem(BA_LAUNCH, "STR_LAUNCH_MISSILE", &id);
		}
		else
		{
			if (weapon->getAccuracyAuto() != 0)
			{
				addItem(BA_AUTOSHOT, "STR_AUTO_SHOT", &id);
			}
			if (weapon->getAccuracySnap() != 0)
			{
				addItem(BA_SNAPSHOT, "STR_SNAP_SHOT", &id);
			}
			if (weapon->getAccuracyAimed() != 0)
			{
				addItem(BA_AIMEDSHOT, "STR_AIMED_SHOT", &id);
			}
		}
	}
	else if (weapon->getBattleType() == BT_MELEE)
	{
		// stun rod
		if (weapon->getDamageType() == DT_STUN)
		{
			addItem(BA_HIT, "STR_STUN", &id);
		}
		else
		// melee weapon
		{
			addItem(BA_HIT, "STR_HIT", &id);
		}
	}
	// special items
	else if (weapon->getBattleType() == BT_MEDIKIT)
	{
		addItem(BA_USE, "STR_USE_MEDI_KIT", &id);
	}
	else if (weapon->getBattleType() == BT_SCANNER)
	{
		addItem(BA_USE, "STR_USE_SCANNER", &id);
	}
	else if (weapon->getBattleType() == BT_PSIAMP && _action->actor->getStats()->psiSkill > 0)
	{
		addItem(BA_MINDCONTROL, "STR_MIND_CONTROL", &id);
		addItem(BA_PANIC, "STR_PANIC_UNIT", &id);
	}
	else if (weapon->getBattleType() == BT_MINDPROBE)
	{
		addItem(BA_USE, "STR_USE_MIND_PROBE", &id);
	}

}

/**
 * Deletes the ActionMenuState
 */
ActionMenuState::~ActionMenuState()
{

}

/**
 * Adds a new menu item for an action.
 * @param ba Action type.
 * @param name Action description.
 * @param id Pointer to the new item ID.
 */
void ActionMenuState::addItem(BattleActionType ba, const std::string &name, int *id)
{
	std::wstringstream ss1, ss2;
	int acc = (int)floor(_action->actor->getFiringAccuracy(ba, _action->weapon) * 100);
	if (ba == BA_THROW)
		acc = (int)floor(_action->actor->getThrowingAccuracy() * 100);
	int tu = _action->actor->getActionTUs(ba, _action->weapon);

	if (acc > 0)
		ss1 << _game->getLanguage()->getString("STR_ACC") << acc << "%";
	ss2 << _game->getLanguage()->getString("STR_TUS") << tu;
	_actionMenu[*id]->setAction(ba, _game->getLanguage()->getString(name), ss1.str(), ss2.str(), tu);
	_actionMenu[*id]->setVisible(true);
	(*id)++;
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
	RuleItem *weapon = _action->weapon->getRules();

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
			if (Options::getBool("battleAltGrenade") || weapon->getBattleType() == BT_PROXIMITYGRENADE)
			{
				_action->value = 1;
				_game->popState();
			}
			else
			{
				_game->pushState(new PrimeGrenadeState(_game, _action, false, 0));
			}
		}
		else if (_action->type == BA_USE && weapon->getBattleType() == BT_MEDIKIT)
		{
			BattleUnit *targetUnit = NULL;
			std::vector<BattleUnit*> *const units (_game->getSavedGame()->getBattleGame()->getUnits());
			for(std::vector<BattleUnit*>::const_iterator i = units->begin (); i != units->end () && !targetUnit; ++i)
			{
				// we can heal a unit that is at the same position, unconscious and healable(=woundable)
				if ((*i)->getPosition() == _action->actor->getPosition() && *i != _action->actor && (*i)->getStatus () == STATUS_UNCONSCIOUS && (*i)->isWoundable())
				{
					targetUnit = *i;
				}
			}
			if (!targetUnit)
			{
				Position p;
				Pathfinding::directionToVector(_action->actor->getDirection(), &p);
				Tile * tile (_game->getSavedGame()->getBattleGame()->getTile(_action->actor->getPosition() + p));
				if (tile->getUnit() && tile->getUnit()->isWoundable())
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
		else if (_action->type == BA_USE && weapon->getBattleType() == BT_SCANNER)
		{
			// spend TUs first, then show the scanner
			if (_action->actor->spendTimeUnits (_action->TU))
			{
				_game->pushState (new ScannerState (_game, _action));
			}
			else
			{
				_action->result = "STR_NOT_ENOUGH_TIME_UNITS";
				_game->popState();
			}
		}
		else if (_action->type == BA_LAUNCH)
		{
			// check beforehand if we have enough time units
			if (_action->TU > _action->actor->getTimeUnits())
			{
				_action->result = "STR_NOT_ENOUGH_TIME_UNITS";
			}
			else if (_action->weapon->getAmmoItem() ==0 || (_action->weapon->getAmmoItem() && _action->weapon->getAmmoItem()->getAmmoQuantity() == 0))
			{
				_action->result = "STR_NO_AMMUNITION_LOADED";
			}
			else
			{
				_action->targeting = true;
			}
			_game->popState();
		}
		else if ((_action->type == BA_STUN || _action->type == BA_HIT) && weapon->getBattleType() == BT_MELEE)
		{
			BattleUnit *targetUnit = NULL;
			Position p;
			Pathfinding::directionToVector(_action->actor->getDirection(), &p);
			for (int x = 0; x != _action->actor->getArmor()->getSize(); ++x)
			{
				for (int y = 0; y != _action->actor->getArmor()->getSize(); ++y)
				{
					Tile * tile (_game->getSavedGame()->getBattleGame()->getTile(Position(_action->actor->getPosition().x + x, _action->actor->getPosition().y + y, _action->actor->getPosition().z) + p));		
					if (tile)
					{
							if (tile->getUnit() && tile->getUnit() != _action->actor)
						{
							BattleUnit *target (tile->getUnit());
							for (std::vector<BattleUnit*>::iterator b = _action->actor->getVisibleUnits()->begin(); b != _action->actor->getVisibleUnits()->end(); ++b)
							{
								if (*b == target && !_game->getSavedGame()->getBattleGame()->getPathfinding()->isBlocked(_game->getSavedGame()->getBattleGame()->getTile(_action->actor->getPosition() + Position(x, y, 0)), tile, _action->actor->getDirection(), 0))
								{
									targetUnit = tile->getUnit();
									break;
								}
							}
						}
					}
					if (targetUnit)
						break;
				}
				if (targetUnit)
					break;
			}


			if (targetUnit)
			{
				_game->popState();
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
