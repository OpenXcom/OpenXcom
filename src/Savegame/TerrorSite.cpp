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
#include "TerrorSite.h"
#include <sstream>
#include "../Engine/Language.h"

namespace OpenXcom
{

/**
 * Initializes a terror site.
 */
TerrorSite::TerrorSite() : Target(), _id(0)
{
}

/**
 *
 */
TerrorSite::~TerrorSite()
{
}

/**
 * Loads the terror site from a YAML file.
 * @param node YAML node.
 */
void TerrorSite::load(const YAML::Node &node)
{
	Target::load(node);
	node["id"] >> _id;
}

/**
 * Saves the terror site to a YAML file.
 * @param out YAML emitter.
 */
void TerrorSite::save(YAML::Emitter &out) const
{
	Target::save(out);
	out << YAML::Key << "id" << YAML::Value << _id;
	out << YAML::EndMap;
}

/**
 * Saves the terror site's unique identifiers to a YAML file.
 * @param out YAML emitter.
 */
void TerrorSite::saveId(YAML::Emitter &out) const
{
	Target::saveId(out);
	out << YAML::Key << "type" << YAML::Value << "STR_TERROR_SITE";
	out << YAML::Key << "id" << YAML::Value << _id;
	out << YAML::EndMap;
}

/**
 * Returns the terror site's unique ID.
 * @return Unique ID.
 */
int TerrorSite::getId() const
{
	return _id;
}

/**
 * Changes the terror site's unique ID.
 * @param id Unique ID.
 */
void TerrorSite::setId(int id)
{
	_id = id;
}

/**
 * Returns the terror site's unique identifying name.
 * @param lang Language to get strings from.
 * @return Full name.
 */
std::wstring TerrorSite::getName(Language *lang) const
{
	std::wstringstream name;
	name << lang->getString("STR_TERROR_SITE_UC") << _id;
	return name.str();
}

}
