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
	bool _singleImage;
	int _modIndex, _subX, _subY;
public:
	/// Creates a blank external sprite set.
	ExtraSprites();
	/// Cleans up the external sprite set.
	virtual ~ExtraSprites();
	/// Loads the data from yaml
	void load(const YAML::Node &node, int modIndex);
	/// Gets the list of sprites defined by this mod
	std::map<int, std::string> *getSprites();
	/// get the width of the surfaces (used for single images and new spritesets)
	const int getWidth() const;
	/// get the height of the surfaces (used for single images and new spritesets)
	const int getHeight() const;
	/// is this a single surface, or a set of surfaces?
	const bool getSingleImage() const;
	/// get the mod index for this external sprite set.
	const int getModIndex() const;
	const int getSubX() const;
	const int getSubY() const;

};

}

#endif