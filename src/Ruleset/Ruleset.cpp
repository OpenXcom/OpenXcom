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
#include "Ruleset.h"
#include <fstream>
#include "../Engine/Options.h"
#include "../Engine/Exception.h"
#include "../Engine/CrossPlatform.h"
#include "SoldierNamePool.h"
#include "RuleCountry.h"
#include "RuleRegion.h"
#include "RuleBaseFacility.h"
#include "RuleCraft.h"
#include "RuleCraftWeapon.h"
#include "RuleItem.h"
#include "RuleUfo.h"
#include "RuleTerrain.h"
#include "MapDataSet.h"
#include "RuleSoldier.h"
#include "Unit.h"
#include "AlienRace.h"
#include "AlienDeployment.h"
#include "Armor.h"
#include "ArticleDefinition.h"
#include "RuleInventory.h"
#include "RuleResearch.h"
#include "RuleManufacture.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Region.h"
#include "../Savegame/Base.h"
#include "../Savegame/Country.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/Craft.h"
#include "../Ufopaedia/Ufopaedia.h"
#include "../Savegame/AlienStrategy.h"
#include "UfoTrajectory.h"
#include "RuleAlienMission.h"
#include "City.h"
#include "../Engine/Logger.h"
#include <algorithm>
#include <SDL.h>

