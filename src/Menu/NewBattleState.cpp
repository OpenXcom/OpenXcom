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
#include "../Ruleset/Ruleset.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Savegame/Craft.h"
#include "../Savegame/ItemContainer.h"
#include "../Battlescape/BattlescapeGenerator.h"
#include "../Battlescape/BattlescapeState.h"
#include "../Battlescape/BriefingState.h"
#include "../Savegame/Ufo.h"
#include "../Savegame/TerrorSite.h"
#include "../Ruleset/RuleCraft.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Music.h"
#include "../Engine/Action.h"
#include "../Basescape/CraftInfoState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the New Battle window.
 * @param game Pointer to the core game.
 */
NewBattleState::NewBattleState(Game *game) : State(game), _craft(0)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0, POPUP_BOTH);
	_txtTitle = new Text(320, 16, 0, 9);
	_txtMissionType = new Text(100, 9, 5, 30);
	_txtTerrainType = new Text(100, 9, 110, 30);
	_txtAlienRace = new Text(100, 9, 215, 30);
	_txtDifficulty = new Text(100, 9, 5, 80);
	_txtDarkness = new Text(100, 9, 110, 80);
	_txtCraft = new Text(100, 9, 215, 80);
	_btnMissionType = new TextButton(100, 20, 5, 40);
	_btnTerrainType = new TextButton(100, 20, 110, 40);
	_btnAlienRace = new TextButton(100, 20, 215, 40);
	_btnDifficulty = new TextButton(100, 20, 5, 90);
	_btnDarkness = new TextButton(100, 20, 110, 90);
	_btnCraft = new TextButton(100, 20, 215, 90);
	_btnEquip = new TextButton(148, 16, 86, 154);
	_btnOk = new TextButton(148, 16, 8, 176);
	_btnCancel = new TextButton(148, 16, 164, 176);

	add(_window);
	add(_txtTitle);
	add(_txtMissionType);
	add(_txtTerrainType);
	add(_txtAlienRace);
	add(_txtDifficulty);
	add(_txtDarkness);
	add(_txtCraft);
	add(_btnMissionType);
	add(_btnTerrainType);
	add(_btnAlienRace);
	add(_btnDifficulty);
	add(_btnDarkness);
	add(_btnCraft);
	add(_btnEquip);
	add(_btnOk);
	add(_btnCancel);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(_game->getLanguage()->getString("STR_GAME_OPTIONS"));

	_txtMissionType->setColor(Palette::blockOffset(8)+10);
	_txtMissionType->setText(_game->getLanguage()->getString("STR_MISSION"));

	_txtTerrainType->setColor(Palette::blockOffset(8)+10);
	_txtTerrainType->setText(_game->getLanguage()->getString("STR_TERRAIN"));

	_txtAlienRace->setColor(Palette::blockOffset(8)+10);
	_txtAlienRace->setText(_game->getLanguage()->getString("STR_RACE"));

	_txtDifficulty->setColor(Palette::blockOffset(8)+10);
	_txtDifficulty->setText(_game->getLanguage()->getString("STR_DIFFICULTY_LEVEL"));

	_txtDarkness->setColor(Palette::blockOffset(8)+10);
	_txtDarkness->setText(_game->getLanguage()->getString("STR_DARKNESS"));

	_txtCraft->setColor(Palette::blockOffset(8)+10);
	_txtCraft->setText(_game->getLanguage()->getString("STR_CRAFT"));

	_selMission = 0;
	_missionTypes = _game->getRuleset()->getDeploymentsList();
	//TODO: Figure out how to generate base defenses
	_missionTypes.pop_back();

	_selTerrain = 0;
	_terrainTypes.push_back("STR_FARM");
	_terrainTypes.push_back("STR_FOREST");
	_terrainTypes.push_back("STR_JUNGLE");
	_terrainTypes.push_back("STR_MOUNTAIN");
	_terrainTypes.push_back("STR_DESERT");
	_terrainTypes.push_back("STR_POLAR");

	_selAlien = 0;
	//_alienRaces = _game->getRuleset()->getAlienRacesList();
	_alienRaces.push_back("STR_SECTOID");
	_alienRaces.push_back("STR_FLOATER");

	_selDifficulty = 0;
	_difficulty.push_back("STR_1_BEGINNER");
	_difficulty.push_back("STR_2_EXPERIENCED");
	_difficulty.push_back("STR_3_VETERAN");
	_difficulty.push_back("STR_4_GENIUS");
	_difficulty.push_back("STR_5_SUPERHUMAN");

	_selDarkness = 0;
	_darkness.push_back("0");
	_darkness.push_back("3");
	_darkness.push_back("6");
	_darkness.push_back("9");
	_darkness.push_back("12");
	_darkness.push_back("15");

	_selCraft = 0;
	std::vector<std::string> crafts = _game->getRuleset()->getCraftsList();
	for (std::vector<std::string>::iterator i = crafts.begin(); i != crafts.end(); ++i)
	{
		RuleCraft *rule = _game->getRuleset()->getCraft(*i);
		if (rule->getSoldiers() > 0)
		{
			_crafts.push_back(*i);
		}
	}

	_btnMissionType->setColor(Palette::blockOffset(15)-1);
	_btnMissionType->setText(_game->getLanguage()->getString(_missionTypes[_selMission]));
	_btnMissionType->onMouseClick((ActionHandler)&NewBattleState::btnMissionTypeClick, 0);

	_btnTerrainType->setColor(Palette::blockOffset(15)-1);
	_btnTerrainType->setText(_game->getLanguage()->getString(_terrainTypes[_selTerrain]));
	_btnTerrainType->onMouseClick((ActionHandler)&NewBattleState::btnTerrainTypeClick, 0);

	_btnAlienRace->setColor(Palette::blockOffset(15)-1);
	_btnAlienRace->setText(_game->getLanguage()->getString(_alienRaces[_selAlien]));
	_btnAlienRace->onMouseClick((ActionHandler)&NewBattleState::btnAlienRaceClick, 0);

	_btnDifficulty->setColor(Palette::blockOffset(15)-1);
	_btnDifficulty->setText(_game->getLanguage()->getString(_difficulty[_selDifficulty]));
	_btnDifficulty->onMouseClick((ActionHandler)&NewBattleState::btnDifficultyClick, 0);

	_btnDarkness->setColor(Palette::blockOffset(15)-1);
	_btnDarkness->setText(Language::utf8ToWstr(_darkness[_selDarkness]));
	_btnDarkness->onMouseClick((ActionHandler)&NewBattleState::btnDarknessClick, 0);

	_btnCraft->setColor(Palette::blockOffset(15)-1);
	_btnCraft->setText(_game->getLanguage()->getString(_crafts[_selCraft]));
	_btnCraft->onMouseClick((ActionHandler)&NewBattleState::btnCraftClick, 0);

	_btnEquip->setColor(Palette::blockOffset(8)+5);
	_btnEquip->setText(_game->getLanguage()->getString("STR_EQUIP_CRAFT"));
	_btnEquip->onMouseClick((ActionHandler)&NewBattleState::btnEquipClick);

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&NewBattleState::btnOkClick);

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&NewBattleState::btnCancelClick);

	_music = true;
}

