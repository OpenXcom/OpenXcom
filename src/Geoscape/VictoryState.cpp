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
#include "../Engine/CrossPlatform.h"
#include "../Engine/Options.h"
#include "../Engine/Screen.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Victory screen.
 * @param game Pointer to the core game.
 */
VictoryState::VictoryState() : _screen(-1)
{
	Options::baseXResolution = Screen::ORIGINAL_WIDTH;
	Options::baseYResolution = Screen::ORIGINAL_HEIGHT;
	_game->getScreen()->resetDisplay(false);
	const char *files[] = {"PICT1.LBM", "PICT2.LBM", "PICT3.LBM", "PICT6.LBM", "PICT7.LBM"};

	_timer = new Timer(30000);

	_text[0] = new Text(195, 56, 5, 0);
	_text[1] = new Text(232, 64, 88, 136);
	_text[2] = new Text(254, 48, 66, 152);
	_text[3] = new Text(300, 200, 5, 0);
	_text[4] = new Text(310, 42, 5, 158);

	for (int i = 0; i < SCREENS; ++i)
	{
		Surface *screen = _game->getResourcePack()->getSurface(files[i]);
		_bg[i] = new InteractiveSurface(320, 200, 0, 0);

		setPalette(screen->getPalette());

		add(_bg[i]);
		add(_text[i], "victoryText", "gameOver");

		screen->blit(_bg[i]);
		_bg[i]->setVisible(false);
		_bg[i]->onMouseClick((ActionHandler)&VictoryState::screenClick);

		std::ostringstream ss;
		ss << "STR_VICTORY_" << i + 1;
		_text[i]->setText(tr(ss.str()));
		_text[i]->setWordWrap(true);
		_text[i]->setVisible(false);
	}
	
	_game->getResourcePack()->playMusic("GMWIN");

	centerAllSurfaces();

	_timer->onTimer((StateHandler)&VictoryState::screenTimer);
	_timer->start();

	screenClick(0);

	// Ironman is over
	if (_game->getSavedGame()->isIronman())
	{
		std::string filename = CrossPlatform::sanitizeFilename(Language::wstrToFs(_game->getSavedGame()->getName())) + ".sav";
		CrossPlatform::deleteFile(Options::getUserFolder() + filename);
	}
}

/**
 *
 */
VictoryState::~VictoryState()
{
	delete _timer;
}

/**
 * Shows the next screen on a timed basis.
 */
void VictoryState::screenTimer()
{
	screenClick(0);
}

/**
 * Handle timers.
 */
void VictoryState::think()
{
	_timer->think(this, 0);
}

/**
 * Shows the next screen in the slideshow
 * or goes back to the Main Menu.
 * @param action Pointer to an action.
 */
void VictoryState::screenClick(Action *)
{
	_timer->start();
	if (_screen >= 0)
	{
		_bg[_screen]->setVisible(false);
		_text[_screen]->setVisible(false);
	}

	++_screen;

	// next screen
	if (_screen < SCREENS)
	{
		setPalette(_bg[_screen]->getPalette());
		_bg[_screen]->setVisible(true);
		_text[_screen]->setVisible(true);
		init();
	}
	// quit game
	else
	{
		_game->popState();
		Screen::updateScale(Options::geoscapeScale, Options::geoscapeScale, Options::baseXGeoscape, Options::baseYGeoscape, true);
		_game->getScreen()->resetDisplay(false);
		_game->setState(new MainMenuState);
		_game->setSavedGame(0);
	}
}

}
