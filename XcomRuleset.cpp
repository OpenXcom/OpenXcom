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
	skyranger->setMaxHealth(150);
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
	interceptor->setMaxHealth(100);
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
	save->getCountries()->insert(pair<LangString, Country*>(STR_USA, new Country(RNG::generate(600, 1200)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_RUSSIA, new Country(RNG::generate(230, 460)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_UK, new Country(RNG::generate(240, 480)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_FRANCE, new Country(RNG::generate(320, 640)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_GERMANY, new Country(RNG::generate(250, 500)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_ITALY, new Country(RNG::generate(160, 320)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_SPAIN, new Country(RNG::generate(140, 280)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_CHINA, new Country(RNG::generate(245, 490)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_JAPAN, new Country(RNG::generate(400, 800)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_INDIA, new Country(RNG::generate(150, 300)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_BRAZIL, new Country(RNG::generate(300, 600)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_AUSTRALIA, new Country(RNG::generate(280, 560)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_NIGERIA, new Country(RNG::generate(180, 360)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_SOUTH_AFRICA, new Country(RNG::generate(310, 620)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_EGYPT, new Country(RNG::generate(160, 320)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_CANADA, new Country(RNG::generate(110, 220)*1000)));
	save->setFunds(save->getCountryFunding());
	
	// Set up craft IDs
	save->getCraftIds()->insert(pair<LangString, int>(STR_SKYRANGER, 1));
	save->getCraftIds()->insert(pair<LangString, int>(STR_LIGHTNING, 1));
	save->getCraftIds()->insert(pair<LangString, int>(STR_AVENGER, 1));
	save->getCraftIds()->insert(pair<LangString, int>(STR_INTERCEPTOR, 1));
	save->getCraftIds()->insert(pair<LangString, int>(STR_FIRESTORM, 1));

	// Set up initial base
	Base *base = new Base(0.0, 0.0);
	base->setName("X-COM BASE 1");
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
	base->getCrafts()->push_back(skyranger);
	base->getCrafts()->push_back(new Craft(getCraft(STR_INTERCEPTOR), save->getCraftIds(), 0.0, 0.0));
	base->getCrafts()->push_back(new Craft(getCraft(STR_INTERCEPTOR), save->getCraftIds(), 0.0, 0.0));

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