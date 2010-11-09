/*
 * Copyright 2010 OpenXcom Developers.
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

#include "Position.h"

/**
 * Initializes a new position.
 */
Position::Position(int x, int y, int z) : _x(x), _y(y), _z(z)
{

}

/**
 * Deletes the position.
 */
Position::~Position()
{
	
}

/**
 * Gets X coordinate.
 * @return X coordinate
 */
int Position::getX()
{
	return _x;
}

/**
 * Gets Y coordinate.
 * @return Y coordinate
 */
int Position::getY()
{
	return _y;
}

/**
 * Gets Z coordinate.
 * @return Z coordinate
 */
int Position::getZ()
{
	return _z;
}

/**
 * == operator
 * @param pos
 * @return bool
 */
bool Position::operator== (const Position& pos)
{
	return _x == pos._x && _y == pos._y && _z == pos._z;
}

/**
 * != operator
 * @param pos
 * @return bool
 */
bool Position::operator!= (const Position& pos)
{
	return _x != pos._x || _y != pos._y || _z != pos._z;
}