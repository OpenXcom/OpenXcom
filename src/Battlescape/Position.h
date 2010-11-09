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
#ifndef OPENXCOM_POSITION_H
#define OPENXCOM_POSITION_H

/**
 * Easy handling of X-Y-Z coordinates.
 */
class Position
{
private:
	int _x, _y, _z;
public:
	/// Creates a position.
	Position(int x, int y, int z);
	/// Cleans up the position.
	~Position();
	/// gets X coord
	int getX();
	/// gets Y coord
	int getY();
	/// gets Z coord
	int getZ();
	/// == operator
    bool operator== (const Position& pos);
	/// != operator
    bool operator!= (const Position& pos);

};

#endif
