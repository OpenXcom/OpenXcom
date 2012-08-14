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

#include "XcomRuleset.h"
#include "../Savegame/SavedGame.h"
#include "../Ufopaedia/Ufopaedia.h"
#include "../Savegame/UfopaediaSaved.h"
#include "ArticleDefinition.h"

namespace OpenXcom
{
/**
 * Initializes the X-Com ruleset with all the rules
 * mimicking the original game.
 */
XcomRuleset::XcomRuleset() : Ruleset()
{
	load("Xcom1Ruleset");

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
 * @return New saved game.
 */
SavedGame *XcomRuleset::newSave() const
{
	SavedGame *save = Ruleset::newSave();

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
