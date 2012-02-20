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
#include "Ruleset.h"
#include <fstream>
#include "yaml.h"
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
#include "RuleResearchProject.h"
#include "RuleManufactureInfo.h"

namespace OpenXcom
{

/**
 * Creates a ruleset with blank sets of rules.
 */
Ruleset::Ruleset() : _names(), _countries(), _regions(), _facilities(), _crafts(), _craftWeapons(), _items(), _ufos(),
					 _terrains(), _mapDataSets(), _soldiers(), _units(), _invs(), _costSoldier(0), _costEngineer(0), _costScientist(0), _timePersonnel(0)
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
	for (std::map<std::string, RuleResearchProject *>::const_iterator i = _researchProjects.begin (); i != _researchProjects.end (); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleManufactureInfo *>::const_iterator i = _manufacture.begin (); i != _manufacture.end (); ++i)
	{
		delete i->second;
	}
}

/**
 * Loads a ruleset's contents from a YAML file.
 * @param filename YAML filename.
 */
void Ruleset::load(const std::string &filename)
{
	std::string s = Options::getDataFolder() + "Ruleset/" + filename + ".rul";
	std::ifstream fin(s.c_str());
	if (!fin)
	{
		throw Exception("Failed to load ruleset");
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
				j.second()["type"] >> type;
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
				rule->load(j.second());
			}
		}
		else if (key == "regions")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				j.second()["type"] >> type;
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
				rule->load(j.second());
			}
		}
		else if (key == "facilities")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				j.second()["type"] >> type;
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
				rule->load(j.second());
			}
		}
		else if (key == "mapDataSets")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				j.second()["name"] >> type;
				MapDataSet *rule;
				if (_mapDataSets.find(type) != _mapDataSets.end())
				{
					rule = _mapDataSets[type];
				}
				else
				{
					rule = new MapDataSet(type, 0);
					_mapDataSets[type] = rule;
				}
				rule->load(j.second());
			}
		}
		else if (key == "crafts")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				j.second()["type"] >> type;
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
				rule->load(j.second(), this);
			}
		}
		else if (key == "craftWeapons")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				j.second()["type"] >> type;
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
				rule->load(j.second());
			}
		}
		else if (key == "items")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				j.second()["type"] >> type;
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
				rule->load(j.second());
			}
		}
		else if (key == "ufos")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				j.second()["type"] >> type;
				RuleUfo *rule;
				if (_ufos.find(type) != _ufos.end())
				{
					rule = _ufos[type];
				}
				else
				{
					rule = new RuleUfo(type);
					_ufos[type] = rule;
				}
				rule->load(j.second(), this);
			}
		}
		else if (key == "invs")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				j.second()["id"] >> type;
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
				rule->load(j.second());
			}
		}
		else if (key == "terrains")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				j.second()["name"] >> type;
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
				rule->load(j.second(), this);
			}
		}
		else if (key == "armors")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string type;
				j.second()["type"] >> type;
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
				rule->load(j.second());
			}
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
	}

	fin.close();
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

	out << YAML::BeginDoc;
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
	out << YAML::Key << "mapDataSets" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::map<std::string, MapDataSet*>::const_iterator i = _mapDataSets.begin(); i != _mapDataSets.end(); ++i)
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
	out << YAML::Key << "costSoldier" << YAML::Value << _costSoldier;
	out << YAML::Key << "costEngineer" << YAML::Value << _costEngineer;
	out << YAML::Key << "costScientist" << YAML::Value << _costScientist;
	out << YAML::Key << "timePersonnel" << YAML::Value << _timePersonnel;
	out << YAML::EndMap;
	sav << out.c_str();
	sav.close();
}

/**
 * Generates a brand new blank saved game.
 * @param diff Difficulty for the save.
 * @return New saved game.
 */
SavedGame *Ruleset::newSave(GameDifficulty diff) const
{
	SavedGame *save = new SavedGame(diff);

	return save;
}

/**
 * Returns the list of soldier name pools.
 * @return Pointer to soldier name pool list.
 */
std::vector<SoldierNamePool*> *const Ruleset::getPools()
{
	return &_names;
}

/**
 * Returns the rules for the specified country.
 * @param id Country type.
 * @return Rules for the country.
 */
RuleCountry *const Ruleset::getCountry(const std::string &id) const
{
	return _countries.find(id)->second;
}

/**
 * Returns the rules for the specified region.
 * @param id Region type.
 * @return Rules for the region.
 */
RuleRegion *const Ruleset::getRegion(const std::string &id) const
{
	return _regions.find(id)->second;
}

