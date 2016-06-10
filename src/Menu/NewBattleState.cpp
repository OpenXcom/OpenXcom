/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "../Mod/Mod.h"
#include "../Mod/RuleItem.h"
#include "../Engine/LocalizedText.h"
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
#include "../Battlescape/BriefingState.h"
#include "../Savegame/Ufo.h"
#include "../Savegame/MissionSite.h"
#include "../Savegame/AlienBase.h"
#include "../Mod/RuleCraft.h"
#include "../Mod/RuleTerrain.h"
#include "../Mod/AlienDeployment.h"
#include "../Engine/RNG.h"
#include "../Engine/Action.h"
#include "../Engine/Options.h"
#include "../Engine/Logger.h"
#include "../Basescape/CraftInfoState.h"
#include "../Engine/CrossPlatform.h"
#include "../Mod/RuleAlienMission.h"
#include "../Mod/RuleGlobe.h"

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
	setInterface("newBattleMenu");

	add(_window, "window", "newBattleMenu");
	add(_txtTitle, "heading", "newBattleMenu");
	add(_txtMapOptions, "heading", "newBattleMenu");
	add(_frameLeft, "frames", "newBattleMenu");
	add(_txtAlienOptions, "heading", "newBattleMenu");
	add(_frameRight, "frames", "newBattleMenu");

	add(_txtMission, "text", "newBattleMenu");
	add(_txtCraft, "text", "newBattleMenu");
	add(_btnEquip, "button1", "newBattleMenu");

	add(_txtDarkness, "text", "newBattleMenu");
	add(_slrDarkness, "button1", "newBattleMenu");
	add(_txtDepth, "text", "newBattleMenu");
	add(_slrDepth, "button1", "newBattleMenu");
	add(_txtTerrain, "text", "newBattleMenu");
	add(_txtDifficulty, "text", "newBattleMenu");
	add(_txtAlienRace, "text", "newBattleMenu");
	add(_txtAlienTech, "text", "newBattleMenu");
	add(_slrAlienTech, "button1", "newBattleMenu");

	add(_btnOk, "button2", "newBattleMenu");
	add(_btnCancel, "button2", "newBattleMenu");
	add(_btnRandom, "button2", "newBattleMenu");

	add(_cbxTerrain, "button1", "newBattleMenu");
	add(_cbxAlienRace, "button1", "newBattleMenu");
	add(_cbxDifficulty, "button1", "newBattleMenu");
	add(_cbxCraft, "button1", "newBattleMenu");
	add(_cbxMission, "button1", "newBattleMenu");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK01.SCR"));

	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(tr("STR_MISSION_GENERATOR"));

	_txtMapOptions->setText(tr("STR_MAP_OPTIONS"));

	_frameLeft->setThickness(3);

	_txtAlienOptions->setText(tr("STR_ALIEN_OPTIONS"));

	_frameRight->setThickness(3);

	_txtMission->setText(tr("STR_MISSION"));

	_txtCraft->setText(tr("STR_CRAFT"));

	_txtDarkness->setText(tr("STR_MAP_DARKNESS"));

	_txtDepth->setText(tr("STR_MAP_DEPTH"));

	_txtTerrain->setText(tr("STR_MAP_TERRAIN"));

	_txtDifficulty->setText(tr("STR_DIFFICULTY"));

	_txtAlienRace->setText(tr("STR_ALIEN_RACE"));

	_txtAlienTech->setText(tr("STR_ALIEN_TECH_LEVEL"));

	_missionTypes = _game->getMod()->getDeploymentsList();
	_cbxMission->setOptions(_missionTypes);
	_cbxMission->onChange((ActionHandler)&NewBattleState::cbxMissionChange);

	const std::vector<std::string> &crafts = _game->getMod()->getCraftsList();
	for (std::vector<std::string>::const_iterator i = crafts.begin(); i != crafts.end(); ++i)
	{
		RuleCraft *rule = _game->getMod()->getCraft(*i);
		if (rule->getSoldiers() > 0)
		{
			_crafts.push_back(*i);
		}
	}
	_cbxCraft->setOptions(_crafts);
	_cbxCraft->onChange((ActionHandler)&NewBattleState::cbxCraftChange);

	_slrDarkness->setRange(0, 15);

	_slrDepth->setRange(1, 3);

	_cbxTerrain->onChange((ActionHandler)&NewBattleState::cbxTerrainChange);

	std::vector<std::string> difficulty;
	difficulty.push_back("STR_1_BEGINNER");
	difficulty.push_back("STR_2_EXPERIENCED");
	difficulty.push_back("STR_3_VETERAN");
	difficulty.push_back("STR_4_GENIUS");
	difficulty.push_back("STR_5_SUPERHUMAN");
	_cbxDifficulty->setOptions(difficulty);

	_alienRaces = _game->getMod()->getAlienRacesList();
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
		_cbxAlienRace->setOptions(_alienRaces);

	_slrAlienTech->setRange(0, _game->getMod()->getAlienItemLevels().size()-1);

	_btnEquip->setText(tr("STR_EQUIP_CRAFT"));
	_btnEquip->onMouseClick((ActionHandler)&NewBattleState::btnEquipClick);

	_btnRandom->setText(tr("STR_RANDOMIZE"));
	_btnRandom->onMouseClick((ActionHandler)&NewBattleState::btnRandomClick);

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&NewBattleState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&NewBattleState::btnOkClick, Options::keyOk);

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
	std::string s = Options::getMasterUserFolder() + filename + ".cfg";
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
			cbxMissionChange(0);
			_cbxCraft->setSelected(std::min(doc["craft"].as<size_t>(0), _crafts.size() - 1));
			_slrDarkness->setValue(doc["darkness"].as<size_t>(0));
			_cbxTerrain->setSelected(std::min(doc["terrain"].as<size_t>(0), _terrainTypes.size() - 1));
			_cbxAlienRace->setSelected(std::min(doc["alienRace"].as<size_t>(0), _alienRaces.size() - 1));
			_cbxDifficulty->setSelected(doc["difficulty"].as<size_t>(0));
			_slrAlienTech->setValue(doc["alienTech"].as<size_t>(0));

			if (doc["base"])
			{
				const Mod *mod = _game->getMod();
				SavedGame *save = new SavedGame();

				Base *base = new Base(mod);
				base->load(doc["base"], save, false);
				save->getBases()->push_back(base);

				// Add research
				const std::vector<std::string> &research = mod->getResearchList();
				for (std::vector<std::string>::const_iterator i = research.begin(); i != research.end(); ++i)
				{
					save->addFinishedResearch(mod->getResearch(*i));
				}

				// Generate items
				base->getStorageItems()->getContents()->clear();
				const std::vector<std::string> &items = mod->getItemsList();
				for (std::vector<std::string>::const_iterator i = items.begin(); i != items.end(); ++i)
				{
					RuleItem *rule = _game->getMod()->getItem(*i);
					if (rule->getBattleType() != BT_CORPSE && rule->isRecoverable())
					{
						base->getStorageItems()->addItem(*i, 1);
					}
				}

				// Fix invalid contents
				if (base->getCrafts()->empty())
				{
					std::string craftType = _crafts[_cbxCraft->getSelected()];
					_craft = new Craft(_game->getMod()->getCraft(craftType), base, save->getId(craftType));
					base->getCrafts()->push_back(_craft);
				}
				else
				{
					_craft = base->getCrafts()->front();
					for (std::map<std::string, int>::iterator i = _craft->getItems()->getContents()->begin(); i != _craft->getItems()->getContents()->end(); ++i)
					{
						RuleItem *rule = _game->getMod()->getItem(i->first);
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
		catch (YAML::Exception &e)
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
	std::string s = Options::getMasterUserFolder() + filename + ".cfg";
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
	const Mod *mod = _game->getMod();
	SavedGame *save = new SavedGame();
	Base *base = new Base(mod);
	const YAML::Node &starter = _game->getMod()->getStartingBase();
	base->load(starter, save, true, true);
	save->getBases()->push_back(base);

	// Kill everything we don't want in this base
	for (std::vector<Soldier*>::iterator i = base->getSoldiers()->begin(); i != base->getSoldiers()->end(); ++i) delete (*i);
	base->getSoldiers()->clear();
	for (std::vector<Craft*>::iterator i = base->getCrafts()->begin(); i != base->getCrafts()->end(); ++i) delete (*i);
	base->getCrafts()->clear();
	base->getStorageItems()->getContents()->clear();

	_craft = new Craft(mod->getCraft(_crafts[_cbxCraft->getSelected()]), base, 1);
	base->getCrafts()->push_back(_craft);

	// Generate soldiers
	for (int i = 0; i < 30; ++i)
	{
		int randomType = RNG::generate(0, _game->getMod()->getSoldiersList().size() - 1);
		Soldier *soldier = mod->genSoldier(save, _game->getMod()->getSoldiersList().at(randomType));

		for (int n = 0; n < 5; ++n)
		{
			if (RNG::percent(70))
				continue;
			soldier->promoteRank();

			UnitStats* stats = soldier->getCurrentStats();
			stats->tu 			+= RNG::generate(0, 5);
			stats->stamina		+= RNG::generate(0, 5);
			stats->health		+= RNG::generate(0, 5);
			stats->bravery		+= RNG::generate(0, 5);
			stats->reactions	+= RNG::generate(0, 5);
			stats->firing		+= RNG::generate(0, 5);
			stats->throwing		+= RNG::generate(0, 5);
			stats->strength		+= RNG::generate(0, 5);
			stats->psiStrength	+= RNG::generate(0, 5);
			stats->melee		+= RNG::generate(0, 5);
			stats->psiSkill		+= RNG::generate(0, 20);
		}
		UnitStats* stats = soldier->getCurrentStats();
		stats->bravery = (int)ceil(stats->bravery / 10.0) * 10; // keep it a multiple of 10

		base->getSoldiers()->push_back(soldier);
		if (i < _craft->getRules()->getSoldiers())
			soldier->setCraft(_craft);
	}

	// Generate items
	const std::vector<std::string> &items = mod->getItemsList();
	for (std::vector<std::string>::const_iterator i = items.begin(); i != items.end(); ++i)
	{
		RuleItem *rule = _game->getMod()->getItem(*i);
		if (rule->getBattleType() != BT_CORPSE && rule->isRecoverable())
		{
			base->getStorageItems()->addItem(*i, 1);
			if (rule->getBattleType() != BT_NONE && !rule->isFixed() && rule->getBigSprite() > -1)
			{
				_craft->getItems()->addItem(*i, 1);
			}
		}
	}

	// Add research
	const std::vector<std::string> &research = mod->getResearchList();
	for (std::vector<std::string>::const_iterator i = research.begin(); i != research.end(); ++i)
	{
		save->addFinishedResearch(mod->getResearch(*i));
	}

	_game->setSavedGame(save);
	cbxMissionChange(0);
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
	Base *base = 0;

	bgen.setTerrain(_game->getMod()->getTerrain(_terrainTypes[_cbxTerrain->getSelected()]));

	// base defense
	if (_missionTypes[_cbxMission->getSelected()] == "STR_BASE_DEFENSE")
	{
		base = _craft->getBase();
		bgen.setBase(base);
		_craft = 0;
	}
	// alien base
	else if (_game->getMod()->getDeployment(bgame->getMissionType())->isAlienBase())
	{
		AlienBase *b = new AlienBase(_game->getMod()->getDeployment(bgame->getMissionType()));
		b->setId(1);
		b->setAlienRace(_alienRaces[_cbxAlienRace->getSelected()]);
		_craft->setDestination(b);
		bgen.setAlienBase(b);
		_game->getSavedGame()->getAlienBases()->push_back(b);
	}
	// ufo assault
	else if (_craft && _game->getMod()->getUfo(_missionTypes[_cbxMission->getSelected()]))
	{
		Ufo *u = new Ufo(_game->getMod()->getUfo(_missionTypes[_cbxMission->getSelected()]));
		u->setId(1);
		_craft->setDestination(u);
		bgen.setUfo(u);
		// either ground assault or ufo crash
		if (RNG::generate(0,1) == 1)
			bgame->setMissionType("STR_UFO_GROUND_ASSAULT");
		else
			bgame->setMissionType("STR_UFO_CRASH_RECOVERY");
		_game->getSavedGame()->getUfos()->push_back(u);
	}
	// mission site
	else
	{
		const AlienDeployment *deployment = _game->getMod()->getDeployment(bgame->getMissionType());
		const RuleAlienMission *mission = _game->getMod()->getAlienMission(_game->getMod()->getAlienMissionList().front()); // doesn't matter
		MissionSite *m = new MissionSite(mission, deployment);
		m->setId(1);
		m->setAlienRace(_alienRaces[_cbxAlienRace->getSelected()]);
		_craft->setDestination(m);
		bgen.setMissionSite(m);
		_game->getSavedGame()->getMissionSites()->push_back(m);
	}

	if (_craft)
	{
		_craft->setSpeed(0);
		bgen.setCraft(_craft);
	}

	_game->getSavedGame()->setDifficulty((GameDifficulty)_cbxDifficulty->getSelected());

	bgen.setWorldShade(_slrDarkness->getValue());
	bgen.setAlienRace(_alienRaces[_cbxAlienRace->getSelected()]);
	bgen.setAlienItemlevel(_slrAlienTech->getValue());
	bgame->setDepth(_slrDepth->getValue());

	bgen.run();

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
	initSave();

	_cbxMission->setSelected(RNG::generate(0, _missionTypes.size()-1));
	cbxMissionChange(0);
	_cbxCraft->setSelected(RNG::generate(0, _crafts.size() - 1));
	cbxCraftChange(0);
	_slrDarkness->setValue(RNG::generate(0, 15));
	_cbxTerrain->setSelected(RNG::generate(0, _terrainTypes.size() - 1));
	cbxTerrainChange(0);
	_cbxAlienRace->setSelected(RNG::generate(0, _alienRaces.size()-1));
	_cbxDifficulty->setSelected(RNG::generate(0, 4));
	_slrAlienTech->setValue(RNG::generate(0, _game->getMod()->getAlienItemLevels().size()-1));
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
	AlienDeployment *ruleDeploy = _game->getMod()->getDeployment(_missionTypes[_cbxMission->getSelected()]);
	std::set<std::string> terrains;

	// Get terrains associated with this mission
	std::vector<std::string> deployTerrains, globeTerrains;
	deployTerrains = ruleDeploy->getTerrains();
	if (deployTerrains.empty())
	{
		globeTerrains = _game->getMod()->getGlobe()->getTerrains("");
	}
	else
	{
		globeTerrains = _game->getMod()->getGlobe()->getTerrains(ruleDeploy->getType());
	}
	for (std::vector<std::string>::const_iterator i = deployTerrains.begin(); i != deployTerrains.end(); ++i)
	{
		terrains.insert(*i);
	}
	for (std::vector<std::string>::const_iterator i = globeTerrains.begin(); i != globeTerrains.end(); ++i)
	{
		terrains.insert(*i);
	}
	_terrainTypes.clear();
	std::vector<std::string> terrainStrings;
	for (std::set<std::string>::const_iterator i = terrains.begin(); i != terrains.end(); ++i)
	{
		_terrainTypes.push_back(*i);
		terrainStrings.push_back("MAP_" + *i);
	}

	// Hide controls that don't apply to mission
	_txtDarkness->setVisible(ruleDeploy->getShade() == -1);
	_slrDarkness->setVisible(ruleDeploy->getShade() == -1);
	_txtTerrain->setVisible(_terrainTypes.size() > 1);
	_cbxTerrain->setVisible(_terrainTypes.size() > 1);
	_cbxTerrain->setOptions(terrainStrings);
	_cbxTerrain->setSelected(0);
	cbxTerrainChange(0);
}

/**
 * Updates craft accordingly.
 * @param action Pointer to an action.
 */
void NewBattleState::cbxCraftChange(Action *)
{
	_craft->changeRules(_game->getMod()->getCraft(_crafts[_cbxCraft->getSelected()]));
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
	AlienDeployment *ruleDeploy = _game->getMod()->getDeployment(_missionTypes[_cbxMission->getSelected()]);
	int minDepth = 0;
	int maxDepth = 0;
	if (ruleDeploy->getMaxDepth() > 0 || _game->getMod()->getTerrain(_terrainTypes.at(_cbxTerrain->getSelected()))->getMaxDepth() > 0 ||
		(!ruleDeploy->getTerrains().empty() && _game->getMod()->getTerrain(ruleDeploy->getTerrains().front())->getMaxDepth() > 0))
	{
		minDepth = 1;
		maxDepth = 3;
	}
	_txtDepth->setVisible(minDepth != maxDepth);
	_slrDepth->setVisible(minDepth != maxDepth);
	_slrDepth->setRange(minDepth, maxDepth);
	_slrDepth->setValue(minDepth);
}

}
