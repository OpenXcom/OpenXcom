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
#include "InventoryState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Interface/Text.h"
#include "../Engine/Action.h"
#include "../Engine/InteractiveSurface.h"
#include "../Engine/SurfaceSet.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/Soldier.h"
#include "Inventory.h"
#include "../Ruleset/Ruleset.h"
#include "UnitInfoState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Inventory screen.
 * @param game Pointer to the core game.
 */
InventoryState::InventoryState(Game *game) : State(game)
{
	_battleGame = _game->getSavedGame()->getBattleGame();

	// Create objects
	_bg = new Surface(320, 200, 0, 0);
	_soldier = new Surface(320, 200, 0, 0);
	_txtName = new Text(200, 16, 36, 6);
	_txtTus = new Text(40, 9, 250, 24);
	_btnOk = new InteractiveSurface(35, 22, 237, 1);
	_btnPrev = new InteractiveSurface(23, 22, 273, 1);
	_btnNext = new InteractiveSurface(23, 22, 297, 1);
	_btnUnload = new InteractiveSurface(32, 25, 288, 32);
	_btnGround = new InteractiveSurface(32, 15, 289, 137);
	_btnRank = new InteractiveSurface(26, 23, 0, 0);
	_inv = new Inventory(_game, 320, 200, 0, 0);

	add(_bg);
	add(_soldier);
	add(_txtName);
	add(_txtTus);
	add(_btnOk);
	add(_btnPrev);
	add(_btnNext);
	add(_btnUnload);
	add(_btnGround);
	add(_btnRank);
	add(_inv);

	// Set up objects
	_game->getResourcePack()->getSurface("TAC01.SCR")->blit(_bg);

	_txtName->setColor(Palette::blockOffset(4)-1);
	_txtName->setBig();
	_txtName->setHighContrast(true);

	_btnOk->onMouseClick((ActionHandler)&InventoryState::btnOkClick);
	_btnPrev->onMouseClick((ActionHandler)&InventoryState::btnPrevClick);
	_btnNext->onMouseClick((ActionHandler)&InventoryState::btnNextClick);
	_btnRank->onMouseClick((ActionHandler)&InventoryState::btnRankClick);

	_inv->setRuleInventory(_game->getRuleset()->getInventories());
	_inv->draw();
}

/**
 *
 */
InventoryState::~InventoryState()
{
	
}

/**
 * Update soldier stats when the soldier changes.
 */
void InventoryState::init()
{
	BattleUnit *unit = _battleGame->getSelectedUnit();
	_soldier->clear();
	_btnRank->clear();

	_txtName->setText(unit->getUnit()->getName());

	Soldier *s = dynamic_cast<Soldier*>(unit->getUnit());
	if (s)
	{
		SurfaceSet *texture = _game->getResourcePack()->getSurfaceSet("BASEBITS.PCK");
		texture->getFrame(s->getRankSprite())->setX(0);
		texture->getFrame(s->getRankSprite())->setY(0);
		texture->getFrame(s->getRankSprite())->blit(_btnRank);

		std::string look = "MAN_0";
		if (s->getGender() == GENDER_MALE)
			look += "M";
		else
			look += "F";
		if (s->getLook() == LOOK_BLONDE)
			look += "0";
		if (s->getLook() == LOOK_BROWNHAIR)
			look += "1";
		if (s->getLook() == LOOK_ORIENTAL)
			look += "2";
		if (s->getLook() == LOOK_AFRICAN)
			look += "3";
		look += ".SPK";
		_game->getResourcePack()->getSurface(look)->blit(_soldier);
	}
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void InventoryState::btnOkClick(Action *action)
{
	_game->popState();
}

/**
 * Selects the previous soldier.
 * @param action Pointer to an action.
 */
void InventoryState::btnPrevClick(Action *action)
{
	_battleGame->selectPreviousPlayerUnit();
	init();
}

/**
 * Selects the next soldier.
 * @param action Pointer to an action.
 */
void InventoryState::btnNextClick(Action *action)
{
	_battleGame->selectNextPlayerUnit();
	init();
}

/**
 * Shows the unit info screen.
 * @param action Pointer to an action.
 */
void InventoryState::btnRankClick(Action *action)
{
	_game->pushState(new UnitInfoState(_game, _battleGame->getSelectedUnit()));
}

}