namespace OpenXcom
{

/**
 * Creates a ruleset with blank sets of rules.
 */
Ruleset::Ruleset() : _costSoldier(0), _costEngineer(0), _costScientist(0), _timePersonnel(0)
{
	// Add soldier names
	std::vector<std::string> names = CrossPlatform::getFolderContents(Options::getDataFolder() + "SoldierName/", "nam");

	for (std::vector<std::string>::iterator i = names.begin(); i != names.end(); ++i)
	{
		std::string file = (*i).substr(0, (*i).length()-4);
		SoldierNamePool *pool = new SoldierNamePool();
		pool->load(file);
		_names.push_back(pool);
	}
}

/**
 * Deletes all the contained rules from memory.
 */
Ruleset::~Ruleset()
{
	for (std::vector<SoldierNamePool*>::iterator i = _names.begin(); i != _names.end(); ++i)
	{
		delete *i;
	}
	for (std::map<std::string, RuleCountry*>::iterator i = _countries.begin(); i != _countries.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleRegion*>::iterator i = _regions.begin(); i != _regions.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleBaseFacility*>::iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleCraft*>::iterator i = _crafts.begin(); i != _crafts.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleCraftWeapon*>::iterator i = _craftWeapons.begin(); i != _craftWeapons.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleItem*>::iterator i = _items.begin(); i != _items.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleUfo*>::iterator i = _ufos.begin(); i != _ufos.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleTerrain*>::iterator i = _terrains.begin(); i != _terrains.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, MapDataSet*>::iterator i = _mapDataSets.begin(); i != _mapDataSets.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleSoldier*>::iterator i = _soldiers.begin(); i != _soldiers.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, Unit*>::iterator i = _units.begin(); i != _units.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, AlienRace*>::iterator i = _alienRaces.begin(); i != _alienRaces.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, AlienDeployment*>::iterator i = _alienDeployments.begin(); i != _alienDeployments.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, Armor*>::iterator i = _armors.begin(); i != _armors.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, ArticleDefinition*>::iterator i = _ufopaediaArticles.begin(); i != _ufopaediaArticles.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleInventory*>::iterator i = _invs.begin(); i != _invs.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleResearch *>::const_iterator i = _research.begin (); i != _research.end (); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleManufacture *>::const_iterator i = _manufacture.begin (); i != _manufacture.end (); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, UfoTrajectory *>::const_iterator i = _ufoTrajectories.begin (); i != _ufoTrajectories.end (); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleAlienMission *>::const_iterator i = _alienMissions.begin (); i != _alienMissions.end (); ++i)
	{
		delete i->second;
	}
}

/**
 * Loads a ruleset's contents from the given source.
 * @param source The source to use.
 */
void Ruleset::load(const std::string &source)
{
	Uint32 startTime = SDL_GetTicks();

	std::string dirname = Options::getDataFolder() + "Ruleset/" + source + '/';
	if (!CrossPlatform::folderExists(dirname))
		loadFile(Options::getDataFolder() + "Ruleset/" + source + ".rul");
	else
		loadFiles(dirname);
	
	Log(LOG_DEBUG) << "Ruleset loaded in " << (SDL_GetTicks() - startTime) << "ms";
}

/**
 * Loads a ruleset's contents from a YAML file.
 * Rules that match pre-existing rules overwrite them.
 * @param filename YAML filename.
 */
void Ruleset::loadFile(const std::string &filename)
{
	std::ifstream fin(filename.c_str());
	if (!fin)
	{
		throw Exception(filename + " not found");
	}
	YAML::Parser parser(fin);
	YAML::Node doc;

	parser.GetNextDocument(doc);
	for (YAML::Iterator i = doc.begin(); i != doc.end(); ++i)
	{
		std::string key;
		i.first() >> key;
		if (key == "countries")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				(*j)["type"] >> type;
				RuleCountry *rule;
				if (_countries.find(type) != _countries.end())
				{
					rule = _countries[type];
				}
				else
				{
					rule = new RuleCountry(type);
					_countries[type] = rule;
					_countriesIndex.push_back(type);
				}
				rule->load(*j);
			}
		}
		else if (key == "regions")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				(*j)["type"] >> type;
				RuleRegion *rule;
				if (_regions.find(type) != _regions.end())
				{
					rule = _regions[type];
				}
				else
				{
					rule = new RuleRegion(type);
					_regions[type] = rule;
					_regionsIndex.push_back(type);
				}
				rule->load(*j);
			}
		}
		else if (key == "facilities")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				(*j)["type"] >> type;
				RuleBaseFacility *rule;
				if (_facilities.find(type) != _facilities.end())
				{
					rule = _facilities[type];
				}
				else
				{
					rule = new RuleBaseFacility(type);
					_facilities[type] = rule;
					_facilitiesIndex.push_back(type);
				}
				rule->load(*j);
			}
		}
		else if (key == "crafts")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				(*j)["type"] >> type;
				RuleCraft *rule;
				if (_crafts.find(type) != _crafts.end())
				{
					rule = _crafts[type];
				}
				else
				{
					rule = new RuleCraft(type);
					_crafts[type] = rule;
					_craftsIndex.push_back(type);
				}
				rule->load(*j, this);
			}
		}
		else if (key == "craftWeapons")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				(*j)["type"] >> type;
				RuleCraftWeapon *rule;
				if (_craftWeapons.find(type) != _craftWeapons.end())
				{
					rule = _craftWeapons[type];
				}
				else
				{
					rule = new RuleCraftWeapon(type);
					_craftWeapons[type] = rule;
					_craftWeaponsIndex.push_back(type);
				}
				rule->load(*j);
			}
		}
		else if (key == "items")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				(*j)["type"] >> type;
				RuleItem *rule;
				if (_items.find(type) != _items.end())
				{
					rule = _items[type];
				}
				else
				{
					rule = new RuleItem(type);
					_items[type] = rule;
					_itemsIndex.push_back(type);
				}
				rule->load(*j);
			}
		}
		else if (key == "ufos")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				(*j)["type"] >> type;
				RuleUfo *rule;
				if (_ufos.find(type) != _ufos.end())
				{
					rule = _ufos[type];
				}
				else
				{
					rule = new RuleUfo(type);
					_ufos[type] = rule;
					_ufosIndex.push_back(type);
				}
				rule->load(*j, this);
			}
		}
		else if (key == "invs")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				(*j)["id"] >> type;
				RuleInventory *rule;
				if (_invs.find(type) != _invs.end())
				{
					rule = _invs[type];
				}
				else
				{
					rule = new RuleInventory(type);
					_invs[type] = rule;
				}
				rule->load(*j);
			}
		}
		else if (key == "terrains")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				(*j)["name"] >> type;
				RuleTerrain *rule;
				if (_terrains.find(type) != _terrains.end())
				{
					rule = _terrains[type];
				}
				else
				{
					rule = new RuleTerrain(type);
					_terrains[type] = rule;
				}
				rule->load(*j, this);
			}
		}
		else if (key == "armors")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				(*j)["type"] >> type;
				Armor *rule;
				if (_armors.find(type) != _armors.end())
				{
					rule = _armors[type];
				}
				else
				{
					rule = new Armor(type, "", 0);
					_armors[type] = rule;
					_armorsIndex.push_back(type);
				}
				rule->load(*j);
			}
		}
		else if (key == "soldiers")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				(*j)["type"] >> type;
				RuleSoldier *rule;
				if (_soldiers.find(type) != _soldiers.end())
				{
					rule = _soldiers[type];
				}
				else
				{
					rule = new RuleSoldier(type);
					_soldiers[type] = rule;
				}
				rule->load(*j);
			}
		}
		else if (key == "units")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				(*j)["type"] >> type;
				Unit *rule;
				if (_units.find(type) != _units.end())
				{
					rule = _units[type];
				}
				else
				{
					rule = new Unit(type, "", "");
					_units[type] = rule;
				}
				rule->load(*j);
			}
		}
		else if (key == "alienRaces")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				(*j)["id"] >> type;
				AlienRace *rule;
				if (_alienRaces.find(type) != _alienRaces.end())
				{
					rule = _alienRaces[type];
				}
				else
				{
					rule = new AlienRace(type);
					_alienRaces[type] = rule;
					_aliensIndex.push_back(type);
				}
				rule->load(*j);
			}
		}
		else if (key == "alienDeployments")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				(*j)["type"] >> type;
				AlienDeployment *rule;
				if (_alienDeployments.find(type) != _alienDeployments.end())
				{
					rule = _alienDeployments[type];
				}
				else
				{
					rule = new AlienDeployment(type);
					_alienDeployments[type] = rule;
					_deploymentsIndex.push_back(type);
				}
				rule->load(*j);
			}
		}
		else if (key == "research")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				(*j)["name"] >> type;
				RuleResearch *rule;
				if (_research.find(type) != _research.end())
				{
					rule = _research[type];
				}
				else
				{
					rule = new RuleResearch(type);
					_research[type] = rule;
					_researchIndex.push_back(type);
				}
				rule->load(*j);
			}
		}
		else if (key == "manufacture")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				(*j)["name"] >> type;
				RuleManufacture *rule;
				if (_manufacture.find(type) != _manufacture.end())
				{
					rule = _manufacture[type];
				}
				else
				{
					rule = new RuleManufacture(type);
					_manufacture[type] = rule;
					_manufactureIndex.push_back(type);
				}
				rule->load(*j);
			}
		}
		else if (key == "ufopaedia")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string id;
				(*j)["id"] >> id;
				ArticleDefinition *rule;
				if (_ufopaediaArticles.find(id) != _ufopaediaArticles.end())
				{
					rule = _ufopaediaArticles[id];
				}
				else
				{
					int type;
					(*j)["type_id"] >> type;
					switch ((UfopaediaTypeId)type)
					{
					case UFOPAEDIA_TYPE_CRAFT: rule = new ArticleDefinitionCraft(); break;
					case UFOPAEDIA_TYPE_CRAFT_WEAPON: rule = new ArticleDefinitionCraftWeapon(); break;
					case UFOPAEDIA_TYPE_VEHICLE: rule = new ArticleDefinitionVehicle(); break;
					case UFOPAEDIA_TYPE_ITEM: rule = new ArticleDefinitionItem(); break;
					case UFOPAEDIA_TYPE_ARMOR: rule = new ArticleDefinitionArmor(); break;
					case UFOPAEDIA_TYPE_BASE_FACILITY: rule = new ArticleDefinitionBaseFacility(); break;
					case UFOPAEDIA_TYPE_TEXTIMAGE: rule = new ArticleDefinitionTextImage(); break;
					case UFOPAEDIA_TYPE_TEXT: rule = new ArticleDefinitionText(); break;
					case UFOPAEDIA_TYPE_UFO: rule = new ArticleDefinitionUfo(); break;
					default: rule = 0; break;
					}
					_ufopaediaArticles[id] = rule;
					_ufopaediaIndex.push_back(id);
				}
				rule->load(*j);
			}
		}
		else if (key == "startingBase")
		{
			//_startingBase->load(i.second(), 0);
			_startingBase = i.second().Clone();
		}
		else if (key == "costSoldier")
		{
			i.second() >> _costSoldier;
		}
		else if (key == "costEngineer")
		{
			i.second() >> _costEngineer;
		}
		else if (key == "costScientist")
		{
			i.second() >> _costScientist;
		}
		else if (key == "timePersonnel")
		{
			i.second() >> _timePersonnel;
		}
		else if (key == "ufoTrajectories")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string id;
				(*j)["id"] >> id;
				if (_ufoTrajectories.find(id) != _ufoTrajectories.end())
				{
					_ufoTrajectories[id]->load(*j);
				}
				else
				{
					std::auto_ptr<UfoTrajectory> rule(new UfoTrajectory);
					rule->load(*j);
					_ufoTrajectories[id] = rule.release();
				}
			}
		}
		else if (key == "alienMissions")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				(*j)["type"] >> type;
				if (_alienMissions.find(type) != _alienMissions.end())
				{
					_alienMissions[type]->load(*j);
				}
				else
				{
					std::auto_ptr<RuleAlienMission> rule(new RuleAlienMission());
					rule->load(*j);
					_alienMissions[type] = rule.release();
					_alienMissionsIndex.push_back(type);
				}
			}
		}
		else if (key == "alienItemLevels")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::vector<int> type;
				(*j) >> type;
				_alienItemLevels.push_back(type);
			}
		}
	}

	fin.close();
}

