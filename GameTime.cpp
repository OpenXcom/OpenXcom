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
#include "GameTime.h"

GameTime::GameTime(int weekday, int day, int month, int year, int hour, int minute, int second) : _second(second), _minute(minute), _hour(hour), _weekday(weekday), _day(day), _month(month), _year(year)
{
}

GameTime::~GameTime()
{
}

void GameTime::advance()
{
	_second += 5;

	if (_second >= 60)
	{
		_minute++;
		_second = 0;
	}
	if (_minute >= 60)
	{
		_hour++;
		_minute = 0;
	}
	if (_hour >= 24)
	{
		_day++;
		_weekday++;
		_hour = 0;
	}
	if (_weekday > 7)
	{
		_weekday = 1;
	}
	switch (_month)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		if (_day > 31)
		{
			_day = 1;
			_month++;
		}
		break;
	case 2:
		if (_day > 29)
		{
			_day = 1;
			_month++;
		}
		break;
	case 4:
	case 6:
	case 9:
	case 11:
		if (_day > 30)
		{
			_day = 1;
			_month++;
		}
		break;
	}
	if (_month > 12)
	{
		_month = 1;
		_year++;
	}
}

int GameTime::getSecond()
{
	return _second;
}

int GameTime::getMinute()
{
	return _minute;
}

int GameTime::getHour()
{
	return _hour;
}

int GameTime::getWeekday()
{
	return _weekday;
}

int GameTime::getDay()
{
	return _day;
}

int GameTime::getMonth()
{
	return _month;
}

int GameTime::getYear()
{
	return _year;
}