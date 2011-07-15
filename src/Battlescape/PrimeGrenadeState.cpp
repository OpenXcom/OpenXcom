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
#include "PrimeGrenadeState.h"
#include <sstream>
#include <cmath>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Action.h"
#include "../Interface/Text.h"
#include "../Interface/Window.h"
#include "../Engine/InteractiveSurface.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"


namespace OpenXcom
{

/**
 * Initializes all the elements in the Prime Grenade window.
 * @param game Pointer to the core game.
 * @param action Pointer to  the action.
 */
PrimeGrenadeState::PrimeGrenadeState(Game *game, BattleAction *action) : State(game), _action(action)
{
	_screen = false;

	// Create objects
	_title = new Text(191, 24, 65, 45);
	_window = new Window(this, 191, 27, 65, 37);
	_bg = new Surface(191, 93, 65, 45);

	int x = 67;
	int y = 69;
	for (int i = 0; i < 24; ++i)
	{
		_button[i] = new InteractiveSurface(20, 20, x+((i%8)*24), y-3+((i/8)*25));
		_number[i] = new Text(20, 20, x+((i%8)*24), y+((i/8)*25));
	}

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_4")->getColors());

	// Set up objects
	SDL_Rect square;
	Uint8 fcolor = Palette::blockOffset(2)+9;
	Uint8 bcolor = Palette::blockOffset(2)+4;
	square.x = 0;
	square.y = 0;
	square.w = _bg->getWidth();
	square.h = _bg->getHeight();
	add(_bg);
	_bg->drawRect(&square, bcolor);
	_bg->draw();

	add(_window);
	_window->setColor(fcolor);

	add(_title);
	_title->setAlign(ALIGN_CENTER);
	_title->setFonts(_game->getResourcePack()->getFont("BIGLETS.DAT"), 0);
	_title->setBig();
	_title->setText(_game->getLanguage()->getString("STR_SET_TIMER"));
	_title->setVisible(true);
	_title->setColor(Palette::blockOffset(1)+1);

	for (int i = 0; i < 24; ++i)
	{
		add(_button[i]);
		_button[i]->onMouseClick((ActionHandler)&PrimeGrenadeState::btnClick);
		square.x = 0;
		square.y = 0;
		square.w = _button[i]->getWidth();
		square.h = _button[i]->getHeight();
		_button[i]->drawRect(&square, fcolor);
		_button[i]->draw();

		std::wstringstream ss;
		ss << i;
		add(_number[i]);
		_number[i]->setFonts(_game->getResourcePack()->getFont("BIGLETS.DAT"), 0);
		_number[i]->setBig();
		_number[i]->setText(ss.str());
		_number[i]->setColor(Palette::blockOffset(1)+1);
		_number[i]->setVisible(true);
		_number[i]->setAlign(ALIGN_CENTER);
		_number[i]->setVerticalAlign(ALIGN_MIDDLE);
	}


}

/**
 *
 */
PrimeGrenadeState::~PrimeGrenadeState()
{

}

/**
 *
 */
void PrimeGrenadeState::init()
{
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_4")->getColors());
}


/**
 * Execute the action corresponding with this action menu item.
 * @param action Pointer to an action.
 */
void PrimeGrenadeState::btnClick(Action *action)
{
	int btnID = -1;

	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		_action->value = btnID;
		_game->popState();
		return;
	}

	// got to find out which button was pressed
	for (int i = 0; i < 10 && btnID == -1; ++i)
	{
		if (action->getSender() == _button[i])
		{
			btnID = i;
		}
	}

	if (btnID != -1)
	{
		_action->value = btnID;
		_game->popState();
		_game->popState();
	}
}

}
