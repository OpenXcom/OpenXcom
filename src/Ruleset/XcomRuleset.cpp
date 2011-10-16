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
#include <fstream>
#include <algorithm>
#include "RuleManufactureInfo.h"

namespace OpenXcom
{
/**
 * Initializes the X-Com ruleset with all the rules
 * mimicking the original game.
 */
XcomRuleset::XcomRuleset() : Ruleset()
{
	// Add soldier names
	SoldierNamePool *american = new SoldierNamePool();
	american->load("American");
	_names.push_back(american);

	SoldierNamePool *british = new SoldierNamePool();
	british->load("British");
	_names.push_back(british);

	SoldierNamePool *french = new SoldierNamePool();
	french->load("French");
	_names.push_back(french);

	SoldierNamePool *german = new SoldierNamePool();
	german->load("German");
	_names.push_back(german);

	SoldierNamePool *japanese = new SoldierNamePool();
	japanese->load("Japanese");
	_names.push_back(japanese);

	SoldierNamePool *russian = new SoldierNamePool();
	russian->load("Russian");
	_names.push_back(russian);

	// Add countries
	RuleCountry *usa = new RuleCountry("STR_USA");
	usa->setMinFunding(600);
	usa->setMaxFunding(1200);
	usa->setLabelLongitude(4.53786);
	usa->setLabelLatitude(-0.698132);

	RuleCountry *russia = new RuleCountry("STR_RUSSIA");
	russia->setMinFunding(230);
	russia->setMaxFunding(460);
	russia->setLabelLongitude(1.0472);
	russia->setLabelLatitude(-1.0472);

	RuleCountry *uk = new RuleCountry("STR_UK");
	uk->setMinFunding(240);
	uk->setMaxFunding(480);
	uk->setLabelLongitude(6.24828);
	uk->setLabelLatitude(-0.935933);

	RuleCountry *france = new RuleCountry("STR_FRANCE");
	france->setMinFunding(320);
	france->setMaxFunding(640);
	france->setLabelLongitude(0.0436332);
	france->setLabelLatitude(-0.811578);

	RuleCountry *germany = new RuleCountry("STR_GERMANY");
	germany->setMinFunding(250);
	germany->setMaxFunding(500);
	germany->setLabelLongitude(0.200713);
	germany->setLabelLatitude(-0.872665);

	RuleCountry *italy = new RuleCountry("STR_ITALY");
	italy->setMinFunding(160);
	italy->setMaxFunding(320);
	italy->setLabelLongitude(0.218166);
	italy->setLabelLatitude(-0.765763);

	RuleCountry *spain = new RuleCountry("STR_SPAIN");
	spain->setMinFunding(140);
	spain->setMaxFunding(280);
	spain->setLabelLongitude(6.23955);
	spain->setLabelLatitude(-0.743947);

	RuleCountry *china = new RuleCountry("STR_CHINA");
	china->setMinFunding(245);
	china->setMaxFunding(490);
	china->setLabelLongitude(1.74533);
	china->setLabelLatitude(-0.610865);

	RuleCountry *japan = new RuleCountry("STR_JAPAN");
	japan->setMinFunding(400);
	japan->setMaxFunding(800);
	japan->setLabelLongitude(2.40855);
	japan->setLabelLatitude(-0.667588);

	RuleCountry *india = new RuleCountry("STR_INDIA");
	india->setMinFunding(150);
	india->setMaxFunding(300);
	india->setLabelLongitude(1.39626);
	india->setLabelLatitude(-0.418879);

	RuleCountry *brazil = new RuleCountry("STR_BRAZIL");
	brazil->setMinFunding(300);
	brazil->setMaxFunding(600);
	brazil->setLabelLongitude(5.32325);
	brazil->setLabelLatitude(0.0872665);

	RuleCountry *australia = new RuleCountry("STR_AUSTRALIA");
	australia->setMinFunding(280);
	australia->setMaxFunding(560);
	australia->setLabelLongitude(2.35619);
	australia->setLabelLatitude(0.436332);

	RuleCountry *nigeria = new RuleCountry("STR_NIGERIA");
	nigeria->setMinFunding(180);
	nigeria->setMaxFunding(360);
	nigeria->setLabelLongitude(0.1309);
	nigeria->setLabelLatitude(-0.174533);

	RuleCountry *africa = new RuleCountry("STR_SOUTH_AFRICA");
	africa->setMinFunding(230);
	africa->setMaxFunding(460);
	africa->setLabelLongitude(0.436332);
	africa->setLabelLatitude(0.523599);

	RuleCountry *egypt = new RuleCountry("STR_EGYPT");
	egypt->setMinFunding(230);
	egypt->setMaxFunding(460);
	egypt->setLabelLongitude(0.506145);
	egypt->setLabelLatitude(-0.453786);

	RuleCountry *canada = new RuleCountry("STR_CANADA");
	canada->setMinFunding(230);
	canada->setMaxFunding(460);
	canada->setLabelLongitude(4.53786);
	canada->setLabelLatitude(-0.959931);

	_countries.insert(std::pair<std::string, RuleCountry*>("STR_USA", usa));
	_countries.insert(std::pair<std::string, RuleCountry*>("STR_RUSSIA", russia));
	_countries.insert(std::pair<std::string, RuleCountry*>("STR_UK", uk));
	_countries.insert(std::pair<std::string, RuleCountry*>("STR_FRANCE", france));
	_countries.insert(std::pair<std::string, RuleCountry*>("STR_GERMANY", germany));
	_countries.insert(std::pair<std::string, RuleCountry*>("STR_ITALY", italy));
	_countries.insert(std::pair<std::string, RuleCountry*>("STR_SPAIN", spain));
	_countries.insert(std::pair<std::string, RuleCountry*>("STR_CHINA", china));
	_countries.insert(std::pair<std::string, RuleCountry*>("STR_JAPAN", japan));
	_countries.insert(std::pair<std::string, RuleCountry*>("STR_INDIA", india));
	_countries.insert(std::pair<std::string, RuleCountry*>("STR_BRAZIL", brazil));
	_countries.insert(std::pair<std::string, RuleCountry*>("STR_AUSTRALIA", australia));
	_countries.insert(std::pair<std::string, RuleCountry*>("STR_NIGERIA", nigeria));
	_countries.insert(std::pair<std::string, RuleCountry*>("STR_SOUTH_AFRICA", africa));
	_countries.insert(std::pair<std::string, RuleCountry*>("STR_EGYPT", egypt));
	_countries.insert(std::pair<std::string, RuleCountry*>("STR_CANADA", canada));

	// Add regions
	RuleRegion* namerica = new RuleRegion("STR_NORTH_AMERICA");
	namerica->setBaseCost(800000);
	namerica->addArea(3.40339, 5.32107, -1.22173, -0.962113);
	namerica->addArea(4.01426, 5.32107, -0.959931, -0.52578);
	namerica->addArea(4.18879, 5.23381, -0.523599, -0.176715);
	namerica->getCities()->push_back(new City("STR_NEW_YORK", 4.99382, -0.711222));
	namerica->getCities()->push_back(new City("STR_WASHINGTON", 4.9371, -0.676315));
	namerica->getCities()->push_back(new City("STR_LOS_ANGELES", 4.21933, -0.595594));
	namerica->getCities()->push_back(new City("STR_MONTREAL", 4.9611, -0.794125));
	namerica->getCities()->push_back(new City("STR_HAVANA", 4.84547, -0.392699));
	namerica->getCities()->push_back(new City("STR_MEXICO_CITY", 4.55313, -0.338158));
	namerica->getCities()->push_back(new City("STR_CHICAGO", 4.75384, -0.730857));
	namerica->getCities()->push_back(new City("STR_VANCOUVER", 4.13207, -0.861756));
	namerica->getCities()->push_back(new City("STR_DALLAS", 4.59676, -0.571595));
	
	RuleRegion* arctic = new RuleRegion("STR_ARCTIC");
	arctic->setBaseCost(950000);
	arctic->addArea(0, 6.281, -1.5708, -1.22391);
	
	RuleRegion* antarctica = new RuleRegion("STR_ANTARCTICA");
	antarctica->setBaseCost(900000);
	antarctica->addArea(0, 6.281, 1.0472, 1.5708);
	
	RuleRegion* samerica = new RuleRegion("STR_SOUTH_AMERICA");
	samerica->setBaseCost(600000);
	samerica->addArea(4.71239, 5.49561, -0.174533, -0.00218166);
	samerica->addArea(4.79966, 5.7574, 0, 0.259618);
	samerica->addArea(4.79966, 5.67014, 0.261799, 1.04502);
	samerica->getCities()->push_back(new City("STR_BRASILIA", 5.44761, 0.274889));
	samerica->getCities()->push_back(new City("STR_BOGOTA", 4.98946, -0.0785398));
	samerica->getCities()->push_back(new City("STR_BUENOS_AIRES", 5.27962, 0.602139));
	samerica->getCities()->push_back(new City("STR_SANTIAGO", 5.05055, 0.582504));
	samerica->getCities()->push_back(new City("STR_RIO_DE_JANEIRO", 5.53051, 0.399244));
	samerica->getCities()->push_back(new City("STR_LIMA", 4.93928, 0.20944));
	samerica->getCities()->push_back(new City("STR_CARACAS", 5.116, -0.18326));
	
	RuleRegion* europe = new RuleRegion("STR_EUROPE");
	europe->setBaseCost(1000000);
	europe->addArea(5.84685, 1.04502, -1.22173, -0.613047);
	europe->getCities()->push_back(new City("STR_LONDON", 6.281, -0.898845));
	europe->getCities()->push_back(new City("STR_PARIS", 0.0414516, -0.850848));
	europe->getCities()->push_back(new City("STR_BERLIN", 0.233438, -0.916298));
	europe->getCities()->push_back(new City("STR_MOSCOW", 0.65668, -0.973021));
	europe->getCities()->push_back(new City("STR_ROME", 0.218166, -0.730857));
	europe->getCities()->push_back(new City("STR_MADRID", 6.21774, -0.704677));
	europe->getCities()->push_back(new City("STR_BUDAPEST", 0.333794, -0.829031));
	
	RuleRegion* nafrica = new RuleRegion("STR_NORTH_AFRICA");
	nafrica->setBaseCost(650000);
	nafrica->addArea(5.84685, 0.69595, -0.610865, -0.263981);
	nafrica->addArea(5.84685, 0.957749, -0.261799, -0.00218166);
	nafrica->getCities()->push_back(new City("STR_LAGOS", 0.0545415, -0.113446));
	nafrica->getCities()->push_back(new City("STR_CAIRO", 0.545415, -0.523599));
	nafrica->getCities()->push_back(new City("STR_CASABLANCA", 6.1501, -0.584685));
	
	RuleRegion* safrica = new RuleRegion("STR_SOUTHERN_AFRICA");
	safrica->setBaseCost(550000);
	safrica->addArea(0.0872665, 0.957749, 0, 0.69595);
	safrica->getCities()->push_back(new City("STR_PRETORIA", 0.490874, 0.458149));
	safrica->getCities()->push_back(new City("STR_NAIROBI", 0.641409, 0.0218166));
	safrica->getCities()->push_back(new City("STR_CAPE_TOWN", 0.322886, 0.593412));
	safrica->getCities()->push_back(new City("STR_KINSHASA", 0.268344, 0.0763582));
	
	RuleRegion* casia = new RuleRegion("STR_CENTRAL_ASIA");
	casia->setBaseCost(500000);
	casia->addArea(0.698132, 1.21955, -0.610865, -0.263981);
	casia->addArea(1.0472, 1.56861, -0.872665, -0.613047);
	casia->addArea(1.22173, 1.56861, -0.610865, -0.0894481);
	casia->getCities()->push_back(new City("STR_ANKARA", 0.571595, -0.69595));
	casia->getCities()->push_back(new City("STR_DELHI", 1.34827, -0.4996));
	casia->getCities()->push_back(new City("STR_KARACHI", 1.16937, -0.434151));
	casia->getCities()->push_back(new City("STR_BAGHDAD", 0.776672, -0.580322));
	casia->getCities()->push_back(new City("STR_TEHRAN", 0.898845, -0.621774));
	casia->getCities()->push_back(new City("STR_BOMBAY", 1.27627, -0.329431));
	casia->getCities()->push_back(new City("STR_CALCUTTA", 1.54243, -0.394881));
	
	RuleRegion* seasia = new RuleRegion("STR_SOUTH_EAST_ASIA");
	seasia->setBaseCost(750000);
	seasia->addArea(1.5708, 1.83041, -0.872665, 0.172351);
	seasia->addArea(1.8326, 2.61581, -0.872665, -0.0894481);
	seasia->getCities()->push_back(new City("STR_TOKYO", 2.4391, -0.621774));
	seasia->getCities()->push_back(new City("STR_BEIJING", 2.03113, -0.69595));
	seasia->getCities()->push_back(new City("STR_BANGKOK", 1.75624, -0.237801));
	seasia->getCities()->push_back(new City("STR_MANILA", 2.11185, -0.255254));
	seasia->getCities()->push_back(new City("STR_SEOUL", 2.21657, -0.654498));
	seasia->getCities()->push_back(new City("STR_SINGAPORE", 1.81296, -0.0239983));
	seasia->getCities()->push_back(new City("STR_JAKARTA", 1.86314, 0.109083));
	seasia->getCities()->push_back(new City("STR_SHANGHAI", 2.12058, -0.545415));
	seasia->getCities()->push_back(new City("STR_HONG_KONG", 1.99186, -0.388336));
	
	RuleRegion* siberia = new RuleRegion("STR_SIBERIA");
	siberia->setBaseCost(800000);
	siberia->addArea(1.0472, 3.13941, -1.22173, -0.874846);
	siberia->getCities()->push_back(new City("STR_NOVOSIBIRSK", 1.44426, -0.959931));
	
	RuleRegion* australasia = new RuleRegion("STR_AUSTRALASIA");
	australasia->setBaseCost(750000);
	//australasia->addArea(1.8326, 3.13941, -0.0872665, 0.870483);
	australasia->addArea(1.8326, 3.13941, -0.0872665, 1.04502);
	australasia->getCities()->push_back(new City("STR_CANBERRA", 2.60272, 0.61741));
	australasia->getCities()->push_back(new City("STR_WELLINGTON", 3.05651, 0.719948));
	australasia->getCities()->push_back(new City("STR_MELBOURNE", 2.53073, 0.661043));
	australasia->getCities()->push_back(new City("STR_PERTH", 2.02022, 0.558505));
	
	RuleRegion* pacific = new RuleRegion("STR_PACIFIC");
	pacific->setBaseCost(600000);
	pacific->addArea(3.14159, 3.40121, -1.22173, -0.962113);
	pacific->addArea(3.14159, 4.01208, -0.959931, -0.52578);
	pacific->addArea(3.14159, 4.18661, -0.523599, -0.176715);
	pacific->addArea(3.14159, 4.71021, -0.174533, -0.00218166);
	pacific->addArea(3.14159, 4.79747, 0, 1.04502);
	pacific->addArea(2.61799, 3.13941, -0.872665, -0.0894481);
	
	RuleRegion* natlantic = new RuleRegion("STR_NORTH_ATLANTIC");
	natlantic->setBaseCost(500000);
	natlantic->addArea(5.32325, 5.84467, -1.22173, -0.52578);
	natlantic->addArea(5.23599, 5.84467, -0.523599, -0.176715);
	natlantic->addArea(5.49779, 5.84467, -0.174533, -0.00218166);
	
	RuleRegion* satlantic = new RuleRegion("STR_SOUTH_ATLANTIC");
	satlantic->setBaseCost(500000);
	satlantic->addArea(5.75959, 0.0850848, 0, 0.259618);
	satlantic->addArea(5.67232, 0.0850848, 0.261799, 1.04502);
	satlantic->addArea(0.0872665, 0.959931, 0.698132, 1.04502);
	
	RuleRegion* indian = new RuleRegion("STR_INDIAN_OCEAN");
	indian->setBaseCost(500000);
	indian->addArea(0.959931, 1.21955, -0.261799, 0.172351);
	indian->addArea(1.22173, 1.56861, -0.0872665, 0.172351);
	indian->addArea(0.959931, 1.83041, 0.174533, 1.04502);
	
	_regions.insert(std::pair<std::string, RuleRegion*>("STR_NORTH_AMERICA", namerica));
	_regions.insert(std::pair<std::string, RuleRegion*>("STR_ARCTIC", arctic));
	_regions.insert(std::pair<std::string, RuleRegion*>("STR_ANTARCTICA", antarctica));
	_regions.insert(std::pair<std::string, RuleRegion*>("STR_SOUTH_AMERICA", samerica));
	_regions.insert(std::pair<std::string, RuleRegion*>("STR_EUROPE", europe));
	_regions.insert(std::pair<std::string, RuleRegion*>("STR_NORTH_AFRICA", nafrica));
	_regions.insert(std::pair<std::string, RuleRegion*>("STR_SOUTHERN_AFRICA", safrica));
	_regions.insert(std::pair<std::string, RuleRegion*>("STR_CENTRAL_ASIA", casia));
	_regions.insert(std::pair<std::string, RuleRegion*>("STR_SOUTH_EAST_ASIA", seasia));
	_regions.insert(std::pair<std::string, RuleRegion*>("STR_SIBERIA", siberia));
	_regions.insert(std::pair<std::string, RuleRegion*>("STR_AUSTRALASIA", australasia));
	_regions.insert(std::pair<std::string, RuleRegion*>("STR_PACIFIC", pacific));
	_regions.insert(std::pair<std::string, RuleRegion*>("STR_NORTH_ATLANTIC", natlantic));
	_regions.insert(std::pair<std::string, RuleRegion*>("STR_SOUTH_ATLANTIC", satlantic));
	_regions.insert(std::pair<std::string, RuleRegion*>("STR_INDIAN_OCEAN", indian));

	// Add base facilities
	RuleBaseFacility *lift = new RuleBaseFacility("STR_ACCESS_LIFT");
	lift->setSpriteShape(2);
	lift->setSpriteFacility(17);
	lift->setBuildCost(300000);
	lift->setBuildTime(1);
	lift->setMonthlyCost(4000);
	lift->setLift(true);

	RuleBaseFacility *quarters = new RuleBaseFacility("STR_LIVING_QUARTERS");
	quarters->setSpriteShape(1);
	quarters->setSpriteFacility(18);
	quarters->setBuildCost(400000);
	quarters->setBuildTime(16);
	quarters->setMonthlyCost(10000);
	quarters->setPersonnel(50);

	RuleBaseFacility *lab = new RuleBaseFacility("STR_LABORATORY");
	lab->setSpriteShape(1);
	lab->setSpriteFacility(19);
	lab->setBuildCost(750000);
	lab->setBuildTime(26);
	lab->setMonthlyCost(30000);
	lab->setLaboratories(50);

	RuleBaseFacility *workshop = new RuleBaseFacility("STR_WORKSHOP");
	workshop->setSpriteShape(1);
	workshop->setSpriteFacility(20);
	workshop->setBuildCost(800000);
	workshop->setBuildTime(32);
	workshop->setMonthlyCost(35000);
	workshop->setWorkshops(50);

	RuleBaseFacility *stores = new RuleBaseFacility("STR_GENERAL_STORES");
	stores->setSpriteShape(1);
	stores->setSpriteFacility(24);
	stores->setBuildCost(150000);
	stores->setBuildTime(10);
	stores->setMonthlyCost(5000);
	stores->setStorage(50);

	RuleBaseFacility *aliens = new RuleBaseFacility("STR_ALIEN_CONTAINMENT");
	aliens->setSpriteShape(1);
	aliens->setSpriteFacility(25);
	aliens->setBuildCost(400000);
	aliens->setBuildTime(18);
	aliens->setMonthlyCost(15000);
	aliens->setAliens(10);

	RuleBaseFacility *hangar = new RuleBaseFacility("STR_HANGAR");
	hangar->setSpriteShape(9);
	hangar->setSpriteFacility(9);
	hangar->setSize(2);
	hangar->setBuildCost(200000);
	hangar->setBuildTime(25);
	hangar->setMonthlyCost(25000);
	hangar->setCrafts(1);

	RuleBaseFacility *sRadar = new RuleBaseFacility("STR_SMALL_RADAR_SYSTEM");
	sRadar->setSpriteShape(2);
	sRadar->setSpriteFacility(21);
	sRadar->setBuildCost(500000);
	sRadar->setBuildTime(12);
	sRadar->setMonthlyCost(10000);
	sRadar->setRadarRange(1500);
	sRadar->setRadarChance(10);

	RuleBaseFacility *lRadar = new RuleBaseFacility("STR_LARGE_RADAR_SYSTEM");
	lRadar->setSpriteShape(1);
	lRadar->setSpriteFacility(22);
	lRadar->setBuildCost(800000);
	lRadar->setBuildTime(25);
	lRadar->setMonthlyCost(15000);
	lRadar->setRadarRange(2250);
	lRadar->setRadarChance(20);

	RuleBaseFacility *missile = new RuleBaseFacility("STR_MISSILE_DEFENSES");
	missile->setSpriteShape(2);
	missile->setSpriteFacility(23);
	missile->setBuildCost(200000);
	missile->setBuildTime(16);
	missile->setMonthlyCost(5000);
	missile->setDefenceValue(500);
	missile->setHitRatio(50);

	_facilities.insert(std::pair<std::string, RuleBaseFacility*>("STR_ACCESS_LIFT", lift));
	_facilities.insert(std::pair<std::string, RuleBaseFacility*>("STR_LIVING_QUARTERS", quarters));
	_facilities.insert(std::pair<std::string, RuleBaseFacility*>("STR_LABORATORY", lab));
	_facilities.insert(std::pair<std::string, RuleBaseFacility*>("STR_WORKSHOP", workshop));
	_facilities.insert(std::pair<std::string, RuleBaseFacility*>("STR_GENERAL_STORES", stores));
	_facilities.insert(std::pair<std::string, RuleBaseFacility*>("STR_ALIEN_CONTAINMENT", aliens));
	_facilities.insert(std::pair<std::string, RuleBaseFacility*>("STR_HANGAR", hangar));
	_facilities.insert(std::pair<std::string, RuleBaseFacility*>("STR_SMALL_RADAR_SYSTEM", sRadar));
	_facilities.insert(std::pair<std::string, RuleBaseFacility*>("STR_LARGE_RADAR_SYSTEM", lRadar));
	_facilities.insert(std::pair<std::string, RuleBaseFacility*>("STR_MISSILE_DEFENSES", missile));

	// Add mapdatafiles
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("AVENGER",new MapDataSet("AVENGER",59)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("BARN",new MapDataSet("BARN",29)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("BLANKS",new MapDataSet("BLANKS",2)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("BRAIN",new MapDataSet("BRAIN",4)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("CULTIVAT",new MapDataSet("CULTIVAT",37)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("DESERT",new MapDataSet("DESERT",66)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("FOREST",new MapDataSet("FOREST",83)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("FRNITURE",new MapDataSet("FRNITURE",26)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("JUNGLE",new MapDataSet("JUNGLE",82)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("LIGHTNIN",new MapDataSet("LIGHTNIN",42)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("MARS",new MapDataSet("MARS",36)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("MOUNT",new MapDataSet("MOUNT",78)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("PLANE",new MapDataSet("PLANE",65)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("POLAR",new MapDataSet("POLAR",81)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("ROADS",new MapDataSet("ROADS",23)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("UFO1",new MapDataSet("UFO1",20)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("URBAN",new MapDataSet("URBAN",112)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("URBITS",new MapDataSet("URBITS",25)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("U_BASE",new MapDataSet("U_BASE",67)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("U_BITS",new MapDataSet("U_BITS",8)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("U_DISEC2",new MapDataSet("U_DISEC2",17)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("U_EXT02",new MapDataSet("U_EXT02",34)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("U_OPER2",new MapDataSet("U_OPER2",15)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("U_PODS",new MapDataSet("U_PODS",11)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("U_WALL02",new MapDataSet("U_WALL02",47)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("XBASE1",new MapDataSet("XBASE1",97)));
	_mapDataSets.insert(std::pair<std::string, MapDataSet*>("XBASE2",new MapDataSet("XBASE2",62)));

	// Add crafts
	RuleCraft* skyranger = new RuleCraft("STR_SKYRANGER");
	skyranger->setSprite(0);
	skyranger->setMaxSpeed(760);
	skyranger->setAcceleration(2);
	skyranger->setMaxFuel(1500);
	skyranger->setWeapons(0);
	skyranger->setMaxDamage(150);
	skyranger->setSoldiers(14);
	skyranger->setCost(500000);
	skyranger->setHWPs(3);
	skyranger->setRefuelRate(50);
	skyranger->setTransferTime(72);
	skyranger->setScore(200);

	RuleTerrain *ruleTerrain = new RuleTerrain("PLANE");
	skyranger->setBattlescapeTerrainData(ruleTerrain);
	ruleTerrain->getMapDataSets()->push_back(getMapDataSet("BLANKS"));
	ruleTerrain->getMapDataSets()->push_back(getMapDataSet("PLANE"));
	ruleTerrain->getMapBlocks()->push_back(new MapBlock(ruleTerrain,"PLANE",10,20,true));

	RuleCraft* lightning = new RuleCraft("STR_LIGHTNING");
	lightning->setSprite(1);
	lightning->setMaxSpeed(3100);
	lightning->setAcceleration(9);
	lightning->setMaxFuel(30);
	lightning->setWeapons(1);
	lightning->setMaxDamage(800);
	lightning->setSoldiers(12);
	lightning->setHWPs(0);
	lightning->setRefuelRate(5);
	lightning->setScore(300);

	RuleCraft* avenger = new RuleCraft("STR_AVENGER");
	avenger->setSprite(2);
	avenger->setMaxSpeed(5400);
	avenger->setAcceleration(10);
	avenger->setMaxFuel(60);
	avenger->setWeapons(2);
	avenger->setMaxDamage(1200);
	avenger->setSoldiers(26);
	avenger->setHWPs(4);
	avenger->setRefuelRate(5);
	avenger->setScore(400);

	RuleCraft* interceptor = new RuleCraft("STR_INTERCEPTOR");
	interceptor->setSprite(3);
	interceptor->setMaxSpeed(2100);
	interceptor->setAcceleration(3);
	interceptor->setMaxFuel(1000);
	interceptor->setWeapons(2);
	interceptor->setMaxDamage(100);
	interceptor->setSoldiers(0);
	interceptor->setCost(600000);
	interceptor->setHWPs(0);
	interceptor->setRefuelRate(50);
	interceptor->setTransferTime(96);
	interceptor->setScore(250);

	RuleCraft* firestorm = new RuleCraft("STR_FIRESTORM");
	firestorm->setSprite(4);
	firestorm->setMaxSpeed(4200);
	firestorm->setAcceleration(9);
	firestorm->setMaxFuel(20);
	firestorm->setWeapons(2);
	firestorm->setMaxDamage(50);
	firestorm->setSoldiers(0);
	firestorm->setHWPs(0);
	firestorm->setRefuelRate(5);
	firestorm->setScore(250);

	_crafts.insert(std::pair<std::string, RuleCraft*>("STR_SKYRANGER", skyranger));
	_crafts.insert(std::pair<std::string, RuleCraft*>("STR_LIGHTNING", lightning));
	_crafts.insert(std::pair<std::string, RuleCraft*>("STR_AVENGER", avenger));
	_crafts.insert(std::pair<std::string, RuleCraft*>("STR_INTERCEPTOR", interceptor));
	_crafts.insert(std::pair<std::string, RuleCraft*>("STR_FIRESTORM", firestorm));

	// Add craft weapons
	RuleCraftWeapon *stingray = new RuleCraftWeapon("STR_STINGRAY");
	stingray->setSprite(0);
	stingray->setSound(8);
	stingray->setDamage(70);
	stingray->setRange(30);
	stingray->setAccuracy(70);
	stingray->setCautiousReload(32);
	stingray->setStandardReload(24);
	stingray->setAggressiveReload(16);
	stingray->setAmmoMax(6);
	stingray->setLauncherItem("STR_STINGRAY_LAUNCHER");
	stingray->setClipItem("STR_STINGRAY_MISSILES");

	RuleCraftWeapon *avalanche = new RuleCraftWeapon("STR_AVALANCHE");
	avalanche->setSprite(1);
	avalanche->setSound(8);
	avalanche->setDamage(100);
	avalanche->setRange(60);
	avalanche->setAccuracy(80);
	avalanche->setCautiousReload(48);
	avalanche->setStandardReload(36);
	avalanche->setAggressiveReload(24);
	avalanche->setAmmoMax(3);
	avalanche->setLauncherItem("STR_AVALANCHE_LAUNCHER");
	avalanche->setClipItem("STR_AVALANCHE_MISSILES");

	RuleCraftWeapon *cannon = new RuleCraftWeapon("STR_CANNON_UC");
	cannon->setSprite(2);
	cannon->setSound(4);
	cannon->setDamage(10);
	cannon->setRange(10);
	cannon->setAccuracy(25);
	cannon->setCautiousReload(2);
	cannon->setStandardReload(2);
	cannon->setAggressiveReload(2);
	cannon->setAmmoMax(200);
	cannon->setRearmRate(50);
	cannon->setLauncherItem("STR_CANNON");
	cannon->setClipItem("STR_CANNON_ROUNDS_X50");

	RuleCraftWeapon *laser = new RuleCraftWeapon("STR_LASER_CANNON_UC");
	laser->setSprite(4);
	laser->setSound(5);
	laser->setDamage(70);
	laser->setRange(21);
	laser->setAccuracy(35);
	laser->setCautiousReload(12);
	laser->setStandardReload(12);
	laser->setAggressiveReload(12);
	laser->setAmmoMax(99);
	laser->setLauncherItem("STR_LASER_CANNON");

	RuleCraftWeapon *plasma = new RuleCraftWeapon("STR_PLASMA_BEAM_UC");
	plasma->setSprite(5);
	plasma->setSound(9);
	plasma->setDamage(140);
	plasma->setRange(52);
	plasma->setAccuracy(50);
	plasma->setCautiousReload(12);
	plasma->setStandardReload(12);
	plasma->setAggressiveReload(12);
	plasma->setAmmoMax(100);
	plasma->setLauncherItem("STR_PLASMA_BEAM");

	RuleCraftWeapon *fusion = new RuleCraftWeapon("STR_FUSION_BALL_UC");
	fusion->setSprite(3);
	fusion->setSound(7);
	fusion->setDamage(230);
	fusion->setRange(65);
	fusion->setAccuracy(100);
	fusion->setCautiousReload(32);
	fusion->setStandardReload(24);
	fusion->setAggressiveReload(16);
	fusion->setAmmoMax(2);
	fusion->setLauncherItem("STR_FUSION_BALL_LAUNCHER");
	fusion->setClipItem("STR_FUSION_BALL");

	_craftWeapons.insert(std::pair<std::string, RuleCraftWeapon*>("STR_STINGRAY", stingray));
	_craftWeapons.insert(std::pair<std::string, RuleCraftWeapon*>("STR_AVALANCHE", avalanche));
	_craftWeapons.insert(std::pair<std::string, RuleCraftWeapon*>("STR_CANNON_UC", cannon));
	_craftWeapons.insert(std::pair<std::string, RuleCraftWeapon*>("STR_FUSION_BALL_UC", fusion));
	_craftWeapons.insert(std::pair<std::string, RuleCraftWeapon*>("STR_LASER_CANNON_UC", laser));
	_craftWeapons.insert(std::pair<std::string, RuleCraftWeapon*>("STR_PLASMA_BEAM_UC", plasma));

	// Add items
	RuleItem *slauncher = new RuleItem("STR_STINGRAY_LAUNCHER");
	slauncher->setSize(0.8f);
	slauncher->setCost(16000);
	slauncher->setTransferTime(48);

	RuleItem *alauncher = new RuleItem("STR_AVALANCHE_LAUNCHER");
	alauncher->setSize(1.0f);
	alauncher->setCost(17000);
	alauncher->setTransferTime(48);

	RuleItem *icannon = new RuleItem("STR_CANNON");
	icannon->setSize(1.5f);
	icannon->setCost(30000);
	icannon->setTransferTime(48);

	RuleItem *smissile = new RuleItem("STR_STINGRAY_MISSILES");
	smissile->setSize(0.4f);
	smissile->setCost(3000);
	smissile->setTransferTime(48);

	RuleItem *amissile = new RuleItem("STR_AVALANCHE_MISSILES");
	amissile->setSize(1.5f);
	amissile->setCost(9000);
	amissile->setTransferTime(48);

	RuleItem *crounds = new RuleItem("STR_CANNON_ROUNDS_X50");
	crounds->setSize(0.0f);
	crounds->setCost(1240);
	crounds->setTransferTime(96);

	RuleItem *pistol = new RuleItem("STR_PISTOL");
	pistol->setSize(0.1f);
	pistol->setCost(800);
	pistol->setBigSprite(3);
	pistol->setHandSprite(96);
	pistol->setBulletSprite(1);
	pistol->setFireSound(4);
	pistol->setAccuracySnap(60);
	pistol->setTUSnap(18);
	pistol->setTUAuto(18);
	pistol->setAccuracyAimed(78);
	pistol->setTUAimed(30);
	pistol->getCompatibleAmmo()->push_back("STR_PISTOL_CLIP");
	pistol->setBattleType(BT_FIREARM);
	pistol->setInventoryWidth(1);
	pistol->setInventoryHeight(2);
	pistol->setWeight(5);

	RuleItem *pclip = new RuleItem("STR_PISTOL_CLIP");
	pclip->setSize(0.1f);
	pclip->setCost(70);
	pclip->setBigSprite(4);
	pclip->setPower(26);
	pclip->setDamageType(DT_AP);
	pclip->setBattleType(BT_AMMO);
	pclip->setHitAnimation(26);
	pclip->setHitSound(22);
	pclip->setClipSize(12);
	pclip->setWeight(3);
	
	RuleItem *rifle = new RuleItem("STR_RIFLE");
	rifle->setSize(0.2f);
	rifle->setCost(3000);
	rifle->setBigSprite(1);
	rifle->setHandSprite(0);
	rifle->setTwoHanded(true);
	rifle->setBulletSprite(2);
	rifle->setFireSound(4);
	rifle->setAccuracyAuto(35);
	rifle->setTUAuto(35);
	rifle->setAccuracySnap(60);
	rifle->setTUSnap(25);
	rifle->setAccuracyAimed(110);
	rifle->setTUAimed(80);
	rifle->getCompatibleAmmo()->push_back("STR_RIFLE_CLIP");
	rifle->setBattleType(BT_FIREARM);
	rifle->setInventoryWidth(1);
	rifle->setInventoryHeight(3);
	rifle->setWeight(8);

	RuleItem *rclip = new RuleItem("STR_RIFLE_CLIP");
	rclip->setSize(0.1f);
	rclip->setCost(200);
	rclip->setBigSprite(2);
	rclip->setPower(30);
	rclip->setDamageType(DT_AP);
	rclip->setBattleType(BT_AMMO);
	rclip->setHitAnimation(26);
	rclip->setHitSound(22);
	rclip->setClipSize(20);
	rclip->setWeight(3);

	RuleItem *hcannon = new RuleItem("STR_HEAVY_CANNON");
	hcannon->setSize(0.3f);
	hcannon->setCost(6400);
	hcannon->setBigSprite(11);
	hcannon->setHandSprite(24);
	hcannon->setTwoHanded(true);
	hcannon->setBulletSprite(4);
	hcannon->setFireSound(2);
	hcannon->setAccuracySnap(60);
	hcannon->setTUSnap(33);
	hcannon->setTUAuto(33);
	hcannon->setAccuracyAimed(90);
	hcannon->setTUAimed(80);
	hcannon->getCompatibleAmmo()->push_back("STR_HC_AP_AMMO");
	hcannon->getCompatibleAmmo()->push_back("STR_HC_HE_AMMO");
	hcannon->getCompatibleAmmo()->push_back("STR_HC_I_AMMO");
	hcannon->setBattleType(BT_FIREARM);
	hcannon->setInventoryWidth(2);
	hcannon->setInventoryHeight(3);
	hcannon->setWeight(18);

	RuleItem *hcap = new RuleItem("STR_HC_AP_AMMO");
	hcap->setSize(0.1f);
	hcap->setCost(300);
	hcap->setBigSprite(12);
	hcap->setPower(56);
	hcap->setDamageType(DT_AP);
	hcap->setBattleType(BT_AMMO);
	hcap->setHitAnimation(26);
	hcap->setHitSound(13);
	hcap->setInventoryWidth(2);
	hcap->setInventoryHeight(1);
	hcap->setClipSize(6);
	hcap->setWeight(6);

	RuleItem *hche = new RuleItem("STR_HC_HE_AMMO");
	hche->setSize(0.1f);
	hche->setCost(500);
	hche->setBigSprite(13);
	hche->setPower(52);
	hche->setDamageType(DT_HE);
	hche->setBattleType(BT_AMMO);
	hche->setHitAnimation(0);
	hche->setHitSound(0);
	hche->setInventoryWidth(2);
	hche->setInventoryHeight(1);
	hche->setClipSize(6);
	hche->setWeight(6);

	RuleItem *hci = new RuleItem("STR_HC_I_AMMO");
	hci->setSize(0.1f);
	hci->setCost(400);
	hci->setBigSprite(14);
	hci->setPower(60);
	hci->setDamageType(DT_IN);
	hci->setBattleType(BT_AMMO);
	hci->setHitAnimation(0);
	hci->setHitSound(0);
	hci->setInventoryWidth(2);
	hci->setInventoryHeight(1);
	hci->setClipSize(6);
	hci->setWeight(6);

	RuleItem *acannon = new RuleItem("STR_AUTO_CANNON");
	acannon->setSize(0.3f);
	acannon->setCost(13500);
	acannon->setBigSprite(7);
	acannon->setHandSprite(32);
	acannon->setTwoHanded(true);
	acannon->setBulletSprite(3);
	acannon->setFireSound(2);
	acannon->setAccuracyAuto(32);
	acannon->setTUAuto(40);
	acannon->setAccuracySnap(56);
	acannon->setTUSnap(33);
	acannon->setAccuracyAimed(82);
	acannon->setTUAimed(80);
	acannon->getCompatibleAmmo()->push_back("STR_AC_AP_AMMO");
	acannon->getCompatibleAmmo()->push_back("STR_AC_HE_AMMO");
	acannon->getCompatibleAmmo()->push_back("STR_AC_I_AMMO");
	acannon->setBattleType(BT_FIREARM);
	acannon->setInventoryWidth(2);
	acannon->setInventoryHeight(3);
	acannon->setWeight(19);
	
	RuleItem *acap = new RuleItem("STR_AC_AP_AMMO");
	acap->setSize(0.1f);
	acap->setCost(500);
	acap->setBigSprite(8);
	acap->setPower(42);
	acap->setDamageType(DT_AP);
	acap->setBattleType(BT_AMMO);
	acap->setHitSound(13);
	acap->setHitAnimation(26);
	acap->setInventoryWidth(2);
	acap->setInventoryHeight(1);
	acap->setClipSize(14);
	acap->setWeight(5);

	RuleItem *ache = new RuleItem("STR_AC_HE_AMMO");
	ache->setSize(0.1f);
	ache->setCost(700);
	ache->setBigSprite(9);
	ache->setPower(44);
	ache->setDamageType(DT_HE);
	ache->setBattleType(BT_AMMO);
	ache->setHitSound(0);
	ache->setHitAnimation(0);
	ache->setInventoryWidth(2);
	ache->setInventoryHeight(1);
	ache->setClipSize(14);
	ache->setWeight(5);

	RuleItem *aci = new RuleItem("STR_AC_I_AMMO");
	aci->setSize(0.1f);
	aci->setCost(650);
	aci->setBigSprite(10);
	aci->setPower(42);
	aci->setDamageType(DT_IN);
	aci->setBattleType(BT_AMMO);
	aci->setHitSound(0);
	aci->setHitAnimation(0);
	aci->setInventoryWidth(2);
	aci->setInventoryHeight(1);
	aci->setClipSize(14);
	aci->setWeight(5);

	RuleItem *rlauncher = new RuleItem("STR_ROCKET_LAUNCHER");
	rlauncher->setSize(0.4f);
	rlauncher->setCost(4000);
	rlauncher->setBigSprite(15);
	rlauncher->setHandSprite(8*9);
	rlauncher->setTwoHanded(true);
	rlauncher->setBulletSprite(0);
	rlauncher->setFireSound(52);
	rlauncher->setAccuracySnap(55);
	rlauncher->setTUSnap(45);
	rlauncher->setTUAuto(45);
	rlauncher->setAccuracyAimed(115);
	rlauncher->setTUAimed(75);
	rlauncher->getCompatibleAmmo()->push_back("STR_SMALL_ROCKET");
	rlauncher->getCompatibleAmmo()->push_back("STR_LARGE_ROCKET");
	rlauncher->getCompatibleAmmo()->push_back("STR_INCENDIARY_ROCKET");
	rlauncher->setBattleType(BT_FIREARM);
	rlauncher->setInventoryWidth(2);
	rlauncher->setInventoryHeight(3);
	rlauncher->setWeight(10);

	RuleItem *srocket = new RuleItem("STR_SMALL_ROCKET");
	srocket->setSize(0.2f);
	srocket->setCost(600);
	srocket->setBigSprite(16);
	srocket->setPower(75);
	srocket->setDamageType(DT_HE);
	srocket->setBattleType(BT_AMMO);
	srocket->setHitSound(0);
	srocket->setHitAnimation(0);
	srocket->setInventoryWidth(1);
	srocket->setInventoryHeight(3);
	srocket->setClipSize(1);
	srocket->setWeight(6);

	RuleItem *lrocket = new RuleItem("STR_LARGE_ROCKET");
	lrocket->setSize(0.2f);
	lrocket->setCost(900);
	lrocket->setBigSprite(17);
	lrocket->setPower(100);
	lrocket->setDamageType(DT_HE);
	lrocket->setBattleType(BT_AMMO);
	lrocket->setHitSound(0);
	lrocket->setHitAnimation(0);
	lrocket->setInventoryWidth(1);
	lrocket->setInventoryHeight(3);
	lrocket->setClipSize(1);
	lrocket->setWeight(8);

	RuleItem *irocket = new RuleItem("STR_INCENDIARY_ROCKET");
	irocket->setSize(0.2f);
	irocket->setCost(1200);
	irocket->setBigSprite(18);
	irocket->setPower(90);
	irocket->setDamageType(DT_IN);
	irocket->setBattleType(BT_AMMO);
	irocket->setHitSound(0);
	irocket->setHitAnimation(0);
	irocket->setInventoryWidth(1);
	irocket->setInventoryHeight(3);
	irocket->setClipSize(1);
	irocket->setWeight(8);

	RuleItem *grenade = new RuleItem("STR_GRENADE");
	grenade->setSize(0.1f);
	grenade->setCost(300);
	grenade->setBigSprite(19);
	grenade->setPower(50);
	grenade->setDamageType(DT_HE);
	grenade->setBattleType(BT_GRENADE);
	grenade->setWeight(3);

	RuleItem *flare = new RuleItem("STR_ELECTRO_FLARE");
	flare->setSize(0.1f);
	flare->setCost(60);
	flare->setBigSprite(55);
	flare->setFloorSprite(72);
	flare->setPower(15);
	flare->setDamageType(DT_NONE);
	flare->setBattleType(BT_FLARE);
	flare->setWeight(3);


	RuleItem *sgrenade = new RuleItem("STR_SMOKE_GRENADE");
	sgrenade->setSize(0.1f);
	sgrenade->setCost(150);
	sgrenade->setBigSprite(20);
	sgrenade->setPower(60);
	sgrenade->setDamageType(DT_SMOKE);
	sgrenade->setBattleType(BT_GRENADE);
	sgrenade->setWeight(3);

	RuleItem *corpse = new RuleItem("STR_CORPSE");
	corpse->setBigSprite(45);
	corpse->setFloorSprite(39);
	corpse->setInventoryWidth(2);
	corpse->setInventoryHeight(3);
	corpse->setWeight(22);

	RuleItem *scorpse = new RuleItem("STR_SECTOID_CORPSE");
	scorpse->setBigSprite(46);
	scorpse->setFloorSprite(42);
	scorpse->setInventoryWidth(2);
	scorpse->setInventoryHeight(3);
	scorpse->setWeight(30);

	RuleItem *ppistol = new RuleItem("STR_PLASMA_PISTOL");
	ppistol->setSize(0.1f);
	ppistol->setCost(800);
	ppistol->setBigSprite(32);
	ppistol->setHandSprite(104);
	ppistol->setFloorSprite(31);
	ppistol->setBulletSprite(8);
	ppistol->setFireSound(18);
	ppistol->setAccuracyAuto(50);
	ppistol->setTUAuto(30);
	ppistol->setAccuracySnap(65);
	ppistol->setTUSnap(30);
	ppistol->setAccuracyAimed(85);
	ppistol->setTUAimed(60);
	ppistol->getCompatibleAmmo()->push_back("STR_PLASMA_PISTOL_CLIP");
	ppistol->setBattleType(BT_FIREARM);
	ppistol->setInventoryWidth(1);
	ppistol->setInventoryHeight(2);
	ppistol->setWeight(3);

	RuleItem *ppclip = new RuleItem("STR_PLASMA_PISTOL_CLIP");
	ppclip->setSize(0.1f);
	ppclip->setCost(70);
	ppclip->setBigSprite(34);
	ppclip->setPower(52);
	ppclip->setDamageType(DT_PLASMA);
	ppclip->setBattleType(BT_AMMO);
	ppclip->setHitAnimation(46);
	ppclip->setHitSound(19);
	ppclip->setClipSize(26);
	ppclip->setWeight(3);

	RuleItem *alloys = new RuleItem("STR_ALIEN_ALLOYS");
	alloys->setSize(0.1f);
	alloys->setCost(6500);
	alloys->setTransferTime(96);
	RuleItem *lp = new RuleItem("STR_LASER_PISTOL");
	lp->setSize(0.1f);
	lp->setCost(70);
	lp->setBigSprite(34);
	lp->setPower(52);
	lp->setDamageType(DT_PLASMA);
	lp->setBattleType(BT_AMMO);
	lp->setHitAnimation(46);
	lp->setHitSound(19);
	lp->setClipSize(26);
	lp->setWeight(3);
	lp->setCategory ("STR_WEAPON");
	lp->setManufactureInfo(new RuleManufactureInfo(2, 300, 8000));

	RuleItem *aa = new RuleItem("STR_ALIEN_ALLOYS");
	aa->setSize(0.1f);
	aa->setCost(70);
	aa->setBigSprite(34);
	aa->setPower(52);
	aa->setDamageType(DT_PLASMA);
	aa->setBattleType(BT_AMMO);
	aa->setHitAnimation(46);
	aa->setHitSound(19);
	aa->setClipSize(26);
	aa->setWeight(3);
	aa->setManufactureInfo(new RuleManufactureInfo(2, 300, 8000));
	aa->setCategory ("STR_EQUIPMENT");

	RuleItem *pa = new RuleItem("STR_PERSONAL_ARMOR");
	pa->setSize(0.1f);
	pa->setCost(70);
	pa->setBigSprite(34);
	pa->setPower(52);
	pa->setDamageType(DT_PLASMA);
	pa->setBattleType(BT_AMMO);
	pa->setHitAnimation(46);
	pa->setHitSound(19);
	pa->setClipSize(26);
	pa->setWeight(3);
	pa->setCategory ("STR_EQUIPMENT");
	RuleManufactureInfo * info = new RuleManufactureInfo(2, 300, 8000);
	info->addNeededItem("STR_ALIEN_ALLOYS", 4);
	pa->setManufactureInfo(info);

	RuleItem *lr = new RuleItem("STR_LASER_RIFLE");
	lr->setSize(0.1f);
	lr->setCost(70);
	lr->setBigSprite(34);
	lr->setPower(52);
	lr->setDamageType(DT_PLASMA);
	lr->setBattleType(BT_AMMO);
	lr->setHitAnimation(46);
	lr->setHitSound(19);
	lr->setClipSize(26);
	lr->setWeight(3);
	lr->setCategory ("STR_WEAPON");
	lr->setManufactureInfo(new RuleManufactureInfo(2, 300, 8000000));

	_items.insert(std::pair<std::string, RuleItem*>("STR_STINGRAY_LAUNCHER", slauncher));
	_items.insert(std::pair<std::string, RuleItem*>("STR_AVALANCHE_LAUNCHER", alauncher));
	_items.insert(std::pair<std::string, RuleItem*>("STR_CANNON", icannon));
	_items.insert(std::pair<std::string, RuleItem*>("STR_STINGRAY_MISSILES", smissile));
	_items.insert(std::pair<std::string, RuleItem*>("STR_AVALANCHE_MISSILES", amissile));
	_items.insert(std::pair<std::string, RuleItem*>("STR_CANNON_ROUNDS_X50", crounds));
	_items.insert(std::pair<std::string, RuleItem*>("STR_PISTOL", pistol));
	_items.insert(std::pair<std::string, RuleItem*>("STR_PISTOL_CLIP", pclip));
	_items.insert(std::pair<std::string, RuleItem*>("STR_RIFLE", rifle));
	_items.insert(std::pair<std::string, RuleItem*>("STR_RIFLE_CLIP", rclip));
	_items.insert(std::pair<std::string, RuleItem*>("STR_HEAVY_CANNON", hcannon));
	_items.insert(std::pair<std::string, RuleItem*>("STR_HC_AP_AMMO", hcap));
	_items.insert(std::pair<std::string, RuleItem*>("STR_HC_HE_AMMO", hche));
	_items.insert(std::pair<std::string, RuleItem*>("STR_HC_I_AMMO", hci));
	_items.insert(std::pair<std::string, RuleItem*>("STR_AUTO_CANNON", acannon));
	_items.insert(std::pair<std::string, RuleItem*>("STR_AC_AP_AMMO", acap));
	_items.insert(std::pair<std::string, RuleItem*>("STR_AC_HE_AMMO", ache));
	_items.insert(std::pair<std::string, RuleItem*>("STR_AC_I_AMMO", aci));
	_items.insert(std::pair<std::string, RuleItem*>("STR_ROCKET_LAUNCHER", rlauncher));
	_items.insert(std::pair<std::string, RuleItem*>("STR_SMALL_ROCKET", srocket));
	_items.insert(std::pair<std::string, RuleItem*>("STR_LARGE_ROCKET", lrocket));
	_items.insert(std::pair<std::string, RuleItem*>("STR_INCENDIARY_ROCKET", irocket));
	_items.insert(std::pair<std::string, RuleItem*>("STR_GRENADE", grenade));
	_items.insert(std::pair<std::string, RuleItem*>("STR_SMOKE_GRENADE", sgrenade));
	_items.insert(std::pair<std::string, RuleItem*>("STR_CORPSE", corpse));
	_items.insert(std::pair<std::string, RuleItem*>("STR_SECTOID_CORPSE", scorpse));
	_items.insert(std::pair<std::string, RuleItem*>("STR_PLASMA_PISTOL", ppistol));
	_items.insert(std::pair<std::string, RuleItem*>("STR_PLASMA_PISTOL_CLIP", ppclip));
	_items.insert(std::pair<std::string, RuleItem*>("STR_ALIEN_ALLOYS", alloys));
	_items.insert(std::pair<std::string, RuleItem*>("STR_ELECTRO_FLARE", flare));
	_items.insert(std::pair<std::string, RuleItem*>("STR_LASER_PISTOL", lp));

	_items.insert(std::pair<std::string, RuleItem*>("STR_ALIEN_ALLOYS", aa));
	_items.insert(std::pair<std::string, RuleItem*>("STR_PERSONAL_ARMOR", pa));
	_items.insert(std::pair<std::string, RuleItem*>("STR_LASER_RIFLE", lr));
	// Add UFOs
	RuleUfo *sscout = new RuleUfo("STR_SMALL_SCOUT");
	sscout->setSize("STR_VERY_SMALL");
	sscout->setSprite(0);
	sscout->setMaxSpeed(2200);
	sscout->setAcceleration(12);
	sscout->setMaxDamage(50);
	sscout->setScore(50);
	ruleTerrain = new RuleTerrain("UFO1A");
	sscout->setBattlescapeTerrainData(ruleTerrain);
	ruleTerrain->getMapDataSets()->push_back(getMapDataSet("BLANKS"));
	ruleTerrain->getMapDataSets()->push_back(getMapDataSet("UFO1"));
	ruleTerrain->getMapBlocks()->push_back(new MapBlock(ruleTerrain,"UFO1A",10,10,true));

	RuleUfo *mscout = new RuleUfo("STR_MEDIUM_SCOUT");
	mscout->setSize("STR_SMALL");
	mscout->setSprite(1);
	mscout->setMaxSpeed(2400);
	mscout->setAcceleration(9);
	mscout->setWeaponPower(20);
	mscout->setWeaponRange(15);
	mscout->setMaxDamage(200);
	mscout->setScore(75);
	ruleTerrain = new RuleTerrain("UFO_110");
	mscout->setBattlescapeTerrainData(ruleTerrain);
	ruleTerrain->getMapDataSets()->push_back(getMapDataSet("BLANKS"));
	ruleTerrain->getMapDataSets()->push_back(getMapDataSet("U_EXT02"));
	ruleTerrain->getMapDataSets()->push_back(getMapDataSet("U_WALL02"));
	ruleTerrain->getMapDataSets()->push_back(getMapDataSet("U_BITS"));
//  ruleTerrain->getMapDataSets()->push_back(getMapDataSet("U_DISEC2"));
//	ruleTerrain->getMapDataSets()->push_back(getMapDataSet("U_OPER2"));
//  ruleTerrain->getMapDataSets()->push_back(getMapDataSet("U_PODS"));
	ruleTerrain->getMapBlocks()->push_back(new MapBlock(ruleTerrain,"UFO_110",10,10,true));

	RuleUfo *lscout = new RuleUfo("STR_LARGE_SCOUT");
	lscout->setSize("STR_SMALL");
	lscout->setSprite(2);
	lscout->setMaxSpeed(2700);
	lscout->setAcceleration(9);
	lscout->setWeaponPower(20);
	lscout->setWeaponRange(34);
	lscout->setMaxDamage(250);
	lscout->setScore(125);
	ruleTerrain = new RuleTerrain("UFO_120");
	lscout->setBattlescapeTerrainData(ruleTerrain);
	ruleTerrain->getMapDataSets()->push_back(getMapDataSet("BLANKS"));
	ruleTerrain->getMapDataSets()->push_back(getMapDataSet("U_EXT02"));
	ruleTerrain->getMapDataSets()->push_back(getMapDataSet("U_WALL02"));
	ruleTerrain->getMapDataSets()->push_back(getMapDataSet("U_BITS"));
	ruleTerrain->getMapBlocks()->push_back(new MapBlock(ruleTerrain,"UFO_120",20,20,true));

	RuleUfo *abducter = new RuleUfo("STR_ABDUCTER");
	abducter->setSize("STR_MEDIUM");
	abducter->setSprite(3);
	abducter->setMaxSpeed(4000);
	abducter->setAcceleration(8);
	abducter->setWeaponPower(40);
	abducter->setWeaponRange(22);
	abducter->setMaxDamage(500);
	abducter->setScore(250);

	RuleUfo *harvester = new RuleUfo("STR_HARVESTER");
	harvester->setSize("STR_MEDIUM");
	harvester->setSprite(4);
	harvester->setMaxSpeed(4300);
	harvester->setAcceleration(8);
	harvester->setWeaponPower(40);
	harvester->setWeaponRange(20);
	harvester->setMaxDamage(500);
	harvester->setScore(250);

	RuleUfo *supply = new RuleUfo("STR_SUPPLY_SHIP");
	supply->setSize("STR_LARGE");
	supply->setSprite(5);
	supply->setMaxSpeed(3200);
	supply->setAcceleration(6);
	supply->setWeaponPower(60);
	supply->setWeaponRange(36);
	supply->setMaxDamage(2200);
	supply->setScore(400);

	RuleUfo *terror = new RuleUfo("STR_TERROR_SHIP");
	terror->setSize("STR_LARGE");
	terror->setSprite(6);
	terror->setMaxSpeed(4800);
	terror->setAcceleration(6);
	terror->setWeaponPower(120);
	terror->setWeaponRange(42);
	terror->setMaxDamage(1200);
	terror->setScore(500);

	RuleUfo *battleship = new RuleUfo("STR_BATTLESHIP");
	battleship->setSize("STR_VERY_LARGE");
	battleship->setSprite(7);
	battleship->setMaxSpeed(5000);
	battleship->setAcceleration(6);
	battleship->setWeaponPower(148);
	battleship->setWeaponRange(65);
	battleship->setMaxDamage(3200);
	battleship->setScore(700);

	_ufos.insert(std::pair<std::string, RuleUfo*>("STR_SMALL_SCOUT", sscout));
	_ufos.insert(std::pair<std::string, RuleUfo*>("STR_MEDIUM_SCOUT", mscout));
	_ufos.insert(std::pair<std::string, RuleUfo*>("STR_LARGE_SCOUT", lscout));
	_ufos.insert(std::pair<std::string, RuleUfo*>("STR_ABDUCTER", abducter));
	_ufos.insert(std::pair<std::string, RuleUfo*>("STR_HARVESTER", harvester));
	_ufos.insert(std::pair<std::string, RuleUfo*>("STR_SUPPLY_SHIP", supply));
	_ufos.insert(std::pair<std::string, RuleUfo*>("STR_TERROR_SHIP", terror));
	_ufos.insert(std::pair<std::string, RuleUfo*>("STR_BATTLESHIP", battleship));

	// Add inventory
	RuleInventory *rshoulder = new RuleInventory("STR_RIGHT_SHOULDER");
	rshoulder->setX(16);
	rshoulder->setY(40);
	rshoulder->addSlot(0, 0);
	rshoulder->addSlot(1, 0);
	rshoulder->addCost("STR_GROUND", 4);
	rshoulder->addCost("STR_RIGHT_HAND", 3);
	rshoulder->addCost("STR_LEFT_HAND", 3);
	rshoulder->addCost("STR_RIGHT_LEG", 12);
	rshoulder->addCost("STR_LEFT_LEG", 12);
	rshoulder->addCost("STR_BELT", 10);
	rshoulder->addCost("STR_LEFT_SHOULDER", 8);
	rshoulder->addCost("STR_BACKPACK", 16);

	RuleInventory *lshoulder = new RuleInventory("STR_LEFT_SHOULDER");
	lshoulder->setX(112);
	lshoulder->setY(40);
	lshoulder->addSlot(0, 0);
	lshoulder->addSlot(1, 0);
	lshoulder->addCost("STR_GROUND", 4);
	lshoulder->addCost("STR_RIGHT_HAND", 3);
	lshoulder->addCost("STR_LEFT_HAND", 3);
	lshoulder->addCost("STR_RIGHT_LEG", 12);
	lshoulder->addCost("STR_LEFT_LEG", 12);
	lshoulder->addCost("STR_BELT", 10);
	lshoulder->addCost("STR_RIGHT_SHOULDER", 8);
	lshoulder->addCost("STR_BACKPACK", 16);

	RuleInventory *rhand = new RuleInventory("STR_RIGHT_HAND");
	rhand->setX(0);
	rhand->setY(64);
	rhand->setType(INV_HAND);
	rhand->addCost("STR_GROUND", 2);
	rhand->addCost("STR_LEFT_HAND", 4);
	rhand->addCost("STR_RIGHT_LEG", 8);
	rhand->addCost("STR_LEFT_LEG", 10);
	rhand->addCost("STR_BELT", 8);
	rhand->addCost("STR_RIGHT_SHOULDER", 10);
	rhand->addCost("STR_LEFT_SHOULDER", 10);
	rhand->addCost("STR_BACK_PACK", 14);

	RuleInventory *lhand = new RuleInventory("STR_LEFT_HAND");
	lhand->setX(128);
	lhand->setY(64);
	lhand->setType(INV_HAND);
	lhand->addCost("STR_GROUND", 2);
	lhand->addCost("STR_RIGHT_HAND", 4);
	lhand->addCost("STR_RIGHT_LEG", 10);
	lhand->addCost("STR_LEFT_LEG", 8);
	lhand->addCost("STR_BELT", 8);
	lhand->addCost("STR_RIGHT_SHOULDER", 10);
	lhand->addCost("STR_LEFT_SHOULDER", 10);
	lhand->addCost("STR_BACK_PACK", 14);

	RuleInventory *rleg = new RuleInventory("STR_RIGHT_LEG");
	rleg->setX(0);
	rleg->setY(120);
	rleg->addSlot(0, 0);
	rleg->addSlot(1, 0);
	rleg->addCost("STR_GROUND", 6);
	rleg->addCost("STR_RIGHT_HAND", 4);
	rleg->addCost("STR_LEFT_HAND", 6);
	rleg->addCost("STR_LEFT_LEG", 10);
	rleg->addCost("STR_BELT", 10);
	rleg->addCost("STR_RIGHT_SHOULDER", 10);
	rleg->addCost("STR_LEFT_SHOULDER", 10);
	rleg->addCost("STR_BACK_PACK", 18);

	RuleInventory *lleg = new RuleInventory("STR_LEFT_LEG");
	lleg->setX(128);
	lleg->setY(120);
	lleg->addSlot(0, 0);
	lleg->addSlot(1, 0);
	lleg->addCost("STR_GROUND", 6);
	lleg->addCost("STR_RIGHT_HAND", 6);
	lleg->addCost("STR_LEFT_HAND", 4);
	lleg->addCost("STR_RIGHT_LEG", 10);
	lleg->addCost("STR_BELT", 10);
	lleg->addCost("STR_RIGHT_SHOULDER", 10);
	lleg->addCost("STR_LEFT_SHOULDER", 10);
	lleg->addCost("STR_BACK_PACK", 18);

	RuleInventory *backpack = new RuleInventory("STR_BACK_PACK");
	backpack->setX(192);
	backpack->setY(40);
	backpack->addSlot(0, 0);
	backpack->addSlot(1, 0);
	backpack->addSlot(2, 0);
	backpack->addSlot(0, 1);
	backpack->addSlot(1, 1);
	backpack->addSlot(2, 1);
	backpack->addSlot(0, 2);
	backpack->addSlot(1, 2);
	backpack->addSlot(2, 2);
	backpack->addCost("STR_GROUND", 10);
	backpack->addCost("STR_RIGHT_HAND", 8);
	backpack->addCost("STR_LEFT_HAND", 8);
	backpack->addCost("STR_RIGHT_LEG", 16);
	backpack->addCost("STR_LEFT_LEG", 16);
	backpack->addCost("STR_BELT", 12);
	backpack->addCost("STR_RIGHT_SHOULDER", 14);
	backpack->addCost("STR_LEFT_SHOULDER", 14);

	RuleInventory *belt = new RuleInventory("STR_BELT");
	belt->setX(192);
	belt->setY(104);
	belt->addSlot(0, 0);
	belt->addSlot(1, 0);
	belt->addSlot(2, 0);
	belt->addSlot(3, 0);
	belt->addSlot(0, 1);
	belt->addSlot(3, 1);
	belt->addCost("STR_GROUND", 6);
	belt->addCost("STR_RIGHT_HAND", 4);
	belt->addCost("STR_LEFT_HAND", 4);
	belt->addCost("STR_RIGHT_LEG", 10);
	belt->addCost("STR_LEFT_LEG", 10);
	belt->addCost("STR_RIGHT_SHOULDER", 12);
	belt->addCost("STR_LEFT_SHOULDER", 12);
	belt->addCost("STR_BACK_PACK", 16);

	RuleInventory *ground = new RuleInventory("STR_GROUND");
	ground->setX(0);
	ground->setY(152);
	ground->setType(INV_GROUND);
	ground->addCost("STR_RIGHT_HAND", 8);
	ground->addCost("STR_LEFT_HAND", 8);
	ground->addCost("STR_RIGHT_LEG", 10);
	ground->addCost("STR_LEFT_LEG", 10);
	ground->addCost("STR_BELT", 12);
	ground->addCost("STR_RIGHT_SHOULDER", 12);
	ground->addCost("STR_LEFT_SHOULDER", 12);
	ground->addCost("STR_BACK_PACK", 20);

	_invs.insert(std::pair<std::string, RuleInventory*>("STR_RIGHT_SHOULDER", rshoulder));
	_invs.insert(std::pair<std::string, RuleInventory*>("STR_LEFT_SHOULDER", lshoulder));
	_invs.insert(std::pair<std::string, RuleInventory*>("STR_RIGHT_HAND", rhand));
	_invs.insert(std::pair<std::string, RuleInventory*>("STR_LEFT_HAND", lhand));
	_invs.insert(std::pair<std::string, RuleInventory*>("STR_RIGHT_LEG", rleg));
	_invs.insert(std::pair<std::string, RuleInventory*>("STR_LEFT_LEG", lleg));
	_invs.insert(std::pair<std::string, RuleInventory*>("STR_BACK_PACK", backpack));
	_invs.insert(std::pair<std::string, RuleInventory*>("STR_BELT", belt));
	_invs.insert(std::pair<std::string, RuleInventory*>("STR_GROUND", ground));
	
	// Add terrain
	RuleTerrain *culta = new RuleTerrain("CULTA");
	culta->getMapDataSets()->push_back(getMapDataSet("BLANKS"));
	culta->getMapDataSets()->push_back(getMapDataSet("CULTIVAT"));
	culta->getMapDataSets()->push_back(getMapDataSet("BARN"));
	culta->getMapBlocks()->push_back(new MapBlock(culta,"CULTA00",10,10,true));
	//culta->getMapBlocks()->push_back(new MapBlock(culta,"CULTA0B",10,10,false));
	culta->getMapBlocks()->push_back(new MapBlock(culta,"CULTA01",10,10,false));
	culta->getMapBlocks()->push_back(new MapBlock(culta,"CULTA02",10,10,false));
	culta->getMapBlocks()->push_back(new MapBlock(culta,"CULTA03",10,10,false));
	culta->getMapBlocks()->push_back(new MapBlock(culta,"CULTA04",10,10,false));
	culta->getMapBlocks()->push_back(new MapBlock(culta,"CULTA05",10,10,false));
	culta->getMapBlocks()->push_back(new MapBlock(culta,"CULTA06",10,10,false));
	culta->getMapBlocks()->push_back(new MapBlock(culta,"CULTA07",10,10,true));
	culta->getMapBlocks()->push_back(new MapBlock(culta,"CULTA08",10,10,false));
	culta->getMapBlocks()->push_back(new MapBlock(culta,"CULTA09",10,10,false));
	culta->getMapBlocks()->push_back(new MapBlock(culta,"CULTA10",10,10,false));
	culta->getMapBlocks()->push_back(new MapBlock(culta,"CULTA11",10,10,true));
	culta->getMapBlocks()->push_back(new MapBlock(culta,"CULTA12",10,10,false));
	culta->getMapBlocks()->push_back(new MapBlock(culta,"CULTA13",10,10,false));
	culta->getMapBlocks()->push_back(new MapBlock(culta,"CULTA14",10,10,false));
	culta->getMapBlocks()->push_back(new MapBlock(culta,"CULTA15",10,10,false));
	culta->getMapBlocks()->push_back(new MapBlock(culta,"CULTA16",10,10,false));
	culta->getMapBlocks()->push_back(new MapBlock(culta,"CULTA17",10,10,false));
	culta->getMapBlocks()->push_back(new MapBlock(culta,"CULTA18",10,10,false));
	RuleTerrain *jungle = new RuleTerrain("JUNGLE");
	jungle->getMapDataSets()->push_back(getMapDataSet("BLANKS"));
	jungle->getMapDataSets()->push_back(getMapDataSet("JUNGLE"));
	jungle->getMapBlocks()->push_back(new MapBlock(jungle,"JUNGLE00",10,10,true));
	jungle->getMapBlocks()->push_back(new MapBlock(jungle,"JUNGLE01",10,10,true));
	jungle->getMapBlocks()->push_back(new MapBlock(jungle,"JUNGLE02",10,10,true));
	jungle->getMapBlocks()->push_back(new MapBlock(jungle,"JUNGLE03",10,10,false));
	jungle->getMapBlocks()->push_back(new MapBlock(jungle,"JUNGLE04",10,10,false));
	jungle->getMapBlocks()->push_back(new MapBlock(jungle,"JUNGLE05",10,10,false));
	jungle->getMapBlocks()->push_back(new MapBlock(jungle,"JUNGLE06",10,10,false));
	jungle->getMapBlocks()->push_back(new MapBlock(jungle,"JUNGLE07",10,10,false));
	jungle->getMapBlocks()->push_back(new MapBlock(jungle,"JUNGLE08",10,10,false));
	jungle->getMapBlocks()->push_back(new MapBlock(jungle,"JUNGLE09",10,10,false));
	jungle->getMapBlocks()->push_back(new MapBlock(jungle,"JUNGLE10",10,10,false));
	jungle->getMapBlocks()->push_back(new MapBlock(jungle,"JUNGLE11",10,10,false));
	RuleTerrain *forest = new RuleTerrain("FOREST");
	forest->getMapDataSets()->push_back(getMapDataSet("BLANKS"));
	forest->getMapDataSets()->push_back(getMapDataSet("FOREST"));
	forest->getMapBlocks()->push_back(new MapBlock(forest,"FOREST00",10,10,true));
	forest->getMapBlocks()->push_back(new MapBlock(forest,"FOREST01",10,10,true));
	forest->getMapBlocks()->push_back(new MapBlock(forest,"FOREST02",10,10,false));
	forest->getMapBlocks()->push_back(new MapBlock(forest,"FOREST03",10,10,false));
	forest->getMapBlocks()->push_back(new MapBlock(forest,"FOREST04",10,10,false));
	forest->getMapBlocks()->push_back(new MapBlock(forest,"FOREST05",10,10,false));
	forest->getMapBlocks()->push_back(new MapBlock(forest,"FOREST06",10,10,false));
	forest->getMapBlocks()->push_back(new MapBlock(forest,"FOREST07",10,10,false));
	forest->getMapBlocks()->push_back(new MapBlock(forest,"FOREST08",10,10,false));
	forest->getMapBlocks()->push_back(new MapBlock(forest,"FOREST09",10,10,false));
	forest->getMapBlocks()->push_back(new MapBlock(forest,"FOREST10",20,20,false));
	forest->getMapBlocks()->push_back(new MapBlock(forest,"FOREST11",20,20,false));
	RuleTerrain *desert = new RuleTerrain("DESERT");
	desert->getMapDataSets()->push_back(getMapDataSet("BLANKS"));
	desert->getMapDataSets()->push_back(getMapDataSet("DESERT"));
	desert->getMapBlocks()->push_back(new MapBlock(desert,"DESERT00",10,10,true));
	desert->getMapBlocks()->push_back(new MapBlock(desert,"DESERT01",10,10,true));
	desert->getMapBlocks()->push_back(new MapBlock(desert,"DESERT02",10,10,true));
	desert->getMapBlocks()->push_back(new MapBlock(desert,"DESERT03",10,10,false));
	desert->getMapBlocks()->push_back(new MapBlock(desert,"DESERT04",10,10,false));
	desert->getMapBlocks()->push_back(new MapBlock(desert,"DESERT05",10,10,false));
	desert->getMapBlocks()->push_back(new MapBlock(desert,"DESERT06",10,10,false));
	desert->getMapBlocks()->push_back(new MapBlock(desert,"DESERT07",10,10,false));
	desert->getMapBlocks()->push_back(new MapBlock(desert,"DESERT08",10,10,false));
	desert->getMapBlocks()->push_back(new MapBlock(desert,"DESERT09",20,20,false));
	desert->getMapBlocks()->push_back(new MapBlock(desert,"DESERT10",20,20,false));
	desert->getMapBlocks()->push_back(new MapBlock(desert,"DESERT11",20,20,false));
	RuleTerrain *mount = new RuleTerrain("MOUNT");
	mount->getMapDataSets()->push_back(getMapDataSet("BLANKS"));
	mount->getMapDataSets()->push_back(getMapDataSet("MOUNT"));
	mount->getMapBlocks()->push_back(new MapBlock(mount,"MOUNT00",10,10,true));
	mount->getMapBlocks()->push_back(new MapBlock(mount,"MOUNT01",10,10,true));
	mount->getMapBlocks()->push_back(new MapBlock(mount,"MOUNT02",10,10,true));
	mount->getMapBlocks()->push_back(new MapBlock(mount,"MOUNT03",10,10,false));
	mount->getMapBlocks()->push_back(new MapBlock(mount,"MOUNT04",10,10,false));
	mount->getMapBlocks()->push_back(new MapBlock(mount,"MOUNT05",10,10,false));
	mount->getMapBlocks()->push_back(new MapBlock(mount,"MOUNT06",10,10,false));
	mount->getMapBlocks()->push_back(new MapBlock(mount,"MOUNT07",10,10,false));
	mount->getMapBlocks()->push_back(new MapBlock(mount,"MOUNT08",10,10,false));
	mount->getMapBlocks()->push_back(new MapBlock(mount,"MOUNT09",10,10,false));
	mount->getMapBlocks()->push_back(new MapBlock(mount,"MOUNT10",10,10,false));
	mount->getMapBlocks()->push_back(new MapBlock(mount,"MOUNT11",20,20,false));
	mount->getMapBlocks()->push_back(new MapBlock(mount,"MOUNT12",20,20,false));
	RuleTerrain *polar = new RuleTerrain("POLAR");
	polar->getMapDataSets()->push_back(getMapDataSet("BLANKS"));
	polar->getMapDataSets()->push_back(getMapDataSet("POLAR"));
	polar->getMapBlocks()->push_back(new MapBlock(polar,"POLAR00",10,10,true));
	polar->getMapBlocks()->push_back(new MapBlock(polar,"POLAR01",10,10,true));
	polar->getMapBlocks()->push_back(new MapBlock(polar,"POLAR02",10,10,true));
	polar->getMapBlocks()->push_back(new MapBlock(polar,"POLAR03",10,10,false));
	polar->getMapBlocks()->push_back(new MapBlock(polar,"POLAR04",10,10,false));
	polar->getMapBlocks()->push_back(new MapBlock(polar,"POLAR05",10,10,false));
	polar->getMapBlocks()->push_back(new MapBlock(polar,"POLAR06",10,10,false));
	polar->getMapBlocks()->push_back(new MapBlock(polar,"POLAR07",10,10,false));
	polar->getMapBlocks()->push_back(new MapBlock(polar,"POLAR08",10,10,false));
	polar->getMapBlocks()->push_back(new MapBlock(polar,"POLAR09",10,10,false));
	polar->getMapBlocks()->push_back(new MapBlock(polar,"POLAR10",10,10,false));
	polar->getMapBlocks()->push_back(new MapBlock(polar,"POLAR11",10,10,false));
	polar->getMapBlocks()->push_back(new MapBlock(polar,"POLAR12",20,20,false));
	polar->getMapBlocks()->push_back(new MapBlock(polar,"POLAR13",20,20,false));
	RuleTerrain *mars = new RuleTerrain("MARS");
	mars->getMapDataSets()->push_back(getMapDataSet("BLANKS"));
	mars->getMapDataSets()->push_back(getMapDataSet("MARS"));
	mars->getMapDataSets()->push_back(getMapDataSet("U_WALL02"));
	mars->getMapBlocks()->push_back(new MapBlock(mars,"MARS00",10,10,true));
	RuleTerrain *urban = new RuleTerrain("URBAN");
	urban->getMapDataSets()->push_back(getMapDataSet("BLANKS"));
	urban->getMapDataSets()->push_back(getMapDataSet("ROADS"));
	urban->getMapDataSets()->push_back(getMapDataSet("URBITS"));
	urban->getMapDataSets()->push_back(getMapDataSet("URBAN"));
	urban->getMapDataSets()->push_back(getMapDataSet("FRNITURE"));
	//urban->getMapBlocks()->push_back(new MapBlock(urban,"URBAN00",10,10,false));
	//urban->getMapBlocks()->push_back(new MapBlock(urban,"URBAN01",10,10,false));
	//urban->getMapBlocks()->push_back(new MapBlock(urban,"URBAN02",10,10,false));
	urban->getMapBlocks()->push_back(new MapBlock(urban,"URBAN03",10,10,true));
	urban->getMapBlocks()->push_back(new MapBlock(urban,"URBAN04",10,10,true));
	urban->getMapBlocks()->push_back(new MapBlock(urban,"URBAN05",20,20,false));
	urban->getMapBlocks()->push_back(new MapBlock(urban,"URBAN06",20,20,false));
	urban->getMapBlocks()->push_back(new MapBlock(urban,"URBAN07",20,20,false));
	urban->getMapBlocks()->push_back(new MapBlock(urban,"URBAN08",20,20,false));
	urban->getMapBlocks()->push_back(new MapBlock(urban,"URBAN09",20,20,false));
	urban->getMapBlocks()->push_back(new MapBlock(urban,"URBAN14",10,10,false));
	urban->getMapBlocks()->push_back(new MapBlock(urban,"URBAN15",10,10,false));
	urban->getMapBlocks()->push_back(new MapBlock(urban,"URBAN16",10,10,false));
	urban->getMapBlocks()->push_back(new MapBlock(urban,"URBAN17",10,10,false));
	urban->getMapBlocks()->push_back(new MapBlock(urban,"URBAN18",10,10,false));

	_terrains.insert(std::pair<std::string, RuleTerrain*>("CULTA",culta));
	_terrains.insert(std::pair<std::string, RuleTerrain*>("JUNGLE",jungle));
	_terrains.insert(std::pair<std::string, RuleTerrain*>("FOREST",forest));
	_terrains.insert(std::pair<std::string, RuleTerrain*>("DESERT",desert));
	_terrains.insert(std::pair<std::string, RuleTerrain*>("MOUNT",mount));
	_terrains.insert(std::pair<std::string, RuleTerrain*>("POLAR",polar));
	_terrains.insert(std::pair<std::string, RuleTerrain*>("MARS",mars));
	_terrains.insert(std::pair<std::string, RuleTerrain*>("URBAN",urban));

	// Add armor
	RuleArmor *coveralls = new RuleArmor("STR_NONE_UC", "XCOM_0.PCK", 0);
	coveralls->setArmor(12, 8, 5, 2);
	coveralls->setCorpseItem("STR_CORPSE");

	RuleArmor *personalArmor = new RuleArmor("STR_PERSONAL_ARMOR_UC", "XCOM_1.PCK", 0);
	personalArmor->setArmor(50, 40, 40, 30);

	RuleArmor *powerSuit = new RuleArmor("STR_POWER_SUIT_UC", "XCOM_2.PCK", 0);
	powerSuit->setArmor(100, 80, 70, 60);

	RuleArmor *flyingSuit = new RuleArmor("STR_FLYING_SUIT_UC", "XCOM_2.PCK", 0);
	flyingSuit->setArmor(110, 90, 80, 70);

	RuleArmor *sectoidSoldierArmor = new RuleArmor("SECTOID_ARMOR0", "SECTOID.PCK", 0);
	sectoidSoldierArmor->setArmor(4, 3, 2, 2);
	sectoidSoldierArmor->setCorpseItem("STR_SECTOID_CORPSE");

	RuleArmor *floaterSoldierArmor = new RuleArmor("FLOATER_ARMOR0", "FLOATER.PCK", 1);
	floaterSoldierArmor->setArmor(8, 6, 4, 12);
	floaterSoldierArmor->setCorpseItem("STR_FLOATER_CORPSE");

	_armors.insert(std::pair<std::string, RuleArmor*>("STR_NONE_UC", coveralls));
	_armors.insert(std::pair<std::string, RuleArmor*>("STR_PERSONAL_ARMOR_UC", personalArmor));
	_armors.insert(std::pair<std::string, RuleArmor*>("STR_POWER_SUIT_UC" ,powerSuit));
	_armors.insert(std::pair<std::string, RuleArmor*>("STR_FLYING_SUIT_UC", flyingSuit));
	_armors.insert(std::pair<std::string, RuleArmor*>("SECTOID_ARMOR0", sectoidSoldierArmor));
	_armors.insert(std::pair<std::string, RuleArmor*>("FLOATER_ARMOR0", floaterSoldierArmor));

	// Add units
	RuleSoldier *xcom = new RuleSoldier("XCOM");
	xcom->setArmor("STR_NONE_UC");
	UnitStats s1;
	s1.tu = 50;
	s1.stamina = 40;
	s1.health = 25;
	s1.bravery = 10;
	s1.reactions = 30;
	s1.firing = 40;
	s1.throwing = 50;
	s1.strength = 20;
	s1.psiStrength = 0;
	s1.psiSkill = 16;
	s1.melee = 20;
	UnitStats s2;
	s2.tu = 60;
	s2.stamina = 70;
	s2.health = 40;
	s2.bravery = 60;
	s2.reactions = 60;
	s2.firing = 70;
	s2.throwing = 80;
	s2.strength = 40;
	s2.psiStrength = 100;
	s2.psiSkill = 24;
	s2.melee = 40;
	xcom->setStats(s1, s2);
	xcom->setVoxelParameters(22, 14, 3);

	_soldiers.insert(std::pair<std::string, RuleSoldier*>("XCOM", xcom));

	RuleAlien *sectoidSoldier = new RuleAlien("SECTOID_SOLDIER", "STR_SECTOID", "STR_LIVE_SOLDIER");
	sectoidSoldier->setArmor("SECTOID_ARMOR0");
	s1.tu = 54;
	s1.stamina = 90;
	s1.health = 30;
	s1.bravery = 80;
	s1.reactions = 63;
	s1.firing = 52;
	s1.throwing = 58;
	s1.strength = 30;
	s1.psiStrength = 40;
	s1.psiSkill = 0;
	s1.melee = 76;
	sectoidSoldier->setStats(s1);
	sectoidSoldier->setVoxelParameters(16, 12, 2);
	sectoidSoldier->setValue(10);

	RuleAlien *sectoidEngineer = new RuleAlien("SECTOID_ENGINEER", "STR_SECTOID", "STR_LIVE_ENGINEER");
	sectoidEngineer->setArmor("SECTOID_ARMOR0");
	s1.tu = 54;
	s1.stamina = 90;
	s1.health = 30;
	s1.bravery = 80;
	s1.reactions = 63;
	s1.firing = 52;
	s1.throwing = 58;
	s1.strength = 30;
	s1.psiStrength = 40;
	s1.psiSkill = 0;
	s1.melee = 76;
	sectoidEngineer->setStats(s1);
	sectoidEngineer->setVoxelParameters(16, 12, 2);
	sectoidEngineer->setValue(16);

	RuleAlien *sectoidNavigator = new RuleAlien("SECTOID_NAVIGATOR", "STR_SECTOID", "STR_LIVE_NAVIGATOR");
	sectoidNavigator->setArmor("SECTOID_ARMOR0");
	s1.tu = 54;
	s1.stamina = 90;
	s1.health = 30;
	s1.bravery = 80;
	s1.reactions = 63;
	s1.firing = 52;
	s1.throwing = 58;
	s1.strength = 30;
	s1.psiStrength = 40;
	s1.psiSkill = 0;
	s1.melee = 76;
	sectoidNavigator->setStats(s1);
	sectoidNavigator->setVoxelParameters(16, 12, 2);
	sectoidNavigator->setValue(12);

	RuleAlien *floaterSoldier = new RuleAlien("FLOATER_SOLDIER", "STR_FLOATER", "STR_LIVE_FLOATER");
	floaterSoldier->setArmor("FLOATER_ARMOR0");
	s1.tu = 50;
	s1.stamina = 90;
	s1.health = 35;
	s1.bravery = 80;
	s1.reactions = 50;
	s1.firing = 50;
	s1.throwing = 58;
	s1.strength = 40;
	s1.psiStrength = 35;
	s1.psiSkill = 0;
	s1.melee = 70;
	floaterSoldier->setStats(s1);
	floaterSoldier->setVoxelParameters(21, 16, 3);
	floaterSoldier->setValue(12);

	_aliens.insert(std::pair<std::string, RuleAlien*>("SECTOID_SOLDIER", sectoidSoldier));
	_aliens.insert(std::pair<std::string, RuleAlien*>("SECTOID_ENGINEER", sectoidEngineer));
	_aliens.insert(std::pair<std::string, RuleAlien*>("SECTOID_NAVIGATOR", sectoidNavigator));
	_aliens.insert(std::pair<std::string, RuleAlien*>("FLOATER_SOLDIER", floaterSoldier));

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
	
	
	ArticleDefinitionArmor article_armor;
	article_armor.section = UFOPAEDIA_WEAPONS_AND_EQUIPMENT;
	
	article_armor.id = "STR_PERSONAL_ARMOR";
	article_armor.title = "STR_PERSONAL_ARMOR_UC";
	article_armor.armor = _armors["STR_PERSONAL_ARMOR_UC"]; // FIXME: this is not good, using _UC terms as IDs! 
	article_armor.sort_key = sort_key++;
	
	_ufopaediaArticles[article_armor.id] = new ArticleDefinitionArmor(article_armor);
	
	
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
	skyranger->getItems()->addItem("STR_ELECTRO_FLARE", 4);
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
	save->getUfopaedia()->insertArticle(getUfopaediaArticle("STR_PERSONAL_ARMOR"));

	
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