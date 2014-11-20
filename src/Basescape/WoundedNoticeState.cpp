/*
* Copyright 2010-2014 OpenXcom Developers.
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
#include "WoundedNoticeState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Savegame/Base.h"

namespace OpenXcom
{

	/**
	 * Initializes all the elements in the Wounded Notice window.
	 * @param base Pointer to the destination base.
	 */
	WoundedNoticeState::WoundedNoticeState()
	{
		_screen = false;

		// Create objects
		_window = new Window(this, 250, 100, 30, 50);
		_btnOk = new TextButton(50, 16, 130, 118);
		_txtTitle = new Text(160, 17, 75, 70);
		_txtNotice = new Text(210, 17, 50, 90);

		// Set palette
		setPalette("PAL_BASESCAPE", 2);

		add(_window);
		add(_btnOk);
		add(_txtTitle);
		add(_txtNotice);

		centerAllSurfaces();

		// Set up objects
		_window->setColor(Palette::blockOffset(15) + 1);
		_window->setBackground(_game->getResourcePack()->getSurface("BACK02.SCR"));

		_btnOk->setColor(Palette::blockOffset(13) + 10);
		_btnOk->setText(tr("STR_OK"));
		_btnOk->onMouseClick((ActionHandler)&WoundedNoticeState::btnOkClick);
		_btnOk->onKeyboardPress((ActionHandler)&WoundedNoticeState::btnOkClick, Options::keyOk);

		_txtTitle->setColor(Palette::blockOffset(13) + 10);
		_txtTitle->setBig();
		_txtTitle->setAlign(ALIGN_CENTER);
		_txtTitle->setText(tr("STR_WOUNDED_SOLDIER_TITLE"));

		_txtNotice->setColor(Palette::blockOffset(15) + 1);
		_txtNotice->setText(tr("STR_WOUNDED_SOLDIER_NOTICE"));
	}

	/**
	 *
	 */
	WoundedNoticeState::~WoundedNoticeState()
	{
	}

	/**
	 * Dismisses the notice.
	 * @param action Pointer to an action.
	 */
	void WoundedNoticeState::btnOkClick(Action *)
	{
		_game->popState();
	}
}
