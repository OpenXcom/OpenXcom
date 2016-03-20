/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "../Engine/Game.h"
#include "../Engine/Action.h"
#include "../Engine/Options.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Mod/Mod.h"

namespace OpenXcom
{	
	UfopaediaStartState::UfopaediaStartState()
	{
		_screen = false;

		// set background window
		_window = new Window(this, 256, 180, 32, 10, POPUP_BOTH);

		// set title
		_txtTitle = new Text(224, 17, 48, 33);

		// Set palette
		setInterface("ufopaedia");

		add(_window, "window", "ufopaedia");
		add(_txtTitle, "text", "ufopaedia");

		_btnOk = new TextButton(224, 12, 48, 167);
		add(_btnOk, "button1", "ufopaedia");

		// set buttons
		const std::vector<std::string> &list = _game->getMod()->getUfopaediaCategoryList();
		int y = 50;
		y -= 13 * (list.size() - 9);
		for (std::vector<std::string>::const_iterator i = list.begin(); i != list.end(); ++i)
		{
			TextButton *button = new TextButton(224, 12, 48, y);
			y += 13;

			add(button, "button1", "ufopaedia");

			button->setText(tr(*i));
			button->onMouseClick((ActionHandler)&UfopaediaStartState::btnSectionClick);

			_btnSections.push_back(button);
		}
		_txtTitle->setY(_btnSections.front()->getY() - _txtTitle->getHeight());

		centerAllSurfaces();

		_window->setBackground(_game->getMod()->getSurface("BACK01.SCR"));

		_txtTitle->setBig();
		_txtTitle->setAlign(ALIGN_CENTER);
		_txtTitle->setText(tr("STR_UFOPAEDIA"));
		
		_btnOk->setText(tr("STR_OK"));
		_btnOk->onMouseClick((ActionHandler)&UfopaediaStartState::btnOkClick);
		_btnOk->onKeyboardPress((ActionHandler)&UfopaediaStartState::btnOkClick, Options::keyCancel);
		_btnOk->onKeyboardPress((ActionHandler)&UfopaediaStartState::btnOkClick, Options::keyGeoUfopedia);
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
	}

	/**
	 * Displays the list of articles for this section.
	 * @param action Pointer to an action.
	 */
	void UfopaediaStartState::btnSectionClick(Action *action)
	{
		const std::vector<std::string> &list = _game->getMod()->getUfopaediaCategoryList();
		for (size_t i = 0; i < list.size(); ++i)
		{
			if (action->getSender() == _btnSections[i])
			{
				_game->pushState(new UfopaediaSelectState(list[i]));
				break;
			}
		}
	}

}
