/*
 * Copyright 2010 OpenXcom Developers
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
#include "Spawnpoint.h"

/**
 * Initializes a spawnpoint.
 */
Spawnpoint::Spawnpoint() : _id(0)
{
}

/**
 *
 */
Spawnpoint::~Spawnpoint()
{
}

/**
 * Returns the Spawnpoint's unique ID.
 * @return Unique ID.
 */
int Spawnpoint::getId()
{
	return _id;
}

/**
 * Changes the Spawnpoint's unique ID.
 * @param id Unique ID.
 */
void Spawnpoint::setId(int id)
{
	_id = id;
}
