/*
 * Copyright 2010 Daniel Albano
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
#include "BuildFacilitiesState.h"

/**
 * Initializes all the elements in the Build Facilities window.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param state Pointer to the base state to refresh.
 */
BuildFacilitiesState::BuildFacilitiesState(Game *game, Base *base, State *state) : State(game), _base(base), _state(state)
{
	_screen = false;

	// Create objects
	_window = new Window(128, 160, 192, 40, POPUP_VERTICAL);
	_btnOk = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 112, 16, 200, 176);
	_txtTitle = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 118, 16, 197, 48);
	_lstFacilities = new TextList(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 110, 110, 202, 64);
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_lstFacilities);

	// Set up objects
	_window->setColor(Palette::blockOffset(13)+8);
	_window->setBackground(game->getResourcePack()->getSurface("BACK05.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+8);
	_btnOk->setText(_game->getResourcePack()->getLanguage()->getString(STR_OK));
	_btnOk->onMouseClick((EventHandler)&BuildFacilitiesState::btnOkClick);

	_txtTitle->setColor(Palette::blockOffset(13));
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getResourcePack()->getLanguage()->getString(STR_INSTALLATION));

	_lstFacilities->setColor(Palette::blockOffset(13)+5);
	_lstFacilities->setColumns(1, 110);
	_lstFacilities->setSelectable(true);
	_lstFacilities->setBackground(_window);
	for (int i = STR_LIVING_QUARTERS; i <= STR_ALIEN_CONTAINMENT; i++)
		_lstFacilities->addRow(1, _game->getResourcePack()->getLanguage()->getString((LangString)i).c_str());
	_lstFacilities->addRow(1, _game->getResourcePack()->getLanguage()->getString(STR_HANGAR).c_str());
	_lstFacilities->onMouseClick((EventHandler)&BuildFacilitiesState::lstFacilitiesClick);
}

/**
 *
 */
BuildFacilitiesState::~BuildFacilitiesState()
{
	
}

/**
 * The player can change the selected base
 * or change info on other screens.
 */
void BuildFacilitiesState::init()
{
	_state->init();
}

/**
 * Returns to the previous screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void BuildFacilitiesState::btnOkClick(SDL_Event *ev, int scale)
{
	_game->popState();
}

/**
 * Places the selected facility.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void BuildFacilitiesState::lstFacilitiesClick(SDL_Event *ev, int scale)
{
	LangString facilities[9];
	for (int i = 0; i < 8; i++)
		facilities[i] = (LangString)(STR_LIVING_QUARTERS + i);
	facilities[8] = STR_HANGAR;

	_game->pushState(new PlaceFacilityState(_game, _base, _game->getRuleset()->getBaseFacility(facilities[_lstFacilities->getSelectedRow()])));
}