/**
 * Load the contents of all rule files in the given directory.
 * @param dirname The name of an existing directory containing rule files.
 */
void Ruleset::loadFiles(const std::string &dirname)
{
	std::vector<std::string> names = CrossPlatform::getFolderContents(dirname, "rul");

	for (std::vector<std::string>::iterator i = names.begin(); i != names.end(); ++i)
	{
		loadFile(dirname + *i);
	}
}

/**
 * Saves a ruleset's contents to a YAML file.
 * @param filename YAML filename.
 */
void Ruleset::save(const std::string &filename) const
{
	std::string s = Options::getDataFolder() + "Ruleset/" + filename + ".rul";
	std::ofstream sav(s.c_str());
	if (!sav)
	{
		throw Exception("Failed to save ruleset");
	}

	YAML::Emitter out;

	out << YAML::BeginMap;
	out << YAML::Key << "countries" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::map<std::string, RuleCountry*>::const_iterator i = _countries.begin(); i != _countries.end(); ++i)
	{
		i->second->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "regions" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::map<std::string, RuleRegion*>::const_iterator i = _regions.begin(); i != _regions.end(); ++i)
	{
		i->second->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "facilities" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::map<std::string, RuleBaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		i->second->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "crafts" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::map<std::string, RuleCraft*>::const_iterator i = _crafts.begin(); i != _crafts.end(); ++i)
	{
		i->second->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "craftWeapons" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::map<std::string, RuleCraftWeapon*>::const_iterator i = _craftWeapons.begin(); i != _craftWeapons.end(); ++i)
	{
		i->second->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "items" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::map<std::string, RuleItem*>::const_iterator i = _items.begin(); i != _items.end(); ++i)
	{
		i->second->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "ufos" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::map<std::string, RuleUfo*>::const_iterator i = _ufos.begin(); i != _ufos.end(); ++i)
	{
		i->second->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "invs" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::map<std::string, RuleInventory*>::const_iterator i = _invs.begin(); i != _invs.end(); ++i)
	{
		i->second->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "terrains" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::map<std::string, RuleTerrain*>::const_iterator i = _terrains.begin(); i != _terrains.end(); ++i)
	{
		i->second->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "armors" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::map<std::string, Armor*>::const_iterator i = _armors.begin(); i != _armors.end(); ++i)
	{
		i->second->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "soldiers" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::map<std::string, RuleSoldier*>::const_iterator i = _soldiers.begin(); i != _soldiers.end(); ++i)
	{
		i->second->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "units" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::map<std::string, Unit*>::const_iterator i = _units.begin(); i != _units.end(); ++i)
	{
		i->second->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "alienRaces" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::map<std::string, AlienRace*>::const_iterator i = _alienRaces.begin(); i != _alienRaces.end(); ++i)
	{
		i->second->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "alienDeployments" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::map<std::string, AlienDeployment*>::const_iterator i = _alienDeployments.begin(); i != _alienDeployments.end(); ++i)
	{
		i->second->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "research" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::map<std::string, RuleResearch*>::const_iterator i = _research.begin(); i != _research.end(); ++i)
	{
		i->second->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "manufacture" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::map<std::string, RuleManufacture*>::const_iterator i = _manufacture.begin(); i != _manufacture.end(); ++i)
	{
		i->second->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "ufopaedia" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::map<std::string, ArticleDefinition*>::const_iterator i = _ufopaediaArticles.begin(); i != _ufopaediaArticles.end(); ++i)
	{
		i->second->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "ufoTrajectories" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::map<std::string, UfoTrajectory*>::const_iterator i = _ufoTrajectories.begin(); i != _ufoTrajectories.end(); ++i)
	{
		i->second->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "alienMissions" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::map<std::string, RuleAlienMission*>::const_iterator i = _alienMissions.begin(); i != _alienMissions.end(); ++i)
	{
		i->second->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "alienItemLevels" << YAML::Value;
	out << YAML::BeginSeq;
	for ( std::vector<std::vector<int> >::const_iterator i = _alienItemLevels.begin(); i != _alienItemLevels.end(); ++i)
	{
		out << *i;
	}
	out << YAML::EndSeq;
	/*out << YAML::Key << "startingBase" << YAML::Value;
	_startingBase->save(out);*/
	out << YAML::Key << "costSoldier" << YAML::Value << _costSoldier;
	out << YAML::Key << "costEngineer" << YAML::Value << _costEngineer;
	out << YAML::Key << "costScientist" << YAML::Value << _costScientist;
	out << YAML::Key << "timePersonnel" << YAML::Value << _timePersonnel;
	out << YAML::EndMap;
	sav << out.c_str();
	sav.close();
}

