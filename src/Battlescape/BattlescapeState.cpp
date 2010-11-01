/*
 * Copyright 2010 OpenXcom Developers
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
#define _USE_MATH_DEFINES
#include "BattlescapeState.h"
#include <cmath>
#include <string>
#include <sstream>
#include "../Engine/Game.h"
#include "../Engine/Timer.h"
#include "../Engine/Music.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Engine/RNG.h"
#include "../Engine/Surface.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Screen.h"
#include "../Resource/ResourcePack.h"
#include "../Resource/LangString.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/ImageButton.h"
#include "../Savegame/SavedGame.h"
#include "../Ruleset/Ruleset.h"
#include "Map.h"

/**
 * Initializes all the elements in the Battlescape screen.
 * @param game Pointer to the core game.
 */
BattlescapeState::BattlescapeState(Game *game) : State(game), _pause(false), _popups()
{

	// Create the battlemap view
	_map = new Map(320,200,0,0);

	// Create buttonbar
	_icons = new Surface(320, 200, 0, 0);

	// Create buttons
	_btnAbort = new InteractiveSurface(30, 15, 240, 160);
	_btnMapUp = new InteractiveSurface(30, 15, 80, 144);
	_btnMapDown = new InteractiveSurface(30, 15, 80, 160);

	// Create soldier stats summary
	_txtName = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 120, 10, 135, 176);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_4")->getColors());

	// last 16 colors is a grey gradient
	SDL_Color color[16];
	for (int i = 0; i < 16; i++)
	{
	color[i].r = 128 - (i + 1) * 8;
	color[i].g = 128 - (i + 1) * 8;
	color[i].b = 128 - (i + 1) * 8;
	}

	// leave cursor palette alone
	_game->getScreen()->setPalette(color, Palette::backPos+16, 16);
	_game->getResourcePack()->setPalette(color, Palette::backPos+16, 16);

	add(_map);
	add(_icons);
	add(_btnAbort);
	add(_btnMapUp);
	add(_btnMapDown);
	add(_txtName);

	// Set up objects
	_game->getResourcePack()->getSurface("ICONS.PCK")->blit(_icons);
	
	_map->setSavedGame(_game->getSavedGame()->getBattleGame());
	_map->setResourcePack(_game->getResourcePack());
	_map->init();

	_btnAbort->onMouseClick((ActionHandler)&BattlescapeState::btnAbortClick);
	_btnMapUp->onMouseClick((ActionHandler)&BattlescapeState::btnMapUpClick);
	_btnMapDown->onMouseClick((ActionHandler)&BattlescapeState::btnMapDownClick);

	_txtName->setColor(Palette::blockOffset(15)+4);
	_txtName->setText("");
	_txtName->setAlign(ALIGN_LEFT);

	// Set music
	_game->getResourcePack()->getMusic("GMTACTIC")->play();
}

/**
 *
 */
BattlescapeState::~BattlescapeState()
{

}

/**
 * Updates the timer display and resets the palette
 * since it's bound to change on other screens.
 */
void BattlescapeState::init()
{
	_map->onMouseClick((ActionHandler)&BattlescapeState::mapClick);
	_map->focus();
	_map->draw();
}

/**
 * Runs the game timers
 */
void BattlescapeState::think()
{
	State::think();
	_map->think();
}


/**
 * Returns a pointer to the map for
 * access by other substates.
 */
Map *BattlescapeState::getMap()
{
	return _map;
}

/**
 * Processes any left-clicks on globe markers,
 * or right-clicks to scroll the globe.
 * @param action Pointer to an action.
 */

void BattlescapeState::mapClick(Action *action)
{
}

/**
 * Move unit up
 * @param action Pointer to an action.
 */
void BattlescapeState::btnUnitUpClick(Action *action)
{}

/**
 * Move unit down
 * @param action Pointer to an action.
 */
void BattlescapeState::btnUnitDownClick(Action *action)
{}

/**
 * Show next map layer
 * @param action Pointer to an action.
 */
void BattlescapeState::btnMapUpClick(Action *action)
{
	_map->up();
}

/**
 * Show previous map layer
 * @param action Pointer to an action.
 */
void BattlescapeState::btnMapDownClick(Action *action)
{
	_map->down();
}

/**
 * Show minimap
 * @param action Pointer to an action.
 */
void BattlescapeState::btnShowMapClick(Action *action)
{}

/**
 * Kneel/Standup 
 * @param action Pointer to an action.
 */
void BattlescapeState::btnKneelClick(Action *action)
{}

/**
 * Go to soldier info screen
 * @param action Pointer to an action.
 */
void BattlescapeState::btnSoldierClick(Action *action)
{}

/**
 * Center on current selected soldier
 * @param action Pointer to an action.
 */
void BattlescapeState::btnCenterClick(Action *action)
{}

/**
 * Select next soldier
 * @param action Pointer to an action.
 */
void BattlescapeState::btnNextSoldierClick(Action *action)
{}

/**
 * Select next soldier and ...
 * @param action Pointer to an action.
 */
void BattlescapeState::btnNextStopClick(Action *action)
{}

/**
 * Show/hide all map layers
 * @param action Pointer to an action.
 */
void BattlescapeState::btnShowLayersClick(Action *action)
{}

/**
 * Show help screen
 * @param action Pointer to an action.
 */
void BattlescapeState::btnHelpClick(Action *action)
{}

/**
 * End turn
 * @param action Pointer to an action.
 */
void BattlescapeState::btnEndTurnClick(Action *action)
{}

/**
 * Abort game.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnAbortClick(Action *action)
{
	_game->getRuleset()->endBattle(_game->getSavedGame()->getBattleGame(),_game->getSavedGame());
	_game->popState();

	// Set music
	std::stringstream ss;
	ss << "GMGEO" << RNG::generate(1, 2);
	_game->getResourcePack()->getMusic(ss.str())->play();
}
