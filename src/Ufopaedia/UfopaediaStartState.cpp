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

#include "UfopaediaStartState.h"
#include "UfopaediaSelectState.h"
#include "../Ruleset/ArticleDefinition.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Engine/Language.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Resource/ResourcePack.h"

namespace OpenXcom
{
	UfopaediaStartState::UfopaediaStartState(Game *game) : State(game)
	{
		_screen = false;

		// set background window
		_window = new Window(this, 256, 180, 32, 10, POPUP_BOTH);

		// set title
		_txtTitle = new Text(224, 16, 48, 33);

		// set buttons
		_btnOk = new TextButton(224, 12, 48, 167);
		_btnCraftArmament = new TextButton(224, 12, 48, 50);
		_btnWeaponsEquipment = new TextButton(224, 12, 48, 76);
		_btnBaseFacilities = new TextButton(224, 12, 48, 102);
		_btnAlienLifeforms = new TextButton(224, 12, 48, 115);
		_btnAlienResearch = new TextButton(224, 12, 48, 128);
		_btnUfos = new TextButton(224, 12, 48, 154);

		// Set palette
		_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_0")->getColors());

		add(_window);
		add(_txtTitle);
		add(_btnOk);
		add(_btnCraftArmament);
		add(_btnWeaponsEquipment);
		add(_btnBaseFacilities);
		add(_btnAlienLifeforms);
		add(_btnAlienResearch);
		add(_btnUfos);

		_window->setColor(Palette::blockOffset(15)-1);
		_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

		_txtTitle->setColor(Palette::blockOffset(8)+10);
		_txtTitle->setBig();
		_txtTitle->setAlign(ALIGN_CENTER);
		_txtTitle->setText(_game->getLanguage()->getString("STR_UFOPAEDIA"));

		_btnOk->setColor(Palette::blockOffset(8)+5);
		_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
		_btnOk->onMouseClick((ActionHandler)&UfopaediaStartState::btnOkClick);

		_btnCraftArmament->setColor(Palette::blockOffset(8)+5);
		_btnCraftArmament->setText(_game->getLanguage()->getString("STR_XCOM_CRAFT_ARMAMENT"));
		_btnCraftArmament->onMouseClick((ActionHandler)&UfopaediaStartState::btnCraftArmamentClick);

		_btnWeaponsEquipment->setColor(Palette::blockOffset(8)+5);
		_btnWeaponsEquipment->setText(_game->getLanguage()->getString("STR_WEAPONS_AND_EQUIPMENT"));
		_btnWeaponsEquipment->onMouseClick((ActionHandler)&UfopaediaStartState::btnWeaponsEquipmentClick);

		_btnBaseFacilities->setColor(Palette::blockOffset(8)+5);
		_btnBaseFacilities->setText(_game->getLanguage()->getString("STR_BASE_FACILITIES"));
		_btnBaseFacilities->onMouseClick((ActionHandler)&UfopaediaStartState::btnBaseFacilitiesClick);

		_btnAlienLifeforms->setColor(Palette::blockOffset(8)+5);
		_btnAlienLifeforms->setText(_game->getLanguage()->getString("STR_ALIEN_LIFE_FORMS"));
		_btnAlienLifeforms->onMouseClick((ActionHandler)&UfopaediaStartState::btnAlienLifeformsClick);

		_btnAlienResearch->setColor(Palette::blockOffset(8)+5);
		_btnAlienResearch->setText(_game->getLanguage()->getString("STR_ALIEN_RESEARCH_UC"));
		_btnAlienResearch->onMouseClick((ActionHandler)&UfopaediaStartState::btnAlienResearchClick);

		_btnUfos->setColor(Palette::blockOffset(8)+5);
		_btnUfos->setText(_game->getLanguage()->getString("STR_UFOS"));
		_btnUfos->onMouseClick((ActionHandler)&UfopaediaStartState::btnUfosClick);
	}

	UfopaediaStartState::~UfopaediaStartState()
	{}

	/**
	 * Returns to the previous screen.
	 * @param action Pointer to an action.
	 */
	void UfopaediaStartState::btnOkClick(Action *action)
	{
		_game->popState();
//		_game->quit();
	}

	/**
	 *
	 * @param action Pointer to an action.
	 */
	void UfopaediaStartState::btnCraftArmamentClick(Action *action)
	{
		_game->pushState(new UfopaediaSelectState(_game, UFOPAEDIA_XCOM_CRAFT_ARMAMENT));
	}

	/**
	 *
	 * @param action Pointer to an action.
	 */
	void UfopaediaStartState::btnWeaponsEquipmentClick(Action *action)
	{
		_game->pushState(new UfopaediaSelectState(_game, UFOPAEDIA_WEAPONS_AND_EQUIPMENT));
	}

	/**
	 *
	 * @param action Pointer to an action.
	 */
	void UfopaediaStartState::btnBaseFacilitiesClick(Action *action)
	{
		_game->pushState(new UfopaediaSelectState(_game, UFOPAEDIA_BASE_FACILITIES));
	}

	/**
	 *
	 * @param action Pointer to an action.
	 */
	void UfopaediaStartState::btnAlienLifeformsClick(Action *action)
	{
		_game->pushState(new UfopaediaSelectState(_game, UFOPAEDIA_ALIEN_LIFE_FORMS));
	}

	/**
	 *
	 * @param action Pointer to an action.
	 */
	void UfopaediaStartState::btnAlienResearchClick(Action *action)
	{
		_game->pushState(new UfopaediaSelectState(_game, UFOPAEDIA_ALIEN_RESEARCH));
	}

	/**
	 *
	 * @param action Pointer to an action.
	 */
	void UfopaediaStartState::btnUfosClick(Action *action)
	{
		_game->pushState(new UfopaediaSelectState(_game, UFOPAEDIA_UFOS));
	}

}
