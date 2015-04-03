/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include "DefeatState.h"
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
 * Initializes all the elements in the Defeat screen.
 * @param game Pointer to the core game.
 */
DefeatState::DefeatState() : _screen(-1)
{
	Options::baseXResolution = Screen::ORIGINAL_WIDTH;
	Options::baseYResolution = Screen::ORIGINAL_HEIGHT;
	_game->getScreen()->resetDisplay(false);
	const char *files[] = {"PICT4.LBM", "PICT5.LBM"};

	_timer = new Timer(30000);

	_text[0] = new Text(190, 104, 0, 0);
	_text[1] = new Text(200, 34, 32, 0);

	for (int i = 0; i < SCREENS; ++i)
	{
		Surface *screen = _game->getResourcePack()->getSurface(files[i]);
		_bg[i] = new InteractiveSurface(320, 200, 0, 0);

		setPalette(screen->getPalette());

		add(_bg[i]);
		add(_text[i], "defeatText", "gameOver");

		screen->blit(_bg[i]);
		_bg[i]->setVisible(false);
		_bg[i]->onMouseClick((ActionHandler)&DefeatState::screenClick);

		std::ostringstream ss;
		ss << "STR_GAME_OVER_" << i + 1;
		_text[i]->setText(tr(ss.str()));
		_text[i]->setWordWrap(true);
		_text[i]->setVisible(false);
	}
	
	_game->getResourcePack()->playMusic("GMLOSE");

	centerAllSurfaces();

	_timer->onTimer((StateHandler)&DefeatState::screenTimer);
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
DefeatState::~DefeatState()
{
	delete _timer;
}

/**
 * Handle timers.
 */
void DefeatState::think()
{
	_timer->think(this, 0);
}

/**
 * Shows the next screen on a timed basis.
 */
void DefeatState::screenTimer()
{
	screenClick(0);
}

/**
 * Shows the next screen in the slideshow
 * or goes back to the Main Menu.
 * @param action Pointer to an action.
 */
void DefeatState::screenClick(Action *)
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
