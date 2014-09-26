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
#include "NewBattleState.h"
#include <cmath>
#include <fstream>
#include <algorithm>
#include <yaml-cpp/yaml.h>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/Ruleset.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/ComboBox.h"
#include "../Interface/Slider.h"
#include "../Interface/Frame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Savegame/Craft.h"
#include "../Savegame/ItemContainer.h"
#include "../Battlescape/BattlescapeGenerator.h"
#include "../Battlescape/BattlescapeState.h"
#include "../Battlescape/BriefingState.h"
#include "../Savegame/Ufo.h"
#include "../Savegame/TerrorSite.h"
#include "../Savegame/AlienBase.h"
#include "../Ruleset/RuleCraft.h"
#include "../Ruleset/RuleTerrain.h"
#include "../Ruleset/AlienDeployment.h"
#include "../Engine/Music.h"
#include "../Engine/RNG.h"
#include "../Engine/Action.h"
#include "../Engine/Options.h"
#include "../Engine/Logger.h"
#include "../Basescape/CraftInfoState.h"
#include "../Engine/CrossPlatform.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the New Battle window.
 * @param game Pointer to the core game.
 */
NewBattleState::NewBattleState() : _craft(0)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0, POPUP_BOTH);
	_txtTitle = new Text(320, 17, 0, 9);
	
	_txtMapOptions = new Text(148, 9, 8, 68);
	_frameLeft = new Frame(148, 96, 8, 78);
	_txtAlienOptions = new Text(148, 9, 164, 68);
	_frameRight = new Frame(148, 96, 164, 78);

	_txtMission = new Text(100, 9, 8, 30);
	_cbxMission = new ComboBox(this, 214, 16, 98, 26);

	_txtCraft = new Text(100, 9, 8, 50);
	_cbxCraft = new ComboBox(this, 106, 16, 98, 46);
	_btnEquip = new TextButton(106, 16, 206, 46);
	
	_txtDarkness = new Text(120, 9, 22, 83);
	_slrDarkness = new Slider(120, 16, 22, 93);

	_txtTerrain = new Text(120, 9, 22, 113);
	_cbxTerrain = new ComboBox(this, 120, 16, 22, 123);

	_txtDepth = new Text(120, 9, 22, 143);
	_slrDepth = new Slider(120, 16, 22, 153);
	
	_txtDifficulty = new Text(120, 9, 178, 83);
	_cbxDifficulty = new ComboBox(this, 120, 16, 178, 93);

	_txtAlienRace = new Text(120, 9, 178, 113);
	_cbxAlienRace = new ComboBox(this, 120, 16, 178, 123);

	_txtAlienTech = new Text(120, 9, 178, 143);
	_slrAlienTech = new Slider(120, 16, 178, 153);
	
	_btnOk = new TextButton(100, 16, 8, 176);
	_btnCancel = new TextButton(100, 16, 110, 176);
	_btnRandom = new TextButton(100, 16, 212, 176);

	// Set palette
	setPalette("PAL_GEOSCAPE", 0);

	add(_window);
	add(_txtTitle);
	add(_txtMapOptions);
	add(_frameLeft);
	add(_txtAlienOptions);
	add(_frameRight);

	add(_txtMission);
	add(_txtCraft);
	add(_btnEquip);
	
	add(_txtDarkness);
	add(_slrDarkness);
	add(_txtDepth);
	add(_slrDepth);
	add(_txtTerrain);
	add(_txtDifficulty);
	add(_txtAlienRace);
	add(_txtAlienTech);
	add(_slrAlienTech);

	add(_btnOk);
	add(_btnCancel);
	add(_btnRandom);
	
	add(_cbxTerrain);
	add(_cbxAlienRace);
	add(_cbxDifficulty);
	add(_cbxCraft);
	add(_cbxMission);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(tr("STR_MISSION_GENERATOR"));

	_txtMapOptions->setColor(Palette::blockOffset(15)-1);
	_txtMapOptions->setText(tr("STR_MAP_OPTIONS"));

	_frameLeft->setThickness(3);
	_frameLeft->setColor(Palette::blockOffset(15)-1);

	_txtAlienOptions->setColor(Palette::blockOffset(15)-1);
	_txtAlienOptions->setText(tr("STR_ALIEN_OPTIONS"));

	_frameRight->setThickness(3);
	_frameRight->setColor(Palette::blockOffset(15)-1);

	_txtMission->setColor(Palette::blockOffset(8)+10);
	_txtMission->setText(tr("STR_MISSION"));

	_txtCraft->setColor(Palette::blockOffset(8)+10);
	_txtCraft->setText(tr("STR_CRAFT"));

	_txtDarkness->setColor(Palette::blockOffset(8)+10);
	_txtDarkness->setText(tr("STR_MAP_DARKNESS"));
	
	_txtDepth->setColor(Palette::blockOffset(8)+10);
	_txtDepth->setText(tr("STR_MAP_DEPTH"));

	_txtTerrain->setColor(Palette::blockOffset(8)+10);
	_txtTerrain->setText(tr("STR_MAP_TERRAIN"));

	_txtDifficulty->setColor(Palette::blockOffset(8)+10);
	_txtDifficulty->setText(tr("STR_ALIEN_DIFFICULTY"));

	_txtAlienRace->setColor(Palette::blockOffset(8)+10);
	_txtAlienRace->setText(tr("STR_ALIEN_RACE"));
	
	_txtAlienTech->setColor(Palette::blockOffset(8)+10);
	_txtAlienTech->setText(tr("STR_ALIEN_TECH_LEVEL"));

	_missionTypes = _game->getRuleset()->getDeploymentsList();

	const std::vector<std::string> &terrainTypes = _game->getRuleset()->getTerrainList();
	std::vector<std::string> terrainStrings;
	for (std::vector<std::string>::const_iterator i = terrainTypes.begin(); i != terrainTypes.end(); ++i)
	{
		if (!_game->getRuleset()->getTerrain(*i)->getTextures()->empty())
		{
			_terrainTypes.push_back(*i);
			terrainStrings.push_back("STR_" + *i);
			_textures.push_back(_game->getRuleset()->getTerrain(*i)->getTextures()->at(0));
		}
	}

	_alienRaces = _game->getRuleset()->getAlienRacesList();
	for (std::vector<std::string>::iterator i = _alienRaces.begin(); i != _alienRaces.end();)
	{
		if ((*i).find("_UNDERWATER") != std::string::npos)
		{
			i = _alienRaces.erase(i);
		}
		else
		{
			++i;
		}
	}

	std::vector<std::string> difficulty;
	difficulty.push_back("STR_1_BEGINNER");
	difficulty.push_back("STR_2_EXPERIENCED");
	difficulty.push_back("STR_3_VETERAN");
	difficulty.push_back("STR_4_GENIUS");
	difficulty.push_back("STR_5_SUPERHUMAN");

	const std::vector<std::string> &crafts = _game->getRuleset()->getCraftsList();
	for (std::vector<std::string>::const_iterator i = crafts.begin(); i != crafts.end(); ++i)
	{
		RuleCraft *rule = _game->getRuleset()->getCraft(*i);
		if (rule->getSoldiers() > 0)
		{
			_crafts.push_back(*i);
		}
	}

	_cbxMission->setColor(Palette::blockOffset(15)-1);
	_cbxMission->setOptions(_missionTypes);
	_cbxMission->onChange((ActionHandler)&NewBattleState::cbxMissionChange);

	_cbxCraft->setColor(Palette::blockOffset(15)-1);
	_cbxCraft->setOptions(_crafts);
	_cbxCraft->onChange((ActionHandler)&NewBattleState::cbxCraftChange);

	_slrDarkness->setColor(Palette::blockOffset(15)-1);
	_slrDarkness->setRange(0, 15);
	
	_slrDepth->setColor(Palette::blockOffset(15)-1);
	_slrDepth->setRange(1, 3);

	_cbxTerrain->setColor(Palette::blockOffset(15)-1);
	_cbxTerrain->setOptions(terrainStrings);
	_cbxTerrain->onChange((ActionHandler)&NewBattleState::cbxTerrainChange);

	_cbxDifficulty->setColor(Palette::blockOffset(15)-1);
	_cbxDifficulty->setOptions(difficulty);

	_cbxAlienRace->setColor(Palette::blockOffset(15)-1);
	_cbxAlienRace->setOptions(_alienRaces);

	_slrAlienTech->setColor(Palette::blockOffset(15)-1);
	_slrAlienTech->setRange(0, _game->getRuleset()->getAlienItemLevels().size()-1);

	_btnEquip->setColor(Palette::blockOffset(15)-1);
	_btnEquip->setText(tr("STR_EQUIP_CRAFT"));
	_btnEquip->onMouseClick((ActionHandler)&NewBattleState::btnEquipClick);

	_btnRandom->setColor(Palette::blockOffset(8)+5);
	_btnRandom->setText(tr("STR_RANDOMIZE"));
	_btnRandom->onMouseClick((ActionHandler)&NewBattleState::btnRandomClick);

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&NewBattleState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&NewBattleState::btnOkClick, Options::keyOk);

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(tr("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&NewBattleState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&NewBattleState::btnCancelClick, Options::keyCancel);

	load();
}

/**
 *
 */
NewBattleState::~NewBattleState()
{
	
}

/**
 * Resets the menu music and savegame
 * when coming back from the battlescape.
 */
void NewBattleState::init()
{
	State::init();

	if (_craft == 0)
	{
		load();
	}
}

/**
 * Loads new battle data from a YAML file.
 * @param filename YAML filename.
 */
void NewBattleState::load(const std::string &filename)
{
	std::string s = Options::getConfigFolder() + filename + ".cfg";
	if (!CrossPlatform::fileExists(s))
	{
		initSave();
	}
	else
	{
		try
		{
			YAML::Node doc = YAML::LoadFile(s);
			_cbxMission->setSelected(std::min(doc["mission"].as<size_t>(0), _missionTypes.size() - 1));
			_cbxCraft->setSelected(std::min(doc["craft"].as<size_t>(0), _crafts.size() - 1));
			_slrDarkness->setValue(doc["darkness"].as<size_t>(0));
			_cbxTerrain->setSelected(std::min(doc["terrain"].as<size_t>(0), _terrainTypes.size() - 1));
			_cbxAlienRace->setSelected(std::min(doc["alienRace"].as<size_t>(0), _alienRaces.size() - 1));
			_cbxDifficulty->setSelected(doc["difficulty"].as<size_t>(0));
			_slrAlienTech->setValue(doc["alienTech"].as<size_t>(0));
			cbxMissionChange(NULL);

			if (doc["base"])
			{
				const Ruleset *rule = _game->getRuleset();
				SavedGame *save = new SavedGame();

				Base *base = new Base(rule);
				base->load(doc["base"], save, false);
				save->getBases()->push_back(base);

				// Add research
				const std::vector<std::string> &research = rule->getResearchList();
				for (std::vector<std::string>::const_iterator i = research.begin(); i != research.end(); ++i)
				{
					save->addFinishedResearch(rule->getResearch(*i));
				}

				// Generate items
				base->getItems()->getContents()->clear();
				const std::vector<std::string> &items = rule->getItemsList();
				for (std::vector<std::string>::const_iterator i = items.begin(); i != items.end(); ++i)
				{
					RuleItem *rule = _game->getRuleset()->getItem(*i);
					if (rule->getBattleType() != BT_CORPSE && rule->isRecoverable())
					{
						base->getItems()->addItem(*i, 1);
					}
				}

				// Fix invalid contents
				if (base->getCrafts()->empty())
				{
					std::string craftType = _crafts[_cbxCraft->getSelected()];
					_craft = new Craft(_game->getRuleset()->getCraft(craftType), base, save->getId(craftType));
					base->getCrafts()->push_back(_craft);
				}
				else
				{
					_craft = base->getCrafts()->front();
					for (std::map<std::string, int>::iterator i = _craft->getItems()->getContents()->begin(); i != _craft->getItems()->getContents()->end(); ++i)
					{
						RuleItem *rule = _game->getRuleset()->getItem(i->first);
						if (!rule)
						{
							i->second = 0;
						}
					}
				}

				_game->setSavedGame(save);
			}
			else
			{
				initSave();
			}
		}
		catch (YAML::Exception e)
		{
			Log(LOG_WARNING) << e.what();
			initSave();
		}
	}
}

/**
 * Saves new battle data to a YAML file.
 * @param filename YAML filename.
 */
void NewBattleState::save(const std::string &filename)
{
	std::string s = Options::getConfigFolder() + filename + ".cfg";
	std::ofstream sav(s.c_str());
	if (!sav)
	{
		Log(LOG_WARNING) << "Failed to save " << filename << ".cfg";
		return;
	}
	YAML::Emitter out;

	YAML::Node node;
	node["mission"] = _cbxMission->getSelected();
	node["craft"] = _cbxCraft->getSelected();
	node["darkness"] = _slrDarkness->getValue();
	node["terrain"] = _cbxTerrain->getSelected();
	node["alienRace"] = _cbxAlienRace->getSelected();
	node["difficulty"] = _cbxDifficulty->getSelected();
	node["alienTech"] = _slrAlienTech->getValue();
	node["base"] = _game->getSavedGame()->getBases()->front()->save();
	out << node;

	sav << out.c_str();
	sav.close();
}

/**
 * Initializes a new savegame with
 * everything available.
 */
void NewBattleState::initSave()
{
	const Ruleset *rule = _game->getRuleset();
	SavedGame *save = new SavedGame();
	Base *base = new Base(rule);
	const YAML::Node &starter = _game->getRuleset()->getStartingBase();
	base->load(starter, save, true, true);
	save->getBases()->push_back(base);

	// kill everything we don't want in this base
	for (std::vector<Soldier*>::iterator i = base->getSoldiers()->begin(); i != base->getSoldiers()->end(); ++i) delete (*i);
	base->getSoldiers()->clear();
	for (std::vector<Craft*>::iterator i = base->getCrafts()->begin(); i != base->getCrafts()->end(); ++i) delete (*i);
	base->getCrafts()->clear();
	base->getItems()->getContents()->clear();

	_craft = new Craft(rule->getCraft(_crafts[_cbxCraft->getSelected()]), base, 1);
	base->getCrafts()->push_back(_craft);

	// Generate soldiers
	for (int i = 0; i < 30; ++i)
	{
		Soldier *soldier = rule->genSoldier(save);

		for (int n = 0; n < 5; ++n)
		{
			if (RNG::percent(70))
				continue;
			soldier->promoteRank();

			UnitStats* stats = soldier->getCurrentStats();
			stats->tu		  += RNG::generate(0, 5);
			stats->stamina	 += RNG::generate(0, 5);
			stats->health	  += RNG::generate(0, 5);
			stats->bravery	 += RNG::generate(0, 5);
			stats->reactions   += RNG::generate(0, 5);
			stats->firing	  += RNG::generate(0, 5);
			stats->throwing	+= RNG::generate(0, 5);
			stats->strength	+= RNG::generate(0, 5);
			stats->psiStrength += RNG::generate(0, 5);
			stats->melee	   += RNG::generate(0, 5);
			stats->psiSkill	+= RNG::generate(0, 20);
		}
		UnitStats* stats = soldier->getCurrentStats();
		stats->bravery = (int)ceil(stats->bravery / 10.0) * 10; // keep it a multiple of 10

		base->getSoldiers()->push_back(soldier);
		if (i < _craft->getRules()->getSoldiers())
			soldier->setCraft(_craft);
	}

	// Generate items
	const std::vector<std::string> &items = rule->getItemsList();
	for (std::vector<std::string>::const_iterator i = items.begin(); i != items.end(); ++i)
	{
		RuleItem *rule = _game->getRuleset()->getItem(*i);
		if (rule->getBattleType() != BT_CORPSE && rule->isRecoverable())
		{
			base->getItems()->addItem(*i, 1);
			if (rule->getBattleType() != BT_NONE && !rule->isFixed() && rule->getBigSprite() > -1)
			{
				_craft->getItems()->addItem(*i, 1);
			}
		}
	}

	// Add research
	const std::vector<std::string> &research = rule->getResearchList();
	for (std::vector<std::string>::const_iterator i = research.begin(); i != research.end(); ++i)
	{
		save->addFinishedResearch(rule->getResearch(*i));
	}

	_game->setSavedGame(save);
}

/**
 * Starts the battle.
 * @param action Pointer to an action.
 */
void NewBattleState::btnOkClick(Action *)
{
	save();
	if (_missionTypes[_cbxMission->getSelected()] != "STR_BASE_DEFENSE" && _craft->getNumSoldiers() == 0 && _craft->getNumVehicles() == 0)
	{
		return;
	}

	SavedBattleGame *bgame = new SavedBattleGame();
	_game->getSavedGame()->setBattleGame(bgame);
	bgame->setMissionType(_missionTypes[_cbxMission->getSelected()]);
	BattlescapeGenerator bgen = BattlescapeGenerator(_game);

	bgen.setWorldTexture(_textures[_cbxTerrain->getSelected()]);

	if (_missionTypes[_cbxMission->getSelected()] == "STR_TERROR_MISSION")
	{
		TerrorSite *t = new TerrorSite();
		t->setId(1);
		_craft->setDestination(t);
		bgen.setTerrorSite(t);
		bgen.setCraft(_craft);
		_game->getSavedGame()->getTerrorSites()->push_back(t);
	}
	else if (_missionTypes[_cbxMission->getSelected()] == "STR_BASE_DEFENSE")
	{
		bgen.setBase(_craft->getBase());
	}
	else if (_missionTypes[_cbxMission->getSelected()] == "STR_ALIEN_BASE_ASSAULT")
	{
		AlienBase *b = new AlienBase();
		b->setId(1);
		_craft->setDestination(b);
		bgen.setAlienBase(b);
		bgen.setCraft(_craft);
		_game->getSavedGame()->getAlienBases()->push_back(b);
	}
	else if (_missionTypes[_cbxMission->getSelected()] == "STR_MARS_CYDONIA_LANDING" || _missionTypes[_cbxMission->getSelected()] == "STR_MARS_THE_FINAL_ASSAULT")
	{
		bgen.setCraft(_craft);
	}
	else if (_craft)
	{
		Ufo *u = new Ufo(_game->getRuleset()->getUfo(_missionTypes[_cbxMission->getSelected()]));
		u->setId(1);
		_craft->setDestination(u);
		bgen.setUfo(u);
		bgen.setCraft(_craft);
		if (_terrainTypes[_cbxTerrain->getSelected()] == "FOREST")
		{
			u->setLatitude(-0.5);
		}
		// either ground assault or ufo crash
		if (RNG::generate(0,1) == 1)
			bgame->setMissionType("STR_UFO_GROUND_ASSAULT");
		else
			bgame->setMissionType("STR_UFO_CRASH_RECOVERY");
		_game->getSavedGame()->getUfos()->push_back(u);
	}
	if (_craft)
		_craft->setSpeed(0);
	_game->getSavedGame()->setDifficulty((GameDifficulty)_cbxDifficulty->getSelected());

	bgen.setWorldShade(_slrDarkness->getValue());
	bgen.setAlienRace(_alienRaces[_cbxAlienRace->getSelected()]);
	bgen.setAlienItemlevel(_slrAlienTech->getValue());
	bgame->setDepth(_slrDepth->getValue());

	bgen.run();
	//_game->pushState(new BattlescapeState);
	Base *base = 0;
	if (_missionTypes[_cbxMission->getSelected()] == "STR_BASE_DEFENSE")
	{
		base = _craft->getBase();
		_craft = 0;
	}
	_game->popState();
	_game->popState();
	_game->pushState(new BriefingState(_craft, base));
	_craft = 0;
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void NewBattleState::btnCancelClick(Action *)
{
	save();
	_game->setSavedGame(0);
	_game->popState();
}

/**
 * Randomize the state
 * @param action Pointer to an action.
 */
void NewBattleState::btnRandomClick(Action *)
{
	_cbxMission->setSelected(RNG::generate(0, _missionTypes.size()-1));
	_cbxCraft->setSelected(RNG::generate(0, _crafts.size()-1));
	_slrDarkness->setValue(RNG::generate(0, 15));
	_cbxTerrain->setSelected(RNG::generate(0, _terrainTypes.size()-1));
	_cbxAlienRace->setSelected(RNG::generate(0, _alienRaces.size()-1));
	_cbxDifficulty->setSelected(RNG::generate(0, 4));
	_slrAlienTech->setValue(RNG::generate(0, _game->getRuleset()->getAlienItemLevels().size()-1));
	cbxMissionChange(0);
	cbxCraftChange(0);
	cbxTerrainChange(0);

	initSave();
}

/**
 * Shows the Craft Info screen.
 * @param action Pointer to an action.
 */
void NewBattleState::btnEquipClick(Action *)
{
	_game->pushState(new CraftInfoState(_game->getSavedGame()->getBases()->front(), 0));
}

/**
 * Updates Map Options based on the
 * current Mission type.
 * @param action Pointer to an action.
 */
void NewBattleState::cbxMissionChange(Action *)
{
	AlienDeployment *ruleDeploy = _game->getRuleset()->getDeployment(_missionTypes[_cbxMission->getSelected()]);
	_txtDarkness->setVisible(ruleDeploy->getShade() == -1);
	_slrDarkness->setVisible(ruleDeploy->getShade() == -1);
	_txtTerrain->setVisible(ruleDeploy->getTerrains().empty());
	_cbxTerrain->setVisible(ruleDeploy->getTerrains().empty());
	cbxTerrainChange(0);
}

/**
 * Updates craft accordingly.
 * @param action Pointer to an action.
 */
void NewBattleState::cbxCraftChange(Action *)
{
	_craft->changeRules(_game->getRuleset()->getCraft(_crafts[_cbxCraft->getSelected()]));
	int current = _craft->getNumSoldiers();
	int max = _craft->getRules()->getSoldiers();
	if (current > max)
	{
		for (std::vector<Soldier*>::reverse_iterator i = _craft->getBase()->getSoldiers()->rbegin(); i != _craft->getBase()->getSoldiers()->rend() && current > max; ++i)
		{
			if ((*i)->getCraft() == _craft)
			{
				(*i)->setCraft(0);
				current--;
			}
		}
	}
}

/**
 * Updates the depth slider accordingly when terrain selection changes.
 * @param action Pointer to an action.
 */
void NewBattleState::cbxTerrainChange(Action *)
{
	AlienDeployment *ruleDeploy = _game->getRuleset()->getDeployment(_missionTypes[_cbxMission->getSelected()]);
	int minDepth = _game->getRuleset()->getTerrain(_terrainTypes.at(_cbxTerrain->getSelected()))->getMinDepth();
	int maxDepth = _game->getRuleset()->getTerrain(_terrainTypes.at(_cbxTerrain->getSelected()))->getMaxDepth();
	if (!ruleDeploy->getTerrains().empty())
	{
		minDepth = _game->getRuleset()->getTerrain(ruleDeploy->getTerrains().front())->getMinDepth();
		maxDepth = _game->getRuleset()->getTerrain(ruleDeploy->getTerrains().front())->getMaxDepth();
	}
	_txtDepth->setVisible(minDepth != maxDepth && maxDepth != 0);
	_slrDepth->setVisible(minDepth != maxDepth && maxDepth != 0);
	_slrDepth->setRange(minDepth, maxDepth);
	_slrDepth->setValue(minDepth);
}

}
