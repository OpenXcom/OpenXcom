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
#include "RuleCraftWeapon.h"
#include "RuleItem.h"
#include "RuleUfo.h"
#include "SoldierNamePool.h"
#include "Region.h"
#include "RNG.h"
#include "LangString.h"
#include "Base.h"
#include "Country.h"
#include "BaseFacility.h"
#include "Craft.h"
#include "CraftWeapon.h"
#include "Soldier.h"
#include "Item.h"
#include "City.h"

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
	sRadar->setRadarRange(1500);
	sRadar->setRadarChance(10);

	RuleBaseFacility *lRadar = new RuleBaseFacility(STR_LARGE_RADAR_SYSTEM);
	lRadar->setSpriteShape(1);
	lRadar->setSpriteFacility(22);
	lRadar->setBuildCost(800000);
	lRadar->setBuildTime(25);
	lRadar->setMonthlyCost(15000);
	lRadar->setRadarRange(2250);
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
	skyranger->setSprite(0);
	skyranger->setMaxSpeed(760);
	skyranger->setAcceleration(2);
	skyranger->setMaxFuel(1500);
	skyranger->setWeapons(0);
	skyranger->setMaxDamage(150);
	skyranger->setSoldiers(14);
	skyranger->setMonthlyFee(500000);
	skyranger->setHWPs(3);
	skyranger->setRefuelRate(50);

	RuleCraft* lightning = new RuleCraft(STR_LIGHTNING);
	lightning->setSprite(1);
	lightning->setMaxSpeed(3100);
	lightning->setAcceleration(9);
	lightning->setMaxFuel(30);
	lightning->setWeapons(1);
	lightning->setMaxDamage(800);
	lightning->setSoldiers(12);
	lightning->setHWPs(0);
	lightning->setRefuelRate(5);

	RuleCraft* avenger = new RuleCraft(STR_AVENGER);
	avenger->setSprite(2);
	avenger->setMaxSpeed(5400);
	avenger->setAcceleration(10);
	avenger->setMaxFuel(60);
	avenger->setWeapons(2);
	avenger->setMaxDamage(1200);
	avenger->setSoldiers(26);
	avenger->setHWPs(4);
	avenger->setRefuelRate(5);

	RuleCraft* interceptor = new RuleCraft(STR_INTERCEPTOR);
	interceptor->setSprite(3);
	interceptor->setMaxSpeed(2100);
	interceptor->setAcceleration(3);
	interceptor->setMaxFuel(1000);
	interceptor->setWeapons(2);
	interceptor->setMaxDamage(100);
	interceptor->setSoldiers(0);
	interceptor->setMonthlyFee(600000);
	interceptor->setHWPs(0);
	interceptor->setRefuelRate(50);

	RuleCraft* firestorm = new RuleCraft(STR_FIRESTORM);
	firestorm->setSprite(4);
	firestorm->setMaxSpeed(4200);
	firestorm->setAcceleration(9);
	firestorm->setMaxFuel(20);
	firestorm->setWeapons(2);
	firestorm->setMaxDamage(50);
	firestorm->setSoldiers(0);
	firestorm->setHWPs(0);
	firestorm->setRefuelRate(5);

	_crafts.insert(pair<LangString, RuleCraft*>(STR_SKYRANGER, skyranger));
	_crafts.insert(pair<LangString, RuleCraft*>(STR_LIGHTNING, lightning));
	_crafts.insert(pair<LangString, RuleCraft*>(STR_AVENGER, avenger));
	_crafts.insert(pair<LangString, RuleCraft*>(STR_INTERCEPTOR, interceptor));
	_crafts.insert(pair<LangString, RuleCraft*>(STR_FIRESTORM, firestorm));

	// Add craft weapons
	RuleCraftWeapon *stingray = new RuleCraftWeapon(STR_STINGRAY_UC);
	stingray->setSprite(0);
	stingray->setDamage(70);
	stingray->setRange(30);
	stingray->setAccuracy(70);
	stingray->setReloadTime(16);
	stingray->setAmmoMax(6);
	stingray->setLauncherItem(STR_STINGRAY_LAUNCHER);
	stingray->setClipItem(STR_STINGRAY_MISSILE);

	RuleCraftWeapon *avalanche = new RuleCraftWeapon(STR_AVALANCHE_UC);
	avalanche->setSprite(1);
	avalanche->setDamage(100);
	avalanche->setRange(60);
	avalanche->setAccuracy(80);
	avalanche->setReloadTime(24);
	avalanche->setAmmoMax(3);
	avalanche->setLauncherItem(STR_AVALANCHE_LAUNCHER);
	avalanche->setClipItem(STR_AVALANCHE_MISSILE);

	RuleCraftWeapon *cannon = new RuleCraftWeapon(STR_CANNON_UC);
	cannon->setSprite(2);
	cannon->setDamage(10);
	cannon->setRange(10);
	cannon->setAccuracy(25);
	cannon->setReloadTime(2);
	cannon->setAmmoMax(200);
	cannon->setRearmRate(50);
	cannon->setLauncherItem(STR_CANNON);
	cannon->setClipItem(STR_CANNON_ROUNDS);

	RuleCraftWeapon *laser = new RuleCraftWeapon(STR_LASER_CANNON_UC);
	laser->setSprite(4);
	laser->setDamage(70);
	laser->setRange(21);
	laser->setAccuracy(35);
	laser->setReloadTime(12);
	laser->setAmmoMax(99);
	laser->setLauncherItem(STR_LASER_CANNON);

	RuleCraftWeapon *plasma = new RuleCraftWeapon(STR_PLASMA_BEAM_UC);
	plasma->setSprite(5);
	plasma->setDamage(140);
	plasma->setRange(52);
	plasma->setAccuracy(50);
	plasma->setReloadTime(12);
	plasma->setAmmoMax(100);
	plasma->setLauncherItem(STR_PLASMA_BEAM);

	RuleCraftWeapon *fusion = new RuleCraftWeapon(STR_FUSION_BALL_UC);
	fusion->setSprite(3);
	fusion->setDamage(230);
	fusion->setRange(65);
	fusion->setAccuracy(100);
	fusion->setReloadTime(16);
	fusion->setAmmoMax(2);
	fusion->setLauncherItem(STR_FUSION_BALL_LAUNCHER);
	fusion->setClipItem(STR_FUSION_BALL);

	_craftWeapons.insert(pair<LangString, RuleCraftWeapon*>(STR_STINGRAY_UC, stingray));
	_craftWeapons.insert(pair<LangString, RuleCraftWeapon*>(STR_AVALANCHE_UC, avalanche));
	_craftWeapons.insert(pair<LangString, RuleCraftWeapon*>(STR_CANNON_UC, cannon));
	_craftWeapons.insert(pair<LangString, RuleCraftWeapon*>(STR_FUSION_BALL_UC, fusion));
	_craftWeapons.insert(pair<LangString, RuleCraftWeapon*>(STR_LASER_CANNON_UC, laser));
	_craftWeapons.insert(pair<LangString, RuleCraftWeapon*>(STR_PLASMA_BEAM_UC, plasma));

	// Add items
	RuleItem *slauncher = new RuleItem(STR_STINGRAY_LAUNCHER);
	slauncher->setSize(0.8);
	slauncher->setEquippable(false);

	RuleItem *alauncher = new RuleItem(STR_AVALANCHE_LAUNCHER);
	alauncher->setSize(1.0);
	alauncher->setEquippable(false);

	RuleItem *icannon = new RuleItem(STR_CANNON);
	icannon->setSize(1.5);
	icannon->setEquippable(false);

	RuleItem *smissile = new RuleItem(STR_STINGRAY_MISSILE);
	smissile->setSize(0.4);
	smissile->setEquippable(false);

	RuleItem *amissile = new RuleItem(STR_AVALANCHE_MISSILE);
	amissile->setSize(1.5);
	amissile->setEquippable(false);

	RuleItem *crounds = new RuleItem(STR_CANNON_ROUNDS);
	crounds->setSize(0.0);
	crounds->setEquippable(false);

	RuleItem *pistol = new RuleItem(STR_PISTOL);
	pistol->setSize(0.1);

	RuleItem *pclip = new RuleItem(STR_PISTOL_CLIP);
	pclip->setSize(0.1);

	RuleItem *rifle = new RuleItem(STR_RIFLE);
	rifle->setSize(0.2);

	RuleItem *rclip = new RuleItem(STR_RIFLE_CLIP);
	rclip->setSize(0.1);

	RuleItem *hcannon = new RuleItem(STR_HEAVY_CANNON);
	hcannon->setSize(0.3);

	RuleItem *hcap = new RuleItem(STR_HC_AP_AMMO);
	hcap->setSize(0.1);

	RuleItem *hche = new RuleItem(STR_HC_HE_AMMO);
	hche->setSize(0.1);

	RuleItem *acannon = new RuleItem(STR_AUTO_CANNON);
	acannon->setSize(0.3);

	RuleItem *acap = new RuleItem(STR_AC_AP_AMMO);
	acap->setSize(0.1);

	RuleItem *rlauncher = new RuleItem(STR_ROCKET_LAUNCHER);
	rlauncher->setSize(0.4);

	RuleItem *srocket = new RuleItem(STR_SMALL_ROCKET);
	srocket->setSize(0.2);

	RuleItem *grenade = new RuleItem(STR_GRENADE);
	grenade->setSize(0.1);

	RuleItem *sgrenade = new RuleItem(STR_SMOKE_GRENADE);
	sgrenade->setSize(0.1);

	_items.insert(pair<LangString, RuleItem*>(STR_STINGRAY_LAUNCHER, slauncher));
	_items.insert(pair<LangString, RuleItem*>(STR_AVALANCHE_LAUNCHER, alauncher));
	_items.insert(pair<LangString, RuleItem*>(STR_CANNON, icannon));
	_items.insert(pair<LangString, RuleItem*>(STR_STINGRAY_MISSILE, smissile));
	_items.insert(pair<LangString, RuleItem*>(STR_AVALANCHE_MISSILE, amissile));
	_items.insert(pair<LangString, RuleItem*>(STR_CANNON_ROUNDS, crounds));
	_items.insert(pair<LangString, RuleItem*>(STR_PISTOL, pistol));
	_items.insert(pair<LangString, RuleItem*>(STR_PISTOL_CLIP, pclip));
	_items.insert(pair<LangString, RuleItem*>(STR_RIFLE, rifle));
	_items.insert(pair<LangString, RuleItem*>(STR_RIFLE_CLIP, rclip));
	_items.insert(pair<LangString, RuleItem*>(STR_HEAVY_CANNON, hcannon));
	_items.insert(pair<LangString, RuleItem*>(STR_HC_AP_AMMO, hcap));
	_items.insert(pair<LangString, RuleItem*>(STR_HC_HE_AMMO, hche));
	_items.insert(pair<LangString, RuleItem*>(STR_AUTO_CANNON, acannon));
	_items.insert(pair<LangString, RuleItem*>(STR_AC_AP_AMMO, acap));
	_items.insert(pair<LangString, RuleItem*>(STR_ROCKET_LAUNCHER, rlauncher));
	_items.insert(pair<LangString, RuleItem*>(STR_SMALL_ROCKET, srocket));
	_items.insert(pair<LangString, RuleItem*>(STR_GRENADE, grenade));
	_items.insert(pair<LangString, RuleItem*>(STR_SMOKE_GRENADE, sgrenade));

	// Add UFOs
	RuleUfo *sscout = new RuleUfo(STR_SMALL_SCOUT);
	sscout->setSize(STR_VERY_SMALL);
	sscout->setMaxSpeed(2200);
	sscout->setAcceleration(12);
	sscout->setMaxDamage(50);
	sscout->setScore(50);

	RuleUfo *mscout = new RuleUfo(STR_MEDIUM_SCOUT);
	mscout->setSize(STR_SMALL);
	mscout->setMaxSpeed(2400);
	mscout->setAcceleration(9);
	mscout->setWeaponPower(20);
	mscout->setWeaponRange(15);
	mscout->setMaxDamage(200);
	mscout->setScore(75);

	RuleUfo *lscout = new RuleUfo(STR_LARGE_SCOUT);
	lscout->setSize(STR_SMALL);
	lscout->setMaxSpeed(2700);
	lscout->setAcceleration(9);
	lscout->setWeaponPower(20);
	lscout->setWeaponRange(34);
	lscout->setMaxDamage(250);
	lscout->setScore(125);

	RuleUfo *abducter = new RuleUfo(STR_ABDUCTER);
	abducter->setSize(STR_MEDIUM);
	abducter->setMaxSpeed(4000);
	abducter->setAcceleration(8);
	abducter->setWeaponPower(40);
	abducter->setWeaponRange(22);
	abducter->setMaxDamage(500);
	abducter->setScore(250);

	RuleUfo *harvester = new RuleUfo(STR_HARVESTER);
	harvester->setSize(STR_MEDIUM);
	harvester->setMaxSpeed(4300);
	harvester->setAcceleration(8);
	harvester->setWeaponPower(40);
	harvester->setWeaponRange(20);
	harvester->setMaxDamage(500);
	harvester->setScore(250);

	RuleUfo *supply = new RuleUfo(STR_SUPPLY_SHIP);
	supply->setSize(STR_LARGE);
	supply->setMaxSpeed(3200);
	supply->setAcceleration(6);
	supply->setWeaponPower(60);
	supply->setWeaponRange(36);
	supply->setMaxDamage(2200);
	supply->setScore(400);

	RuleUfo *terror = new RuleUfo(STR_TERROR_SHIP);
	terror->setSize(STR_LARGE);
	terror->setMaxSpeed(4800);
	terror->setAcceleration(6);
	terror->setWeaponPower(120);
	terror->setWeaponRange(42);
	terror->setMaxDamage(1200);
	terror->setScore(500);

	RuleUfo *battleship = new RuleUfo(STR_BATTLESHIP);
	battleship->setSize(STR_VERY_LARGE);
	battleship->setMaxSpeed(5000);
	battleship->setAcceleration(6);
	battleship->setWeaponPower(148);
	battleship->setWeaponRange(65);
	battleship->setMaxDamage(3200);
	battleship->setScore(700);

	_ufos.insert(pair<LangString, RuleUfo*>(STR_SMALL_SCOUT, sscout));
	_ufos.insert(pair<LangString, RuleUfo*>(STR_MEDIUM_SCOUT, mscout));
	_ufos.insert(pair<LangString, RuleUfo*>(STR_LARGE_SCOUT, lscout));
	_ufos.insert(pair<LangString, RuleUfo*>(STR_ABDUCTER, abducter));
	_ufos.insert(pair<LangString, RuleUfo*>(STR_HARVESTER, harvester));
	_ufos.insert(pair<LangString, RuleUfo*>(STR_SUPPLY_SHIP, supply));
	_ufos.insert(pair<LangString, RuleUfo*>(STR_TERROR_SHIP, terror));
	_ufos.insert(pair<LangString, RuleUfo*>(STR_BATTLESHIP, battleship));
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
	namerica->getCities()->push_back(new City(STR_NEW_YORK, 4.99382, -0.711222));
	namerica->getCities()->push_back(new City(STR_WASHINGTON, 4.9371, -0.676315));
	namerica->getCities()->push_back(new City(STR_LOS_ANGELES, 4.21933, -0.595594));
	namerica->getCities()->push_back(new City(STR_MONTREAL, 4.9611, -0.794125));
	namerica->getCities()->push_back(new City(STR_HAVANA, 4.84547, -0.392699));
	namerica->getCities()->push_back(new City(STR_MEXICO_CITY, 4.55313, -0.338158));
	namerica->getCities()->push_back(new City(STR_CHICAGO, 4.75384, -0.730857));
	namerica->getCities()->push_back(new City(STR_VANCOUVER, 4.13207, -0.861756));
	namerica->getCities()->push_back(new City(STR_DALLAS, 4.59676, -0.571595));
	
	Region* arctic = new Region(950000);
	arctic->addArea(0, 6.281, -1.5708, -1.22391);
	
	Region* antarctica = new Region(900000);
	antarctica->addArea(0, 6.281, 1.0472, 1.5708);
	
	Region* samerica = new Region(600000);
	samerica->addArea(4.71239, 5.49561, -0.174533, -0.00218166);
	samerica->addArea(4.79966, 5.7574, 0, 0.259618);
	samerica->addArea(4.79966, 5.67014, 0.261799, 1.04502);
	samerica->getCities()->push_back(new City(STR_BRASILIA, 5.44761, 0.274889));
	samerica->getCities()->push_back(new City(STR_BOGOTA, 4.98946, -0.0785398));
	samerica->getCities()->push_back(new City(STR_BUENOS_AIRES, 5.27962, 0.602139));
	samerica->getCities()->push_back(new City(STR_SANTIAGO, 5.05055, 0.582504));
	samerica->getCities()->push_back(new City(STR_RIO_DE_JANEIRO, 5.53051, 0.399244));
	samerica->getCities()->push_back(new City(STR_LIMA, 4.93928, 0.20944));
	samerica->getCities()->push_back(new City(STR_CARACAS, 5.116, -0.18326));
	
	Region* europe = new Region(1000000);
	europe->addArea(5.84685, 1.04502, -1.22173, -0.613047);
	europe->getCities()->push_back(new City(STR_LONDON, 6.281, -0.898845));
	europe->getCities()->push_back(new City(STR_PARIS, 0.0414516, -0.850848));
	europe->getCities()->push_back(new City(STR_BERLIN, 0.233438, -0.916298));
	europe->getCities()->push_back(new City(STR_MOSCOW, 0.65668, -0.973021));
	europe->getCities()->push_back(new City(STR_ROME, 0.218166, -0.730857));
	europe->getCities()->push_back(new City(STR_MADRID, 6.21774, -0.704677));
	europe->getCities()->push_back(new City(STR_BUDAPEST, 0.333794, -0.829031));
	
	Region* nafrica = new Region(650000);
	nafrica->addArea(5.84685, 0.69595, -0.610865, -0.263981);
	nafrica->addArea(5.84685, 0.957749, -0.261799, -0.00218166);
	nafrica->getCities()->push_back(new City(STR_LAGOS, 0.0545415, -0.113446));
	nafrica->getCities()->push_back(new City(STR_CAIRO, 0.545415, -0.523599));
	nafrica->getCities()->push_back(new City(STR_CASABLANCA, 6.1501, -0.584685));
	
	Region* safrica = new Region(550000);
	safrica->addArea(0.0872665, 0.957749, 0, 0.69595);
	safrica->getCities()->push_back(new City(STR_PRETORIA, 0.490874, 0.458149));
	safrica->getCities()->push_back(new City(STR_NAIROBI, 0.641409, 0.0218166));
	safrica->getCities()->push_back(new City(STR_CAPE_TOWN, 0.322886, 0.593412));
	safrica->getCities()->push_back(new City(STR_KINSHASA, 0.268344, 0.0763582));
	
	Region* casia = new Region(500000);
	casia->addArea(0.698132, 1.21955, -0.610865, -0.263981);
	casia->addArea(1.0472, 1.56861, -0.872665, -0.613047);
	casia->addArea(1.22173, 1.56861, -0.610865, -0.0894481);
	casia->getCities()->push_back(new City(STR_ANKARA, 0.571595, -0.69595));
	casia->getCities()->push_back(new City(STR_DELHI, 1.34827, -0.4996));
	casia->getCities()->push_back(new City(STR_KARACHI, 1.16937, -0.434151));
	casia->getCities()->push_back(new City(STR_BAGHDAD, 0.776672, -0.580322));
	casia->getCities()->push_back(new City(STR_TEHRAN, 0.898845, -0.621774));
	casia->getCities()->push_back(new City(STR_BOMBAY, 1.27627, -0.329431));
	casia->getCities()->push_back(new City(STR_CALCUTTA, 1.54243, -0.394881));
	
	Region* seasia = new Region(750000);
	seasia->addArea(1.5708, 1.83041, -0.872665, 0.172351);
	seasia->addArea(1.8326, 2.61581, -0.872665, -0.0894481);
	seasia->getCities()->push_back(new City(STR_TOKYO, 2.4391, -0.621774));
	seasia->getCities()->push_back(new City(STR_BEIJING, 2.03113, -0.69595));
	seasia->getCities()->push_back(new City(STR_BANGKOK, 1.75624, -0.237801));
	seasia->getCities()->push_back(new City(STR_MANILA, 2.11185, -0.255254));
	seasia->getCities()->push_back(new City(STR_SEOUL, 2.21657, -0.654498));
	seasia->getCities()->push_back(new City(STR_SINGAPORE, 1.81296, -0.0239983));
	seasia->getCities()->push_back(new City(STR_JAKARTA, 1.86314, 0.109083));
	seasia->getCities()->push_back(new City(STR_SHANGHAI, 2.12058, -0.545415));
	seasia->getCities()->push_back(new City(STR_HONG_KONG, 1.99186, -0.388336));
	
	Region* siberia = new Region(800000);
	siberia->addArea(1.0472, 3.13941, -1.22173, -0.874846);
	siberia->getCities()->push_back(new City(STR_NOVOSIBIRSK, 1.44426, -0.959931));
	
	Region* australasia = new Region(750000);
	//australasia->addArea(1.8326, 3.13941, -0.0872665, 0.870483);
	australasia->addArea(1.8326, 3.13941, -0.0872665, 1.04502);
	australasia->getCities()->push_back(new City(STR_CANBERRA, 2.60272, 0.61741));
	australasia->getCities()->push_back(new City(STR_WELLINGTON, 3.05651, 0.719948));
	australasia->getCities()->push_back(new City(STR_MELBOURNE, 2.53073, 0.661043));
	australasia->getCities()->push_back(new City(STR_PERTH, 2.02022, 0.558505));
	
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

	// Add items
	base->getItems()->insert(pair<LangString, Item*>(STR_STINGRAY_LAUNCHER, new Item(getItem(STR_STINGRAY_LAUNCHER), 1)));
	base->getItems()->insert(pair<LangString, Item*>(STR_AVALANCHE_LAUNCHER, new Item(getItem(STR_AVALANCHE_LAUNCHER), 1)));
	base->getItems()->insert(pair<LangString, Item*>(STR_CANNON, new Item(getItem(STR_CANNON), 2)));
	base->getItems()->insert(pair<LangString, Item*>(STR_STINGRAY_MISSILE, new Item(getItem(STR_STINGRAY_MISSILE), 25)));
	base->getItems()->insert(pair<LangString, Item*>(STR_AVALANCHE_MISSILE, new Item(getItem(STR_AVALANCHE_MISSILE), 10)));
	base->getItems()->insert(pair<LangString, Item*>(STR_CANNON_ROUNDS, new Item(getItem(STR_CANNON_ROUNDS), 1)));
	base->getItems()->insert(pair<LangString, Item*>(STR_PISTOL, new Item(getItem(STR_PISTOL), 2)));
	base->getItems()->insert(pair<LangString, Item*>(STR_PISTOL_CLIP, new Item(getItem(STR_PISTOL_CLIP), 8)));
	base->getItems()->insert(pair<LangString, Item*>(STR_RIFLE, new Item(getItem(STR_RIFLE), 2)));
	base->getItems()->insert(pair<LangString, Item*>(STR_RIFLE_CLIP, new Item(getItem(STR_RIFLE_CLIP), 8)));
	base->getItems()->insert(pair<LangString, Item*>(STR_HEAVY_CANNON, new Item(getItem(STR_HEAVY_CANNON), 1)));
	base->getItems()->insert(pair<LangString, Item*>(STR_HC_AP_AMMO, new Item(getItem(STR_HC_AP_AMMO), 6)));
	base->getItems()->insert(pair<LangString, Item*>(STR_AUTO_CANNON, new Item(getItem(STR_AUTO_CANNON), 1)));
	base->getItems()->insert(pair<LangString, Item*>(STR_AC_AP_AMMO, new Item(getItem(STR_AC_AP_AMMO), 6)));
	base->getItems()->insert(pair<LangString, Item*>(STR_ROCKET_LAUNCHER, new Item(getItem(STR_ROCKET_LAUNCHER), 1)));
	base->getItems()->insert(pair<LangString, Item*>(STR_SMALL_ROCKET, new Item(getItem(STR_SMALL_ROCKET), 4)));
	base->getItems()->insert(pair<LangString, Item*>(STR_GRENADE, new Item(getItem(STR_GRENADE), 5)));
	base->getItems()->insert(pair<LangString, Item*>(STR_SMOKE_GRENADE, new Item(getItem(STR_SMOKE_GRENADE), 5)));

	// Add crafts
	Craft *skyranger = new Craft(getCraft(STR_SKYRANGER), save->getCraftIds(), base);
	skyranger->setFuel(skyranger->getRules()->getMaxFuel());
	skyranger->getItems()->insert(pair<LangString, Item*>(STR_PISTOL, new Item(getItem(STR_PISTOL), 3)));
	skyranger->getItems()->insert(pair<LangString, Item*>(STR_PISTOL_CLIP, new Item(getItem(STR_PISTOL_CLIP), 5)));
	skyranger->getItems()->insert(pair<LangString, Item*>(STR_RIFLE, new Item(getItem(STR_RIFLE), 6)));
	skyranger->getItems()->insert(pair<LangString, Item*>(STR_RIFLE_CLIP, new Item(getItem(STR_RIFLE_CLIP), 12)));
	skyranger->getItems()->insert(pair<LangString, Item*>(STR_HEAVY_CANNON, new Item(getItem(STR_HEAVY_CANNON), 1)));
	skyranger->getItems()->insert(pair<LangString, Item*>(STR_HC_AP_AMMO, new Item(getItem(STR_HC_AP_AMMO), 2)));
	skyranger->getItems()->insert(pair<LangString, Item*>(STR_HC_HE_AMMO, new Item(getItem(STR_HC_HE_AMMO), 2)));
	skyranger->getItems()->insert(pair<LangString, Item*>(STR_GRENADE, new Item(getItem(STR_GRENADE), 8)));
	base->getCrafts()->push_back(skyranger);

	for (int i = 0; i < 2; i++)
	{
		Craft *interceptor = new Craft(getCraft(STR_INTERCEPTOR), save->getCraftIds(), base);
		interceptor->setFuel(interceptor->getRules()->getMaxFuel());
		interceptor->getWeapons()->at(0) = new CraftWeapon(getCraftWeapon(STR_STINGRAY_UC), getCraftWeapon(STR_STINGRAY_UC)->getAmmoMax());
		interceptor->getWeapons()->at(1) = new CraftWeapon(getCraftWeapon(STR_CANNON_UC), getCraftWeapon(STR_CANNON_UC)->getAmmoMax());
		base->getCrafts()->push_back(interceptor);
	}

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
