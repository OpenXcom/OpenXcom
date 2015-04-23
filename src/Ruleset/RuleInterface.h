/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#ifndef OPENXCOM_RULEINTERFACE_H
#define OPENXCOM_RULEINTERFACE_H

#include <string>
#include <map>
#include <SDL.h>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

struct Element
{
	/// basic rect info, and 3 colors.
	int x, y, w, h, color, color2, border;
	/// defines inversion behaviour
	bool TFTDMode;
};

class RuleInterface
{
private:
	std::string _type;
	std::string _palette;
	std::string _parent;

	std::map <std::string, Element> _elements;
public:
	/// Consructor.
	RuleInterface(const std::string & type);
	/// Destructor.
	~RuleInterface();
	/// Load from YAML.
	void load(const YAML::Node& node);
	/// Get an element.
	Element *getElement(const std::string id);
	/// Get palette.
	const std::string &getPalette() const;
	/// Get parent interface rule.
	const std::string &getParent() const;
};

}
#endif