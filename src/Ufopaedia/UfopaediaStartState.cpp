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

#include "UfopaediaStartState.h"
#include "UfopaediaSelectState.h"
#include "Ufopaedia.h"
#include "../Ruleset/ArticleDefinition.h"
#include "../Engine/Game.h"
#include "../Engine/Action.h"
#include "../Engine/Options.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Engine/Language.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Resource/ResourcePack.h"

namespace OpenXcom
{
	const std::string UfopaediaStartState::SECTIONS[] = {UFOPAEDIA_XCOM_CRAFT_ARMAMENT,
	                                                     UFOPAEDIA_HEAVY_WEAPONS_PLATFORMS,
	                                                     UFOPAEDIA_WEAPONS_AND_EQUIPMENT,
	                                                     UFOPAEDIA_ALIEN_ARTIFACTS,
	                                                     UFOPAEDIA_BASE_FACILITIES,
	                                                     UFOPAEDIA_ALIEN_LIFE_FORMS,
	                                                     UFOPAEDIA_ALIEN_RESEARCH,
	                                                     UFOPAEDIA_UFO_COMPONENTS,
	                                                     UFOPAEDIA_UFOS};
	
	UfopaediaStartState::UfopaediaStartState(Game *game) : State(game)
	{
		_screen = false;

		// set background window
		_window = new Window(this, 256, 180, 32, 10, POPUP_BOTH);

		// set title
		_txtTitle = new Text(224, 17, 48, 33);

		// set buttons
		int y = 50;
		for (int i = 0; i < NUM_SECTIONS; ++i)
		{
			_btnSection[i] = new TextButton(224, 12, 48, y);
			y += 13;
		}
		_btnOk = new TextButton(224, 12, 48, y);

		// Set palette
		_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_0")->getColors());

		add(_window);
		add(_txtTitle);
		for (int i = 0; i < NUM_SECTIONS; ++i)
		{
			add(_btnSection[i]);
		}
		add(_btnOk);

		centerAllSurfaces();

		_window->setColor(Palette::blockOffset(15)-1);
		_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

		_txtTitle->setColor(Palette::blockOffset(8)+10);
		_txtTitle->setBig();
		_txtTitle->setAlign(ALIGN_CENTER);
		_txtTitle->setText(tr("STR_UFOPAEDIA"));
		
		for (int i = 0; i < NUM_SECTIONS; ++i)
		{
			_btnSection[i]->setColor(Palette::blockOffset(8)+5);
			_btnSection[i]->setText(tr(SECTIONS[i]));
			_btnSection[i]->onMouseClick((ActionHandler)&UfopaediaStartState::btnSectionClick);
		}		

		_btnOk->setColor(Palette::blockOffset(8)+5);
		_btnOk->setText(tr("STR_OK"));
		_btnOk->onMouseClick((ActionHandler)&UfopaediaStartState::btnOkClick);
		_btnOk->onKeyboardPress((ActionHandler)&UfopaediaStartState::btnOkClick, (SDLKey)Options::getInt("keyCancel"));
	}

	UfopaediaStartState::~UfopaediaStartState()
	{}

	/**
	 * Returns to the previous screen.
	 * @param action Pointer to an action.
	 */
	void UfopaediaStartState::btnOkClick(Action *)
	{
		_game->popState();
//		_game->quit();
	}

	/**
	 * Displays the list of articles for this section.
	 * @param action Pointer to an action.
	 */
	void UfopaediaStartState::btnSectionClick(Action *action)
	{
		for (int i = 0; i < NUM_SECTIONS; ++i)
		{
			if (action->getSender() == _btnSection[i])
			{
				_game->pushState(new UfopaediaSelectState(_game, SECTIONS[i]));
				break;
			}
		}
	}

}