/**
 * Returns the rules for the specified base facility.
 * @param id Facility type.
 * @return Rules for the facility.
 */
RuleBaseFacility *const Ruleset::getBaseFacility(const std::string &id) const
{
	return _facilities.find(id)->second;
}

/**
 * Returns the rules for the specified craft.
 * @param id Craft type.
 * @return Rules for the craft.
 */
RuleCraft *const Ruleset::getCraft(const std::string &id) const
{
	return _crafts.find(id)->second;
}

/**
 * Returns the rules for the specified craft weapon.
 * @param id Craft weapon type.
 * @return Rules for the craft weapon.
 */
RuleCraftWeapon *const Ruleset::getCraftWeapon(const std::string &id) const
{
	return _craftWeapons.find(id)->second;
}
/**
 * Returns the rules for the specified item.
 * @param id Item type.
 * @return Rules for the item. Or 0 when the item is not found.
 */
RuleItem *const Ruleset::getItem(const std::string &id) const
{
	if (_items.find(id) != _items.end())
		return _items.find(id)->second;
	else
		return 0;
}

/**
 * Returns the rules for the specified UFO.
 * @param id UFO type.
 * @return Rules for the UFO.
 */
RuleUfo *const Ruleset::getUfo(const std::string &id) const
{
	return _ufos.find(id)->second;
}

/**
 * Returns the rules for the specified terrain.
 * @param name terrain name.
 * @return Rules for the terrain.
 */
RuleTerrain *const Ruleset::getTerrain(const std::string &name) const
{
	return _terrains.find(name)->second;
}

/**
 * Returns the info about a specific map data file
 * @param name datafile name.
 * @return Rules for the datafile.
 */
MapDataSet *const Ruleset::getMapDataSet(const std::string &name) const
{
	return _mapDataSets.find(name)->second;
}

/**
 * Returns the info about a specific unit
 * @param name Unit name.
 * @return Rules for the units.
 */
RuleSoldier *const Ruleset::getSoldier(const std::string &name) const
{
	return _soldiers.find(name)->second;
}

/**
 * Returns the info about a specific unit
 * @param name Unit name.
 * @return Rules for the units.
 */
Unit *const Ruleset::getUnit(const std::string &name) const
{
	return _units.find(name)->second;
}

/**
 * Returns the info about a specific alien race
 * @param name Race name.
 * @return Rules for the race.
 */
AlienRace *const Ruleset::getAlienRace(const std::string &name) const
{
	return _alienRaces.find(name)->second;
}

/**
 * Returns the info about a specific deployment
 * @param name Deployment name.
 * @return Rules for the deployment.
 */
AlienDeployment *const Ruleset::getDeployment(const std::string &name) const
{
	return _alienDeployments.find(name)->second;
}

/**
 * Returns the info about a specific armor
 * @param name Armor name.
 * @return Rules for the armor.
 */
Armor *const Ruleset::getArmor(const std::string &name) const
{
	return _armors.find(name)->second;
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
ArticleDefinition *const Ruleset::getUfopaediaArticle(const std::string &name) const
{
	return _ufopaediaArticles.find(name)->second;
}

/**
 * Returns the list of inventories.
 * @return Pointer to inventory list.
 */
std::map<std::string, RuleInventory*> *const Ruleset::getInventories()
{
	return &_invs;
}

/**
 * Returns the rules for a specific inventory.
 * @param id Inventory type.
 * @return Inventory ruleset.
 */
RuleInventory *const Ruleset::getInventory(const std::string &id) const
{
	return _invs.find(id)->second;
}

/**
 * Returns the rules for the specified research project.
 * @param id Research project type.
 * @return Rules for the research project.
 */
RuleResearchProject *Ruleset::getResearchProject (const std::string &id) const
{
	return _researchProjects.find(id)->second;
}

/**
 * Returns the list of research projects.
 * @return The list of research projects.
 */
const std::map<std::string, RuleResearchProject *> & Ruleset::getResearchProjects () const
{
	return _researchProjects;
}

/**
 * Returns the rules for the specified manufacture project.
 * @param id Manufacture project type.
 * @return Rules for the manufacture project.
 */
RuleManufactureInfo *Ruleset::getManufactureProject (const std::string &id) const
{
	return _manufacture.find(id)->second;
}

/**
 * Returns the list of manufacture projects.
 * @return The list of manufacture projects.
 */
const std::map<std::string, RuleManufactureInfo *> & Ruleset::getManufactureProjects () const
{
	return _manufacture;
}

}
