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
#include "MedikitState.h"
#include "MedikitView.h"
#include "../Engine/InteractiveSurface.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Engine/CrossPlatform.h"
#include "../Engine/Action.h"
#include "../Engine/Palette.h"
#include "../Interface/Text.h"
#include "../Engine/Screen.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/BattleUnit.h"
#include "../Ruleset/RuleItem.h"
#include "../Resource/ResourcePack.h"
#include <iostream>
#include <sstream>
#include "../Engine/Options.h"

namespace OpenXcom
{
/**
 * Helper function return a string representation a type(mainly used for number)
 * t the value to stringify
 * @return a string representation of value
 */
template<typename type>
std::wstring toString (type t)
{
	std::wstringstream ss;
	ss << t;
	return ss.str();
}

/**
 * Helper class for medikit title
 */
class MedikitTitle : public Text
{
public:
	/// Create a medikit title
	MedikitTitle(int y, const std::wstring & title);
};

/**
 * Initialize a Medikit title
 * @param y the Title y origin
 * @param title the title
 */
MedikitTitle::MedikitTitle (int y, const std::wstring & title) : Text (60, 16, 192, y)
{
	this->setText(title);
	this->setHighContrast(true);
	this->setAlign(ALIGN_CENTER);
}

/**
 * Helper class for medikit value
 */
class MedikitTxt : public Text
{
public:
	/// Create a medikit text
	MedikitTxt(int y);
};

/**
 * Initialize a Medikit text
 * @param y the Text y origin
 */
MedikitTxt::MedikitTxt(int y) : Text(30, 22, 220, y)
{
	// Note: we can't set setBig here. The needed font is only setted when added to State
	this->setColor(Palette::blockOffset(1));
	this->setHighContrast(true);
	this->setAlign(ALIGN_CENTER);
	this->setVerticalAlign(ALIGN_MIDDLE);
}

/**
 * Helper class for medikit button
 */
class MedikitButton : public InteractiveSurface
{
public:
	/// Create a medikit button
	MedikitButton(int y);
};

/**
 * Initialize a Medikit button
 * @param y the button y origin
 */
MedikitButton::MedikitButton(int y) : InteractiveSurface(30, 20, 190, y)
{
}

/**
 * Initialize the Medikit State
 * @param game Pointer to the core game.
 * @param targetUnit The wounded unit.
 * @param action The healing action.
 */
MedikitState::MedikitState (Game * game, BattleUnit * targetUnit, BattleAction *action) : State (game), _targetUnit(targetUnit), _action(action)
{
	_unit = action->actor;
	_item = action->weapon;
	_surface = new InteractiveSurface(320, 200);

	if (Screen::getDY() > 50)
	{
		_screen = false;
		SDL_Rect current;
		current.w = 190;
		current.h = 100;
		current.x = 67;
		current.y = 44;
		_surface->drawRect(&current, Palette::blockOffset(15)+15);
	}
	_partTxt = new Text(50, 15, 90, 120);
	_woundTxt = new Text(10, 15, 145, 120);
	_medikitView = new MedikitView(52, 58, 95, 60, _game, _targetUnit, _partTxt, _woundTxt);
	InteractiveSurface *endButton = new InteractiveSurface(20, 20,
							       220, 140);

	InteractiveSurface *stimulantButton = new MedikitButton(84);

	InteractiveSurface *pkButton = new MedikitButton(48);
	InteractiveSurface *healButton = new MedikitButton(120);
	_pkText = new MedikitTxt (50);
	_stimulantTxt = new MedikitTxt (85);
	_healTxt = new MedikitTxt (120);
	add(_surface);
	add(_medikitView);
	add(endButton);
	add(new MedikitTitle (37, _game->getLanguage()->getString("STR_PAIN_KILLER")));
	add(new MedikitTitle (73, _game->getLanguage()->getString("STR_STIMULANT")));
	add(new MedikitTitle (109, _game->getLanguage()->getString("STR_HEAL")));
	add(healButton);
	add(stimulantButton);
	add(pkButton);
	add(_pkText);
	add(_stimulantTxt);
	add(_healTxt);
	add(_partTxt);
	add(_woundTxt);

	centerAllSurfaces();

	_game->getResourcePack()->getSurface("MEDIBORD.PCK")->blit(_surface);
	_pkText->setBig();
	_stimulantTxt->setBig();
	_healTxt->setBig();
	_partTxt->setColor(Palette::blockOffset(2));
	_partTxt->setHighContrast(true);
	_woundTxt->setColor(Palette::blockOffset(2));
	_woundTxt->setHighContrast(true);
	endButton->onMouseClick((ActionHandler)&MedikitState::onEndClick);
	endButton->onKeyboardPress((ActionHandler)&MedikitState::onEndClick, (SDLKey)Options::getInt("keyCancel"));
	healButton->onMouseClick((ActionHandler)&MedikitState::onHealClick);
	stimulantButton->onMouseClick((ActionHandler)&MedikitState::onStimulantClick);
	pkButton->onMouseClick((ActionHandler)&MedikitState::onPainKillerClick);
	update();
}

/**
 * Closes the window on right-click.
 * @param action Pointer to an action.
 */
void MedikitState::handle(Action *action)
{
	State::handle(action);
	if (action->getDetails()->type == SDL_MOUSEBUTTONDOWN && action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		_game->popState();
	}
}

/**
 * return to the previous screen
 * @param action Pointer to an action.
 */
void MedikitState::onEndClick(Action *)
{
	_game->popState();
}

/**
 * Handler for clicking on the heal button
 * @param action Pointer to an action.
 */
void MedikitState::onHealClick(Action *)
{
	int heal = _item->getHealQuantity();
	RuleItem *rule = _item->getRules();
	if (heal == 0)
	{
		return;
	}
	if (_unit->spendTimeUnits (rule->getTUUse()))
	{
		_targetUnit->heal(_medikitView->getSelectedPart(), rule->getHealAmount(), rule->getHealthAmount());
		_item->setHealQuantity(--heal);
		_medikitView->invalidate();
		update();
	}
	else
	{
		_action->result = "STR_NOT_ENOUGH_TIME_UNITS";
		_game->popState();
	}
}

/**
 * Handler for clicking on the stimulant button
 * @param action Pointer to an action.
 */
void MedikitState::onStimulantClick(Action *)
{
	int stimulant = _item->getStimulantQuantity();
	RuleItem *rule = _item->getRules();
	if (stimulant == 0)
	{
		return;
	}
	if (_unit->spendTimeUnits (rule->getTUUse()))
	{
		_targetUnit->stimulant(rule->getEnergy(), rule->getStun());
		_item->setStimulantQuantity(--stimulant);
		update();

		// if the unit has revived we quit this screen automatically
		if (_targetUnit->getStatus() == STATUS_UNCONSCIOUS && _targetUnit->getStunlevel() < _targetUnit->getHealth() && _targetUnit->getHealth() > 0)
		{
			_targetUnit->setTimeUnits(0);
			_game->popState();
		}
	}
	else
	{
		_action->result = "STR_NOT_ENOUGH_TIME_UNITS";
		_game->popState();
	}
}

/**
 * Handler for clicking on the pain killer button
 * @param action Pointer to an action.
 */
void MedikitState::onPainKillerClick(Action *)
{
	int pk = _item->getPainKillerQuantity();
	RuleItem *rule = _item->getRules();
	if (pk == 0)
	{
		return;
	}
	if (_unit->spendTimeUnits (rule->getTUUse()))
	{
		_targetUnit->painKillers();
		_item->setPainKillerQuantity(--pk);
		update();
	}
	else
	{
		_action->result = "STR_NOT_ENOUGH_TIME_UNITS";
		_game->popState();
	}
}

/**
 * Update medikit state
 */
void MedikitState::update()
{
	_pkText->setText(toString(_item->getPainKillerQuantity()));
	_stimulantTxt->setText(toString(_item->getStimulantQuantity()));
	_healTxt->setText(toString(_item->getHealQuantity()));
}

}
