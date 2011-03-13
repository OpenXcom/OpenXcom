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
	 * Constructor (only setting type of base class)
	 */
	ArticleDefinitionText::ArticleDefinitionText() : ArticleDefinition(UFOPAEDIA_TYPE_TEXT)
	{}
	
	/**
	 * Constructor (only setting type of base class)
	 */
	ArticleDefinitionTextImage::ArticleDefinitionTextImage() : ArticleDefinition(UFOPAEDIA_TYPE_TEXTIMAGE)
	{}
}
