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
#include "StateAppearance.h"

namespace YAML
{
	template<>
	struct convert<OpenXcom::AppearanceColors>
	{
		static Node encode(const OpenXcom::AppearanceColors& rhs)
		{
			Node node;
			for (int i = 0; i < 8; ++i)
			{
				if (rhs.c[i] != 0)
					node.push_back(rhs.c[i]);
				else
					break;
			}
			return node;
		}

		static bool decode(const Node& node, OpenXcom::AppearanceColors& rhs)
		{
			if(!node.IsSequence() || node.size() < 4)
				return false;

			for (int i = 0; i < node.size(); ++i)
				rhs.c[i] = node[i].as<int>();
			return true;
		}
	};
}

namespace OpenXcom
{

/**
 * Initializes a StateAppearance with certain data.
 * @param type String defining the type.
 */
StateAppearance::StateAppearance(const std::string &type) : _background(""), _palette(""), _highContrast(false), _colors(0)
{
}

/**
 * Cleans up the StateAppearance.
 */
StateAppearance::~StateAppearance()
{
}

/**
 * Loads the StateAppearance from YAML.
 * @param node YAML node.
 */
void StateAppearance::load(const YAML::Node& node)
{
	_background = node["background"].as<std::string>(_background);
	_palette = node["palette"].as<std::string>(_palette);
	_highContrast = node["highContrast"].as<bool>(_highContrast);
	_colors = node["colors"].as<AppearanceColors>(_colors);
}


}
