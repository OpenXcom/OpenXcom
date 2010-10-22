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

/**
 * Initializes a new ingame time with a certain starting point.
 * @param weekday Starting weekday.
 * @param day Starting day.
 * @param month Starting month.
 * @param year Starting year.
 * @param hour Starting hour.
 * @param minute Starting minute.
 * @param second Starting second.
 */
GameTime::GameTime(int weekday, int day, int month, int year, int hour, int minute, int second) : _second(second), _minute(minute), _hour(hour), _weekday(weekday), _day(day), _month(month), _year(year)
{
}

/**
 *
 */
GameTime::~GameTime()
{
}

/**
 * Advances the ingame time by 5 seconds, automatically correcting
 * the other components when necessary and sending out a trigger when
 * a certain time span has elapsed for time-dependent events.
 * @return Time span trigger.
 */
TimeTrigger GameTime::advance()
{
	TimeTrigger trigger = TIME_5SEC;
	int monthDays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	// Leap year
	if ((_year % 4 == 0) && !(_year % 100 == 0 && _year % 400 != 0))
		monthDays[1]++;

	_second += 5;

	if (_second >= 60)
	{
		_minute++;
		_second = 0;
		if (_minute % 10 == 0)
			trigger = TIME_10MIN;
		if (_minute % 30 == 0)
			trigger = TIME_30MIN;
	}
	if (_minute >= 60)
	{
		_hour++;
		_minute = 0;
		trigger = TIME_1HOUR;
	}
	if (_hour >= 24)
	{
		_day++;
		_weekday++;
		_hour = 0;
		trigger = TIME_1DAY;
	}
	if (_weekday > 7)
	{
		_weekday = 1;
	}
	if (_day > monthDays[_month - 1])
	{
		_day = 1;
		_month++;
		trigger = TIME_1MONTH;
	}
	if (_month > 12)
	{
		_month = 1;
		_year++;
	}

	return trigger;
}

/**
 * Returns the current ingame second.
 * @return Second (0-59).
 */
int GameTime::getSecond()
{
	return _second;
}

/**
 * Returns the current ingame minute.
 * @return Minute (0-59).
 */
int GameTime::getMinute()
{
	return _minute;
}

/**
 * Returns the current ingame hour.
 * @return Hour (0-23).
 */
int GameTime::getHour()
{
	return _hour;
}

/**
 * Returns the current ingame weekday.
 * @return Weekday (1-7), starts on Sunday.
 */
int GameTime::getWeekday()
{
	return _weekday;
}

/**
 * Returns a localizable-std::string representation of
 * the current ingame weekday.
 * @return Weekday std::string ID.
 */
LangString GameTime::getWeekdayString()
{
	return (LangString)(STR_SUNDAY - 1 + _weekday);
}

/**
 * Returns the current ingame day.
 * @return Day (1-31).
 */
int GameTime::getDay()
{
	return _day;
}

/**
 * Returns a localizable-std::string representation of
 * the cardinal operator for the current ingame day.
 * @return Day std::string ID.
 */
LangString GameTime::getDayString()
{
	switch (_day)
	{
	case 1:
	case 21:
	case 31:
		return STR_ST;
	case 2:
	case 22:
		return STR_ND;
	case 3:
	case 23:
		return STR_RD;
	default:
		return STR_TH;
	}
}

/**
 * Returns the current ingame month.
 * @return Month (1-12).
 */
int GameTime::getMonth()
{
	return _month;
}

/**
 * Returns a localizable-std::string representation of
 * the current ingame month.
 * @return Month std::string ID.
 */
LangString GameTime::getMonthString()
{
	return (LangString)(STR_JAN - 1 + _month);
}

/**
 * Returns the current ingame year.
 * @return Year.
 */
int GameTime::getYear()
{
	return _year;
}

/**
 * Returns the current position of the daylight emitted on the globe
 * according to the current ingame time, so the value is 0 when the light
 * starts at 0º longitude (6h) and 1 when the light ends at 0º longitude (18h).
 * @return Daylight position (0-1).
 */
double GameTime::getDaylight()
{
	return (double)((((((_hour + 18) % 24) * 60) + _minute) * 60) + _second) / (60 * 60 * 24);
}