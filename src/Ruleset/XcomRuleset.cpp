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
#include "Unit.h"
#include "Armor.h"
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
#include "AlienRace.h"
#include "AlienDeployment.h"
#include "../Engine/Options.h"

namespace OpenXcom
{
/**
 * Initializes the X-Com ruleset with all the rules
 * mimicking the original game.
 */
XcomRuleset::XcomRuleset() : Ruleset()
{
	load("Xcom1Ruleset");

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

	// Add manufactures
	RuleManufactureInfo *mlp = new RuleManufactureInfo("STR_LASER_PISTOL");
	mlp->setCategory("STR_WEAPON");
	mlp->setRequiredSpace(2);
	mlp->setManufactureTime(300);
	mlp->setManufactureCost(8000);

	RuleManufactureInfo *maa = new RuleManufactureInfo("STR_ALIEN_ALLOYS");
	maa->setCategory("STR_EQUIPMENT");
	maa->setRequiredSpace(2);
	maa->setManufactureTime(300);
	maa->setManufactureCost(8000);

	RuleManufactureInfo *mpa = new RuleManufactureInfo("STR_PERSONAL_ARMOR");
	mpa->setCategory("STR_EQUIPMENT");
	mpa->setRequiredSpace(2);
	mpa->setManufactureTime(300);
	mpa->setManufactureCost(8000);


	RuleManufactureInfo *mlr = new RuleManufactureInfo("STR_LASER_RIFLE");
	mlr->setCategory("STR_WEAPON");
	mlr->setRequiredSpace(2);
	mlr->setManufactureTime(300);
	mlr->setManufactureCost(8000000);

	RuleManufactureInfo *mdd = new RuleManufactureInfo("STR_MEDI_KIT");
	mdd->setCategory("STR_EQUIPMENT");
	mdd->setRequiredSpace(2);
	mdd->setManufactureTime(420);
	mdd->setManufactureCost(28000);

	RuleManufactureInfo *msc = new RuleManufactureInfo("STR_MOTION_SCANNER");
	msc->setCategory("STR_EQUIPMENT");
	msc->setRequiredSpace(2);
	msc->setManufactureTime(440);
	msc->setManufactureCost(34000);

	_manufacture.insert(std::pair<std::string, RuleManufactureInfo*>("STR_ALIEN_ALLOYS", maa));
	_manufacture.insert(std::pair<std::string, RuleManufactureInfo*>("STR_LASER_PISTOL", mlp));
	_manufacture.insert(std::pair<std::string, RuleManufactureInfo*>("STR_PERSONAL_ARMOR", mpa));
	_manufacture.insert(std::pair<std::string, RuleManufactureInfo*>("STR_LASER_RIFLE", mlr));
	_manufacture.insert(std::pair<std::string, RuleManufactureInfo*>("STR_MEDI_KIT", mdd));
	_manufacture.insert(std::pair<std::string, RuleManufactureInfo*>("STR_MOTION_SCANNER", msc));

	/* deployment data */
	/* items that not yet exist in the ruleset are put in comment */
	AlienDeployment *asscout = new AlienDeployment("STR_SMALL_SCOUT");
	asscout->setDimensions(40, 40, 4);
	DeploymentData dd;
	dd.alienRank = AR_SOLDIER;
	dd.lowQty = 1;
	dd.highQty = 1;
	dd.dQty = 0;
	dd.percentageOutsideUFO = 50;
	ItemSet is;
	is.items.push_back("STR_PLASMA_PISTOL");
	is.items.push_back("STR_PLASMA_PISTOL_CLIP");
	is.items.push_back("STR_PLASMA_PISTOL_CLIP");
	is.items.push_back("STR_MIND_PROBE");
	dd.itemSets.push_back(is);
	is.items.clear();
	is.items.push_back("STR_PLASMA_RIFLE");
	is.items.push_back("STR_PLASMA_RIFLE_CLIP");
	is.items.push_back("STR_PLASMA_RIFLE_CLIP");
	is.items.push_back("STR_MIND_PROBE");
	dd.itemSets.push_back(is);
	is.items.clear();
	is.items.push_back("STR_HEAVY_PLASMA");
	is.items.push_back("STR_HEAVY_PLASMA_CLIP");
	is.items.push_back("STR_MIND_PROBE");
	dd.itemSets.push_back(is);
	asscout->getDeploymentData()->push_back(dd);

	AlienDeployment *amscout = new AlienDeployment("STR_MEDIUM_SCOUT");
	amscout->setDimensions(40, 40, 4);
	dd.alienRank = AR_SOLDIER;
	dd.lowQty = 2;
	dd.highQty = 4;
	dd.dQty = 2;
	dd.percentageOutsideUFO = 60;
	dd.itemSets.clear();
	is.items.clear();
	is.items.push_back("STR_PLASMA_PISTOL");
	is.items.push_back("STR_PLASMA_PISTOL_CLIP");
	is.items.push_back("STR_PLASMA_PISTOL_CLIP");
	dd.itemSets.push_back(is);
	is.items.clear();
	is.items.push_back("STR_PLASMA_RIFLE");
	is.items.push_back("STR_PLASMA_RIFLE_CLIP");
	is.items.push_back("STR_PLASMA_RIFLE_CLIP");
	dd.itemSets.push_back(is);
	is.items.clear();
	is.items.push_back("STR_HEAVY_PLASMA");
	is.items.push_back("STR_HEAVY_PLASMA_CLIP");
	is.items.push_back("STR_ALIEN_GRENADE");
	dd.itemSets.push_back(is);
	amscout->getDeploymentData()->push_back(dd);

	dd.alienRank = AR_NAVIGATOR;
	dd.lowQty = 1;
	dd.highQty = 2;
	dd.dQty = 1;
	dd.percentageOutsideUFO = 20;
	dd.itemSets.clear();
	is.items.clear();
	is.items.push_back("STR_PLASMA_PISTOL");
	is.items.push_back("STR_PLASMA_PISTOL_CLIP");
	is.items.push_back("STR_MIND_PROBE");
	dd.itemSets.push_back(is);
	is.items.clear();
	is.items.push_back("STR_PLASMA_RIFLE");
	is.items.push_back("STR_PLASMA_RIFLE_CLIP");
	dd.itemSets.push_back(is);
	is.items.clear();
	is.items.push_back("STR_HEAVY_PLASMA");
	is.items.push_back("STR_HEAVY_PLASMA_CLIP");
	dd.itemSets.push_back(is);
	amscout->getDeploymentData()->push_back(dd);

	AlienDeployment *alscout = new AlienDeployment("STR_LARGE_SCOUT");
	alscout->setDimensions(50, 50, 4);
	dd.alienRank = AR_SOLDIER;
	dd.lowQty = 1;
	dd.highQty = 3;
	dd.dQty = 5;
	dd.percentageOutsideUFO = 65;
	dd.itemSets.clear();
	is.items.clear();
	is.items.push_back("STR_PLASMA_PISTOL");
	is.items.push_back("STR_PLASMA_PISTOL_CLIP");
	is.items.push_back("STR_PLASMA_PISTOL_CLIP");
	dd.itemSets.push_back(is);
	is.items.clear();
	is.items.push_back("STR_PLASMA_RIFLE");
	is.items.push_back("STR_PLASMA_RIFLE_CLIP");
	is.items.push_back("STR_PLASMA_RIFLE_CLIP");
	dd.itemSets.push_back(is);
	is.items.clear();
	is.items.push_back("STR_HEAVY_PLASMA");
	is.items.push_back("STR_HEAVY_PLASMA_CLIP");
	is.items.push_back("STR_HEAVY_PLASMA_CLIP");
	dd.itemSets.push_back(is);
	alscout->getDeploymentData()->push_back(dd);

	dd.alienRank = AR_NAVIGATOR;
	dd.lowQty = 1;
	dd.highQty = 1;
	dd.dQty = 1;
	dd.percentageOutsideUFO = 25;
	dd.itemSets.clear();
	is.items.clear();
	is.items.push_back("STR_PLASMA_PISTOL");
	is.items.push_back("STR_PLASMA_PISTOL_CLIP");
	is.items.push_back("STR_MIND_PROBE");
	dd.itemSets.push_back(is);
	is.items.clear();
	is.items.push_back("STR_PLASMA_RIFLE");
	is.items.push_back("STR_PLASMA_RIFLE_CLIP");
	is.items.push_back("STR_PLASMA_RIFLE_CLIP");
	dd.itemSets.push_back(is);
	is.items.clear();
	is.items.push_back("STR_HEAVY_PLASMA");
	is.items.push_back("STR_HEAVY_PLASMA_CLIP");
	is.items.push_back("STR_HEAVY_PLASMA_CLIP");
	is.items.push_back("STR_ALIEN_GRENADE");
	dd.itemSets.push_back(is);
	alscout->getDeploymentData()->push_back(dd);

	dd.alienRank = AR_ENGINEER;
	dd.lowQty = 1;
	dd.highQty = 2;
	dd.dQty = 1;
	dd.percentageOutsideUFO = 35;
	dd.itemSets.clear();
	is.items.clear();
	is.items.push_back("STR_PLASMA_PISTOL");
	is.items.push_back("STR_PLASMA_PISTOL_CLIP");
	is.items.push_back("STR_MIND_PROBE");
	dd.itemSets.push_back(is);
	is.items.clear();
	is.items.push_back("STR_PLASMA_RIFLE");
	is.items.push_back("STR_PLASMA_RIFLE_CLIP");
	is.items.push_back("STR_PLASMA_RIFLE_CLIP");
	is.items.push_back("STR_MIND_PROBE");
	dd.itemSets.push_back(is);
	is.items.clear();
	is.items.push_back("STR_SMALL_LAUNCHER");
	is.items.push_back("STR_STUN_MISSLE");
	is.items.push_back("STR_STUN_MISSLE");
	is.items.push_back("STR_STUN_MISSLE");
	dd.itemSets.push_back(is);
	alscout->getDeploymentData()->push_back(dd);

	AlienDeployment *aterror = new AlienDeployment("STR_TERROR_MISSION");
	aterror->setDimensions(50, 50, 4);
	dd.alienRank = AR_SOLDIER;
	dd.lowQty = 4;
	dd.highQty = 6;
	dd.dQty = 1;
	dd.percentageOutsideUFO = 0;
	dd.itemSets.clear();
	is.items.clear();
	is.items.push_back("STR_PLASMA_PISTOL");
	is.items.push_back("STR_PLASMA_PISTOL_CLIP");
	is.items.push_back("STR_ALIEN_GRENADE");
	dd.itemSets.push_back(is);
	aterror->getDeploymentData()->push_back(dd);

	dd.alienRank = AR_TERRORIST;
	dd.lowQty = 1;
	dd.highQty = 3;
	dd.dQty = 2;
	dd.percentageOutsideUFO = 0;
	dd.itemSets.clear();
	is.items.clear();
	dd.itemSets.push_back(is);
	aterror->getDeploymentData()->push_back(dd);

	AlienDeployment *basedef = new AlienDeployment("STR_BASE_DEFENCE");
	basedef->setDimensions(60, 60, 2);
	dd.alienRank = AR_SOLDIER;
	dd.lowQty = 4;
	dd.highQty = 8;
	dd.dQty = 4;
	dd.percentageOutsideUFO = 0;
	dd.itemSets.clear();
	is.items.clear();
	is.items.push_back("STR_PLASMA_PISTOL");
	is.items.push_back("STR_PLASMA_PISTOL_CLIP");
	is.items.push_back("STR_ALIEN_GRENADE");
	dd.itemSets.push_back(is);
	basedef->getDeploymentData()->push_back(dd);

	dd.alienRank = AR_TERRORIST;
	dd.lowQty = 1;
	dd.highQty = 3;
	dd.dQty = 2;
	dd.percentageOutsideUFO = 0;
	dd.itemSets.clear();
	is.items.clear();
	dd.itemSets.push_back(is);
	basedef->getDeploymentData()->push_back(dd);

	_alienDeployments.insert(std::pair<std::string, AlienDeployment*>("STR_SMALL_SCOUT", asscout));
	_alienDeployments.insert(std::pair<std::string, AlienDeployment*>("STR_MEDIUM_SCOUT", amscout));
	_alienDeployments.insert(std::pair<std::string, AlienDeployment*>("STR_LARGE_SCOUT", alscout));
	_alienDeployments.insert(std::pair<std::string, AlienDeployment*>("STR_TERROR_MISSION", aterror));
	_alienDeployments.insert(std::pair<std::string, AlienDeployment*>("STR_BASE_DEFENCE", basedef));

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

	// Add research
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
			++iter)
		{
			for(std::vector<std::string>::iterator itDep = iter->second.begin ();
				itDep != iter->second.end ();
				++itDep)
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
			++iter)
		{
			for(std::vector<std::string>::iterator itDep = iter->second.begin ();
				itDep != iter->second.end ();
				++itDep)
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
	base->getItems()->addItem("STR_ELECTRO_FLARE", 5);

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
