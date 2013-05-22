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
#ifndef OPENXCOM_EXTRASPRITES_H
#define OPENXCOM_EXTRASPRITES_H

#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class ExtraSprites
{
private:
	std::map<int, std::string> _sprites;
	int _width, _height;
public:
	ExtraSprites();
	virtual ~ExtraSprites();
	void load(const YAML::Node &node);
	std::map<int, std::string> *getSprites();
	int getWidth();
	int getHeight();
};

}

#endif