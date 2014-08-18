/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#ifndef OPENXCOM_RULEGLOBE_H
#define OPENXCOM_RULEGLOBE_H

#include <list>
#include <string>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class Polygon;
class Polyline;

/**
 * Represents the contents of the Geoscape globe,
 * such as world polygons, polylines, etc.
 * @sa Globe
 */
class RuleGlobe
{
private:
	std::list<Polygon*> _polygons;
	std::list<Polyline*> _polylines;
public:
	/// Creates a blank globe ruleset.
	RuleGlobe();
	/// Cleans up the globe ruleset.
	~RuleGlobe();
	/// Loads the globe from YAML.
	void load(const YAML::Node& node);
	/// Gets the list of world polygons.
	std::list<Polygon*> *getPolygons();
	/// Gets the list of world polylines.
	std::list<Polyline*> *getPolylines();
	/// Loads a set of polygons from a DAT file.
	void loadDat(const std::string &filename);
};

}

#endif
