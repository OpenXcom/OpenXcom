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
#include "Country.h"

Country::Country(int funding) : _funding(funding), _change(0)
{
}

Country::~Country()
{
}

int Country::getFunding()
{
	return _funding;
}

void Country::setFunding(int funding)
{
	_funding = funding;
}

int Country::getChange()
{
	return _change;
}

void Country::setChange(int change)
{
	_change = change;
}