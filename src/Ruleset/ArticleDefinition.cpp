/*
 * Copyright 2011 OpenXcom Developers.
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
	UfopaediaTypeId ArticleDefinition::getType()
	{
		return _type_id;
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
	
	/**
	 * Constructor (only setting type of base class)
	 */
	ArticleDefinitionCraft::ArticleDefinitionCraft() : ArticleDefinition(UFOPAEDIA_TYPE_CRAFT), craft(0)
	{}
	
	/**
	 * Constructor (only setting type of base class)
	 */
	ArticleDefinitionCraftWeapon::ArticleDefinitionCraftWeapon() : ArticleDefinition(UFOPAEDIA_TYPE_CRAFT_WEAPON), weapon(0)
	{}
	
	/**
	 * Constructor (only setting type of base class)
	 */
	ArticleDefinitionText::ArticleDefinitionText() : ArticleDefinition(UFOPAEDIA_TYPE_TEXT)
	{}
	
	/**
	 * Constructor (only setting type of base class)
	 */
	ArticleDefinitionTextImage::ArticleDefinitionTextImage() : ArticleDefinition(UFOPAEDIA_TYPE_TEXTIMAGE)
	{}

	/**
	 * Constructor (only setting type of base class)
	 */
	ArticleDefinitionBaseFacility::ArticleDefinitionBaseFacility() : ArticleDefinition(UFOPAEDIA_TYPE_BASE_FACILITY), facility(0)
	{}
	
	/**
	 * Constructor (only setting type of base class)
	 */
	ArticleDefinitionItem::ArticleDefinitionItem() : ArticleDefinition(UFOPAEDIA_TYPE_ITEM), item(0)
	{}
	
	/**
	 * Constructor (only setting type of base class)
	 */
	ArticleDefinitionUfo::ArticleDefinitionUfo() : ArticleDefinition(UFOPAEDIA_TYPE_UFO), ufo(0)
	{}
	
}
