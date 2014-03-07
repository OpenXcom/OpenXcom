/*
 * Copyright 2010-2013 OpenXcom Developers.
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

#include "RuleMusic.h"

namespace OpenXcom
{

RuleMusic::RuleMusic() : _type(""), _mode(""), _midiIndex(-1)
{
}

RuleMusic::~RuleMusic()
{
}

void RuleMusic::load(const YAML::Node &node)
{
	_type = node["type"].as<std::string>(_type);
	
	if (node["mode"])
	  _mode = node["mode"].as<std::string>(_mode);
	else
	  _mode = "replace";
	
	if (node["midiPack"])
	  _midiIndex = node["midiPack"].as<int>(_midiIndex);
	else
	  _midiIndex = -1;
	
	_terrains = node["terrain"].as< std::vector<std::string> >(_terrains);
	_files = node["files"].as< std::vector<std::string> >(_files);
	_indexes = node["indexes"].as< std::vector<int> >(_indexes);
}

std::string RuleMusic::getMode()
{
  return _mode;
}
int RuleMusic::getMidiIndex()
{
	return _midiIndex;
}
std::vector<std::string> RuleMusic::getTerrains()
{
  return _terrains;
}
std::vector<std::string> RuleMusic::getFiles()
{
  return _files;
}
std::vector<int> RuleMusic::getIndexes()
{
  return _indexes;
}
/*
bool RuleMusic::hasTerrainSpecification()
{
  return !_terrains.empty();
}*/	
}
