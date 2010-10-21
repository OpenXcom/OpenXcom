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
#include "StoresState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Resource/LangString.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Ruleset/Ruleset.h"
#include "../Savegame/Item.h"

using namespace std;

/**
 * Initializes all the elements in the Stores window.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 */
StoresState::StoresState(Game *game, Base *base) : State(game), _base(base)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 300, 16, 10, 176);
	_txtTitle = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 310, 16, 5, 8);
	_txtItem = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 142, 8, 10, 32);
	_txtQuantity = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 88, 8, 152, 32);
	_txtSpaceUsed = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 74, 8, 240, 32);
	_lstStores = new TextList(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 286, 128, 10, 40);
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtItem);
	add(_txtQuantity);
	add(_txtSpaceUsed);
	add(_lstStores);

	// Set up objects
	_window->setColor(Palette::blockOffset(13)+13);
	_window->setBackground(game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+13);
	_btnOk->setText(_game->getResourcePack()->getLanguage()->getString(STR_OK));
	_btnOk->onMouseClick((EventHandler)&StoresState::btnOkClick);

	_txtTitle->setColor(Palette::blockOffset(13)+10);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getResourcePack()->getLanguage()->getString(STR_STORES));

	_txtItem->setColor(Palette::blockOffset(13)+10);
	_txtItem->setText(_game->getResourcePack()->getLanguage()->getString(STR_ITEM));

	_txtQuantity->setColor(Palette::blockOffset(13)+10);
	_txtQuantity->setText(_game->getResourcePack()->getLanguage()->getString(STR_QUANTITY_UC));

	_txtSpaceUsed->setColor(Palette::blockOffset(13)+10);
	_txtSpaceUsed->setText(_game->getResourcePack()->getLanguage()->getString(STR_SPACE_USED));
	
	_lstStores->setColor(Palette::blockOffset(13)+10);
	_lstStores->setColumns(3, 162, 92, 32);
	_lstStores->setSelectable(true);
	_lstStores->setBackground(_window);

	for (map<LangString, Item*>::iterator i = _base->getItems()->begin(); i != _base->getItems()->end(); i++)
	{
		stringstream ss, ss2;
		ss << i->second->getQuantity();
		ss2 << i->second->getTotalSize();
		_lstStores->addRow(0, 3, _game->getResourcePack()->getLanguage()->getString(i->first).c_str(), ss.str().c_str(), ss2.str().c_str());
	}
}

/**
 *
 */
StoresState::~StoresState()
{
	
}

/**
 * Returns to the previous screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void StoresState::btnOkClick(SDL_Event *ev, int scale)
{
	_game->popState();
}
