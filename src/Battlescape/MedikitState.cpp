/*
 * Copyright 2011 OpenXcom Developers.
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
#include "../Savegame/BattleItem.h"
#include "../Savegame/BattleUnit.h"
#include "../Ruleset/RuleItem.h"
#include "../Resource/ResourcePack.h"
#include <iostream>
#include <sstream>

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
 * @param targetUnit Tthe wounded unit
 * @param unit The unit hodling the medikit
 * @param item The medikit item
 */
MedikitState::MedikitState (Game * game, BattleUnit * targetUnit, BattleUnit * unit, BattleItem *item) : State (game), _targetUnit(targetUnit), _unit(unit), _item(item)
{
	_surface = new InteractiveSurface(320, 200);
	_surface->loadSpk(CrossPlatform::getDataFile("UFOGRAPH/MEDIBORD.PCK"));
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
	_pkText->setBig();
	_stimulantTxt->setBig();
	_healTxt->setBig();
	_partTxt->setColor(Palette::blockOffset(2));
	_partTxt->setHighContrast(true);
	_woundTxt->setColor(Palette::blockOffset(2));
	_woundTxt->setHighContrast(true);
	endButton->onMouseClick((ActionHandler)&MedikitState::onEndClick);
	healButton->onMouseClick((ActionHandler)&MedikitState::onHealClick);
	stimulantButton->onMouseClick((ActionHandler)&MedikitState::onStimulantClick);
	pkButton->onMouseClick((ActionHandler)&MedikitState::onPainKillerClick);
	update();
}

/**
 * return to the previous screen
 * @param action Pointer to an action.
 */
void MedikitState::onEndClick(Action * action)
{
	_game->popState();
	_game->popState();
}

/**
 * Handler for clicking on the heal button
 * @param action Pointer to an action.
 */
void MedikitState::onHealClick(Action * action)
{
	int heal = _item->getHealQuantity();
	RuleItem *rule = _item->getRules();
	if (heal == 0 || _unit->getTimeUnits () < rule->getTUMedikit())
	{
		return;
	}
	_targetUnit->heal(_medikitView->getSelectedPart(), rule->getHealAmount(), rule->getHealthAmount());
	_item->setHealQuantity(--heal);
	_unit->spendTimeUnits (rule->getTUMedikit(), false);
	_medikitView->invalidate();
	update();
}

/**
 * Handler for clicking on the stimulant button
 * @param action Pointer to an action.
 */
void MedikitState::onStimulantClick(Action * action)
{
	int stimulant = _item->getStimulantQuantity();
	RuleItem *rule = _item->getRules();
	if (stimulant == 0 || _unit->getTimeUnits () < rule->getTUMedikit())
	{
		return;
	}
	if (_targetUnit->getStatus () == STATUS_UNCONSCIOUS &&
	    _targetUnit->getTile() != _unit->getTile())
	{
		return;
	}
	_targetUnit->stimulant(rule->getEnergy(), rule->getStun());
	_unit->spendTimeUnits (rule->getTUMedikit(), false);
	_item->setStimulantQuantity(--stimulant);
	update();
}

/**
 * Handler for clicking on the pain killer button
 * @param action Pointer to an action.
 */
void MedikitState::onPainKillerClick(Action * action)
{
	int pk = _item->getPainKillerQuantity();
	RuleItem *rule = _item->getRules();
	if (pk == 0 || _unit->getTimeUnits () < rule->getTUMedikit())
	{
		return;
	}
	_unit->spendTimeUnits (rule->getTUMedikit(), false);
	_item->setPainKillerQuantity(--pk);
	update();
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
