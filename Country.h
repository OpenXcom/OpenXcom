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
#ifndef OPENXCOM__COUNTRY_H
#define OPENXCOM__COUNTRY_H

/**
 * Represents a country who funds the player.
 * Countries provide an amount of monthly funding that
 * changes based on X-Com's or alien's actions in their
 * area.
 */
class Country
{
private:
	int _funding, _change;
public:
	/// Creates a new country with a certain funding.
	Country(int funding);
	/// Cleans up the country.
	~Country();
	/// Gets the country's funding.
	int getFunding();
	/// Sets the country's funding.
	void setFunding(int funding);
	/// Gets the country's funding change.
	int getChange();
};

#endif