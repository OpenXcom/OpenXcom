/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include "NewBattleState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Ruleset/XcomRuleset.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Savegame/Craft.h"
#include "../Savegame/ItemContainer.h"
#include "../Battlescape/BattlescapeGenerator.h"
#include "../Battlescape/BattlescapeState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Difficulty window.
 * @param game Pointer to the core game.
 */
NewBattleState::NewBattleState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0, POPUP_BOTH);
	_txtTitle = new Text(320, 16, 0, 8);
	_btnOk = new TextButton(148, 16, 8, 176);
	_btnCancel = new TextButton(148, 16, 164, 176);

	add(_window);
	add(_txtTitle);
	add(_btnOk);
	add(_btnCancel);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_txtTitle->setColor(Palette::blockOffset(8)+10);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(L"THE SIMULATRON");

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&NewBattleState::btnOkClick);

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&NewBattleState::btnCancelClick);

	initSave();
}

/**
 *
 */
NewBattleState::~NewBattleState()
{
	
}

void NewBattleState::initSave()
{
	//_game->setRuleset(new XcomRuleset());
	Ruleset *rule = _game->getRuleset();
	SavedGame *save = new SavedGame(DIFF_BEGINNER);
	Base *base = new Base(rule);
	save->getBases()->push_back(base);
	Craft *craft = new Craft(rule->getCraft("STR_SKYRANGER"), base, save->getCraftIds());
	base->getCrafts()->push_back(craft);

	// Generate soldiers
	for (int i = 0; i < 30; ++i)
	{
		Soldier *soldier = new Soldier(rule->getSoldier("XCOM"), rule->getArmor("STR_NONE_UC"), rule->getPools(), save->getSoldierId());
		base->getSoldiers()->push_back(soldier);
		if (i < 8)
			soldier->setCraft(craft);
	}

	// Generate items
	std::vector<std::string> items = rule->getItemsList();
	for (std::vector<std::string>::iterator i = items.begin(); i != items.end(); ++i)
	{
		base->getItems()->addItem(*i, 99);
	}

	_game->setSavedGame(save);
}

/**
 * Starts the battle.
 * @param action Pointer to an action.
 */
void NewBattleState::btnOkClick(Action *action)
{
	SavedBattleGame *bgame = new SavedBattleGame();
	_game->getSavedGame()->setBattleGame(bgame);
	bgame->setMissionType("STR_TERROR_MISSION");
	BattlescapeGenerator bgen = BattlescapeGenerator(_game);
	bgen.setWorldTexture(1);
	bgen.setWorldShade(0);
	bgen.setCraft(_game->getSavedGame()->getBases()->at(0)->getCrafts()->at(0));
	bgen.setBase(_game->getSavedGame()->getBases()->at(0));
	bgen.setAlienRace("STR_SECTOID");
	bgen.setAlienItemlevel(0);
	bgen.run();
	_game->getSavedGame()->getBattleGame()->resetUnitTiles();
	_game->pushState(new BattlescapeState(_game));
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void NewBattleState::btnCancelClick(Action *action)
{
	//_game->setRuleset(0);
	_game->setSavedGame(0);
	_game->popState();
}

}
