/*
 * Copyright 2010 Daniel Albano
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
#ifndef OPENXCOM_WAYPOINT_H
#define OPENXCOM_WAYPOINT_H

#include "Target.h"
#include "../Resource/LangString.h"

using namespace std;

/**
 * Represents a fixed waypoint on the world.
 */
class Waypoint : public Target
{
private:
	int _id;
public:
	/// Creates a waypoint.
	Waypoint();
	/// Cleans up the waypoint.
	~Waypoint();
	/// Gets the waypoint's ID.
	int getId();
	/// Sets the waypoint's ID.
	void setId(int id);
	/// Gets the waypoint's name.
	string getName(Language *lang);
};

#endif
