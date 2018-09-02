/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "Waypoint.h"
#include "../Engine/Language.h"

namespace OpenXcom
{

/**
 * Initializes a waypoint.
 */
Waypoint::Waypoint() : Target()
{
}

/**
 *
 */
Waypoint::~Waypoint()
{
}

/**
 * Returns the waypoint's unique type used for
 * savegame purposes.
 * @return ID.
 */
std::string Waypoint::getType() const
{
	return "STR_WAY_POINT";
}

/**
 * Returns the globe marker for the waypoint.
 * @return Marker sprite, -1 if none.
 */
int Waypoint::getMarker() const
{
	return 6;
}

}