/**
 * Generates a brand new saved game with starting data.
 * @return New saved game.
 */
SavedGame *Ruleset::newSave() const
{
	SavedGame *save = new SavedGame();

	// Add countries
	for (std::vector<std::string>::const_iterator i = _countriesIndex.begin(); i != _countriesIndex.end(); ++i)
	{
		save->getCountries()->push_back(new Country(getCountry(*i)));
	}
	// Adjust funding to total $6M
	int missing = ((6000 - save->getCountryFunding()/1000) / (int)save->getCountries()->size()) * 1000;
	for (std::vector<Country*>::iterator i = save->getCountries()->begin(); i != save->getCountries()->end(); ++i)
	{
		(*i)->setFunding((*i)->getFunding().back() + missing);
	}
	save->setFunds(save->getCountryFunding());

	// Add regions
	for (std::vector<std::string>::const_iterator i = _regionsIndex.begin(); i != _regionsIndex.end(); ++i)
	{
		save->getRegions()->push_back(new Region(getRegion(*i)));
	}

	// Set up IDs
	std::map<std::string, int> ids;
	for (std::vector<std::string>::const_iterator i = _craftsIndex.begin(); i != _craftsIndex.end(); ++i)
	{
		ids[*i] = 1;
	}
	ids["STR_UFO"] = 1;
	ids["STR_WAYPOINT"] = 1;
	ids["STR_TERROR_SITE"] = 1;
	ids["STR_ALIEN_BASE"] = 1;
	ids["STR_SOLDIER"] = 1;
	ids["ALIEN_MISSIONS"] = 1;
	save->initIds(ids);

	// Set up starting base
	Base *base = new Base(this);
	base->load(*_startingBase->begin(), save, true);

	// Correct IDs
	for (std::vector<Craft*>::const_iterator i = base->getCrafts()->begin(); i != base->getCrafts()->end(); ++i)
	{
		save->getId((*i)->getRules()->getType());
	}

	// Generate soldiers
	int soldiers = 0;
	if (const YAML::Node *pName = (*_startingBase->begin()).FindValue("randomSoldiers"))
	{
		(*pName) >> soldiers;
	}
	for (int i = 0; i < soldiers; ++i)
	{
		Soldier *soldier = new Soldier(getSoldier("XCOM"), getArmor("STR_NONE_UC"), &_names, save->getId("STR_SOLDIER"));
		soldier->setCraft(base->getCrafts()->front());
		base->getSoldiers()->push_back(soldier);
	}

	save->getBases()->push_back(base);
	// Setup alien strategy
	save->getAlienStrategy().init(this);

	return save;
}

