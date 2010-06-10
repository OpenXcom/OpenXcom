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
#include "RuleCraft.h"

RuleCraft::RuleCraft(LangString type) : _type(type), _sprite(0), _fuelMax(0), _healthMax(0), _speed(0), _accel(0), _weapons(0), _soldiers(0), _hwp(0), _cost(0), _monthlyFee(0)
{
}

RuleCraft::~RuleCraft()
{
}

LangString RuleCraft::getType()
{
	return _type;
}

int RuleCraft::getSprite()
{
	return _sprite;
}

void RuleCraft::setSprite(int sprite)
{
	_sprite = sprite;
}

int RuleCraft::getMonthlyFee()
{
	return _monthlyFee;
}

void RuleCraft::setMonthlyFee(int fee)
{
	_monthlyFee = fee;
}