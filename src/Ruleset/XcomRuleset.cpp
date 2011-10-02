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
	RuleArmor *coveralls = new RuleArmor("STR_NONE_UC", "XCOM_0.PCK");
	coveralls->setArmor(12, 8, 5, 2);
	coveralls->setCorpseItem("STR_CORPSE");

	RuleArmor *personalArmor = new RuleArmor("STR_PERSONAL_ARMOR_UC", "XCOM_1.PCK");
	personalArmor->setArmor(50, 40, 40, 30);

	RuleArmor *powerSuit = new RuleArmor("STR_POWER_SUIT_UC", "XCOM_2.PCK");
	powerSuit->setArmor(100, 80, 70, 60);

	RuleArmor *flyingSuit = new RuleArmor("STR_FLYING_SUIT_UC", "XCOM_2.PCK");
	flyingSuit->setArmor(110, 90, 80, 70);

	RuleArmor *sectoidSoldierArmor = new RuleArmor("SECTOID_ARMOR0", "SECTOID.PCK");
	sectoidSoldierArmor->setArmor(4, 3, 2, 2);
	sectoidSoldierArmor->setCorpseItem("STR_SECTOID_CORPSE");

	_armors.insert(std::pair<std::string, RuleArmor*>("STR_NONE_UC", coveralls));
	_armors.insert(std::pair<std::string, RuleArmor*>("STR_PERSONAL_ARMOR_UC", personalArmor));
	_armors.insert(std::pair<std::string, RuleArmor*>("STR_POWER_SUIT_UC" ,powerSuit));
	_armors.insert(std::pair<std::string, RuleArmor*>("STR_FLYING_SUIT_UC", flyingSuit));
	_armors.insert(std::pair<std::string, RuleArmor*>("SECTOID_ARMOR0", sectoidSoldierArmor));

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

	_aliens.insert(std::pair<std::string, RuleAlien*>("SECTOID_SOLDIER", sectoidSoldier));
	_aliens.insert(std::pair<std::string, RuleAlien*>("SECTOID_ENGINEER", sectoidEngineer));
	_aliens.insert(std::pair<std::string, RuleAlien*>("SECTOID_NAVIGATOR", sectoidNavigator));

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
