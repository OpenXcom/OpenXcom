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
#ifndef OPENXCOM_XCOMRESOURCEPACK_H
#define OPENXCOM_XCOMRESOURCEPACK_H

#include "ResourcePack.h"

namespace OpenXcom
{
class ExtraSprites;
class ExtraSounds;

/**
 * Resource pack for the X-Com: UFO Defense game.
 */
class XcomResourcePack : public ResourcePack
{
public:
	/// Creates the X-Com ruleset.
	XcomResourcePack(std::vector<std::pair<std::string, ExtraSprites *> > extraSprites, std::vector<std::pair<std::string, ExtraSounds *> > extraSounds);
	/// Cleans up the X-Com ruleset.
	~XcomResourcePack();
	/// Loads battlescape specific resources
	void loadBattlescapeResources();
	bool isImageFile(std::string extension);
};

}

#endif
