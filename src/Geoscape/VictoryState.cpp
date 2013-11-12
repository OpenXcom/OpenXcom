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
#include "VictoryState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/Text.h"
#include "../Engine/InteractiveSurface.h"
#include "../Savegame/SavedGame.h"
#include "../Menu/MainMenuState.h"
#include "../Engine/Music.h"
#include "../Engine/Timer.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Victory screen.
 * @param game Pointer to the core game.
 */
VictoryState::VictoryState(Game *game) : State(game), _screenNumber(0)
{
	// Create objects
	_screen = new InteractiveSurface(320, 200, 0, 0);
	_txtText.push_back(new Text(195, 56, 5, 0));
	_txtText.push_back(new Text(232, 64, 88, 136));
	_txtText.push_back(new Text(254, 48, 66, 152));
	_txtText.push_back(new Text(300, 200, 5, 0));
	_txtText.push_back(new Text(310, 42, 5, 158));
	_timer = new Timer(20000);

	add(_screen);
	// Set up objects
	_screen->onMouseClick((ActionHandler)&VictoryState::windowClick);
	
	_game->getResourcePack()->getMusic("GMWIN")->play();

	for (int text = 0; text != 5; ++text)
	{
		std::ostringstream ss2;
		ss2 << "STR_VICTORY_" << text+1;
		_txtText[text]->setText(tr(ss2.str()));
		_txtText[text]->setWordWrap(true);
		add(_txtText[text]);
		_txtText[text]->setVisible(false);
	}

	centerAllSurfaces();

	_timer->onTimer((StateHandler)&VictoryState::windowClick);
	_timer->start();
}

/**
 *
 */
VictoryState::~VictoryState()
{
	delete _timer;
}

void VictoryState::init()
{
	nextScreen();
}

void VictoryState::think()
{
	_timer->think(this, 0);
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void VictoryState::windowClick(Action *)
{
	if(_screenNumber == 5)
	{
		_game->popState();
		_game->setState(new MainMenuState(_game));
		_game->setSavedGame(0);
	}
	else
		nextScreen();
}

void VictoryState::nextScreen()
{
	++_screenNumber;
	int offset = 0;
	if(_screenNumber>3)
		offset = 2;
	std::ostringstream ss;
	ss << "PICT" << _screenNumber+offset << ".LBM";
	_game->setPalette(_game->getResourcePack()->getSurface(ss.str())->getPalette());
	_screen->setPalette(_game->getResourcePack()->getSurface(ss.str())->getPalette());
	_game->getResourcePack()->getSurface(ss.str())->blit(_screen);
	_txtText[_screenNumber-1]->setPalette(_game->getResourcePack()->getSurface(ss.str())->getPalette());
	_txtText[_screenNumber-1]->setColor(Palette::blockOffset(15)+9);
	_txtText[_screenNumber-1]->setVisible(true);
	if (_screenNumber > 1)
		_txtText[_screenNumber-2]->setVisible(false);
}

}
