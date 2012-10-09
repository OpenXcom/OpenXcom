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

#include "ArticleDefinition.h"

namespace OpenXcom
{

	/**
	 * Constructor
	 * @param type_id Article type of this instance.
	 */
	ArticleDefinition::ArticleDefinition(UfopaediaTypeId type_id) : _type_id(type_id)
	{}

	/**
	 * Destructor
	 */
	ArticleDefinition::~ArticleDefinition()
	{}

	/**
	 * Get the article definition type. (Text, TextImage, Craft, ...)
	 * @returns The type of article definition of this instance.
	 */
	UfopaediaTypeId ArticleDefinition::getType() const
	{
		return _type_id;
	}

	/**
	 * Loads the article definition from a YAML file.
	 * @param node YAML node.
	 */
	void ArticleDefinition::load(const YAML::Node &node)
	{
		int a = 0;
		for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
		{
			std::string key;
			i.first() >> key;
			if (key == "id")
			{
				i.second() >> id;
				i.second() >> title;
			}
			else if (key == "type_id")
			{
				i.second() >> a;
				_type_id = (UfopaediaTypeId)a;
			}
			else if (key == "title")
			{
				i.second() >> title;
			}
			else if (key == "section")
			{
				i.second() >> section;
			}
		}
	}

	/**
	 * Saves the article definition to a YAML file.
	 * @param out YAML emitter.
	 */
	void ArticleDefinition::save(YAML::Emitter &out) const
	{
		out << YAML::BeginMap;
		out << YAML::Key << "id" << YAML::Value << id;
		out << YAML::Key << "type_id" << YAML::Value << _type_id;
		out << YAML::Key << "title" << YAML::Value << title;
		out << YAML::Key << "section" << YAML::Value << section;
	}

	/**
	 * Constructor
	 */
	ArticleDefinitionRect::ArticleDefinitionRect() : x(0), y(0), width(0), height(0) {}

	/**
	 * Set the rectangle parameters in a function
	 */
	void ArticleDefinitionRect::set(int set_x, int set_y, int set_width, int set_height)
	{
		x = set_x;
		y = set_y;
		width = set_width;
		height = set_height;
	}

	void operator>> (const YAML::Node& node, ArticleDefinitionRect& rect)
	{
		node["x"] >> rect.x;
		node["y"] >> rect.y;
		node["width"] >> rect.width;
		node["height"] >> rect.height;
	}

