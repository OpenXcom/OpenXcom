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
#include "AlienBase.h"
#include <sstream>
#include "../Engine/Language.h"

namespace OpenXcom
{

/**
 * Initializes an alien base
 */
AlienBase::AlienBase() : Target(), _id(0), _hoursActive(-1), _inBattlescape(false), _discovered(false), _supplyTime(0)
{
}

/**
 *
 */
AlienBase::~AlienBase()
{
}

/**
 * Loads the alien base from a YAML file.
 * @param node YAML node.
 */
void AlienBase::load(const YAML::Node &node)
{
	Target::load(node);
	node["id"] >> _id;
	node["race"] >> _race;
	node["inBattlescape"] >> _inBattlescape;
	node["discovered"] >> _discovered;
	node["supplyTime"] >> _supplyTime;
}

/**
 * Saves the alien base to a YAML file.
 * @param out YAML emitter.
 */
void AlienBase::save(YAML::Emitter &out) const
{
	Target::save(out);
	out << YAML::Key << "id" << YAML::Value << _id;
	out << YAML::Key << "race" << YAML::Value << _race;
	out << YAML::Key << "inBattlescape" << YAML::Value << _inBattlescape;
	out << YAML::Key << "discovered" << YAML::Value << _discovered;
	out << YAML::Key << "supplyTime" << YAML::Value << _supplyTime;
	out << YAML::EndMap;
}

/**
 * Saves the alien base's unique identifiers to a YAML file.
 * @param out YAML emitter.
 */
void AlienBase::saveId(YAML::Emitter &out) const
{
	Target::saveId(out);
	out << YAML::Key << "type" << YAML::Value << "STR_ALIEN_BASE";
	out << YAML::Key << "id" << YAML::Value << _id;
	out << YAML::EndMap;
}

/**
 * Returns the alien base's unique ID.
 * @return Unique ID.
 */
int AlienBase::getId() const
{
	return _id;
}

/**
 * Changes the alien base's unique ID.
 * @param id Unique ID.
 */
void AlienBase::setId(int id)
{
	_id = id;
}

/**
 * Returns the alien base's unique identifying name.
 * @param lang Language to get strings from.
 * @return Full name.
 */
std::wstring AlienBase::getName(Language *lang) const
{
	std::wstringstream name;
	name << lang->getString("STR_ALIEN_BASE_") << _id;
	return name.str();
}

/**
 * Returns the alien race currently residing in the alien base.
 * @return Alien race.
 */
std::string AlienBase::getAlienRace() const
{
	return _race;
}

/**
 * Changes the alien race currently residing in the alien base.
 * @param race Alien race.
 */
void AlienBase::setAlienRace(const std::string &race)
{
	_race = race;
}

/**
 * Gets the alien base's battlescape status.
 * @return bool
 */
bool AlienBase::isInBattlescape() const
{
	return _inBattlescape;
}

/**
 * Sets the alien base's battlescape status.
 * @param inbattle .
 */
void AlienBase::setInBattlescape(bool inbattle)
{
	_inBattlescape = inbattle;
}

/**
 * Gets the alien base's battlescape status.
 * @return bool.
 */
bool AlienBase::isDiscovered() const
{
	return _discovered;
}

/**
 * Sets the alien base's discovered status.
 * @param discovered.
 */
void AlienBase::setDiscovered(bool discovered)
{
	_discovered = discovered;
}

/**
 * Gets the alien base's time since last resupply.
 * @return int.
 */
int AlienBase::getSupplyTime() const
{
	return _supplyTime;
}

/**
 * Resets the alien base's time since last resupply.
 */
void AlienBase::setSupplyTime(int time)
{
	_supplyTime = time;
}
}