/**
 *
 */
NewBattleState::~NewBattleState()
{
	
}

/**
 * Updates one of the list indexes.
 * @param index Index value.
 * @param list List the index belongs to.
 * @param change Amount to change the index.
 */
void NewBattleState::updateIndex(size_t &index, std::vector<std::string> &list, int change)
{
	int i = index;
	if (i + change >= (int)list.size())
	{
		index = 0;
	}
	else if (i + change < 0)
	{
		index = list.size() - 1;
	}
	else
	{
		index += change;
	}
}

/**
 * Resets the menu music and savegame
 * when coming back from the battlescape.
 */
void NewBattleState::init()
{
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_0")->getColors());

	// Set music
	if (!_music)
	{
		_music = true;
		_game->getResourcePack()->getMusic("GMSTORY")->play();
	}

	if (_craft == 0)
	{
		_game->setSavedGame(0);
		initSave();
	}
}

/**
 * Initializes a new savegame with
 * everything available.
 */
void NewBattleState::initSave()
{
	Ruleset *rule = _game->getRuleset();
	SavedGame *save = new SavedGame();
	Base *base = new Base(rule);
	save->getBases()->push_back(base);
	_craft = new Craft(rule->getCraft("STR_SKYRANGER"), base, 1);
	base->getCrafts()->push_back(_craft);

	// Generate soldiers
	for (int i = 0; i < 30; ++i)
	{
		Soldier *soldier = new Soldier(rule->getSoldier("XCOM"), rule->getArmor("STR_NONE_UC"), rule->getPools(), save->getId("STR_SOLDIER"));
		base->getSoldiers()->push_back(soldier);
		if (i < 8)
			soldier->setCraft(_craft);
	}

	// Generate items
	std::vector<std::string> items = rule->getItemsList();
	for (std::vector<std::string>::iterator i = items.begin(); i != items.end(); ++i)
	{
		RuleItem *rule = _game->getRuleset()->getItem(*i);
		if (rule->getBattleType() != BT_CORPSE && rule->isRecoverable())
		{
			base->getItems()->addItem(*i, 99);
			if (rule->getBattleType() != BT_NONE && !rule->isFixed() && (*i).substr(0, 8) != "STR_HWP_")
			{
				_craft->getItems()->addItem(*i);
			}
		}
	}

	// Add research
	std::vector<std::string> research = rule->getResearchList();
	for (std::vector<std::string>::iterator i = research.begin(); i != research.end(); ++i)
	{
		save->addFinishedResearch(rule->getResearch(*i));
	}

	_game->setSavedGame(save);
}

/**
 * Starts the battle.
 * @param action Pointer to an action.
 */
