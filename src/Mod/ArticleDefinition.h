#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{
	/// define article types
	enum UfopaediaTypeId {
		UFOPAEDIA_TYPE_UNKNOWN            = 0,
		UFOPAEDIA_TYPE_CRAFT              = 1,
		UFOPAEDIA_TYPE_CRAFT_WEAPON       = 2,
		UFOPAEDIA_TYPE_VEHICLE            = 3,
		UFOPAEDIA_TYPE_ITEM               = 4,
		UFOPAEDIA_TYPE_ARMOR              = 5,
		UFOPAEDIA_TYPE_BASE_FACILITY      = 6,
		UFOPAEDIA_TYPE_TEXTIMAGE          = 7,
		UFOPAEDIA_TYPE_TEXT               = 8,
		UFOPAEDIA_TYPE_UFO                = 9,
		UFOPAEDIA_TYPE_TFTD               = 10,
		UFOPAEDIA_TYPE_TFTD_CRAFT         = 11,
		UFOPAEDIA_TYPE_TFTD_CRAFT_WEAPON  = 12,
		UFOPAEDIA_TYPE_TFTD_VEHICLE       = 13,
		UFOPAEDIA_TYPE_TFTD_ITEM          = 14,
		UFOPAEDIA_TYPE_TFTD_ARMOR         = 15,
		UFOPAEDIA_TYPE_TFTD_BASE_FACILITY = 16,
		UFOPAEDIA_TYPE_TFTD_USO           = 17
	};

	/**
	 * ArticleDefinition is the base class for all article types.
	 * This class is used to store all information about articles
	 * required to generate an ArticleState from.
	 */

	class ArticleDefinition
	{
	protected:
		/// Constructor (protected, so this class cannot be instantiated directly).
		ArticleDefinition(UfopaediaTypeId type_id);

	public:
		/// Destructor.
		virtual ~ArticleDefinition();
		/// Gets the type of article definition.
		UfopaediaTypeId getType() const;
		/// Loads the article from YAML.
		virtual void load(const YAML::Node& node, int listOrder);
		/// Gets the article's list weight.
		int getListOrder() const;

		std::string id;
		std::string title;
		std::string section;
		std::vector<std::string> requires;

	protected:
		UfopaediaTypeId _type_id;
	private:
		int _listOrder;
	};

	class ArticleDefinitionRect
	{
	public:
		ArticleDefinitionRect();

		void set(int set_x, int set_y, int set_width, int set_height);

		int x;
		int y;
		int width;
		int height;
	};

	/**
	 * ArticleDefinitionCraft defines articles for craft, e.g. SKYRANGER.
	 * They have a large background image, a stats block and a description positioned differently.
	 */

	class ArticleDefinitionCraft : public ArticleDefinition
	{
	public:
		/// Constructor.
		ArticleDefinitionCraft();
		/// Loads the article from YAML.
		void load(const YAML::Node& node, int listOrder);

		std::string image_id;
		ArticleDefinitionRect rect_stats;
		ArticleDefinitionRect rect_text;
		std::string text;
	};

	/**
	 * ArticleDefinitionCraftWeapon defines articles for craft weapons, e.g. STINGRAY, AVALANCHE.
	 * They have a large background image and a stats block.
	 */

	class ArticleDefinitionCraftWeapon : public ArticleDefinition
	{
	public:
		/// Constructor.
		ArticleDefinitionCraftWeapon();
		/// Loads the article from YAML.
		void load(const YAML::Node& node, int listOrder);

		std::string image_id;
		std::string text;
	};

	/**
	 * ArticleDefinitionText defines articles with only text, e.g. ALIEN RESEARCH.
	 */

	class ArticleDefinitionText : public ArticleDefinition
	{
		public:
		/// Constructor.
		ArticleDefinitionText();
		/// Loads the article from YAML.
		void load(const YAML::Node& node, int listOrder);

		std::string text;
	};

	/**
	 * ArticleDefinitionTextImage defines articles with text on the left and
	 * an image on the right side of the screen, e.g. ALIEN LIFEFORMS, UFO COMPONENTS.
	 */

	class ArticleDefinitionTextImage : public ArticleDefinition
	{
		public:
		/// Constructor.
		ArticleDefinitionTextImage();
		/// Loads the article from YAML.
		void load(const YAML::Node& node, int listOrder);

		std::string image_id;
		std::string text;
		int text_width;
	};

	/**
	 * ArticleDefinitionTextImage defines articles with text on the left and
	 * an image on the right side of the screen, e.g. ALIEN LIFEFORMS, UFO COMPONENTS.
	 */

	class ArticleDefinitionTFTD : public ArticleDefinition
	{
		public:
		/// Constructor.
		ArticleDefinitionTFTD();
		/// Loads the article from YAML.
		void load(const YAML::Node& node, int listOrder);

		std::string image_id;
		std::string text;
		int text_width;
		std::string weapon;
	};

	/**
	 * ArticleDefinitionBaseFacility defines articles for base facilities, e.g. Access lift.
	 * They have an image (found in BASEBITS.PCK), a stats block and a description.
	 */

	class ArticleDefinitionBaseFacility : public ArticleDefinition
	{
	public:
		/// Constructor.
		ArticleDefinitionBaseFacility();
		/// Loads the article from YAML.
		void load(const YAML::Node& node, int listOrder);

		std::string text;
	};

	/**
	 * ArticleDefinitionItem defines articles for all Items, e.g. Weapons, Ammo, Equipment, etc.
	 * They have an image (found in BIGOBS.PCK), an optional stats block, maybe ammo and a description.
	 */

	class ArticleDefinitionItem : public ArticleDefinition
	{
	public:
		/// Constructor.
		ArticleDefinitionItem();
		/// Loads the article from YAML.
		void load(const YAML::Node& node, int listOrder);

		std::string text;
	};

	/**
	 * ArticleDefinitionUfo defines articles for UFOs, e.g. Small Scout, Terror Ship, etc.
	 * They have an image (found in INTERWIN.DAT), a stats block and a description.
	 */

	class ArticleDefinitionUfo : public ArticleDefinition
	{
	public:
		/// Constructor.
		ArticleDefinitionUfo();
		/// Loads the article from YAML.
		void load(const YAML::Node& node, int listOrder);

		std::string text;
	};

	/**
	 * ArticleDefinitionArmor defines articles for Armor, e.g. Personal Armor, Flying Suit, etc.
	 * They have an image (found in MAN_*.SPK) and a stats block.
	 */

	class ArticleDefinitionArmor : public ArticleDefinition
	{
	public:
		/// Constructor.
		ArticleDefinitionArmor();
		/// Loads the article from YAML.
		void load(const YAML::Node& node, int listOrder);

		std::string text;
	};

	/**
	 * ArticleDefinitionVehicle defines articles for Vehicles, e.g. Tanks, etc.
	 * They have a text description and a stats block.
	 */

	class ArticleDefinitionVehicle : public ArticleDefinition
	{
	public:
		/// Constructor.
		ArticleDefinitionVehicle();
		/// Loads the article from YAML.
		void load(const YAML::Node& node, int listOrder);

		std::string text;
		std::string weapon;
	};

}