	YAML::Emitter& operator<< (YAML::Emitter& out, const ArticleDefinitionRect& rect)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "x" << YAML::Value << rect.x;
		out << YAML::Key << "y" << YAML::Value << rect.y;
		out << YAML::Key << "width" << YAML::Value << rect.width;
		out << YAML::Key << "height" << YAML::Value << rect.height;
		out << YAML::EndMap;
		return out;
	}

	/**
	 * Constructor (only setting type of base class)
	 */
	ArticleDefinitionCraft::ArticleDefinitionCraft() : ArticleDefinition(UFOPAEDIA_TYPE_CRAFT)
	{}

	/**
	 * Loads the article definition from a YAML file.
	 * @param node YAML node.
	 */
	void ArticleDefinitionCraft::load(const YAML::Node &node)
	{
		ArticleDefinition::load(node);
		for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
		{
			std::string key;
			i.first() >> key;
			if (key == "image_id")
			{
				i.second() >> image_id;
			}
			else if (key == "rect_stats")
			{
				i.second() >> rect_stats;
			}
			else if (key == "rect_text")
			{
				i.second() >> rect_text;
			}
			else if (key == "text")
			{
				i.second() >> text;
			}
		}
	}

	/**
	 * Saves the article definition to a YAML file.
	 * @param out YAML emitter.
	 */
	void ArticleDefinitionCraft::save(YAML::Emitter &out) const
	{
		ArticleDefinition::save(out);
		out << YAML::Key << "image_id" << YAML::Value << image_id;
		out << YAML::Key << "rect_stats" << YAML::Value << rect_stats;
		out << YAML::Key << "rect_text" << YAML::Value << rect_text;
		out << YAML::Key << "text" << YAML::Value << text;
		out << YAML::EndMap;
	}

	/**
	 * Constructor (only setting type of base class)
	 */
	ArticleDefinitionCraftWeapon::ArticleDefinitionCraftWeapon() : ArticleDefinition(UFOPAEDIA_TYPE_CRAFT_WEAPON)
	{}

	/**
	 * Loads the article definition from a YAML file.
	 * @param node YAML node.
	 */
	void ArticleDefinitionCraftWeapon::load(const YAML::Node &node)
	{
		ArticleDefinition::load(node);
		for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
		{
			std::string key;
			i.first() >> key;
			if (key == "image_id")
			{
				i.second() >> image_id;
			}
			else if (key == "text")
			{
				i.second() >> text;
			}
		}
	}

	/**
	 * Saves the article definition to a YAML file.
	 * @param out YAML emitter.
	 */
	void ArticleDefinitionCraftWeapon::save(YAML::Emitter &out) const
	{
		ArticleDefinition::save(out);
		out << YAML::Key << "image_id" << YAML::Value << image_id;
		out << YAML::Key << "text" << YAML::Value << text;
		out << YAML::EndMap;
	}

	/**
	 * Constructor (only setting type of base class)
	 */
	ArticleDefinitionText::ArticleDefinitionText() : ArticleDefinition(UFOPAEDIA_TYPE_TEXT)
	{}

	/**
	 * Loads the article definition from a YAML file.
	 * @param node YAML node.
	 */
	void ArticleDefinitionText::load(const YAML::Node &node)
	{
		ArticleDefinition::load(node);
		for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
		{
			std::string key;
			i.first() >> key;
			if (key == "text")
			{
				i.second() >> text;
			}
		}
	}

	/**
	 * Saves the article definition to a YAML file.
	 * @param out YAML emitter.
	 */
	void ArticleDefinitionText::save(YAML::Emitter &out) const
	{
		ArticleDefinition::save(out);
		out << YAML::Key << "text" << YAML::Value << text;
		out << YAML::EndMap;
	}

	/**
	 * Constructor (only setting type of base class)
	 */
	ArticleDefinitionTextImage::ArticleDefinitionTextImage() : ArticleDefinition(UFOPAEDIA_TYPE_TEXTIMAGE), text_width(0)
	{}

	/**
	 * Loads the article definition from a YAML file.
	 * @param node YAML node.
	 */
	void ArticleDefinitionTextImage::load(const YAML::Node &node)
	{
		ArticleDefinition::load(node);
		for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
		{
			std::string key;
			i.first() >> key;
			if (key == "image_id")
			{
				i.second() >> image_id;
			}
			else if (key == "text")
			{
				i.second() >> text;
			}
			else if (key == "text_width")
			{
				i.second() >> text_width;
			}
		}
	}

	/**
	 * Saves the article definition to a YAML file.
	 * @param out YAML emitter.
	 */
	void ArticleDefinitionTextImage::save(YAML::Emitter &out) const
	{
		ArticleDefinition::save(out);
		out << YAML::Key << "image_id" << YAML::Value << image_id;
		out << YAML::Key << "text" << YAML::Value << text;
		out << YAML::Key << "text_width" << YAML::Value << text_width;
		out << YAML::EndMap;
	}

	/**
	 * Constructor (only setting type of base class)
	 */
	ArticleDefinitionBaseFacility::ArticleDefinitionBaseFacility() : ArticleDefinition(UFOPAEDIA_TYPE_BASE_FACILITY)
	{}

	/**
	 * Loads the article definition from a YAML file.
	 * @param node YAML node.
	 */
	void ArticleDefinitionBaseFacility::load(const YAML::Node &node)
	{
		ArticleDefinition::load(node);
		for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
		{
			std::string key;
			i.first() >> key;
			if (key == "text")
			{
				i.second() >> text;
			}
		}
	}

	/**
	 * Saves the article definition to a YAML file.
	 * @param out YAML emitter.
	 */
	void ArticleDefinitionBaseFacility::save(YAML::Emitter &out) const
	{
		ArticleDefinition::save(out);
		out << YAML::Key << "text" << YAML::Value << text;
		out << YAML::EndMap;
	}

	/**
	 * Constructor (only setting type of base class)
	 */
	ArticleDefinitionItem::ArticleDefinitionItem() : ArticleDefinition(UFOPAEDIA_TYPE_ITEM)
	{}

	/**
	 * Loads the article definition from a YAML file.
	 * @param node YAML node.
	 */
	void ArticleDefinitionItem::load(const YAML::Node &node)
	{
		ArticleDefinition::load(node);
		for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
		{
			std::string key;
			i.first() >> key;
			if (key == "text")
			{
				i.second() >> text;
			}
		}
	}

	/**
	 * Saves the article definition to a YAML file.
	 * @param out YAML emitter.
	 */
	void ArticleDefinitionItem::save(YAML::Emitter &out) const
	{
		ArticleDefinition::save(out);
		out << YAML::Key << "text" << YAML::Value << text;
		out << YAML::EndMap;
	}

	/**
	 * Constructor (only setting type of base class)
	 */
	ArticleDefinitionUfo::ArticleDefinitionUfo() : ArticleDefinition(UFOPAEDIA_TYPE_UFO)
	{}

	/**
	 * Loads the article definition from a YAML file.
	 * @param node YAML node.
	 */
	void ArticleDefinitionUfo::load(const YAML::Node &node)
	{
		ArticleDefinition::load(node);
		for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
		{
			std::string key;
			i.first() >> key;
			if (key == "text")
			{
				i.second() >> text;
			}
		}
	}

	/**
	 * Saves the article definition to a YAML file.
	 * @param out YAML emitter.
	 */
	void ArticleDefinitionUfo::save(YAML::Emitter &out) const
	{
		ArticleDefinition::save(out);
		out << YAML::Key << "text" << YAML::Value << text;
		out << YAML::EndMap;
	}

	/**
	 * Constructor (only setting type of base class)
	 */
	ArticleDefinitionArmor::ArticleDefinitionArmor() : ArticleDefinition(UFOPAEDIA_TYPE_ARMOR)
	{}

	/**
	 * Loads the article definition from a YAML file.
	 * @param node YAML node.
	 */
	void ArticleDefinitionArmor::load(const YAML::Node &node)
	{
		ArticleDefinition::load(node);
	}

	/**
	 * Saves the article definition to a YAML file.
	 * @param out YAML emitter.
	 */
	void ArticleDefinitionArmor::save(YAML::Emitter &out) const
	{
		ArticleDefinition::save(out);
		out << YAML::EndMap;
	}

	/**
	 * Constructor (only setting type of base class)
	 */
	ArticleDefinitionVehicle::ArticleDefinitionVehicle() : ArticleDefinition(UFOPAEDIA_TYPE_VEHICLE)
	{}

	/**
	 * Loads the article definition from a YAML file.
	 * @param node YAML node.
	 */
	void ArticleDefinitionVehicle::load(const YAML::Node &node)
	{
		ArticleDefinition::load(node);
		for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
		{
			std::string key;
			i.first() >> key;
			if (key == "text")
			{
				i.second() >> text;
			}
		}
	}

	/**
	 * Saves the article definition to a YAML file.
	 * @param out YAML emitter.
	 */
	void ArticleDefinitionVehicle::save(YAML::Emitter &out) const
	{
		ArticleDefinition::save(out);
		out << YAML::Key << "text" << YAML::Value << text;
		out << YAML::EndMap;
	}

}