void NewBattleState::btnOkClick(Action *action)
{
	_music = false;

	SavedBattleGame *bgame = new SavedBattleGame();
	_game->getSavedGame()->setBattleGame(bgame);
	bgame->setMissionType(_missionTypes[_selMission]);
	BattlescapeGenerator bgen = BattlescapeGenerator(_game);

	int textures[] = {1, 0, 0, 5, 7, 9};
	bgen.setWorldTexture(textures[_selTerrain]);

	if (_missionTypes[_selMission] == "STR_TERROR_MISSION")
	{
		TerrorSite *t = new TerrorSite();
		t->setId(1);
		_craft->setDestination(t);
		bgen.setTerrorSite(t);
		bgen.setCraft(_craft);
	}
	else if (_missionTypes[_selMission] == "STR_BASE_DEFENSE")
	{
		bgen.setBase(_craft->getBase());
	}
	else if (_missionTypes[_selMission] == "STR_ALIEN_BASE_ASSAULT")
	{
		// should be fixed to be an alien base, not a terror site
		TerrorSite *t = new TerrorSite();
		t->setId(1);
		_craft->setDestination(t);
		bgen.setTerrorSite(t);
		bgen.setCraft(_craft);
	}
	else
	{
		Ufo *u = new Ufo(_game->getRuleset()->getUfo(_missionTypes[_selMission]));
		u->setId(1);
		_craft->setDestination(u);
		bgen.setUfo(u);
		bgen.setCraft(_craft);
		if (_terrainTypes[_selTerrain] == "STR_FOREST")
		{
			u->setLatitude(-0.5);
		}
	}

	GameDifficulty diffs[] = {DIFF_BEGINNER, DIFF_EXPERIENCED, DIFF_VETERAN, DIFF_GENIUS, DIFF_SUPERHUMAN};
	_game->getSavedGame()->setDifficulty(diffs[_selDifficulty]);

	std::stringstream ss;
	int shade = 0;
	ss << std::dec << _darkness[_selDarkness];
	ss >> std::dec >> shade;
	bgen.setWorldShade(shade);
	bgen.setAlienRace(_alienRaces[_selAlien]);
	bgen.setAlienItemlevel(0);
	bgen.run();
	//_game->pushState(new BattlescapeState(_game));
	_game->pushState(new BriefingState(_game, _craft));
	_craft = 0;
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void NewBattleState::btnCancelClick(Action *action)
{
	_game->setSavedGame(0);
	_game->popState();
}

/**
 * Shows the Craft Info screen.
 * @param action Pointer to an action.
 */
void NewBattleState::btnEquipClick(Action *action)
{
	_game->pushState(new CraftInfoState(_game, _game->getSavedGame()->getBases()->front(), 0));
}

/**
 * Changes mission type.
 * @param action Pointer to an action.
 */
void NewBattleState::btnMissionTypeClick(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		updateIndex(_selMission, _missionTypes, 1);
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		updateIndex(_selMission, _missionTypes, -1);
	}
	_btnMissionType->setText(_game->getLanguage()->getString(_missionTypes[_selMission]));
}

/**
 * Changes terrain type.
 * @param action Pointer to an action.
 */
void NewBattleState::btnTerrainTypeClick(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		updateIndex(_selTerrain, _terrainTypes, 1);
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		updateIndex(_selTerrain, _terrainTypes, -1);
	}
	_btnTerrainType->setText(_game->getLanguage()->getString(_terrainTypes[_selTerrain]));
}

/**
 * Changes alien race.
 * @param action Pointer to an action.
 */
void NewBattleState::btnAlienRaceClick(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		updateIndex(_selAlien, _alienRaces, 1);
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		updateIndex(_selAlien, _alienRaces, -1);
	}
	_btnAlienRace->setText(_game->getLanguage()->getString(_alienRaces[_selAlien]));
}

/**
 * Changes difficulty level.
 * @param action Pointer to an action.
 */
void NewBattleState::btnDifficultyClick(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		updateIndex(_selDifficulty, _difficulty, 1);
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		updateIndex(_selDifficulty, _difficulty, -1);
	}
	_btnDifficulty->setText(_game->getLanguage()->getString(_difficulty[_selDifficulty]));
}

/**
 * Changes darkness level
 * @param action Pointer to an action.
 */
void NewBattleState::btnDarknessClick(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		updateIndex(_selDarkness, _darkness, 1);
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		updateIndex(_selDarkness, _darkness, -1);
	}
	_btnDarkness->setText(Language::utf8ToWstr(_darkness[_selDarkness]));
}

/**
 * Changes player craft.
 * @param action Pointer to an action.
 */
void NewBattleState::btnCraftClick(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		updateIndex(_selCraft, _crafts, 1);
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		updateIndex(_selCraft, _crafts, -1);
	}
	_btnCraft->setText(_game->getLanguage()->getString(_crafts[_selCraft]));
	_craft->setRules(_game->getRuleset()->getCraft(_crafts[_selCraft]));
}

}