/**
 * Returns the list of soldier name pools.
 * @return Pointer to soldier name pool list.
 */
const std::vector<SoldierNamePool*> &Ruleset::getPools() const
{
	return _names;
}

/**
 * Returns the rules for the specified country.
 * @param id Country type.
 * @return Rules for the country.
 */
RuleCountry *Ruleset::getCountry(const std::string &id) const
{
	std::map<std::string, RuleCountry*>::const_iterator i = _countries.find(id);
	if (_countries.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of all countries.
 * provided by the ruleset.
 * @return List of countries.
 */
const std::vector<std::string> &Ruleset::getCountriesList() const
{
	return _countriesIndex;
}

/**
 * Returns the rules for the specified region.
 * @param id Region type.
 * @return Rules for the region.
 */
RuleRegion *Ruleset::getRegion(const std::string &id) const
{
	std::map<std::string, RuleRegion*>::const_iterator i = _regions.find(id);
	if (_regions.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of all regions
 * provided by the ruleset.
 * @return List of regions.
 */
const std::vector<std::string> &Ruleset::getRegionsList() const
{
	return _regionsIndex;
}

/**
 * Returns the rules for the specified base facility.
 * @param id Facility type.
 * @return Rules for the facility.
 */
RuleBaseFacility *Ruleset::getBaseFacility(const std::string &id) const
{
	std::map<std::string, RuleBaseFacility*>::const_iterator i = _facilities.find(id);
	if (_facilities.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of all base facilities
 * provided by the ruleset.
 * @return List of base faciliies.
 */
const std::vector<std::string> &Ruleset::getBaseFacilitiesList() const
{
	return _facilitiesIndex;
}

/**
 * Returns the rules for the specified craft.
 * @param id Craft type.
 * @return Rules for the craft.
 */
RuleCraft *Ruleset::getCraft(const std::string &id) const
{
	std::map<std::string, RuleCraft*>::const_iterator i = _crafts.find(id);
	if (_crafts.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of all crafts
 * provided by the ruleset.
 * @return List of crafts.
 */
const std::vector<std::string> &Ruleset::getCraftsList() const
{
	return _craftsIndex;
}

/**
 * Returns the rules for the specified craft weapon.
 * @param id Craft weapon type.
 * @return Rules for the craft weapon.
 */
RuleCraftWeapon *Ruleset::getCraftWeapon(const std::string &id) const
{
	std::map<std::string, RuleCraftWeapon*>::const_iterator i = _craftWeapons.find(id);
	if (_craftWeapons.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of all craft weapons
 * provided by the ruleset.
 * @return List of craft weapons.
 */
const std::vector<std::string> &Ruleset::getCraftWeaponsList() const
{
	return _craftWeaponsIndex;
}

/**
 * Returns the rules for the specified item.
 * @param id Item type.
 * @return Rules for the item. Or 0 when the item is not found.
 */
RuleItem *Ruleset::getItem(const std::string &id) const
{
	if (_items.find(id) != _items.end())
		return _items.find(id)->second;
	else
		return 0;
}

/**
 * Returns the list of all items
 * provided by the ruleset.
 * @return List of items.
 */
const std::vector<std::string> &Ruleset::getItemsList() const
{
	return _itemsIndex;
}

/**
 * Returns the rules for the specified UFO.
 * @param id UFO type.
 * @return Rules for the UFO.
 */
RuleUfo *Ruleset::getUfo(const std::string &id) const
{
	std::map<std::string, RuleUfo*>::const_iterator i = _ufos.find(id);
	if (_ufos.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of all ufos
 * provided by the ruleset.
 * @return List of ufos.
 */
const std::vector<std::string> &Ruleset::getUfosList() const
{
	return _ufosIndex;
}

/**
 * Returns the rules for the specified terrain.
 * @param name terrain name.
 * @return Rules for the terrain.
 */
RuleTerrain *Ruleset::getTerrain(const std::string &name) const
{
	std::map<std::string, RuleTerrain*>::const_iterator i = _terrains.find(name);
	if (_terrains.end() != i) return i->second; else return 0;
}

/**
 * Returns the info about a specific map data file
 * @param name datafile name.
 * @return Rules for the datafile.
 */
MapDataSet *Ruleset::getMapDataSet(const std::string &name)
{
	std::map<std::string, MapDataSet*>::iterator map = _mapDataSets.find(name);
	if (map == _mapDataSets.end())
	{
		MapDataSet *set = new MapDataSet(name);
		_mapDataSets[name] = set;
		return set;
	}
	else
	{
		return map->second;
	}
}

/**
 * Returns the info about a specific unit
 * @param name Unit name.
 * @return Rules for the units.
 */
RuleSoldier *Ruleset::getSoldier(const std::string &name) const
{
	std::map<std::string, RuleSoldier*>::const_iterator i = _soldiers.find(name);
	if (_soldiers.end() != i) return i->second; else return 0;
}

/**
 * Returns the info about a specific unit
 * @param name Unit name.
 * @return Rules for the units.
 */
Unit *Ruleset::getUnit(const std::string &name) const
{
	std::map<std::string, Unit*>::const_iterator i = _units.find(name);
	if (_units.end() != i) return i->second; else return 0;
}

/**
 * Returns the info about a specific alien race
 * @param name Race name.
 * @return Rules for the race.
 */
AlienRace *Ruleset::getAlienRace(const std::string &name) const
{
	std::map<std::string, AlienRace*>::const_iterator i = _alienRaces.find(name);
	if (_alienRaces.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of all alien races
 * provided by the ruleset.
 * @return List of alien races.
 */
const std::vector<std::string> &Ruleset::getAlienRacesList() const
{
	return _aliensIndex;
}

/**
 * Returns the info about a specific deployment
 * @param name Deployment name.
 * @return Rules for the deployment.
 */
AlienDeployment *Ruleset::getDeployment(const std::string &name) const
{
	std::map<std::string, AlienDeployment*>::const_iterator i = _alienDeployments.find(name);
	if (_alienDeployments.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of all alien deployments
 * provided by the ruleset.
 * @return List of alien deployments.
 */
const std::vector<std::string> &Ruleset::getDeploymentsList() const
{
	return _deploymentsIndex;
}

/**
 * Returns the info about a specific armor
 * @param name Armor name.
 * @return Rules for the armor.
 */
Armor *Ruleset::getArmor(const std::string &name) const
{
	std::map<std::string, Armor*>::const_iterator i = _armors.find(name);
	if (_armors.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of all armors
 * provided by the ruleset.
 * @return List of armors.
 */
const std::vector<std::string> &Ruleset::getArmorsList() const
{
	return _armorsIndex;
}

/**
 * Returns the cost of an individual soldier
 * for purchase/maintenance.
 * @return Cost.
 */
int Ruleset::getSoldierCost() const
{
	return _costSoldier;
}

/**
 * Returns the cost of an individual engineer
 * for purchase/maintenance.
 * @return Cost.
 */
int Ruleset::getEngineerCost() const
{
	return _costEngineer;
}

/**
 * Returns the cost of an individual scientist
 * for purchase/maintenance.
 * @return Cost.
 */
int Ruleset::getScientistCost() const
{
	return _costScientist;
}

/**
 * Returns the time it takes to transfer personnel
 * between bases.
 * @return Time in hours.
 */
int Ruleset::getPersonnelTime() const
{
	return _timePersonnel;
}

/**
 * Returns the article definition for a given name.
 * @param name Article name.
 * @return Article definition.
 */
ArticleDefinition *Ruleset::getUfopaediaArticle(const std::string &name) const
{
	std::map<std::string, ArticleDefinition*>::const_iterator i = _ufopaediaArticles.find(name);
	if (_ufopaediaArticles.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of all articles
 * provided by the ruleset.
 * @return List of articles.
 */
const std::vector<std::string> &Ruleset::getUfopaediaList() const
{
	return _ufopaediaIndex;
}

/**
 * Returns the list of inventories.
 * @return Pointer to inventory list.
 */
std::map<std::string, RuleInventory*> *Ruleset::getInventories()
{
	return &_invs;
}

/**
 * Returns the rules for a specific inventory.
 * @param id Inventory type.
 * @return Inventory ruleset.
 */
RuleInventory *Ruleset::getInventory(const std::string &id) const
{
	std::map<std::string, RuleInventory*>::const_iterator i = _invs.find(id);
	if (_invs.end() != i) return i->second; else return 0;
}

/**
 * Returns the rules for the specified research project.
 * @param id Research project type.
 * @return Rules for the research project.
 */
RuleResearch *Ruleset::getResearch (const std::string &id) const
{
	std::map<std::string, RuleResearch *>::const_iterator i = _research.find(id);
	if (_research.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of research projects.
 * @return The list of research projects.
 */
const std::vector<std::string> &Ruleset::getResearchList () const
{
	return _researchIndex;
}

/**
 * Returns the rules for the specified manufacture project.
 * @param id Manufacture project type.
 * @return Rules for the manufacture project.
 */
RuleManufacture *Ruleset::getManufacture (const std::string &id) const
{
	std::map<std::string, RuleManufacture *>::const_iterator i = _manufacture.find(id);
	if (_manufacture.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of manufacture projects.
 * @return The list of manufacture projects.
 */
const std::vector<std::string> &Ruleset::getManufactureList () const
{
	return _manufactureIndex;
}

/**
 * Generates and returns a list of facilities for custom bases.
 * The list contains all the facilities that are listed in the 'startingBase'
 * part of the ruleset.
 * @return The list of facilities for custom bases.
 */
std::vector<OpenXcom::RuleBaseFacility*> Ruleset::getCustomBaseFacilities() const
{
	const YAML::Node &node = *_startingBase->begin();
	std::vector<OpenXcom::RuleBaseFacility*> PlaceList;

	for (YAML::Iterator i = node["facilities"].begin(); i != node["facilities"].end(); ++i)
	{
		std::string type;
		(*i)["type"] >> type;
		if (type != "STR_ACCESS_LIFT")
		{
			PlaceList.push_back(getBaseFacility(type));
		}
	}
	return PlaceList;
}

/**
 * Returns the data for the specified ufo trajectory.
 * @param id Ufo trajectory id.
 * @return A pointer to the data for the specified ufo trajectory.
 */
const UfoTrajectory *Ruleset::getUfoTrajectory(const std::string &id) const
{
	std::map<std::string, UfoTrajectory *>::const_iterator i = _ufoTrajectories.find(id);
	if (_ufoTrajectories.end() != i) return i->second; else return 0;
}

/**
 * Returns the rules for the specified alien mission.
 * @param id Alien mission type.
 * @return Rules for the alien mission.
 */
const RuleAlienMission *Ruleset::getAlienMission(const std::string &id) const
{
	std::map<std::string, RuleAlienMission *>::const_iterator i = _alienMissions.find(id);
	if (_alienMissions.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of alien mission types.
 * @return The list of alien mission types.
 */
const std::vector<std::string> &Ruleset::getAlienMissionList() const
{
	return _alienMissionsIndex;
}

/** @brief Match a city based on coordinates.
 * This function object compare a city's coordinates with the stored coordinates.
 */
class EqualCoordinates: std::unary_function<const City *, bool>
{
public:
	/// Remember the coordinates.
	EqualCoordinates(double lon, double lat) : _lon(lon), _lat(lat) { /* Empty by design */ }
	/// Compare with stored coordinates.
	bool operator()(const City *city) const { return city->getLongitude() == _lon && city->getLatitude() == _lat; }
private:
	double _lon, _lat;
};

/**
 * Find the city at coordinates @a lon, @a lat.
 * The search will only match exact coordinates.
 * @param lon The longtitude.
 * @param lat The latitude.
 * @return A pointer to the city information, or 0 if no city was found.
 */
const City *Ruleset::locateCity(double lon, double lat) const
{
	for (std::map<std::string, RuleRegion*>::const_iterator rr = _regions.begin(); rr != _regions.end(); ++rr)
	{
		const std::vector<City*> &cities = *rr->second->getCities();
		std::vector<City *>::const_iterator citer = std::find_if(cities.begin(), cities.end(), EqualCoordinates(lon, lat));
		if (citer != cities.end())
		{
			return *citer;
		}
	}
	return 0;
}

const std::vector<std::vector<int> > &Ruleset::getAlienItemLevels() const
{
	return _alienItemLevels;
}

const YAML::Node &Ruleset::getStartingBase()
{
	return *_startingBase->begin();
}
}
