/*
 * Copyright 2010 Daniel Albano
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
#include "XcomRuleset.h"
#include "SavedGame.h"
#include "RuleBaseFacility.h"
#include "RuleCraft.h"
#include "SoldierNamePool.h"
#include "Region.h"
#include "RNG.h"
#include "LangString.h"
#include "Base.h"
#include "Country.h"
#include "BaseFacility.h"
#include "Craft.h"
#include "Soldier.h"

/**
 * Initializes the X-Com ruleset with all the rules
 * mimicking the original game.
 */
XcomRuleset::XcomRuleset() : Ruleset()
{
	// Add soldier names
	SoldierNamePool *american = new SoldierNamePool();
	american->addMaleName("Austin");
	american->addMaleName("Calvin");
	american->addMaleName("Carl");
	american->addMaleName("Clarence");
	american->addMaleName("Donald");
	american->addMaleName("Dwight");
	american->addMaleName("Ed");
	american->addMaleName("Kevin");
	american->addMaleName("Lester");
	american->addMaleName("Mark");
	american->addMaleName("Oscar");
	american->addMaleName("Samuel");
	american->addMaleName("Spencer");
	american->addMaleName("Tom");
	american->addMaleName("Virgil");
	american->addFemaleName("Barbara");
	american->addFemaleName("Catherine");
	american->addFemaleName("Evelyn");
	american->addFemaleName("Patricia");
	american->addFemaleName("Sigourney");
	american->addLastName("Bradley");
	american->addLastName("Bryant");
	american->addLastName("Carr");
	american->addLastName("Crossett");
	american->addLastName("Dodge");
	american->addLastName("Gallagher");
	american->addLastName("Homburger");
	american->addLastName("Horton");
	american->addLastName("Hudson");
	american->addLastName("Johnson");
	american->addLastName("Kemp");
	american->addLastName("King");
	american->addLastName("McNeil");
	american->addLastName("Miller");
	american->addLastName("Mitchell");
	american->addLastName("Nash");
	american->addLastName("Stephens");
	american->addLastName("Stoddard");
	american->addLastName("Thompson");
	american->addLastName("Webb");
	_names.push_back(american);

	SoldierNamePool *british = new SoldierNamePool();
	british->addMaleName("Adam");
	british->addMaleName("Alan");
	british->addMaleName("Arthur");
	british->addMaleName("Brett");
	british->addMaleName("Damien");
	british->addMaleName("David");
	british->addMaleName("Frank");
	british->addMaleName("James");
	british->addMaleName("John");
	british->addMaleName("Michael");
	british->addMaleName("Neil");
	british->addMaleName("Patrick");
	british->addMaleName("Paul");
	british->addMaleName("Robert");
	british->addMaleName("Scott");
	british->addFemaleName("Andrea");
	british->addFemaleName("Helen");
	british->addFemaleName("Jane");
	british->addFemaleName("Maria");
	british->addFemaleName("Sarah");
	british->addLastName("Bailey");
	british->addLastName("Blake");
	british->addLastName("Davies");
	british->addLastName("Day");
	british->addLastName("Evans");
	british->addLastName("Hill");
	british->addLastName("Jones");
	british->addLastName("Jonlan");
	british->addLastName("Martin");
	british->addLastName("Parker");
	british->addLastName("Pearce");
	british->addLastName("Reynolds");
	british->addLastName("Robinson");
	british->addLastName("Sharpe");
	british->addLastName("Smith");
	british->addLastName("Stewart");
	british->addLastName("Taylor");
	british->addLastName("Watson");
	british->addLastName("White");
	british->addLastName("Wright");
	_names.push_back(british);

	SoldierNamePool *french = new SoldierNamePool();
	french->addMaleName("Armand");
	french->addMaleName("Bernard");
	french->addMaleName("Claude");
	french->addMaleName("Emile");
	french->addMaleName("Gaston");
	french->addMaleName("Gerard");
	french->addMaleName("Henri");
	french->addMaleName("Jacques");
	french->addMaleName("Jean");
	french->addMaleName("Leon");
	french->addMaleName("Louis");
	french->addMaleName("Marc");
	french->addMaleName("Marcel");
	french->addMaleName("Pierre");
	french->addMaleName("Rene");
	french->addFemaleName("Danielle");
	french->addFemaleName("Jacqueline");
	french->addFemaleName("Marielle");
	french->addFemaleName("Micheline");
	french->addFemaleName("Sylvie");
	french->addLastName("Bouissou");
	french->addLastName("Bouton");
	french->addLastName("Buchard");
	french->addLastName("Coicard");
	french->addLastName("Collignon");
	french->addLastName("Cuvelier");
	french->addLastName("Dagallier");
	french->addLastName("Dreyfus");
	french->addLastName("Dujardin");
	french->addLastName("Gaudin");
	french->addLastName("Gautier");
	french->addLastName("Gressier");
	french->addLastName("Guerin");
	french->addLastName("Laroyenne");
	french->addLastName("Lecointe");
	french->addLastName("Lefevre");
	french->addLastName("Luget");
	french->addLastName("Marcelle");
	french->addLastName("Pecheux");
	french->addLastName("Revenu");
	_names.push_back(french);

	SoldierNamePool *german = new SoldierNamePool();
	german->addMaleName("Dieter");
	german->addMaleName("Franz");
	german->addMaleName("Gerhard");
	german->addMaleName("Gunter");
	german->addMaleName("Hans");
	german->addMaleName("Jurgen");
	german->addMaleName("Klaus");
	german->addMaleName("Manfred");
	german->addMaleName("Matthias");
	german->addMaleName("Otto");
	german->addMaleName("Rudi");
	german->addMaleName("Siegfried");
	german->addMaleName("Stefan");
	german->addMaleName("Werner");
	german->addMaleName("Wolfgang");
	german->addFemaleName("Christel");
	german->addFemaleName("Gudrun");
	german->addFemaleName("Helga");
	german->addFemaleName("Karin");
	german->addFemaleName("Uta");
	german->addLastName("Berger");
	german->addLastName("Brehme");
	german->addLastName("Esser");
	german->addLastName("Faerber");
	german->addLastName("Geisler");
	german->addLastName("Gunkel");
	german->addLastName("Hafner");
	german->addLastName("Heinsch");
	german->addLastName("Keller");
	german->addLastName("Krause");
	german->addLastName("Mederow");
	german->addLastName("Meyer");
	german->addLastName("Richter");
	german->addLastName("Schultz");
	german->addLastName("Seidler");
	german->addLastName("Steinbach");
	german->addLastName("Ulbricht");
	german->addLastName("Unger");
	german->addLastName("Vogel");
	german->addLastName("Zander");
	_names.push_back(german);

	SoldierNamePool *japanese = new SoldierNamePool();
	japanese->addMaleName("Akinori");
	japanese->addMaleName("Isao");
	japanese->addMaleName("Jungo");
	japanese->addMaleName("Kenji");
	japanese->addMaleName("Masaharu");
	japanese->addMaleName("Masanori");
	japanese->addMaleName("Naohiro");
	japanese->addMaleName("Shigeo");
	japanese->addMaleName("Shigeru");
	japanese->addMaleName("Shuji");
	japanese->addMaleName("Tatsuo");
	japanese->addMaleName("Toshio");
	japanese->addMaleName("Yasuaki");
	japanese->addMaleName("Yataka");
	japanese->addMaleName("Yuzo");
	japanese->addFemaleName("Mariko");
	japanese->addFemaleName("Michiko");
	japanese->addFemaleName("Sata");
	japanese->addFemaleName("Sumie");
	japanese->addFemaleName("Yoko");
	japanese->addLastName("Akira");
	japanese->addLastName("Fujimoto");
	japanese->addLastName("Ishii");
	japanese->addLastName("Iwahara");
	japanese->addLastName("Iwasaki");
	japanese->addLastName("Kojima");
	japanese->addLastName("Koyama");
	japanese->addLastName("Matsumara");
	japanese->addLastName("Morita");
	japanese->addLastName("Noguchi");
	japanese->addLastName("Okabe");
	japanese->addLastName("Okamoto");
	japanese->addLastName("Sato");
	japanese->addLastName("Shimaoka");
	japanese->addLastName("Shoji");
	japanese->addLastName("Tanida");
	japanese->addLastName("Tanikawa");
	japanese->addLastName("Yamanaka");
	japanese->addLastName("Yamashita");
	japanese->addLastName("Yamazaki");
	_names.push_back(japanese);

	SoldierNamePool *russian = new SoldierNamePool();
	russian->addMaleName("Anatoly");
	russian->addMaleName("Andrei");
	russian->addMaleName("Boris");
	russian->addMaleName("Dmitriy");
	russian->addMaleName("Gennadi");
	russian->addMaleName("Grigoriy");
	russian->addMaleName("Igor");
	russian->addMaleName("Ivan");
	russian->addMaleName("Leonid");
	russian->addMaleName("Mikhail");
	russian->addMaleName("Nikolai");
	russian->addMaleName("Sergei");
	russian->addMaleName("Victor");
	russian->addMaleName("Vladimir");
	russian->addMaleName("Yuri");
	russian->addFemaleName("Astra");
	russian->addFemaleName("Galina");
	russian->addFemaleName("Lyudmila");
	russian->addFemaleName("Olga");
	russian->addFemaleName("Tatyana");
	russian->addLastName("Andianov");
	russian->addLastName("Belov");
	russian->addLastName("Chukarin");
	russian->addLastName("Gorokhova");
	russian->addLastName("Kolotov");
	russian->addLastName("Korkia");
	russian->addLastName("Likhachev");
	russian->addLastName("Maleev");
	russian->addLastName("Mikhailov");
	russian->addLastName("Petrov");
	russian->addLastName("Ragulin");
	russian->addLastName("Romanov");
	russian->addLastName("Samusenko");
	russian->addLastName("Scharov");
	russian->addLastName("Shadrin");
	russian->addLastName("Shalimov");
	russian->addLastName("Torban");
	russian->addLastName("Voronin");
	russian->addLastName("Yakubik");
	russian->addLastName("Zhdanovich");
	_names.push_back(russian);

	// Add base facilities
	RuleBaseFacility *lift = new RuleBaseFacility(STR_ACCESS_LIFT);
	lift->setSpriteShape(2);
	lift->setSpriteFacility(17);
	lift->setBuildCost(300000);
	lift->setBuildTime(1);
	lift->setMonthlyCost(4000);
	lift->setLift(true);

	RuleBaseFacility *quarters = new RuleBaseFacility(STR_LIVING_QUARTERS);
	quarters->setSpriteShape(1);
	quarters->setSpriteFacility(18);
	quarters->setBuildCost(400000);
	quarters->setBuildTime(16);
	quarters->setMonthlyCost(10000);
	quarters->setPersonnel(50);

	RuleBaseFacility *lab = new RuleBaseFacility(STR_LABORATORY);
	lab->setSpriteShape(1);
	lab->setSpriteFacility(19);
	lab->setBuildCost(750000);
	lab->setBuildTime(26);
	lab->setMonthlyCost(30000);
	lab->setLaboratories(50);

	RuleBaseFacility *workshop = new RuleBaseFacility(STR_WORKSHOP);
	workshop->setSpriteShape(1);
	workshop->setSpriteFacility(20);
	workshop->setBuildCost(800000);
	workshop->setBuildTime(32);
	workshop->setMonthlyCost(35000);
	workshop->setWorkshops(50);

	RuleBaseFacility *stores = new RuleBaseFacility(STR_GENERAL_STORES);
	stores->setSpriteShape(1);
	stores->setSpriteFacility(24);
	stores->setBuildCost(150000);
	stores->setBuildTime(10);
	stores->setMonthlyCost(5000);
	stores->setStorage(50);

	RuleBaseFacility *aliens = new RuleBaseFacility(STR_ALIEN_CONTAINMENT);
	aliens->setSpriteShape(1);
	aliens->setSpriteFacility(25);
	aliens->setBuildCost(400000);
	aliens->setBuildTime(18);
	aliens->setMonthlyCost(15000);
	aliens->setAliens(10);

	RuleBaseFacility *hangar = new RuleBaseFacility(STR_HANGAR);
	hangar->setSpriteShape(9);
	hangar->setSpriteFacility(9);
	hangar->setSize(2);
	hangar->setBuildCost(200000);
	hangar->setBuildTime(25);
	hangar->setMonthlyCost(25000);
	hangar->setCrafts(1);

	RuleBaseFacility *sRadar = new RuleBaseFacility(STR_SMALL_RADAR_SYSTEM);
	sRadar->setSpriteShape(2);
	sRadar->setSpriteFacility(21);
	sRadar->setBuildCost(500000);
	sRadar->setBuildTime(12);
	sRadar->setMonthlyCost(10000);
	sRadar->setRadarRange(2000);
	sRadar->setRadarChance(10);

	RuleBaseFacility *lRadar = new RuleBaseFacility(STR_LARGE_RADAR_SYSTEM);
	lRadar->setSpriteShape(1);
	lRadar->setSpriteFacility(22);
	lRadar->setBuildCost(800000);
	lRadar->setBuildTime(25);
	lRadar->setMonthlyCost(15000);
	lRadar->setRadarRange(3000);
	lRadar->setRadarChance(20);

	RuleBaseFacility *missile = new RuleBaseFacility(STR_MISSILE_DEFENCES);
	missile->setSpriteShape(2);
	missile->setSpriteFacility(23);
	missile->setBuildCost(200000);
	missile->setBuildTime(16);
	missile->setMonthlyCost(5000);
	missile->setDefenceValue(500);
	missile->setHitRatio(50);

	_facilities.insert(pair<LangString, RuleBaseFacility*>(STR_ACCESS_LIFT, lift));
	_facilities.insert(pair<LangString, RuleBaseFacility*>(STR_LIVING_QUARTERS, quarters));
	_facilities.insert(pair<LangString, RuleBaseFacility*>(STR_LABORATORY, lab));
	_facilities.insert(pair<LangString, RuleBaseFacility*>(STR_WORKSHOP, workshop));
	_facilities.insert(pair<LangString, RuleBaseFacility*>(STR_GENERAL_STORES, stores));
	_facilities.insert(pair<LangString, RuleBaseFacility*>(STR_ALIEN_CONTAINMENT, aliens));
	_facilities.insert(pair<LangString, RuleBaseFacility*>(STR_HANGAR, hangar));
	_facilities.insert(pair<LangString, RuleBaseFacility*>(STR_SMALL_RADAR_SYSTEM, sRadar));
	_facilities.insert(pair<LangString, RuleBaseFacility*>(STR_LARGE_RADAR_SYSTEM, lRadar));
	_facilities.insert(pair<LangString, RuleBaseFacility*>(STR_MISSILE_DEFENCES, missile));

	// Add crafts
	RuleCraft* skyranger = new RuleCraft(STR_SKYRANGER);
	skyranger->setSprite(33);
	skyranger->setSpeed(760);
	skyranger->setAcceleration(2);
	skyranger->setMaxFuel(1500);
	skyranger->setWeapons(0);
	skyranger->setMaxDamage(150);
	skyranger->setSoldiers(14);
	skyranger->setMonthlyFee(500000);
	skyranger->setHWPs(3);

	RuleCraft* lightning = new RuleCraft(STR_LIGHTNING);
	lightning->setSprite(34);

	RuleCraft* avenger = new RuleCraft(STR_AVENGER);
	avenger->setSprite(35);

	RuleCraft* interceptor = new RuleCraft(STR_INTERCEPTOR);
	interceptor->setSprite(36);
	interceptor->setSpeed(2100);
	interceptor->setAcceleration(3);
	interceptor->setMaxFuel(1000);
	interceptor->setWeapons(2);
	interceptor->setMaxDamage(100);
	interceptor->setSoldiers(0);
	interceptor->setMonthlyFee(600000);
	interceptor->setHWPs(0);

	RuleCraft* firestorm = new RuleCraft(STR_FIRESTORM);
	firestorm->setSprite(37);

	_crafts.insert(pair<LangString, RuleCraft*>(STR_SKYRANGER, skyranger));
	_crafts.insert(pair<LangString, RuleCraft*>(STR_LIGHTNING, lightning));
	_crafts.insert(pair<LangString, RuleCraft*>(STR_AVENGER, avenger));
	_crafts.insert(pair<LangString, RuleCraft*>(STR_INTERCEPTOR, interceptor));
	_crafts.insert(pair<LangString, RuleCraft*>(STR_FIRESTORM, firestorm));
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
SavedGame *XcomRuleset::newSave(GameDifficulty diff)
{
	SavedGame *save = new SavedGame(diff);

	// Generate countries
	Country *usa = new Country(RNG::generate(600, 1200)*1000);
	usa->setLabelLongitude(4.53786);
	usa->setLabelLatitude(-0.698132);

	Country *russia = new Country(RNG::generate(230, 460)*1000);
	russia->setLabelLongitude(1.0472);
	russia->setLabelLatitude(-1.0472);

	Country *uk = new Country(RNG::generate(240, 480)*1000);
	uk->setLabelLongitude(6.24828);
	uk->setLabelLatitude(-0.935933);

	Country *france = new Country(RNG::generate(320, 640)*1000);
	france->setLabelLongitude(0.0436332);
	france->setLabelLatitude(-0.811578);

	Country *germany = new Country(RNG::generate(250, 500)*1000);
	germany->setLabelLongitude(0.200713);
	germany->setLabelLatitude(-0.872665);

	Country *italy = new Country(RNG::generate(160, 320)*1000);
	italy->setLabelLongitude(0.218166);
	italy->setLabelLatitude(-0.765763);

	Country *spain = new Country(RNG::generate(140, 280)*1000);
	spain->setLabelLongitude(6.23955);
	spain->setLabelLatitude(-0.743947);

	Country *china = new Country(RNG::generate(245, 490)*1000);
	china->setLabelLongitude(1.74533);
	china->setLabelLatitude(-0.610865);

	Country *japan = new Country(RNG::generate(400, 800)*1000);
	japan->setLabelLongitude(2.40855);
	japan->setLabelLatitude(-0.667588);

	Country *india = new Country(RNG::generate(150, 300)*1000);
	india->setLabelLongitude(1.39626);
	india->setLabelLatitude(-0.418879);

	Country *brazil = new Country(RNG::generate(300, 600)*1000);
	brazil->setLabelLongitude(5.32325);
	brazil->setLabelLatitude(0.0872665);

	Country *australia = new Country(RNG::generate(280, 560)*1000);
	australia->setLabelLongitude(2.35619);
	australia->setLabelLatitude(0.436332);

	Country *nigeria = new Country(RNG::generate(180, 360)*1000);
	nigeria->setLabelLongitude(0.1309);
	nigeria->setLabelLatitude(-0.174533);

	Country *africa = new Country(RNG::generate(310, 620)*1000);
	africa->setLabelLongitude(0.436332);
	africa->setLabelLatitude(0.523599);

	Country *egypt = new Country(RNG::generate(160, 320)*1000);
	egypt->setLabelLongitude(0.506145);
	egypt->setLabelLatitude(-0.453786);

	Country *canada = new Country(RNG::generate(110, 220)*1000);
	canada->setLabelLongitude(4.53786);
	canada->setLabelLatitude(-0.959931);

	save->getCountries()->insert(pair<LangString, Country*>(STR_USA, usa));
	save->getCountries()->insert(pair<LangString, Country*>(STR_RUSSIA, russia));
	save->getCountries()->insert(pair<LangString, Country*>(STR_UK, uk));
	save->getCountries()->insert(pair<LangString, Country*>(STR_FRANCE, france));
	save->getCountries()->insert(pair<LangString, Country*>(STR_GERMANY, germany));
	save->getCountries()->insert(pair<LangString, Country*>(STR_ITALY, italy));
	save->getCountries()->insert(pair<LangString, Country*>(STR_SPAIN, spain));
	save->getCountries()->insert(pair<LangString, Country*>(STR_CHINA, china));
	save->getCountries()->insert(pair<LangString, Country*>(STR_JAPAN, japan));
	save->getCountries()->insert(pair<LangString, Country*>(STR_INDIA, india));
	save->getCountries()->insert(pair<LangString, Country*>(STR_BRAZIL, brazil));
	save->getCountries()->insert(pair<LangString, Country*>(STR_AUSTRALIA, australia));
	save->getCountries()->insert(pair<LangString, Country*>(STR_NIGERIA, nigeria));
	save->getCountries()->insert(pair<LangString, Country*>(STR_SOUTH_AFRICA, africa));
	save->getCountries()->insert(pair<LangString, Country*>(STR_EGYPT, egypt));
	save->getCountries()->insert(pair<LangString, Country*>(STR_CANADA, canada));
	save->setFunds(save->getCountryFunding());

	// Add regions
	Region* namerica = new Region(800000);
	namerica->addArea(3.40339, 5.32107, -1.22173, -0.962113);
	namerica->addArea(4.01426, 5.32107, -0.959931, -0.52578);
	namerica->addArea(4.18879, 5.23381, -0.523599, -0.176715);
	
	Region* arctic = new Region(950000);
	arctic->addArea(0, 6.281, -1.5708, -1.22391);
	
	Region* antarctica = new Region(900000);
	antarctica->addArea(0, 6.281, 1.0472, 1.5708);
	
	Region* samerica = new Region(600000);
	samerica->addArea(4.71239, 5.49561, -0.174533, -0.00218166);
	samerica->addArea(4.79966, 5.7574, 0, 0.259618);
	samerica->addArea(4.79966, 5.67014, 0.261799, 1.04502);
	
	Region* europe = new Region(1000000);
	europe->addArea(5.84685, 1.04502, -1.22173, -0.613047);
	
	Region* nafrica = new Region(650000);
	nafrica->addArea(5.84685, 0.69595, -0.610865, -0.263981);
	nafrica->addArea(5.84685, 0.957749, -0.261799, -0.00218166);
	
	Region* safrica = new Region(550000);
	safrica->addArea(0.0872665, 0.957749, 0, 0.69595);
	
	Region* casia = new Region(500000);
	casia->addArea(0.698132, 1.21955, -0.610865, -0.263981);
	casia->addArea(1.0472, 1.56861, -0.872665, -0.613047);
	casia->addArea(1.22173, 1.56861, -0.610865, -0.0894481);
	
	Region* seasia = new Region(750000);
	seasia->addArea(1.5708, 1.83041, -0.872665, 0.172351);
	seasia->addArea(1.8326, 2.61581, -0.872665, -0.0894481);
	
	Region* siberia = new Region(800000);
	siberia->addArea(1.0472, 3.13941, -1.22173, -0.874846);
	
	Region* australasia = new Region(750000);
	//australasia->addArea(1.8326, 3.13941, -0.0872665, 0.870483);
	australasia->addArea(1.8326, 3.13941, -0.0872665, 1.04502);
	
	Region* pacific = new Region(600000);
	pacific->addArea(3.14159, 3.40121, -1.22173, -0.962113);
	pacific->addArea(3.14159, 4.01208, -0.959931, -0.52578);
	pacific->addArea(3.14159, 4.18661, -0.523599, -0.176715);
	pacific->addArea(3.14159, 4.71021, -0.174533, -0.00218166);
	pacific->addArea(3.14159, 4.79747, 0, 1.04502);
	pacific->addArea(2.61799, 3.13941, -0.872665, -0.0894481);
	
	Region* natlantic = new Region(500000);
	natlantic->addArea(5.32325, 5.84467, -1.22173, -0.52578);
	natlantic->addArea(5.23599, 5.84467, -0.523599, -0.176715);
	natlantic->addArea(5.49779, 5.84467, -0.174533, -0.00218166);
	
	Region* satlantic = new Region(500000);
	satlantic->addArea(5.75959, 0.0850848, 0, 0.259618);
	satlantic->addArea(5.67232, 0.0850848, 0.261799, 1.04502);
	satlantic->addArea(0.0872665, 0.959931, 0.698132, 1.04502);
	
	Region* indian = new Region(500000);
	indian->addArea(0.959931, 1.21955, -0.261799, 0.172351);
	indian->addArea(1.22173, 1.56861, -0.0872665, 0.172351);
	indian->addArea(0.959931, 1.83041, 0.174533, 1.04502);
	
	save->getRegions()->insert(pair<LangString, Region*>(STR_NORTH_AMERICA, namerica));
	save->getRegions()->insert(pair<LangString, Region*>(STR_ARCTIC, arctic));
	save->getRegions()->insert(pair<LangString, Region*>(STR_ANTARCTICA, antarctica));
	save->getRegions()->insert(pair<LangString, Region*>(STR_SOUTH_AMERICA, samerica));
	save->getRegions()->insert(pair<LangString, Region*>(STR_EUROPE, europe));
	save->getRegions()->insert(pair<LangString, Region*>(STR_NORTH_AFRICA, nafrica));
	save->getRegions()->insert(pair<LangString, Region*>(STR_SOUTHERN_AFRICA, safrica));
	save->getRegions()->insert(pair<LangString, Region*>(STR_CENTRAL_ASIA, casia));
	save->getRegions()->insert(pair<LangString, Region*>(STR_SOUTH_EAST_ASIA, seasia));
	save->getRegions()->insert(pair<LangString, Region*>(STR_SIBERIA, siberia));
	save->getRegions()->insert(pair<LangString, Region*>(STR_AUSTRALASIA, australasia));
	save->getRegions()->insert(pair<LangString, Region*>(STR_PACIFIC, pacific));
	save->getRegions()->insert(pair<LangString, Region*>(STR_NORTH_ATLANTIC, natlantic));
	save->getRegions()->insert(pair<LangString, Region*>(STR_SOUTH_ATLANTIC, satlantic));
	save->getRegions()->insert(pair<LangString, Region*>(STR_INDIAN_OCEAN, indian));
	
	// Set up craft IDs
	save->getCraftIds()->insert(pair<LangString, int>(STR_SKYRANGER, 1));
	save->getCraftIds()->insert(pair<LangString, int>(STR_LIGHTNING, 1));
	save->getCraftIds()->insert(pair<LangString, int>(STR_AVENGER, 1));
	save->getCraftIds()->insert(pair<LangString, int>(STR_INTERCEPTOR, 1));
	save->getCraftIds()->insert(pair<LangString, int>(STR_FIRESTORM, 1));

	// Set up initial base
	Base *base = new Base();
	base->setEngineers(10);
	base->setScientists(10);

	// Add facilities
	base->getFacilities()->push_back(new BaseFacility(getBaseFacility(STR_ACCESS_LIFT), 2, 2));
	base->getFacilities()->push_back(new BaseFacility(getBaseFacility(STR_HANGAR), 2, 0));
	base->getFacilities()->push_back(new BaseFacility(getBaseFacility(STR_HANGAR), 0, 4));
	base->getFacilities()->push_back(new BaseFacility(getBaseFacility(STR_HANGAR), 4, 4));
	base->getFacilities()->push_back(new BaseFacility(getBaseFacility(STR_LIVING_QUARTERS), 3, 2));
	base->getFacilities()->push_back(new BaseFacility(getBaseFacility(STR_GENERAL_STORES), 2, 3));
	base->getFacilities()->push_back(new BaseFacility(getBaseFacility(STR_LABORATORY), 3, 3));
	base->getFacilities()->push_back(new BaseFacility(getBaseFacility(STR_WORKSHOP), 4, 3));
	base->getFacilities()->push_back(new BaseFacility(getBaseFacility(STR_SMALL_RADAR_SYSTEM), 1, 3));

	// Add crafts
	Craft *skyranger = new Craft(getCraft(STR_SKYRANGER), save->getCraftIds(), 0.0, 0.0);
	skyranger->setFuel(skyranger->getRules()->getMaxFuel());

	Craft *interceptor1 = new Craft(getCraft(STR_INTERCEPTOR), save->getCraftIds(), 0.0, 0.0);
	interceptor1->setFuel(interceptor1->getRules()->getMaxFuel());

	Craft *interceptor2 = new Craft(getCraft(STR_INTERCEPTOR), save->getCraftIds(), 0.0, 0.0);
	interceptor2->setFuel(interceptor2->getRules()->getMaxFuel());

	base->getCrafts()->push_back(skyranger);
	base->getCrafts()->push_back(interceptor1);
	base->getCrafts()->push_back(interceptor2);

	// Generate soldiers
	for (int i = 0; i < 8; i++)
	{
		Soldier *soldier = new Soldier(&_names);
		soldier->setCraft(skyranger);
		base->getSoldiers()->push_back(soldier);
	}

	save->getBases()->push_back(base);
	
	return save;
}
