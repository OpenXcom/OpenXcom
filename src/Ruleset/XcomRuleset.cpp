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

#include <sstream>
#include <string>
#include "XcomRuleset.h"
#include "../Savegame/SavedGame.h"
#include "RuleCountry.h"
#include "RuleRegion.h"
#include "RuleBaseFacility.h"
#include "RuleCraft.h"
#include "RuleCraftWeapon.h"
#include "RuleItem.h"
#include "RuleUfo.h"
#include "RuleTerrain.h"
#include "MapBlock.h"
#include "MapDataSet.h"
#include "RuleSoldier.h"
#include "RuleAlien.h"
#include "RuleArmor.h"
#include "SoldierNamePool.h"
#include "../Savegame/Region.h"
#include "../Engine/RNG.h"
#include "../Savegame/Base.h"
#include "../Savegame/Country.h"
#include "../Savegame/BaseFacility.h"
#include "../Savegame/Craft.h"
#include "../Savegame/Ufo.h"
#include "../Savegame/CraftWeapon.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/ItemContainer.h"
#include "City.h"
#include "../Savegame/Tile.h"
#include "../Ufopaedia/Ufopaedia.h"
#include "../Savegame/UfopaediaSaved.h"
#include "ArticleDefinition.h"
#include "RuleInventory.h"
#include "RuleResearchProject.h"
#include "../Engine/CrossPlatform.h"
#include "../Engine/Exception.h"
#include "../Engine/Options.h"
#include <fstream>
#include <algorithm>

namespace OpenXcom
{
/**
 * Initializes the X-Com ruleset with all the rules
 * mimicking the original game.
 */
XcomRuleset::XcomRuleset() : Ruleset()
{
	// first load the game config(s)
	std::string s = Options::getUserFolder() + "config" + ".dat";
	std::ifstream fins(s.c_str());
	if (!fins)
	{
		throw Exception("Failed to load savegame");
	}
    YAML::Parser p(fins);
	YAML::Node d;
	p.GetNextDocument(d);

	// Add soldier names
	initNames(d);

	// Add countries
	initCountries(d);

	// Add regions
	initRegions(d);

	// Add base facilities
	initBaseFacilities(d);

	// Add mapdatafiles
	initMapDataFiles(d);

	// Add crafts
	initCrafts(d);

	// Add craft weapons
	initCraftWeapons(d);

	// Add items
	initItems(d);

	// Add UFOs
	initUFOs(d);

	// Add inventory
	initInventory(d);
	
	// Add terrain
	initTerrains(d);

	// Add armor
	initArmors(d);

	// Add units
	initSoldiers(d);
	initAliens(d);

	// create Ufopaedia article definitions
	int sort_key = 1;
	
	// XCOM CRAFT AND ARMAMENT
	ArticleDefinitionCraft article_craft;
	article_craft.section = UFOPAEDIA_XCOM_CRAFT_ARMAMENT;

	article_craft.id = "STR_SKYRANGER";
	article_craft.title = "STR_SKYRANGER";
	article_craft.image_id = "UP004.SPK";
	article_craft.text = "STR_SKYRANGER_UFOPEDIA";
	article_craft.rect_text.set(5, 40, 140, 100);
	article_craft.craft = _crafts[article_craft.id];
	article_craft.rect_stats.set(160, 5, 140, 60);
	article_craft.sort_key = sort_key++;
	
	_ufopaediaArticles[article_craft.id] = new ArticleDefinitionCraft(article_craft);
	
	article_craft.id = "STR_LIGHTNING";
	article_craft.title = "STR_LIGHTNING";
	article_craft.image_id = "UP003.SPK";
	article_craft.text = "STR_LIGHTNING_UFOPEDIA";
	article_craft.rect_text.set(5, 40, 310, 60);
	article_craft.craft = _crafts[article_craft.id];
	article_craft.rect_stats.set(5, 132, 140, 60);
	article_craft.sort_key = sort_key++;
	
	_ufopaediaArticles[article_craft.id] = new ArticleDefinitionCraft(article_craft);
	
	article_craft.id = "STR_AVENGER";
	article_craft.title = "STR_AVENGER";
	article_craft.image_id = "UP001.SPK";
	article_craft.text = "STR_AVENGER_UFOPEDIA";
	article_craft.rect_text.set(5, 40, 140, 100);
	article_craft.craft = _crafts[article_craft.id];
	article_craft.rect_stats.set(160, 5, 140, 60);
	article_craft.sort_key = sort_key++;
	
	_ufopaediaArticles[article_craft.id] = new ArticleDefinitionCraft(article_craft);

	article_craft.id = "STR_INTERCEPTOR";
	article_craft.title = "STR_INTERCEPTOR";
	article_craft.image_id = "UP002.SPK";
	article_craft.text = "STR_INTERCEPTOR_UFOPEDIA";
	article_craft.rect_text.set(5, 40, 210, 60);
	article_craft.craft = _crafts[article_craft.id];
	article_craft.rect_stats.set(5, 110, 140, 60);
	article_craft.sort_key = sort_key++;
	
	_ufopaediaArticles[article_craft.id] = new ArticleDefinitionCraft(article_craft);
	
	article_craft.id = "STR_FIRESTORM";
	article_craft.title = "STR_FIRESTORM";
	article_craft.image_id = "UP005.SPK";
	article_craft.text = "STR_FIRESTORM_UFOPEDIA";
	article_craft.rect_text.set(5, 40, 140, 100);
	article_craft.craft = _crafts[article_craft.id];
	article_craft.rect_stats.set(160, 5, 140, 60);
	article_craft.sort_key = sort_key++;
	
	_ufopaediaArticles[article_craft.id] = new ArticleDefinitionCraft(article_craft);

	
	ArticleDefinitionCraftWeapon article_craft_weapon;
	article_craft_weapon.section = UFOPAEDIA_XCOM_CRAFT_ARMAMENT;
	
	article_craft_weapon.id = "STR_STINGRAY";
	article_craft_weapon.title = "STR_STINGRAY";
	article_craft_weapon.image_id = "UP006.SPK";
	article_craft_weapon.text = "STR_STINGRAY_UFOPEDIA";
	article_craft_weapon.weapon = _craftWeapons[article_craft_weapon.id];
	article_craft_weapon.sort_key = sort_key++;
	
	_ufopaediaArticles[article_craft_weapon.id] = new ArticleDefinitionCraftWeapon(article_craft_weapon);
	
	article_craft_weapon.id = "STR_AVALANCHE";
	article_craft_weapon.title = "STR_AVALANCHE";
	article_craft_weapon.image_id = "UP007.SPK";
	article_craft_weapon.text = "STR_AVALANCHE_UFOPEDIA";
	article_craft_weapon.weapon = _craftWeapons[article_craft_weapon.id];
	article_craft_weapon.sort_key = sort_key++;
	
	_ufopaediaArticles[article_craft_weapon.id] = new ArticleDefinitionCraftWeapon(article_craft_weapon);
	
	article_craft_weapon.id = "STR_CANNON_UC";
	article_craft_weapon.title = "STR_CANNON_UC";
	article_craft_weapon.image_id = "UP008.SPK";
	article_craft_weapon.text = "STR_CANNON_UFOPEDIA";
	article_craft_weapon.weapon = _craftWeapons[article_craft_weapon.id];
	article_craft_weapon.sort_key = sort_key++;
	
	_ufopaediaArticles[article_craft_weapon.id] = new ArticleDefinitionCraftWeapon(article_craft_weapon);
	
	article_craft_weapon.id = "STR_FUSION_BALL_UC";
	article_craft_weapon.title = "STR_FUSION_BALL_UC";
	article_craft_weapon.image_id = "UP009.SPK";
	article_craft_weapon.text = "STR_FUSION_BALL_UFOPEDIA";
	article_craft_weapon.weapon = _craftWeapons[article_craft_weapon.id];
	article_craft_weapon.sort_key = sort_key++;
	
	_ufopaediaArticles[article_craft_weapon.id] = new ArticleDefinitionCraftWeapon(article_craft_weapon);
	
	article_craft_weapon.id = "STR_LASER_CANNON_UC";
	article_craft_weapon.title = "STR_LASER_CANNON_UC";
	article_craft_weapon.image_id = "UP010.SPK";
	article_craft_weapon.text = "STR_LASER_CANNON_UFOPEDIA";
	article_craft_weapon.weapon = _craftWeapons[article_craft_weapon.id];
	article_craft_weapon.sort_key = sort_key++;
	
	_ufopaediaArticles[article_craft_weapon.id] = new ArticleDefinitionCraftWeapon(article_craft_weapon);
	
	article_craft_weapon.id = "STR_PLASMA_BEAM_UC";
	article_craft_weapon.title = "STR_PLASMA_BEAM_UC";
	article_craft_weapon.image_id = "UP011.SPK";
	article_craft_weapon.text = "STR_PLASMA_BEAM_UFOPEDIA";
	article_craft_weapon.weapon = _craftWeapons[article_craft_weapon.id];
	article_craft_weapon.sort_key = sort_key++;
	
	_ufopaediaArticles[article_craft_weapon.id] = new ArticleDefinitionCraftWeapon(article_craft_weapon);
	
	
	// HEAVY WEAPONS PLATFORMS
	
	
	// WEAPONS AND EQUIPMENT
	ArticleDefinitionItem article_item;
	article_item.section = UFOPAEDIA_WEAPONS_AND_EQUIPMENT;
	
	ArticleDefinitionItem article_item_ammo;
	article_item_ammo.section = UFOPAEDIA_NOT_AVAILABLE;
	
	article_item.id = "STR_PISTOL";
	article_item.title = "STR_PISTOL";
	article_item.text = "STR_PISTOL_UFOPEDIA";
	article_item.item = _items[article_item.id];
	article_item.sort_key = sort_key++;
	
	_ufopaediaArticles[article_item.id] = new ArticleDefinitionItem(article_item);
	
	article_item_ammo.id = "STR_PISTOL_CLIP";
	article_item_ammo.title = "STR_PISTOL_CLIP";
	article_item_ammo.item = _items[article_item_ammo.id];
	article_item_ammo.sort_key = sort_key++;
	
	_ufopaediaArticles[article_item_ammo.id] = new ArticleDefinitionItem(article_item_ammo);
	
	article_item.id = "STR_HEAVY_CANNON";
	article_item.title = "STR_HEAVY_CANNON";
	article_item.text = "STR_HEAVY_CANNON_UFOPEDIA";
	article_item.item = _items[article_item.id];
	article_item.sort_key = sort_key++;
	
	_ufopaediaArticles[article_item.id] = new ArticleDefinitionItem(article_item);
	
	article_item_ammo.id = "STR_HC_AP_AMMO";
	article_item_ammo.title = "STR_HC_AP_AMMO";
	article_item_ammo.item = _items[article_item_ammo.id];
	article_item_ammo.sort_key = sort_key++;
	
	_ufopaediaArticles[article_item_ammo.id] = new ArticleDefinitionItem(article_item_ammo);
	
	article_item_ammo.id = "STR_HC_HE_AMMO";
	article_item_ammo.title = "STR_HC_HE_AMMO";
	article_item_ammo.item = _items[article_item_ammo.id];
	article_item_ammo.sort_key = sort_key++;
	
	_ufopaediaArticles[article_item_ammo.id] = new ArticleDefinitionItem(article_item_ammo);

	article_item_ammo.id = "STR_HC_I_AMMO";
	article_item_ammo.title = "STR_HC_I_AMMO";
	article_item_ammo.item = _items[article_item_ammo.id];
	article_item_ammo.sort_key = sort_key++;
	
	_ufopaediaArticles[article_item_ammo.id] = new ArticleDefinitionItem(article_item_ammo);
	
	article_item.id = "STR_GRENADE";
	article_item.title = "STR_GRENADE";
	article_item.text = "STR_GRENADE_UFOPEDIA";
	article_item.item = _items[article_item.id];
	article_item.sort_key = sort_key++;
	
	_ufopaediaArticles[article_item.id] = new ArticleDefinitionItem(article_item);
	
	article_item.id = "STR_SMOKE_GRENADE";
	article_item.title = "STR_SMOKE_GRENADE";
	article_item.text = "STR_SMOKE_GRENADE_UFOPEDIA";
	article_item.item = _items[article_item.id];
	article_item.sort_key = sort_key++;
	
	_ufopaediaArticles[article_item.id] = new ArticleDefinitionItem(article_item);
	
	
	// ALIEN ARTIFACTS
	

	// BASE FACILITIES
	ArticleDefinitionBaseFacility article_facility;
	article_facility.section = UFOPAEDIA_BASE_FACILITIES;
	
	article_facility.id = "STR_ACCESS_LIFT";
	article_facility.title = "STR_ACCESS_LIFT";
	article_facility.text = "STR_ACCESS_LIFT_UFOPEDIA";
	article_facility.facility = _facilities[article_facility.id];
	article_facility.sort_key = sort_key++;
	
	_ufopaediaArticles[article_facility.id] = new ArticleDefinitionBaseFacility(article_facility);
	
	article_facility.id = "STR_HANGAR";
	article_facility.title = "STR_HANGAR";
	article_facility.text = "STR_HANGAR_UFOPEDIA";
	article_facility.facility = _facilities[article_facility.id];
	article_facility.sort_key = sort_key++;
	
	_ufopaediaArticles[article_facility.id] = new ArticleDefinitionBaseFacility(article_facility);
	

	// ALIEN LIFE FORMS
	ArticleDefinitionTextImage article_textimage;
	article_textimage.text_width = 100;
	article_textimage.section = UFOPAEDIA_ALIEN_LIFE_FORMS;

	article_textimage.id = "STR_SECTOID";
	article_textimage.title = "STR_SECTOID";
	article_textimage.image_id = "UP024.SPK";
	article_textimage.text = "STR_SECTOID_UFOPEDIA";
	article_textimage.sort_key = sort_key++;

	_ufopaediaArticles[article_textimage.id] = new ArticleDefinitionTextImage(article_textimage);
	
	article_textimage.id = "STR_SNAKEMAN";
	article_textimage.title = "STR_SNAKEMAN";
	article_textimage.image_id = "UP030.SPK";
	article_textimage.text = "STR_SNAKEMAN_UFOPEDIA";
	article_textimage.sort_key = sort_key++;

	_ufopaediaArticles[article_textimage.id] = new ArticleDefinitionTextImage(article_textimage);

	
	// ALIEN RESEARCH
	ArticleDefinitionText article_text;
	article_text.section = UFOPAEDIA_ALIEN_RESEARCH;

	article_text.id = "STR_ALIEN_ORIGINS";
	article_text.title = "STR_ALIEN_ORIGINS";
	article_text.text = "STR_ALIEN_ORIGINS_UFOPEDIA";
	article_text.sort_key = sort_key++;
	
	_ufopaediaArticles[article_text.id] = new ArticleDefinitionText(article_text);

	
	// UFO COMPONENTS
	
	
	// UFOs
	ArticleDefinitionUfo article_ufo;
	article_ufo.section = UFOPAEDIA_UFOS;
	
	article_ufo.id = "STR_SMALL_SCOUT";
	article_ufo.title = "STR_SMALL_SCOUT";
	article_ufo.text = "STR_SMALL_SCOUT_UFOPEDIA";
	article_ufo.ufo = _ufos[article_ufo.id];
	article_ufo.sort_key = sort_key++;
	
	_ufopaediaArticles[article_ufo.id] = new ArticleDefinitionUfo(article_ufo);

	article_ufo.id = "STR_HARVESTER";
	article_ufo.title = "STR_HARVESTER";
	article_ufo.text = "STR_HARVESTER_UFOPEDIA";
	article_ufo.ufo = _ufos[article_ufo.id];
	article_ufo.sort_key = sort_key++;
	
	_ufopaediaArticles[article_ufo.id] = new ArticleDefinitionUfo(article_ufo);
	
	
	_costSoldier = 20000;
	_costEngineer = 25000;
	_costScientist = 30000;
	_timePersonnel = 72;

	std::string researchDataFile("research.dat");
	std::ifstream fin(CrossPlatform::getDataFile(researchDataFile).c_str());
	YAML::Parser parser(fin);

	YAML::Node doc;
	std::map<RuleResearchProject *, std::vector<std::string> > projectDependencys;
	std::map<RuleResearchProject *, std::vector<std::string> > unlocks;
	while(parser.GetNextDocument(doc)) 
	{
		for(YAML::Iterator it=doc.begin();it!=doc.end();++it)
		{
			std::string name;
			int cost;
			std::vector<std::string> deps;
			std::vector<std::string> unlock;
			bool needItem;
			(*it)["name"] >> name;
			(*it)["cost"] >> cost;
			(*it)["dependencys"] >> deps;
			(*it)["needItem"] >> needItem;
			(*it)["unlock"] >> unlock;
			RuleResearchProject * r = new RuleResearchProject(name, cost);
			r->setNeedItem(needItem);
			projectDependencys[r] = deps;
			unlocks[r] = unlock;
			_researchProjects[r->getName ()] = r;
		}
		for(std::map<RuleResearchProject *, std::vector<std::string> >::iterator iter = projectDependencys.begin ();
		    iter != projectDependencys.end ();
		    iter++)
		{
			for(std::vector<std::string>::iterator itDep = iter->second.begin ();
			    itDep != iter->second.end ();
			    itDep++)
			{
				std::map<std::string, RuleResearchProject *>::iterator it = _researchProjects.find(*itDep);
				if (it != _researchProjects.end ())
				{
					iter->first->addDependency(it->second);
				}
			}
		  }
		for(std::map<RuleResearchProject *, std::vector<std::string> >::iterator iter = unlocks.begin ();
		    iter != unlocks.end ();
		    iter++)
		{
			for(std::vector<std::string>::iterator itDep = iter->second.begin ();
			    itDep != iter->second.end ();
			    itDep++)
			{
				std::map<std::string, RuleResearchProject *>::iterator it = _researchProjects.find(*itDep);
				if (it != _researchProjects.end ())
				{
					iter->first->addUnlocked(it->second);
				}
			}
		  }
	}
}

/**
 *
 */
XcomRuleset::~XcomRuleset()
{
}

void XcomRuleset::initNames(YAML::Node &doc)
{
	for (YAML::Iterator i = doc["nameFiles"].begin(); i != doc["nameFiles"].end(); ++i)
	{
		std::string nameFile;
		(*i)["file"] >> nameFile;

		SoldierNamePool *pool = new SoldierNamePool();
		pool->load(nameFile);
		_names.push_back(pool);
	}
}

void XcomRuleset::initCountries(YAML::Node &doc)
{
	for (YAML::Iterator i = doc["countries"].begin(); i != doc["countries"].end(); ++i)
	{
		std::string countryStr;
		(*i)["country"] >> countryStr;
		int minFunding;
		(*i)["minFunding"] >> minFunding;
		int maxFunding;
		(*i)["maxFunding"] >> maxFunding;
		double longitude;
		(*i)["longitude"] >> longitude;
		double latitude;
		(*i)["latitude"] >> latitude;
		
		RuleCountry *country = new RuleCountry(countryStr);
		country->setMinFunding(minFunding);
		country->setMaxFunding(maxFunding);
		country->setLabelLongitude(longitude);
		country->setLabelLatitude(latitude);

		_countries.insert(std::pair<std::string, RuleCountry*>(countryStr, country));
	}
}

void XcomRuleset::initRegions(YAML::Node &doc)
{
	for (YAML::Iterator i = doc["regions"].begin(); i != doc["regions"].end(); ++i)
	{
		std::string regionStr;
		(*i)["region"] >> regionStr;
		int baseCost;
		(*i)["baseCost"] >> baseCost;
		
		RuleRegion* region = new RuleRegion(regionStr);
		region->setBaseCost(baseCost);

		for (YAML::Iterator cities = (*i)["cities"].begin(); cities != (*i)["cities"].end(); ++cities)
		{
			std::string cityStr;
			(*cities)["city"] >> cityStr;
			double longitude;
			(*cities)["longitude"] >> longitude;
			double latitude;
			(*cities)["latitude"] >> latitude;

			region->getCities()->push_back(new City(cityStr, longitude, latitude));
		}

		for (YAML::Iterator areas = (*i)["areas"].begin(); areas != (*i)["areas"].end(); ++areas)
		{
			double longitudeMin;
			(*areas)["longitudeMin"] >> longitudeMin;
			double longitudeMax;
			(*areas)["longitudeMax"] >> longitudeMax;
			double latitudeMin;
			(*areas)["latitudeMin"] >> latitudeMin;
			double latitudeMax;
			(*areas)["latitudeMax"] >> latitudeMax;

			region->addArea(longitudeMin, longitudeMax, latitudeMin, latitudeMax);
		}

		_regions.insert(std::pair<std::string, RuleRegion*>(regionStr, region));
	}
}

void XcomRuleset::initBaseFacilities(YAML::Node &doc)
{
	for (YAML::Iterator i = doc["baseFacilities"].begin(); i != doc["baseFacilities"].end(); ++i)
	{
		std::string facilityStr;
		(*i)["facility"] >> facilityStr;
		int spriteShape;
		(*i)["spriteShape"] >> spriteShape;
		int spriteFacility;
		(*i)["spriteFacility"] >> spriteFacility;
		int buildTime;
		(*i)["buildTime"] >> buildTime;
		int buildCost;
		(*i)["buildCost"] >> buildCost;
		int monthlyCost;
		(*i)["monthlyCost"] >> monthlyCost;

		RuleBaseFacility *facility = new RuleBaseFacility(facilityStr);
		facility->setSpriteShape(spriteShape);
		facility->setSpriteFacility(spriteFacility);
		facility->setBuildCost(buildCost);
		facility->setBuildTime(buildTime);
		facility->setMonthlyCost(monthlyCost);

		// optional values
		if(const YAML::Node *pName = (*i).FindValue("isLift")) 
		{
			bool isLift;
			(*i)["isLift"] >> isLift;
			facility->setLift(isLift);
		}

		if(const YAML::Node *pName = (*i).FindValue("personnel")) 
		{
			int personnel;
			(*i)["personnel"] >> personnel;
			facility->setPersonnel(personnel);
		}
		
		if(const YAML::Node *pName = (*i).FindValue("laboratorySpace")) 
		{
			int laboratorySpace;
			(*i)["laboratorySpace"] >> laboratorySpace;
			facility->setLaboratories(laboratorySpace);
		}

		if(const YAML::Node *pName = (*i).FindValue("workshopSpace")) 
		{
			int workshopSpace;
			(*i)["workshopSpace"] >> workshopSpace;
			facility->setWorkshops(workshopSpace);
		}

		if(const YAML::Node *pName = (*i).FindValue("storageSpace")) 
		{
			int storageSpace;
			(*i)["storageSpace"] >> storageSpace;
			facility->setStorage(storageSpace);
		}

		if(const YAML::Node *pName = (*i).FindValue("aliensSpace")) 
		{
			int aliensSpace;
			(*i)["aliensSpace"] >> aliensSpace;
			facility->setAliens(aliensSpace);
		}

		if(const YAML::Node *pName = (*i).FindValue("size")) 
		{
			int size;
			(*i)["size"] >> size;
			facility->setSize(size);
		}

		if(const YAML::Node *pName = (*i).FindValue("craftsSpace")) 
		{
			int craftsSpace;
			(*i)["craftsSpace"] >> craftsSpace;
			facility->setCrafts(craftsSpace);
		}

		if(const YAML::Node *pName = (*i).FindValue("radarRange")) 
		{
			int radarRange;
			(*i)["radarRange"] >> radarRange;
			facility->setRadarRange(radarRange);
		}

		if(const YAML::Node *pName = (*i).FindValue("radarChance")) 
		{
			int radarChance;
			(*i)["radarChance"] >> radarChance;
			facility->setRadarChance(radarChance);
		}

		if(const YAML::Node *pName = (*i).FindValue("defenceValue")) 
		{
			int defenceValue;
			(*i)["defenceValue"] >> defenceValue;
			facility->setDefenceValue(defenceValue);
		}

		if(const YAML::Node *pName = (*i).FindValue("hitRatio")) 
		{
			int hitRatio;
			(*i)["hitRatio"] >> hitRatio;
			facility->setHitRatio(hitRatio);
		}

		_facilities.insert(std::pair<std::string, RuleBaseFacility*>(facilityStr, facility));
	}
}

void XcomRuleset::initMapDataFiles(YAML::Node &doc)
{
	for (YAML::Iterator i = doc["mapDataFiles"].begin(); i != doc["mapDataFiles"].end(); ++i)
	{
		std::string file;
		(*i)["file"] >> file;
		int size;
		(*i)["size"] >> size;

		_mapDataSets.insert(std::pair<std::string, MapDataSet*>(file,new MapDataSet(file, size)));
	}
}

void XcomRuleset::initCrafts(YAML::Node &doc)
{
	for (YAML::Iterator i = doc["crafts"].begin(); i != doc["crafts"].end(); ++i)
	{
		std::string craftStr;
		(*i)["craft"] >> craftStr;
		int sprite;
		(*i)["sprite"] >> sprite;
		int maxSpeed;
		(*i)["maxSpeed"] >> maxSpeed;
		int acceleration;
		(*i)["acceleration"] >> acceleration;
		int maxFuel;
		(*i)["maxFuel"] >> maxFuel;
		int weapons;
		(*i)["weapons"] >> weapons;
		int maxDamage;
		(*i)["maxDamage"] >> maxDamage;
		int soldiers;
		(*i)["soldiers"] >> soldiers;
		int hwps;
		(*i)["HWPs"] >> hwps;
		int refuelRate;
		(*i)["refuelRate"] >> refuelRate;
		int score;
		(*i)["score"] >> score;

		RuleCraft* craft = new RuleCraft(craftStr);
		craft->setSprite(sprite);
		craft->setMaxSpeed(maxSpeed);
		craft->setAcceleration(acceleration);
		craft->setMaxFuel(maxFuel);
		craft->setWeapons(weapons);
		craft->setMaxDamage(maxDamage);
		craft->setSoldiers(soldiers);

		if((*i).FindValue("cost")) 
		{
			int cost;
			(*i)["cost"] >> cost;
			craft->setCost(cost);
		}

		craft->setHWPs(hwps);
		craft->setRefuelRate(refuelRate);

		if((*i).FindValue("transferTime")) 
		{
			int transferTime;
			(*i)["transferTime"] >> transferTime;
			craft->setTransferTime(transferTime);
		}

		craft->setScore(score);

		if((*i).FindValue("terrains")) 
		{
			for (YAML::Iterator terrains = (*i)["terrains"].begin(); terrains != (*i)["terrains"].end(); ++terrains)
			{
				RuleTerrain *ruleTerrain = getTerrainFromYAML(terrains);
				craft->setBattlescapeTerrainData(ruleTerrain);
			}
		}

		_crafts.insert(std::pair<std::string, RuleCraft*>(craftStr, craft));
	}
}

void XcomRuleset::initCraftWeapons(YAML::Node &doc)
{
	for (YAML::Iterator i = doc["craftWeapons"].begin(); i != doc["craftWeapons"].end(); ++i)
	{
		std::string weaponStr;
		(*i)["weapon"] >> weaponStr;
		int sprite;
		(*i)["sprite"] >> sprite;
		int sound;
		(*i)["sound"] >> sound;
		int damage;
		(*i)["damage"] >> damage;
		int range;
		(*i)["range"] >> range;
		int accuracy;
		(*i)["accuracy"] >> accuracy;
		int cautiousReload;
		(*i)["cautiousReload"] >> cautiousReload;
		int standardReload;
		(*i)["standardReload"] >> standardReload;
		int aggressiveReload;
		(*i)["aggressiveReload"] >> aggressiveReload;
		int ammoMax;
		(*i)["ammoMax"] >> ammoMax;
		std::string launcherItem;
		(*i)["launcherItem"] >> launcherItem;
		

		RuleCraftWeapon *weapon = new RuleCraftWeapon(weaponStr);
		weapon->setSprite(sprite);
		weapon->setSound(sound);
		weapon->setDamage(damage);
		weapon->setRange(range);
		weapon->setAccuracy(accuracy);
		weapon->setCautiousReload(cautiousReload);
		weapon->setStandardReload(standardReload);
		weapon->setAggressiveReload(aggressiveReload);
		weapon->setAmmoMax(ammoMax);
		weapon->setLauncherItem(launcherItem);

		if((*i).FindValue("clipItem")) 
		{
			std::string clipItem;
			(*i)["clipItem"] >> clipItem;
			weapon->setClipItem(clipItem);
		}

		if((*i).FindValue("rearmRate")) 
		{
			int rearmRate;
			(*i)["rearmRate"] >> rearmRate;
			weapon->setRearmRate(rearmRate);
		}

		_craftWeapons.insert(std::pair<std::string, RuleCraftWeapon*>(weaponStr, weapon));
	}
}

void XcomRuleset::initItems(YAML::Node &doc)
{
	for (YAML::Iterator i = doc["items"].begin(); i != doc["items"].end(); ++i)
	{
		std::string name;
		(*i)["name"] >> name;

		RuleItem *item = new RuleItem(name);

		if((*i).FindValue("size")) 
		{
			float size;
			(*i)["size"] >> size;
			item->setSize(size);
		}

		if((*i).FindValue("cost")) 
		{
			int cost;
			(*i)["cost"] >> cost;
			item->setCost(cost);
		}

		if((*i).FindValue("transferTime")) 
		{
			int transferTime;
			(*i)["transferTime"] >> transferTime;
			item->setTransferTime(transferTime);
		}

		if((*i).FindValue("bigSprite")) 
		{
			int bigSprite;
			(*i)["bigSprite"] >> bigSprite;
			item->setBigSprite(bigSprite);
		}

		if((*i).FindValue("handSprite")) 
		{
			int handSprite;
			(*i)["handSprite"] >> handSprite;
			item->setHandSprite(handSprite);
		}

		if((*i).FindValue("bulletSprite")) 
		{
			int bulletSprite;
			(*i)["bulletSprite"] >> bulletSprite;
			item->setBulletSprite(bulletSprite);
		}

		if((*i).FindValue("fireSound")) 
		{
			int fireSound;
			(*i)["fireSound"] >> fireSound;
			item->setFireSound(fireSound);
		}

		if((*i).FindValue("accuracySnap")) 
		{
			int accuracySnap;
			(*i)["accuracySnap"] >> accuracySnap;
			item->setAccuracySnap(accuracySnap);
		}

		if((*i).FindValue("TUSnap")) 
		{
			int tuSnap;
			(*i)["TUSnap"] >> tuSnap;
			item->setTUSnap(tuSnap);
		}

		if((*i).FindValue("accuracyAuto")) 
		{
			int accuracyAuto;
			(*i)["accuracyAuto"] >> accuracyAuto;
			item->setAccuracyAuto(accuracyAuto);
		}

		if((*i).FindValue("TUAuto")) 
		{
			int tuAuto;
			(*i)["TUAuto"] >> tuAuto;
			item->setTUAuto(tuAuto);
		}

		if((*i).FindValue("accuracyAimed")) 
		{
			int accuracyAimed;
			(*i)["accuracyAimed"] >> accuracyAimed;
			item->setAccuracyAimed(accuracyAimed);
		}

		if((*i).FindValue("TUAimed")) 
		{
			int tuAimed;
			(*i)["TUAimed"] >> tuAimed;
			item->setTUAimed(tuAimed);
		}

		if((*i).FindValue("battleType")) 
		{
			int battleType;
			(*i)["battleType"] >> battleType;
			item->setBattleType((BattleType)battleType);
		}

		if((*i).FindValue("inventoryWidth")) 
		{
			int inventoryWidth;
			(*i)["inventoryWidth"] >> inventoryWidth;
			item->setInventoryWidth(inventoryWidth);
		}

		if((*i).FindValue("inventoryHeight")) 
		{
			int inventoryHeight;
			(*i)["inventoryHeight"] >> inventoryHeight;
			item->setInventoryHeight(inventoryHeight);
		}

		if((*i).FindValue("weight")) 
		{
			int weight;
			(*i)["weight"] >> weight;
			item->setWeight(weight);
		}

		if((*i).FindValue("power")) 
		{
			int power;
			(*i)["power"] >> power;
			item->setPower(power);
		}

		if((*i).FindValue("damageType")) 
		{
			int damageType;
			(*i)["damageType"] >> damageType;
			item->setDamageType((ItemDamageType)damageType);
		}

		if((*i).FindValue("hitAnimation")) 
		{
			int hitAnimation;
			(*i)["hitAnimation"] >> hitAnimation;
			item->setHitAnimation(hitAnimation);
		}

		if((*i).FindValue("hitSound")) 
		{
			int hitSound;
			(*i)["hitSound"] >> hitSound;
			item->setHitSound(hitSound);
		}

		if((*i).FindValue("clipSize")) 
		{
			int clipSize;
			(*i)["clipSize"] >> clipSize;
			item->setClipSize(clipSize);
		}

		if((*i).FindValue("twoHanded")) 
		{
			bool twoHanded;
			(*i)["twoHanded"] >> twoHanded;
			item->setTwoHanded(twoHanded);
		}

		if((*i).FindValue("floorSprite")) 
		{
			int floorSprite;
			(*i)["floorSprite"] >> floorSprite;
			item->setFloorSprite(floorSprite);
		}

		if((*i).FindValue("compatibleAmmoTypes")) 
		{
			for (YAML::Iterator ammoTypes = (*i)["compatibleAmmoTypes"].begin(); ammoTypes != (*i)["compatibleAmmoTypes"].end(); ++ammoTypes)
			{
				std::string ammo;
				(*ammoTypes)["ammo"] >> ammo;

				item->getCompatibleAmmo()->push_back(ammo);
			}
		}

		_items.insert(std::pair<std::string, RuleItem*>(name, item));
	}
}

void XcomRuleset::initUFOs(YAML::Node &doc)
{
	for (YAML::Iterator i = doc["UFOs"].begin(); i != doc["UFOs"].end(); ++i)
	{
		std::string name;
		(*i)["name"] >> name;
		std::string size;
		(*i)["size"] >> size;
		int sprite;
		(*i)["sprite"] >> sprite;
		int maxSpeed;
		(*i)["maxSpeed"] >> maxSpeed;
		int acceleration;
		(*i)["acceleration"] >> acceleration;
		int maxDamage;
		(*i)["maxDamage"] >> maxDamage;
		int score;
		(*i)["score"] >> score;

		RuleUfo *ufo = new RuleUfo(name);
		ufo->setSize(size);
		ufo->setSprite(sprite);
		ufo->setMaxSpeed(maxSpeed);
		ufo->setAcceleration(acceleration);
		ufo->setMaxDamage(maxDamage);
		ufo->setScore(score);

		if((*i).FindValue("weaponPower")) 
		{
			int weaponPower;
			(*i)["weaponPower"] >> weaponPower;
			ufo->setWeaponPower(weaponPower);
		}

		if((*i).FindValue("weaponRange")) 
		{
			int weaponRange;
			(*i)["weaponRange"] >> weaponRange;
			ufo->setWeaponRange(weaponRange);
		}
		
		if((*i).FindValue("terrains")) 
		{
			for (YAML::Iterator terrains = (*i)["terrains"].begin(); terrains != (*i)["terrains"].end(); ++terrains)
			{
				RuleTerrain *ruleTerrain = getTerrainFromYAML(terrains);
				ufo->setBattlescapeTerrainData(ruleTerrain);
			}
		}

		_ufos.insert(std::pair<std::string, RuleUfo*>(name, ufo));
	}
}

void XcomRuleset::initInventory(YAML::Node &doc)
{
	for (YAML::Iterator i = doc["inventorySlots"].begin(); i != doc["inventorySlots"].end(); ++i)
	{
		std::string name;
		(*i)["name"] >> name;
		int x;
		(*i)["x"] >> x;
		int y;
		(*i)["y"] >> y;

		RuleInventory *inventorySlot = new RuleInventory("STR_GROUND");
		inventorySlot->setX(x);
		inventorySlot->setY(y);

		if((*i).FindValue("type")) 
		{
			int type;
			(*i)["type"] >> type;
			inventorySlot->setType((InventoryType)type);
		}

		if((*i).FindValue("slots")) 
		{
			for (YAML::Iterator slots = (*i)["slots"].begin(); slots != (*i)["slots"].end(); ++slots)
			{
				int x;
				(*slots)["x"] >> x;
				int y;
				(*slots)["y"] >> y;
				inventorySlot->addSlot(x, y);
			}
		}

		for (YAML::Iterator costs = (*i)["costs"].begin(); costs != (*i)["costs"].end(); ++costs)
		{
			std::string slot;
			(*costs)["slot"] >> slot;
			int cost;
			(*costs)["cost"] >> cost;
			inventorySlot->addCost(slot, cost);
		}

		_invs.insert(std::pair<std::string, RuleInventory*>(name, inventorySlot));
	}
}

void XcomRuleset::initTerrains(YAML::Node &doc)
{
	for (YAML::Iterator i = doc["terrains"].begin(); i != doc["terrains"].end(); ++i)
	{
		RuleTerrain *terrain = getTerrainFromYAML(i);

		_terrains.insert(std::pair<std::string, RuleTerrain*>(terrain->getName(),terrain));
	}
}

void XcomRuleset::initArmors(YAML::Node &doc)
{
	for (YAML::Iterator i = doc["armors"].begin(); i != doc["armors"].end(); ++i)
	{
		std::string type;
		(*i)["type"] >> type;
		std::string spriteSheet;
		(*i)["spriteSheet"] >> spriteSheet;
		int front;
		(*i)["front"] >> front;
		int side;
		(*i)["side"] >> side;
		int rear;
		(*i)["rear"] >> rear;
		int under;
		(*i)["under"] >> under;

		RuleArmor *armor = new RuleArmor(type, spriteSheet);
		armor->setArmor(front, side, rear, under);

		if((*i).FindValue("corpseItem")) 
		{
			std::string corpseItem;
			(*i)["corpseItem"] >> corpseItem;
			armor->setCorpseItem(corpseItem);
		}

		_armors.insert(std::pair<std::string, RuleArmor*>(type, armor));
	}
}

void XcomRuleset::initSoldiers(YAML::Node &doc)
{
	for (YAML::Iterator i = doc["soldiers"].begin(); i != doc["soldiers"].end(); ++i)
	{
		std::string name;
		(*i)["name"] >> name;
		std::string armor;
		(*i)["armor"] >> armor;
		int standHeight;
		(*i)["standHeight"] >> standHeight;
		int kneelHeight;
		(*i)["kneelHeight"] >> kneelHeight;
		int loftemps;
		(*i)["loftemps"] >> loftemps;

		RuleSoldier *xcom = new RuleSoldier(name);
		xcom->setArmor(armor);
		xcom->setVoxelParameters(standHeight, kneelHeight, loftemps);

		UnitStats stats [] = {UnitStats(), UnitStats()};
		int counter = 0;
		for (YAML::Iterator statsIterator = (*i)["stats"].begin(); statsIterator != (*i)["stats"].end(); ++statsIterator)
		{
			stats[counter] = getStatsFromYAML(statsIterator);

			counter++;
			if(counter >= 2)
				break;
		}

		xcom->setStats(stats[0], stats[1]);

		_soldiers.insert(std::pair<std::string, RuleSoldier*>(name, xcom));
	}
}

void XcomRuleset::initAliens(YAML::Node &doc)
{
	for (YAML::Iterator i = doc["aliens"].begin(); i != doc["aliens"].end(); ++i)
	{
		std::string name;
		(*i)["name"] >> name;
		std::string race;
		(*i)["race"] >> race;
		std::string rank;
		(*i)["rank"] >> rank;
		std::string armor;
		(*i)["armor"] >> armor;
		int standHeight;
		(*i)["standHeight"] >> standHeight;
		int kneelHeight;
		(*i)["kneelHeight"] >> kneelHeight;
		int loftemps;
		(*i)["loftemps"] >> loftemps;
		int value;
		(*i)["value"] >> value;

		RuleAlien *alien = new RuleAlien(name, race, rank);
		alien->setArmor(armor);
		alien->setVoxelParameters(standHeight, kneelHeight, loftemps);
		alien->setValue(value);

		UnitStats s1;
		for (YAML::Iterator statsIterator = (*i)["stats"].begin(); statsIterator != (*i)["stats"].end(); ++statsIterator)
		{
			s1 = getStatsFromYAML(statsIterator);
			break;
		}
		alien->setStats(s1);
		
		_aliens.insert(std::pair<std::string, RuleAlien*>(name, alien));
	}
}

UnitStats XcomRuleset::getStatsFromYAML(YAML::Iterator &statsIterator)
{
	UnitStats stats;
	(*statsIterator)["tu"] >> stats.tu;
	(*statsIterator)["stamina"] >> stats.stamina;
	(*statsIterator)["health"] >> stats.health;
	(*statsIterator)["bravery"] >> stats.bravery;
	(*statsIterator)["reactions"] >> stats.reactions;
	(*statsIterator)["firing"] >> stats.firing;
	(*statsIterator)["throwing"] >> stats.throwing;
	(*statsIterator)["strength"] >> stats.strength;
	(*statsIterator)["psiStrength"] >> stats.psiStrength;
	(*statsIterator)["psiSkill"] >> stats.psiSkill;
	(*statsIterator)["melee"] >> stats.melee;

	return stats;
}

RuleTerrain* XcomRuleset::getTerrainFromYAML(YAML::Iterator &terrains)
{
	std::string terrainStr;
	(*terrains)["terrain"] >> terrainStr;

	RuleTerrain *ruleTerrain = new RuleTerrain(terrainStr);
	
	for (YAML::Iterator mapDataSets = (*terrains)["mapDataSets"].begin(); mapDataSets != (*terrains)["mapDataSets"].end(); ++mapDataSets)
	{
		std::string name;
		(*mapDataSets)["name"] >> name;

		ruleTerrain->getMapDataSets()->push_back(getMapDataSet(name));
	}

	for (YAML::Iterator mapBlocks = (*terrains)["mapBlocks"].begin(); mapBlocks != (*terrains)["mapBlocks"].end(); ++mapBlocks)
	{
		std::string name;
		(*mapBlocks)["name"] >> name;
		int width;
		(*mapBlocks)["width"] >> width;
		int length;
		(*mapBlocks)["length"] >> length;
		bool landingZone;
		(*mapBlocks)["landingZone"] >> landingZone;

		ruleTerrain->getMapBlocks()->push_back(new MapBlock(ruleTerrain,name,width,length,landingZone));
	}

	return ruleTerrain;
}


/**
 * Generates a brand new saved game with the default countries
 * and a base with all the starting equipment.
 * @param diff Difficulty for the save.
 * @return New saved game.
 */
SavedGame *XcomRuleset::newSave(GameDifficulty diff) const
{
	SavedGame *save = new SavedGame(diff);

	// Add countries
	save->getCountries()->push_back(new Country(getCountry("STR_USA")));
	save->getCountries()->push_back(new Country(getCountry("STR_RUSSIA")));
	save->getCountries()->push_back(new Country(getCountry("STR_UK")));
	save->getCountries()->push_back(new Country(getCountry("STR_FRANCE")));
	save->getCountries()->push_back(new Country(getCountry("STR_GERMANY")));
	save->getCountries()->push_back(new Country(getCountry("STR_ITALY")));
	save->getCountries()->push_back(new Country(getCountry("STR_SPAIN")));
	save->getCountries()->push_back(new Country(getCountry("STR_CHINA")));
	save->getCountries()->push_back(new Country(getCountry("STR_JAPAN")));
	save->getCountries()->push_back(new Country(getCountry("STR_INDIA")));
	save->getCountries()->push_back(new Country(getCountry("STR_BRAZIL")));
	save->getCountries()->push_back(new Country(getCountry("STR_AUSTRALIA")));
	save->getCountries()->push_back(new Country(getCountry("STR_NIGERIA")));
	save->getCountries()->push_back(new Country(getCountry("STR_SOUTH_AFRICA")));
	save->getCountries()->push_back(new Country(getCountry("STR_EGYPT")));
	save->getCountries()->push_back(new Country(getCountry("STR_CANADA")));
	save->setFunds(save->getCountryFunding());

	// Add regions
	save->getRegions()->push_back(new Region(getRegion("STR_NORTH_AMERICA")));
	save->getRegions()->push_back(new Region(getRegion("STR_ARCTIC")));
	save->getRegions()->push_back(new Region(getRegion("STR_ANTARCTICA")));
	save->getRegions()->push_back(new Region(getRegion("STR_SOUTH_AMERICA")));
	save->getRegions()->push_back(new Region(getRegion("STR_EUROPE")));
	save->getRegions()->push_back(new Region(getRegion("STR_NORTH_AFRICA")));
	save->getRegions()->push_back(new Region(getRegion("STR_SOUTHERN_AFRICA")));
	save->getRegions()->push_back(new Region(getRegion("STR_CENTRAL_ASIA")));
	save->getRegions()->push_back(new Region(getRegion("STR_SOUTH_EAST_ASIA")));
	save->getRegions()->push_back(new Region(getRegion("STR_SIBERIA")));
	save->getRegions()->push_back(new Region(getRegion("STR_AUSTRALASIA")));
	save->getRegions()->push_back(new Region(getRegion("STR_PACIFIC")));
	save->getRegions()->push_back(new Region(getRegion("STR_NORTH_ATLANTIC")));
	save->getRegions()->push_back(new Region(getRegion("STR_SOUTH_ATLANTIC")));
	save->getRegions()->push_back(new Region(getRegion("STR_INDIAN_OCEAN")));
	
	// Set up craft IDs
	save->getCraftIds()->insert(std::pair<std::string, int>("STR_SKYRANGER", 1));
	save->getCraftIds()->insert(std::pair<std::string, int>("STR_LIGHTNING", 1));
	save->getCraftIds()->insert(std::pair<std::string, int>("STR_AVENGER", 1));
	save->getCraftIds()->insert(std::pair<std::string, int>("STR_INTERCEPTOR", 1));
	save->getCraftIds()->insert(std::pair<std::string, int>("STR_FIRESTORM", 1));

	// Set up initial base
	Base *base = new Base(this);
	base->setEngineers(10);
	base->setScientists(10);

	// Add facilities
	base->getFacilities()->push_back(new BaseFacility(getBaseFacility("STR_ACCESS_LIFT"), base, 2, 2));
	base->getFacilities()->push_back(new BaseFacility(getBaseFacility("STR_HANGAR"), base, 2, 0));
	base->getFacilities()->push_back(new BaseFacility(getBaseFacility("STR_HANGAR"), base, 0, 4));
	base->getFacilities()->push_back(new BaseFacility(getBaseFacility("STR_HANGAR"), base, 4, 4));
	base->getFacilities()->push_back(new BaseFacility(getBaseFacility("STR_LIVING_QUARTERS"), base, 3, 2));
	base->getFacilities()->push_back(new BaseFacility(getBaseFacility("STR_GENERAL_STORES"), base, 2, 3));
	base->getFacilities()->push_back(new BaseFacility(getBaseFacility("STR_LABORATORY"), base, 3, 3));
	base->getFacilities()->push_back(new BaseFacility(getBaseFacility("STR_WORKSHOP"), base, 4, 3));
	base->getFacilities()->push_back(new BaseFacility(getBaseFacility("STR_SMALL_RADAR_SYSTEM"), base, 1, 3));

	// Add items
	base->getItems()->addItem("STR_STINGRAY_LAUNCHER", 1);
	base->getItems()->addItem("STR_AVALANCHE_LAUNCHER", 1);
	base->getItems()->addItem("STR_CANNON", 2);
	base->getItems()->addItem("STR_STINGRAY_MISSILES", 25);
	base->getItems()->addItem("STR_AVALANCHE_MISSILES", 10);
	base->getItems()->addItem("STR_CANNON_ROUNDS_X50", 1);
	base->getItems()->addItem("STR_PISTOL", 2);
	base->getItems()->addItem("STR_PISTOL_CLIP", 8);
	base->getItems()->addItem("STR_RIFLE", 2);
	base->getItems()->addItem("STR_RIFLE_CLIP", 8);
	base->getItems()->addItem("STR_HEAVY_CANNON", 1);
	base->getItems()->addItem("STR_HC_AP_AMMO", 6);
	base->getItems()->addItem("STR_AUTO_CANNON", 1);
	base->getItems()->addItem("STR_AC_AP_AMMO", 6);
	base->getItems()->addItem("STR_ROCKET_LAUNCHER", 1);
	base->getItems()->addItem("STR_SMALL_ROCKET", 4);
	base->getItems()->addItem("STR_GRENADE", 5);
	base->getItems()->addItem("STR_SMOKE_GRENADE", 5);

	// Add crafts
	Craft *skyranger = new Craft(getCraft("STR_SKYRANGER"), base, save->getCraftIds());
	skyranger->setFuel(skyranger->getRules()->getMaxFuel());
	skyranger->getItems()->addItem("STR_PISTOL", 3);
	skyranger->getItems()->addItem("STR_PISTOL_CLIP", 5);
	skyranger->getItems()->addItem("STR_RIFLE", 6);
	skyranger->getItems()->addItem("STR_RIFLE_CLIP", 12);
	skyranger->getItems()->addItem("STR_HEAVY_CANNON", 1);
	skyranger->getItems()->addItem("STR_HC_AP_AMMO", 2);
	skyranger->getItems()->addItem("STR_HC_HE_AMMO", 2);
	skyranger->getItems()->addItem("STR_GRENADE", 8);
	base->getCrafts()->push_back(skyranger);

	for (int i = 0; i < 2; ++i)
	{
		Craft *interceptor = new Craft(getCraft("STR_INTERCEPTOR"), base, save->getCraftIds());
		interceptor->setFuel(interceptor->getRules()->getMaxFuel());
		interceptor->getWeapons()->at(0) = new CraftWeapon(getCraftWeapon("STR_STINGRAY"), getCraftWeapon("STR_STINGRAY")->getAmmoMax());
		interceptor->getWeapons()->at(1) = new CraftWeapon(getCraftWeapon("STR_CANNON_UC"), getCraftWeapon("STR_CANNON_UC")->getAmmoMax());
		base->getCrafts()->push_back(interceptor);
	}

	// Generate soldiers
	for (int i = 0; i < 8; ++i)
	{
		Soldier *soldier = new Soldier(getSoldier("XCOM"), getArmor("STR_NONE_UC"), &_names, save->getSoldierId());
		soldier->setCraft(skyranger);
		base->getSoldiers()->push_back(soldier);
	}

	save->getBases()->push_back(base);

	// init savedgame articles
	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_SKYRANGER"));
	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_LIGHTNING"));
	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_AVENGER"));
	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_INTERCEPTOR"));
	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_FIRESTORM"));

	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_STINGRAY"));
	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_AVALANCHE"));
	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_CANNON_UC"));
	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_FUSION_BALL_UC"));
	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_LASER_CANNON_UC"));
	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_PLASMA_BEAM_UC"));

	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_PISTOL"));
	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_PISTOL_CLIP"));
	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_HEAVY_CANNON"));	
	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_HC_AP_AMMO"));
	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_HC_HE_AMMO"));
	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_HC_I_AMMO"));
	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_GRENADE"));
	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_SMOKE_GRENADE"));

	
	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_SECTOID"));
//	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_SNAKEMAN"));
	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_ALIEN_ORIGINS"));
	
	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_ACCESS_LIFT"));
	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_HANGAR"));

	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_SMALL_SCOUT"));
	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_HARVESTER"));

	return save;
}

}
